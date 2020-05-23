#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GDriveLib/googledriveservice.h"

namespace Ui {
class MainWindow;
}

class AccountWidgetActs;
class UploadDialog;
class UpdateDialog;
class SearchDialog;
class DownloadDialog;
class FileMataDataDialog;
class QJsonModel;
QT_BEGIN_NAMESPACE
class QSettings;
class QLabel;
QT_END_NAMESPACE

/*!
 * \class MainWindow
 * \brief The main UI widget
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    /// A class to store QWidgetAction for Account menu
    AccountWidgetActs *m_accountActs;
    /// A dialog to set Download file
    DownloadDialog *m_dialogDownload;
    /// A dialog to provide search drive files
    SearchDialog *m_dialogSearch;
    /// A dialog to Query File matadata
    FileMataDataDialog *m_dialogFileMataData;
    /// A dialog to update file
    UpdateDialog *m_dialogUpdate;
    /// A dialog to upload file
    UploadDialog *m_dialogUpload;

    /// Save settings
    QSettings *m_settings;
    /// save m_Drive Operation reply
    QJsonModel *m_model;
    /// Google Drive Api serviece
    GDrive::GDriveService *m_Drive;
    /// the previous access token received, use to determine Login/Refresh/Logout
    QString m_currentOAuthToken = QString();
    /// Ecrypt key to save User refresh token
    const quint64 m_ecryptKey = Q_UINT64_C(0x0c2ad4a4acb9f023);

private slots:
    /// Click menu Login
    void on_action_Login_triggered();
    /// when m_Drive grant succeed, show info and allow other operation
    void onGDrive_granted();
    /// Click menu Logout
    void on_action_Logout_triggered();
    /// click menu Refersh token
    void on_action_Refresh_Token_triggered();
    /// change UI when token changed
    void onGDrive_tokenChanged(const QString &token);
    ///
    void onGDrive_error(const QString &error, const QString &errorDescription, const QUrl &uri);
    /// Click menu Account About
    void on_action_About_User_triggered();
    /// Click menu Download file
    void on_action_Download_file_triggered();
    /// Click menu Search file&folder
    void on_action_Search_file_folder_triggered();
    /// Receive SearchDialog::query, then return search result to m_DialogSearch
    void onSearchDialog_query(const QString &corpora,
                              const QString &driveId,
                              const QString &fields,
                              const QString &orderBy,
                              int pageSize,
                              const QString &pageToken,
                              const QString &q,
                              const QString &spaces);
    /// Click menu Get file matadata
    void on_action_Get_file_matadata_triggered();
    /// Receive FileMataDataDialog::query, then return GDriveFileGet to m_dialogFileMataData
    void onFileMataDataDialog_query(const QString &fileID,const QString &fields);
    /// open update file dialog
    void on_action_Update_file_triggered();
    /// open upload file dialog
    void on_action_Upload_File_triggered();

private:
    ///
    void loadUserIcon(AccountWidgetActs *accountActs,const QUrl &url,QNetworkAccessManager *manager);
    /// request User about message and refresh the UI
    void updateAccountActs();
    /// GDriveService Account get About message
    void accountAbout();

    /// GDriveService upload(Create) file Simple method
    void fileSimpleUpload(const QString &filepath,const GDrive::FileCreateArgs &args);
    /// GDriveService upload(Create) file Multipart method
    void fileMultipartUpload(const QString &filepath,const GDrive::FileCreateArgs &args);
    /// GDriveService upload(Create) file Resumable method
    void fileResumableUpload(const QString &filepath,const GDrive::FileCreateArgs &args);
    /// GDriveService upload(Update) file Simple method
    void fileSimpleUpdate(const QString &filepath,const GDrive::FileUpdateArgs &args);
    /// GDriveService upload(Update) file Multipart method
    void fileMultipartUpdate(const QString &filepath,const GDrive::FileUpdateArgs &args);
    /// GDriveService upload(Update) file Resumable method
    void fileResumableUpdate(const QString &filepath,const GDrive::FileUpdateArgs &args);
    /// GDriveService Download(Get) file
    void fileDownload(const QString &downloadFilePath,const QString &fileId);

    /// write settings to ini file
    void writeSettings();
    /// read Settings refresh token.
    /// If refersh token exist,then set Refersh token and request access token
    /// If No refersh token doesnt exist, then do nothing
    void loadUserAccount(const QSettings &settings);
    /// claer Json model, used for UI update treeView
    void clearModel();
};
#endif // MAINWINDOW_H
