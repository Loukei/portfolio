#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace GDrive {
    class GDriveService;
}

class SearchDialog;
class UIDownloadDialog;
class QSettings;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    /// Download Dialog
    UIDownloadDialog *m_dialogDownload;
    /// A dialog to provide search drive files
    SearchDialog *m_dialogSearch;
    /// Save recent upload file
    QString m_currentUploadFilePath;
    /// Save settings
    QSettings *m_settings;
    /// Google Drive Api
    GDrive::GDriveService *m_Drive;

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
    /// Click menu Search file&folder
    void on_action_Search_file_folder_triggered();
    /// Receive m_DialogSearch::query, then return search result to m_DialogSearch
    void onSearchDialog_query(const QString &q,
                              const QString &pageToken);
private:
    /// GDriveService Account Login
    void accountLogin();
    /// GDriveService Account Logout
    void accountLogout();
    /// GDriveService Account get About message
    void accountAbout();
    /// GDriveService upload(Create) file Simple method
    void fileSimpleUpload(const QString &filepath);
    /// GDriveService upload(Create) file Multipart method
    void fileMultipartUpload(const QString &filepath);
    /// GDriveService upload(Create) file Resumable method
    void fileResumableUpload(const QString &filepath);
    /// GDriveService Download(Get) file
    void fileDownload(const QString &downloadFilePath,const QString &fileId);
    /// read Settings to Window geometry
    inline QRect readGeometry(QSettings *settings);
    /// read Settings(UploadFilePath) to m_currentUploadFilePath
    inline QString readUploadFilePath(QSettings *settings);
    /// read Settings(DownloadFilePath) to downloadDialog
    inline QString readDownloadFilePath(QSettings *settings);
    /// read Settings(DownloadFileID) to downloadDialog
    inline QString readDownloadFileID(QSettings *settings);
    /// write settings to ini file
    void writeSettings();
};

#endif // MAINWINDOW_H
