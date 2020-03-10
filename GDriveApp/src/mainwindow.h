#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace GDrive {
    class GDriveService;
}

class UIDownloadDialog;
class QSettings;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override; //

private slots:
    /// Click menu login
    void on_actionLogin_account_triggered();
    /// when m_Drive grant succeed, show info and allow other operation
    void onGDrive_granted();
    /// Click menu Logout
    void on_action_Logout_Account_triggered();
    /// Click menu Account About
    void on_actionAbout_triggered();

    void on_actionDownload_file_triggered();

    void on_actionSimple_Upload_triggered();

    void on_actionMultipart_Upload_triggered();

    void on_actionResumable_Upload_triggered();
private:
    Ui::MainWindow *ui;
    UIDownloadDialog *downloadDialog;
    QString m_currentUploadFilePath;
    QSettings *m_settings;
    GDrive::GDriveService *m_Drive;

    void accountLogin();
    void accountLogout();
    void accountAbout();
    void fileSimpleUpload(const QString &filepath);
    void fileMultipartUpload(const QString &filepath);
    void fileResumableUpload(const QString &filepath);
    void fileDownload(const QString &downloadFilePath,const QString &fileId);
    ///
    inline QRect readGeometry(QSettings *settings);
    inline QString readUploadFilePath(QSettings *settings);
    inline QString readDownloadFilePath(QSettings *settings);
    inline QString readDownloadFileID(QSettings *settings);
    void writeSettings();
};

#endif // MAINWINDOW_H
