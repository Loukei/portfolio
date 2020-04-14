#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GDriveLib/googledriveservice.h"

namespace Ui {
class MainWindow;
}

class SearchDialog;
class DownloadDialog;
class FileMataDataDialog;
class UpdateDialog;
class QJsonModel;
QT_BEGIN_NAMESPACE
class QSettings;
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
    /// Download Dialog
    DownloadDialog *m_dialogDownload;
    /// A dialog to provide search drive files
    SearchDialog *m_dialogSearch;
    /// A dialog to Query File matadata
    FileMataDataDialog *m_dialogFileMataData;
    /// A dialog to update file
    UpdateDialog *m_dialogUpdate;
    /// Save recent upload file
    QString m_currentUploadFilePath;
    /// Save settings
    QSettings *m_settings;
    /// save m_Drive Operation reply
    QJsonModel *m_model;
    /// Google Drive Api serviece
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
    /// Click menu Download file
    void on_actionDownload_file_triggered();
    /// Click menu Simple Upload
    void on_actionSimple_Upload_triggered();
    /// Click menu Multipart Upload
    void on_actionMultipart_Upload_triggered();
    /// Click menu Resumable Upload
    void on_actionResumable_Upload_triggered();
    /// Click menu Search file&folder
    void on_action_Search_file_folder_triggered();
    /// Receive SearchDialog::query, then return search result to m_DialogSearch
    void onSearchDialog_query(const QString &q,
                              const QString &pageToken);
    /// Click menu Get file matadata
    void on_actionGet_file_matadata_triggered();
    /// Receive FileMataDataDialog::query, then return GDriveFileGet to m_dialogFileMataData
    void onFileMataDataDialog_query(const QString &fileID,
                                    const QString &fields);

    void on_actionUpdate_file_triggered();

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
    /// GDriveService upload(Update) file Simple method
    void fileSimpleUpdate(const QString &filepath,const QString &fileID, const QString &addParents, bool enforceSingleParent, bool keepRevisionForever, const QString &ocrLanguage, const QString &removeParents, bool useContentAsIndexableText);

    void fileSimpleUpdate(const QString &filepath,const GDrive::FileUpdateArgs &args);
    /// GDriveService upload(Update) file Multipart method
    void fileMultipartUpdate(const QString &filepath,const QString &fileID, const QString &addParents, bool enforceSingleParent, bool keepRevisionForever, const QString &ocrLanguage, const QString &removeParents, bool useContentAsIndexableText);

    void fileMultipartUpdate(const QString &filepath,const GDrive::FileUpdateArgs &args);
    /// GDriveService upload(Update) file Resumable method
    void fileResumableUpdate(const QString &filepath,const QString &fileID, const QString &addParents, bool enforceSingleParent, bool keepRevisionForever, const QString &ocrLanguage, const QString &removeParents, bool useContentAsIndexableText);

    void fileResumableUpdate(const QString &filepath,const GDrive::FileUpdateArgs &args);
    /// GDriveService Download(Get) file
    void fileDownload(const QString &downloadFilePath,const QString &fileId);
    /// write settings to ini file
    void writeSettings();
    /// take Google Drive Api response show on Treeview
    void updateModel(const QByteArray &json);
    /// claer Json model, used for UI update treeView
    void clearModel();
};

#endif // MAINWINDOW_H
