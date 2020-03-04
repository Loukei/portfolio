#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gdrivefileresource.h"
#include "gdriveaboutresource.h"

namespace Ui {
class MainWindow;
}

namespace GDrive {
    class GDriveService;
}

class UIDownloadDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override; //

private slots:
    void on_actionLogin_account_triggered();

    void on_actionAbout_triggered();

    void on_actionDownload_file_triggered();

    void on_actionSimple_Upload_triggered();

    void on_actionMultipart_Upload_triggered();

    void on_actionResumable_Upload_triggered();

    void on_action_Logout_Account_triggered();

private:
    Ui::MainWindow *ui;
    UIDownloadDialog *downloadDialog;
    GDrive::GDriveService *m_Drive;
//    GDrive::GDriveFileResource m_fileResource;
    void accountLogin();
    void accountLogout();
    void accountAbout();
    void fileSimpleUpload(const QString &filepath);
    void fileMultipartUpload(const QString &filepath);
    void fileResumableUpload(const QString &filepath);
    void fileDownload(const QString &downloadFilePath,const QString &fileId);
};

#endif // MAINWINDOW_H
