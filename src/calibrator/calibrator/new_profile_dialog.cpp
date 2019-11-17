#include "new_profile_dialog.h"
#include "ui_new_profile_dialog.h"

NewProfileDialog::NewProfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProfileDialog)
{
    ui->setupUi(this);
}

NewProfileDialog::~NewProfileDialog()
{
    delete ui;
}

void NewProfileDialog::on_lineEdit_textChanged(const QString &arg1)
{
    new_profile_name = arg1;
}
