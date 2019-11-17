#ifndef NEW_PROFILE_DIALOG_H
#define NEW_PROFILE_DIALOG_H

#include <QDialog>

namespace Ui {
class NewProfileDialog;
}

class NewProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProfileDialog(QWidget *parent = nullptr);
    ~NewProfileDialog();

    const QString GetNewProfileName() const { return new_profile_name; }

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::NewProfileDialog *ui;

    QString new_profile_name = "";
};

#endif // NEW_PROFILE_DIALOG_H
