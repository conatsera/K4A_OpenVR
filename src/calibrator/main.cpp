// dear imgui: standalone example application for DirectX 12
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// FIXME: 64-bit only for now! (Because sizeof(ImTextureId) == sizeof(void*))

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <thread>
#include <iostream>
#include <math.h>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#include "calibration.h"

struct ErrorEntry
{
    uint64_t Code;
    std::string Text;
};

struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    UINT64                  FenceValue;
};

// Data
static int const                    NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT                         g_frameIndex = 0;

static int const                    NUM_BACK_BUFFERS = 3;
static ID3D12Device*                g_pd3dDevice = NULL;
static ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue*          g_pd3dCommandQueue = NULL;
static ID3D12GraphicsCommandList*   g_pd3dCommandList = NULL;
static ID3D12Fence*                 g_fence = NULL;
static HANDLE                       g_fenceEvent = NULL;
static UINT64                       g_fenceLastSignaledValue = 0;
static IDXGISwapChain3*             g_pSwapChain = NULL;
static HANDLE                       g_hSwapChainWaitableObject = NULL;
static ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
static bool                         g_inputLoopActive = true;
static bool                         g_vrMouseActive = false;
static ImVec2                       g_lastMousePos = ImVec2(0, 0);
static bool                         g_mouseDown[5] = {false, false, false, false, false};
static ImVec2                       g_currentResolution = ImVec2(1280, 720);

static TCHAR szK4ACalibration[] = TEXT("BoneCalibrationMemmap");
#define	CALIBRATION_MEMSIZE (sizeof(Calibration::calibration_data_t))
static HANDLE hK4ACalibrationMapObject = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
void ResizeSwapChain(HWND hWnd, int width, int height);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InputLoop(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayHandle_t ulOverlayThumbnailHandle, ImGuiIO& io);



double _copysign(double sizeval, float signval)
{
    return ((0.0 < signval) - (signval < 0.0)) == 1 ? std::abs(sizeval) : -std::abs(sizeval);
}

vr::HmdQuaternion_t GetRotation(vr::TrackedDevicePose_t matrix)
{
    vr::HmdQuaternion_t q;
    q.w = std::sqrt(std::fmax(0, 1 + matrix.mDeviceToAbsoluteTracking.m[0][0] + matrix.mDeviceToAbsoluteTracking.m[1][1] + matrix.mDeviceToAbsoluteTracking.m[2][2])) / 2;
    q.x = std::sqrt(std::fmax(0, 1 + matrix.mDeviceToAbsoluteTracking.m[0][0] - matrix.mDeviceToAbsoluteTracking.m[1][1] - matrix.mDeviceToAbsoluteTracking.m[2][2])) / 2;
    q.y = std::sqrt(std::fmax(0, 1 - matrix.mDeviceToAbsoluteTracking.m[0][0] + matrix.mDeviceToAbsoluteTracking.m[1][1] - matrix.mDeviceToAbsoluteTracking.m[2][2])) / 2;
    q.z = std::sqrt(std::fmax(0, 1 - matrix.mDeviceToAbsoluteTracking.m[0][0] - matrix.mDeviceToAbsoluteTracking.m[1][1] + matrix.mDeviceToAbsoluteTracking.m[2][2])) / 2;
    q.x = _copysign(q.x, matrix.mDeviceToAbsoluteTracking.m[2][1] - matrix.mDeviceToAbsoluteTracking.m[1][2]);
    q.y = _copysign(q.y, matrix.mDeviceToAbsoluteTracking.m[0][2] - matrix.mDeviceToAbsoluteTracking.m[2][0]);
    q.z = _copysign(q.z, matrix.mDeviceToAbsoluteTracking.m[1][0] - matrix.mDeviceToAbsoluteTracking.m[0][1]);
    return q;
}

vr::HmdQuaternion_t offsetQuatZ{
    std::cos(std::acos(-1) / -4),
    0.0,
    0.0,
    std::sin(std::acos(-1) / -4)
};
vr::HmdQuaternion_t offsetQuatY{
    std::cos(std::acos(-1) / 4),
    0.0,
    std::sin(std::acos(-1) / 4),
    0.0
};
vr::HmdQuaternion_t offsetQuatX{
    std::cos(std::acos(-1) / 4),
    std::sin(std::acos(-1) / 4),
    0.0,
    0.0
};

vr::HmdQuaternion_t offsetQuat1{
    std::cos(std::acos(-1) / -4),
    0.0,
    0.0,
    std::sin(std::acos(-1) / -4)
};
vr::HmdQuaternion_t offsetQuat2{
    std::cos(std::acos(-1) / 4),
    0.0,
    std::sin(std::acos(-1) / 4),
    0.0
};
vr::HmdQuaternion_t offsetQuat3{
    std::cos(std::acos(-1) / 2),
    0.0,
    0.0,
    std::sin(std::acos(-1) / 2)
};
vr::HmdQuaternion_t offsetQuat4{
    std::cos((std::acos(-1) / -4)* (7.3 / 90.0)),
    0.0,
    0.0,
    std::sin((std::acos(-1) / -4)* (7.3 / 90.0))
};

inline vr::HmdQuaternion_t QuaternionProduct(vr::HmdQuaternion_t& quata, vr::HmdQuaternion_t& quatb)
{
    vr::HmdQuaternion_t quat;
    quat.w = ((quata.w * quatb.w) - (quata.x * quatb.x) - (quata.y * quatb.y) - (quata.z * quatb.z));
    quat.x = ((quata.w * quatb.x) + (quata.x * quatb.w) + (quata.y * quatb.z) - (quata.z * quatb.y));
    quat.y = ((quata.w * quatb.y) - (quata.x * quatb.z) + (quata.y * quatb.w) + (quata.z * quatb.x));
    quat.z = ((quata.w * quatb.z) + (quata.x * quatb.y) - (quata.y * quatb.x) + (quata.z * quatb.w));
    return quat;
}

inline vr::HmdQuaternion_t QuaternionInverse(vr::HmdQuaternion_t& quat)
{
    vr::HmdQuaternion_t inverse_quat;
    inverse_quat.w = quat.w;
    inverse_quat.x = -(quat.x);
    inverse_quat.y = -(quat.y);
    inverse_quat.z = -(quat.z);
    return inverse_quat;
}

static Calibration::calibration_data_t* calibrationData;

// Main code
int main(int, char**)
{
    hK4ACalibrationMapObject = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        szK4ACalibration
    );

    if (hK4ACalibrationMapObject == nullptr)
    {
        std::cout << "Could not open file mapping object " << GetLastError() << std::endl;
        return 2;
    }

    calibrationData = reinterpret_cast<Calibration::calibration_data_t*>((LPTSTR)MapViewOfFile(hK4ACalibrationMapObject,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        CALIBRATION_MEMSIZE));

    if (calibrationData == nullptr)
    {
        std::cout << "Could not map view of file " << GetLastError() << std::endl;

        CloseHandle(hK4ACalibrationMapObject);
        return 2;
    }

    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("K4A_OpenVR Calibrator"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("K4A_OpenVR Calibrator"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
        DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
        g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
        g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("./Roboto-Medium.ttf", 32.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ErrorEntry errorLog[16];
    size_t errorCount = 0;

    vr::EVRInitError lastInitError = vr::VRInitError_None;
    vr::IVRSystem* pVRSystem = vr::VR_Init(&lastInitError, vr::VRApplication_Overlay);

    if (lastInitError != vr::VRInitError_None)
    {
        errorLog[errorCount].Code = lastInitError;
        errorLog[errorCount++].Text = "VRInitError";
    }

    //vr::VROverlayHandle_t ulOverlayHandle = vr::k_ulOverlayHandleInvalid;
    //vr::VROverlayHandle_t ulOverlayThumbnailHandle = vr::k_ulOverlayHandleInvalid;
    //bool bSuccess = vr::VRCompositor() != NULL;
    //std::thread inputThread;

    /*if (vr::VROverlay())
    {
        std::string sKey = "K4A_OpenVR.Calibrator";
        std::string sName = "Calibrator";
        vr::VROverlayError overlayError = vr::VROverlay()->CreateDashboardOverlay(sKey.c_str(), sName.c_str(), &ulOverlayHandle, &ulOverlayThumbnailHandle);
        bSuccess = bSuccess && overlayError == vr::VROverlayError_None;
        if (overlayError != vr::VROverlayError_None)
        {
            errorLog[errorCount].Code = overlayError;
            errorLog[errorCount++].Text = "VROverlayError";
        }
    }*/

    //uint32_t width = 0;
    //uint32_t height = 0;

    /*if (bSuccess)
    {
        vr::VROverlay()->SetOverlayWidthInMeters(ulOverlayHandle, 1.5f);
        vr::VROverlay()->SetOverlayInputMethod(ulOverlayHandle, vr::VROverlayInputMethod_Mouse);
        vr::VROverlay()->GetOverlayTextureSize(ulOverlayHandle, &width, &height);

        inputThread = std::thread(&InputLoop, ulOverlayHandle, ulOverlayThumbnailHandle, io);
    }*/

    bool fineTuningEnabled = false;

    bool fineTuningRefQuatValid = false;
    vr::TrackedDevicePose_t refPose;
    vr::HmdQuaternion_t fineTuningRefQuat = { 1.0, 0.0, 0.0, 0.0 };
    vr::HmdQuaternion_t fineCalibrationRotation = { 1.0, 0.0, 0.0, 0.0 };
    int X = 0;
    bool Xneg = false;
    int Y = 0;
    bool Yneg = false;
    int Z = 0;
    bool Zneg = false;

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, (const ImVec4)ImColor(0, 0, 0));
        /*if (g_vrMouseActive)
        {
            io.MousePos = g_lastMousePos;
            std::memcpy(io.MouseDown, g_mouseDown, sizeof(g_mouseDown));
        }*/
        ImGui::NewFrame();

        // Fine tuning and calibration window
        {
            static float w = 0.0f;
            static int counter = 0;

            ImGui::Begin("Calibrate");

            ImGui::SetWindowPos(ImVec2{ 0, 0 });
            ImGui::SetWindowSize(ImVec2(g_currentResolution.x, g_currentResolution.y));

            vr::VRControllerState_t hmdControllerState;
            vr::TrackedDevicePose_t hmdPose;
            vr::VRSystem()->GetControllerStateWithPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding,
                vr::k_unTrackedDeviceIndex_Hmd, &hmdControllerState, sizeof(hmdControllerState), &hmdPose);

            ImGui::Checkbox("Activate fine tuning", &fineTuningEnabled);
            //if (ImGui::SliderFloat("Smoothing", &calibrationData->m_smoothing, 0.0f, 1.0f))
            //    calibrationData->update = true;
            //ImGui::Checkbox("Activate Auto Smoothing(experimental)", &calibrationData->autoSmooth);
            ImGui::Checkbox("Activate more trackers(experimental, mega lag)", &calibrationData->moreTrackers);
            vr::HmdQuaternion_t quat = GetRotation(hmdPose);

            ImGui::Text("{ %.4f, %.4f, %.4f }",
                hmdPose.mDeviceToAbsoluteTracking.m[0][3],
                hmdPose.mDeviceToAbsoluteTracking.m[1][3],
                hmdPose.mDeviceToAbsoluteTracking.m[2][3]);

            ImGui::Text("{ %.4f, %.4f, %.4f, %.4f }",
                quat.w,
                quat.x,
                quat.y,
                quat.z);
            ImGui::Text("fps: %.4f", calibrationData->fps);

            ImGui::RadioButton("X", &X, 0);
            ImGui::RadioButton("Y", &X, 1);
            ImGui::RadioButton("Z", &X, 2);
            /*ImGui::Checkbox("X Negative", &Xneg);

            ImGui::RadioButton("YX", &Y, 0);
            ImGui::RadioButton("YY", &Y, 1);
            ImGui::RadioButton("YZ", &Y, 2);
            ImGui::Checkbox("Y Negative", &Yneg);

            ImGui::RadioButton("ZX", &Z, 0);
            ImGui::RadioButton("ZY", &Z, 1);
            ImGui::RadioButton("ZZ", &Z, 2);
            ImGui::Checkbox("Z Negative", &Zneg);*/

            if (fineTuningEnabled)
            {
                vr::TrackedDeviceIndex_t rightHandIndex = vr::VRSystem()->
                    GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_RightHand);

                if (rightHandIndex != vr::k_unTrackedDeviceIndexInvalid)
                {
                    vr::VRControllerState_t rightHandControllerState;
                    vr::TrackedDevicePose_t rightHandPose;
                    vr::VRSystem()->GetControllerStateWithPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding,
                        rightHandIndex, &rightHandControllerState, sizeof(rightHandControllerState), &rightHandPose);

                    if (rightHandControllerState.ulButtonPressed == (vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger) | vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip)))
                    {
                        if (refPose.bPoseIsValid == true)
                        {
                            calibrationData->x = calibrationData->x - (refPose.mDeviceToAbsoluteTracking.m[0][3] - rightHandPose.mDeviceToAbsoluteTracking.m[0][3]);
                            calibrationData->y = calibrationData->y - (refPose.mDeviceToAbsoluteTracking.m[1][3] - rightHandPose.mDeviceToAbsoluteTracking.m[1][3]);
                            calibrationData->z = calibrationData->z - (refPose.mDeviceToAbsoluteTracking.m[2][3] - rightHandPose.mDeviceToAbsoluteTracking.m[2][3]);

                            calibrationData->update = true;
                        }
                        refPose = rightHandPose;
                    }
                    else if (rightHandControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger))
                    {
                        if (!fineTuningRefQuatValid)
                        {
                            fineTuningRefQuat = GetRotation(rightHandPose);
                            fineTuningRefQuatValid = true;
                        }

                        vr::HmdQuaternion_t quat = GetRotation(rightHandPose);

                        fineCalibrationRotation = QuaternionProduct(quat, QuaternionInverse(fineTuningRefQuat));

                        fineTuningRefQuat = GetRotation(rightHandPose);

                        vr::HmdQuaternion_t currentCalibration = {
                            calibrationData->rotOffset.w,
                            calibrationData->rotOffset.x,
                            calibrationData->rotOffset.y,
                            calibrationData->rotOffset.z
                        };

                        switch (X)
                        {
                        case 0:
                            fineCalibrationRotation = vr::HmdQuaternion_t{
                            fineCalibrationRotation.w,
                            fineCalibrationRotation.z,
                            0.0,
                            0.0
                            };
                            break;
                        case 1:
                            fineCalibrationRotation = vr::HmdQuaternion_t{
                            fineCalibrationRotation.w,
                            0.0,
                            0.0,
                            fineCalibrationRotation.y
                            };
                            break;
                        case 2:
                            fineCalibrationRotation = vr::HmdQuaternion_t{
                            fineCalibrationRotation.w,
                            0.0,
                            fineCalibrationRotation.x,
                            0.0
                            };
                            break;
                        }

                        vr::HmdQuaternion_t fineTunedCalibration = QuaternionProduct(currentCalibration, fineCalibrationRotation);

                        calibrationData->rotOffset.w = fineTunedCalibration.w;
                        calibrationData->rotOffset.x = fineTunedCalibration.x;
                        calibrationData->rotOffset.y = fineTunedCalibration.y;
                        calibrationData->rotOffset.z = fineTunedCalibration.z;

                        calibrationData->update = true;
                    }
                    else
                    {
                        fineTuningRefQuatValid = false;
                        refPose.bPoseIsValid = false;
                    }
                }
            }

            if (ImGui::Button("Calibrate"))
            {

                quat = QuaternionProduct(quat, offsetQuat1);
                quat = QuaternionProduct(quat, offsetQuat2);
                quat = QuaternionProduct(quat, offsetQuat3);
                quat = QuaternionProduct(quat, offsetQuat4);

                calibrationData->rotOffset.x = quat.x;
                calibrationData->rotOffset.y = quat.y;
                calibrationData->rotOffset.z = quat.z;
                calibrationData->rotOffset.w = quat.w;

                calibrationData->x = hmdPose.mDeviceToAbsoluteTracking.m[0][3];
                calibrationData->y = hmdPose.mDeviceToAbsoluteTracking.m[1][3];
                calibrationData->z = hmdPose.mDeviceToAbsoluteTracking.m[2][3];

                calibrationData->update = true;
            }
            ImGui::End();
        }

        // Error log window
       /* {
            ImGui::Begin("Error Log");
            ImGui::SetWindowPos(ImVec2( 0, g_currentResolution.y / 2 ));
            ImGui::SetWindowSize(ImVec2( g_currentResolution.x, g_currentResolution.y / 2 ));
            for (int i = 0; i < errorCount; i++)
            {
                ImGui::Text("Code: %d Message: %s", errorLog[i].Code, errorLog[i].Text.c_str());
            }
            ImGui::End();
        }
        */
        // Rendering
        FrameContext* frameCtxt = WaitForNextFrameResources();
        UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
        frameCtxt->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = g_mainRenderTargetResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;

        g_pd3dCommandList->Reset(frameCtxt->CommandAllocator, NULL);
        g_pd3dCommandList->ResourceBarrier(1, &barrier);
        g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], (float*)&clear_color, 0, NULL);
        g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
        g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
        g_pd3dCommandList->ResourceBarrier(1, &barrier);
        g_pd3dCommandList->Close();

        g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync

        UINT64 fenceValue = g_fenceLastSignaledValue + 1;
        g_pd3dCommandQueue->Signal(g_fence, fenceValue);
        g_fenceLastSignaledValue = fenceValue;
        frameCtxt->FenceValue = fenceValue;

        /*
        if (ulOverlayHandle != vr::k_ulOverlayHandleInvalid)
        {
            vr::D3D12TextureData_t d3D12TextureData{
                g_mainRenderTargetResource[backBufferIdx],
                g_pd3dCommandQueue,
                1
            };

            vr::Texture_t texture = { (void*)(uintptr_t)&d3D12TextureData, vr::TextureType_DirectX12, vr::ColorSpace_Auto };
            vr::VROverlay()->SetOverlayTexture(ulOverlayHandle, &texture);
        }*/
    }

    //g_inputLoopActive = false;
    //inputThread.join();
    vr::VR_Shutdown();

    WaitForLastSubmittedFrame();
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

void InputLoop(vr::VROverlayHandle_t ulOverlayHandle, vr::VROverlayHandle_t ulOverlayThumbnailHandle, ImGuiIO& io) {
    while (g_inputLoopActive)
    {
        if (!vr::VRSystem())
            return;

        vr::VREvent_t vrEvent;
        while (vr::VROverlay()->PollNextOverlayEvent(ulOverlayHandle, &vrEvent, sizeof(vrEvent)))
        {
            switch (vrEvent.eventType)
            {
            case vr::VREvent_MouseMove:
            {
                g_vrMouseActive = true;
                g_lastMousePos = ImVec2(vrEvent.data.mouse.x * 1280.F, (1.F - vrEvent.data.mouse.y) * 1280.F);
            }
            break;

            case vr::VREvent_MouseButtonDown:
            {
                g_vrMouseActive = true;
                if (vrEvent.data.mouse.button == vr::VRMouseButton_Left)
                    g_mouseDown[0] = true;
                else if (vrEvent.data.mouse.button == vr::VRMouseButton_Right)
                    g_mouseDown[1] = true;
            }
            break;

            case vr::VREvent_MouseButtonUp:
            {
                g_vrMouseActive = true;
                if (vrEvent.data.mouse.button == vr::VRMouseButton_Left)
                    g_mouseDown[0] = false;
                else if (vrEvent.data.mouse.button == vr::VRMouseButton_Right)
                    g_mouseDown[1] = false;
            }
            break;

            case vr::VREvent_OverlayShown:
            {
               // m_pWidget->repaint();
            }
            break;

            case vr::VREvent_Quit:
                //QApplication::exit();
                break;
            }
        }

        if (ulOverlayThumbnailHandle != vr::k_ulOverlayHandleInvalid)
        {
            while (vr::VROverlay()->PollNextOverlayEvent(ulOverlayThumbnailHandle, &vrEvent, sizeof(vrEvent)))
            {
                switch (vrEvent.eventType)
                {
                case vr::VREvent_OverlayShown:
                {
                    //m_pWidget->repaint();
                }
                break;
                }
            }
        }
        g_vrMouseActive = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

#ifdef DX12_ENABLE_DEBUG_LAYER
    ID3D12Debug* pdx12Debug = NULL;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
    {
        pdx12Debug->EnableDebugLayer();
        pdx12Debug->Release();
    }
#endif

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
        return false;

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
            return false;

        SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            g_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
            return false;
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
            return false;
    }

    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
            return false;

    if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
        g_pd3dCommandList->Close() != S_OK)
        return false;

    if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
        return false;

    g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (g_fenceEvent == NULL)
        return false;

    {
        IDXGIFactory4* dxgiFactory = NULL;
        IDXGISwapChain1* swapChain1 = NULL;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK ||
            dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK ||
            swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
            return false;
        swapChain1->Release();
        dxgiFactory->Release();
        g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
        g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = NULL; }
    if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
    if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
    if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
    if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
    if (g_fence) { g_fence->Release(); g_fence = NULL; }
    if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }

#ifdef DX12_ENABLE_DEBUG_LAYER
    IDXGIDebug1* pDebug = NULL;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
    {
        pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        pDebug->Release();
    }
#endif
}

void CreateRenderTarget()
{
    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource* pBackBuffer = NULL;
        g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
        g_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void CleanupRenderTarget()
{
    WaitForLastSubmittedFrame();

    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
}

void WaitForLastSubmittedFrame()
{
    FrameContext* frameCtxt = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtxt->FenceValue;
    if (fenceValue == 0)
        return; // No fence was signaled

    frameCtxt->FenceValue = 0;
    if (g_fence->GetCompletedValue() >= fenceValue)
        return;

    g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
    WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameResources()
{
    UINT nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL };
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtxt = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtxt->FenceValue;
    if (fenceValue != 0) // means no fence was signaled
    {
        frameCtxt->FenceValue = 0;
        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtxt;
}

void ResizeSwapChain(HWND hWnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC1 sd;
    g_pSwapChain->GetDesc1(&sd);
    sd.Width = width;
    sd.Height = height;

    IDXGIFactory4* dxgiFactory = NULL;
    g_pSwapChain->GetParent(IID_PPV_ARGS(&dxgiFactory));

    g_pSwapChain->Release();
    CloseHandle(g_hSwapChainWaitableObject);

    IDXGISwapChain1* swapChain1 = NULL;
    dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1);
    swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain));
    swapChain1->Release();
    dxgiFactory->Release();

    g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);

    g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
    assert(g_hSwapChainWaitableObject != NULL);

    g_currentResolution = ImVec2(width, height);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            WaitForLastSubmittedFrame();
            ImGui_ImplDX12_InvalidateDeviceObjects();
            CleanupRenderTarget();
            ResizeSwapChain(hWnd, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
            CreateRenderTarget();
            ImGui_ImplDX12_CreateDeviceObjects();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
