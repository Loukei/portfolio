#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "accountwidgetacts.h"
#include "uploaddialog.h"
#include "downloaddialog.h"
#include "searchdialog.h"
#include "filematadatadialog.h"
#include "updatedialog.h"
#include "networkprogressdialog.h" /* open dialog to show download/upload progress */

#include "Secret/oauthglobal.h" /* Oauth parameter */
#include "GDriveLib/googledriveservice.h"
#include "QJsonModel/qjsonmodel.h"
#include "mainwindow_settings.h" /* namespace Settings */
#include "Ecrypt/simplecrypt.h" /* ecrtpt token */
#include <QDebug>
#include <QFile>

using namespace GDrive;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_accountActs(new AccountWidgetActs)
{
    ui->setupUi(this);
    ui->mainToolBar->setVisible(false);
    // Read Settings
    m_settings = new QSettings(QStringLiteral("GDriveApp_Settings.ini"),QSettings::IniFormat,this);
    m_settings->setIniCodec("UTF-8");
    this->setGeometry(Settings::MainWindow_Geometry(m_settings));
    // Initialize Account widget actions
    m_accountActs->setupUi(ui->menu_Acount);
    ui->menu_Acount->insertActions(ui->action_Login,m_accountActs->actions());
    // Setup upload dialog
    m_dialogUpload = new UploadDialog(this);
    m_dialogUpload->setFilePath(Settings::Upload_FilePath(m_settings));
    // Setup download dialog
    m_dialogDownload = new DownloadDialog(this);
    m_dialogDownload->setDownloadFilePath(Settings::Download_FilePath(m_settings));
    m_dialogDownload->setFileId(Settings::Download_FileID(m_settings));
    // Setup search dialog
    m_dialogSearch = new SearchDialog(this);
    // Setup File::Get dialog
    m_dialogFileMataData = new FileMataDataDialog(this);
    m_dialogFileMataData->setFileID(Settings::FileGet_FileID(m_settings));
    m_dialogFileMataData->setFields(Settings::FileGet_Fields(m_settings));
    // Setup file update dialog
    m_dialogUpdate = new UpdateDialog(this);
    m_dialogUpdate->setFileID(Settings::Update_FileID(m_settings));
    m_dialogUpdate->setFilePath(Settings::Update_FilePath(m_settings));
    // Setup Reply TreeView and model
    m_model = new QJsonModel(this);
    ui->treeView_Reply->setModel(m_model);
    // Create Google Drive Serviece instance
    m_Drive = new GDriveService(OAuth::keyAuthUri(),
                                OAuth::keyTokenUri(),
                                OAuth::keyClientId(),
                                OAuth::keyClientSecert(),
                                OAuth::keyScope(),
                                OAuth::keyRedirectPort(),
                                this);
    loadUserAccount(*m_settings);
    connect(m_Drive,&GDriveService::granted,
            this,&MainWindow::onGDrive_granted);
    connect(m_Drive,&GDriveService::error,
            this,&MainWindow::onGDrive_error);
    connect(m_Drive,&GDriveService::tokenChanged,
            this,&MainWindow::onGDrive_tokenChanged);
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
    delete m_accountActs;
}

void MainWindow::setUiEnable(bool enable)
{
    if(enable){ // login
        updateAccountActs();
    }else {     // logout
        m_accountActs->resetUi();
    }
    // Set ui->menu_Acount, hide login menu if login
    ui->action_Login->setVisible(!enable);
    ui->action_Logout->setVisible(enable);
    ui->action_Remember_me->setVisible(enable);
    ui->action_Remember_me->setChecked(enable);
    ui->action_Refresh_Token->setVisible(enable);
    // OAuth action
    ui->menu_About->setEnabled(enable);
    ui->menu_File->setEnabled(enable);
}

void MainWindow::updateAccountActs()
{
    GDriveAbout* userAbout = m_Drive->about(QStringLiteral("user"));
    auto onAboutFinished = [userAbout,this](){
        if(userAbout->isComplete() && !userAbout->isFailed()){
            GDriveAboutResource resource(userAbout->getReplyString());
            m_accountActs->updateUi(resource.user_displayName(),
                                    resource.user_emailAddress(),
                                    resource.user_photoLink(),
                                    m_Drive->networkAccessManager());
        }else {
            m_accountActs->resetUi();
            consoleError(tr("About message Error."));
        }
        m_model->loadJson(userAbout->getReplyString());
        userAbout->deleteLater();
    };
    connect(userAbout,&GDriveAbout::finished,
            this,onAboutFinished);
}

void MainWindow::aboutGet()
{
    GDriveAbout* userAbout = m_Drive->about(QStringLiteral("user(displayName,photoLink,emailAddress)"));
    auto onAboutFinished = [userAbout,this](){
        if((!userAbout->isComplete()) | userAbout->isFailed()){
            consoleError(tr("About message Error."));
        }
        m_model->loadJson(userAbout->getReplyString());
        userAbout->deleteLater();
    };
    connect(userAbout,&GDriveAbout::finished,
            this,onAboutFinished);
}

void MainWindow::fileSimpleUpload(const QString &filepath, const QUrlQuery &args)
{
    auto task = m_Drive->fileSimpleCreate(filepath,args);
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){ // 上傳任務成功必須(1)完成且(2)沒有失敗
            consoleInfo(tr("%1 simple upload success.").arg(filepath));
        }else {
            consoleError(tr("%1 simple upload error:%2").arg(filepath,task->errorString()));
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileSimpleCreate::finished,
            this,onUploadreceive);
    if(task->start()){
        NetworkProgressDialog *dialog = new NetworkProgressDialog(tr("Uploading %1.").arg(filepath),this);
        connect(task,&GDriveFileSimpleCreate::uploadProgress,
                dialog,&NetworkProgressDialog::networkProgress);
        connect(task,&GDriveFileSimpleCreate::finished,
                dialog,&NetworkProgressDialog::closeAndBlockSignal);
        connect(dialog,&NetworkProgressDialog::canceled,
                task,&GDriveFileSimpleCreate::abort);
        dialog->show();
    }else {
        consoleInfo(task->errorString());
        task->deleteLater();
    }
}

void MainWindow::fileMultipartUpload(const QString &filepath, const QUrlQuery &args)
{
    auto task = m_Drive->fileMultipartCreate(filepath,args);
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            consoleInfo(tr("Multipart Upload success: %1.").arg(filepath));
        }else {
            consoleError(filepath + tr(" Upload error:") + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileMultipartCreate::finished,
            this,onUploadreceive);

    if(task->start()){
        NetworkProgressDialog *dialog = new NetworkProgressDialog(tr("Uploading %1.").arg(filepath),this);
        connect(task,&GDriveFileMultipartCreate::uploadProgress,
                dialog,&NetworkProgressDialog::networkProgress);
        connect(task,&GDriveFileMultipartCreate::finished,
                dialog,&NetworkProgressDialog::closeAndBlockSignal);
        connect(dialog,&NetworkProgressDialog::canceled,
                task,&GDriveFileMultipartCreate::abort);
        dialog->show();
    }else {
        consoleInfo(task->errorString());
        task->deleteLater();
    }
}

void MainWindow::fileResumableUpload(const QString &filepath, const QUrlQuery &args)
{
    auto task = m_Drive->fileResumableCreate(filepath,args);
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            consoleInfo(tr("Resumable Upload success: %1.").arg(filepath));
        }else {
            consoleError(filepath + tr(" Upload error:") + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileResumableCreate::finished,
            this,onUploadreceive);

    if(task->start()){
        NetworkProgressDialog *dialog = new NetworkProgressDialog(tr("Uploading %1.").arg(filepath),this);
        connect(task,&GDriveFileResumableCreate::uploadProgress,
                dialog,&NetworkProgressDialog::networkProgress);
        connect(task,&GDriveFileResumableCreate::finished,
                dialog,&NetworkProgressDialog::closeAndBlockSignal);
        connect(dialog,&NetworkProgressDialog::canceled,
                task,&GDriveFileResumableCreate::abort);
        dialog->show();
    }else {
        consoleInfo(task->errorString());
        task->deleteLater();
    }
}

void MainWindow::fileSimpleUpdate(const QString &filepath, const QString &fileID, const QUrlQuery &args)
{
    auto task = m_Drive->fileSimpleUpdate(filepath,fileID,args);
    auto onUpdatereceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            consoleInfo(tr("Simple Update success: %1.").arg(filepath));
        }else {
            consoleError(filepath + tr(" Update error:") + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileSimpleUpdate::finished,
            this,onUpdatereceive);

    if(task->start()){
        NetworkProgressDialog *dialog = new NetworkProgressDialog(tr("Update %1.").arg(filepath),this);
        connect(task,&GDriveFileSimpleUpdate::uploadProgress,
                dialog,&NetworkProgressDialog::networkProgress);
        connect(task,&GDriveFileSimpleUpdate::finished,
                dialog,&NetworkProgressDialog::closeAndBlockSignal);
        connect(dialog,&NetworkProgressDialog::canceled,
                task,&GDriveFileSimpleUpdate::abort);
        dialog->show();
    }else {
        consoleInfo(task->errorString());
        task->deleteLater();
    }
}

void MainWindow::fileMultipartUpdate(const QString &filepath,const QString &fileID,const QUrlQuery &args)
{
    auto task = m_Drive->fileMultipartUpdate(filepath,fileID,args);
    auto onUpdatereceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            consoleInfo(tr("Multipart Update success: %1.").arg(filepath));
        }else {
            consoleError(filepath + tr(" Multipart Update error:") + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileMultipartUpdate::finished,
            this,onUpdatereceive);

    if(task->start()){
        NetworkProgressDialog *dialog = new NetworkProgressDialog(tr("Update %1.").arg(filepath),this);
        connect(task,&GDriveFileMultipartUpdate::uploadProgress,
                dialog,&NetworkProgressDialog::networkProgress);
        connect(task,&GDriveFileMultipartUpdate::finished,
                dialog,&NetworkProgressDialog::closeAndBlockSignal);
        connect(dialog,&NetworkProgressDialog::canceled,
                task,&GDriveFileMultipartUpdate::abort);
        dialog->show();
    }else {
        consoleInfo(task->errorString());
        task->deleteLater();
    }
}

void MainWindow::fileResumableUpdate(const QString &filepath, const QString &fileID, const QUrlQuery &args)
{
    auto task = m_Drive->fileResumableUpdate(filepath,fileID,args);
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            consoleInfo(tr("Resumable Update success: %1.").arg(filepath));
        }else {
            consoleError(filepath + tr(" Resumable Update error:") + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileResumableUpdate::finished,
            this,onUploadreceive);

    if(task->start()){
        NetworkProgressDialog *dialog = new NetworkProgressDialog(tr("Update %1.").arg(filepath),this);
        connect(task,&GDriveFileResumableUpdate::uploadProgress,
                dialog,&NetworkProgressDialog::networkProgress);
        connect(task,&GDriveFileResumableUpdate::finished,
                dialog,&NetworkProgressDialog::closeAndBlockSignal);
        connect(dialog,&NetworkProgressDialog::canceled,
                task,&GDriveFileResumableUpdate::abort);
        dialog->show();
    }else {
        consoleInfo(task->errorString());
        task->deleteLater();
    }
}

void MainWindow::fileDownload(const QString &downloadFilePath, const QString &fileId)
{
    auto task = m_Drive->fileDownload(fileId,"id,name",downloadFilePath);
    if(task->start()){
        auto onDownloadFinish = [task,this,downloadFilePath](){
            if(task->isComplete() && !task->isFailed()){
                consoleInfo(tr("Download %1 .").arg(downloadFilePath));
            }else {
                consoleError(tr("Download %1 error: %2.").arg(downloadFilePath,task->errorString()));
                QFile::remove(downloadFilePath); // delete failed download on desk
            }
            task->deleteLater();
        };
        connect(task,&GDriveFileDownloader::finished,this,onDownloadFinish);
        NetworkProgressDialog *dialog = new NetworkProgressDialog(tr("Downloading %1.").arg(downloadFilePath),this);
        connect(task,&GDriveFileDownloader::downloadProgress,
                dialog,&NetworkProgressDialog::networkProgress);
        connect(task,&GDriveFileDownloader::finished,
                dialog,&NetworkProgressDialog::closeAndBlockSignal);
        connect(dialog,&NetworkProgressDialog::canceled,
                task,&GDriveFileDownloader::abort);
        dialog->show();
    }else {
        consoleInfo(task->errorString());
        task->deleteLater();
    }
}

void MainWindow::fileSearch(const QUrlQuery &args)
{
    auto task = m_Drive->fileList(args);
    auto onFileSearchFinished = [this,task](){
        if(task->isComplete() && !task->isFailed()){
            consoleInfo(tr("Search complete."));
        }else {
            consoleError(tr("Search error."));
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileSearch::finished,
            this,onFileSearchFinished);
}

void MainWindow::fileGetMatadata(const QString &fileId, const QUrlQuery &args)
{
    auto task = m_Drive->fileGet(fileId,args);
    auto onFileGetFinished = [task,this,fileId](){
        if(task->isComplete() && !task->isFailed()){
            consoleInfo(tr("Get file %1 matadata.").arg(fileId));
        }else {
            task->errorString();
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileGet::finished,
            this,onFileGetFinished);
}

void MainWindow::writeSettings()
{
    m_settings->setValue(Settings::key_AppVersion,QCoreApplication::applicationVersion());
    m_settings->setValue(Settings::key_Geometry,this->geometry());
    m_settings->setValue(Settings::key_Download_FileID,m_dialogDownload->getFileId());
    m_settings->setValue(Settings::key_Download_FilePath,m_dialogDownload->getDownloadFilePath());
    m_settings->setValue(Settings::key_Upload_FilePath,m_dialogUpload->getFilePath());
    m_settings->setValue(Settings::key_FileGet_FileID,m_dialogFileMataData->getFileID());
    m_settings->setValue(Settings::key_FileGet_Fields,m_dialogFileMataData->getFields());
    m_settings->setValue(Settings::key_Update_FilePath,m_dialogUpdate->getFilePath());
    m_settings->setValue(Settings::key_Update_FileID,m_dialogUpdate->getFileID());
    if(ui->action_Remember_me->isChecked()){
        m_settings->setValue(Settings::key_OAuth_UserEmail,m_accountActs->emailAddress());
        SimpleCrypt crypto(m_ecryptKey);
        m_settings->setValue(Settings::key_OAuth_RefreshToken,
                             crypto.encryptToString(m_Drive->refreshToken()));
    }else {
        m_settings->remove(Settings::key_OAuth_UserEmail);
        m_settings->remove(Settings::key_OAuth_RefreshToken);
    }
}

void MainWindow::loadUserAccount(const QSettings &settings)
{
    qDebug() << Q_FUNC_INFO; // "Status: NotAuthenticated"
    SimpleCrypt crypto(m_ecryptKey);
    const QString refreshToken = crypto.decryptToString(Settings::OAuth_RefreshToken(&settings));
    if(!refreshToken.isEmpty())
    {
        m_Drive->setRefreshToken(refreshToken);
        m_Drive->refreshAccessToken(); // use refresh token to login
        this->statusBar()->showMessage(tr("Login by previous Refresh Token..."));
    }
}

void MainWindow::clearModel()
{
    m_model->loadJson("{}");
}

void MainWindow::consoleError(const QString &errStr) const
{
    ui->plainTextEdit->appendPlainText(tr("[Error]") + errStr);
}

void MainWindow::consoleInfo(const QString &info) const
{
    ui->plainTextEdit->appendPlainText(tr("[Info]") + info);
}

void MainWindow::on_action_Login_triggered()
{
    m_Drive->grant();
    this->statusBar()->showMessage(tr("Login from browser..."));
}

void MainWindow::onGDrive_granted()
{
    qDebug() << Q_FUNC_INFO;
    const QString info = QString("Token: %1\nRefresh token: %2")
            .arg(m_Drive->token(),m_Drive->refreshToken());
    consoleInfo(info);
    this->statusBar()->showMessage(tr("Access Token received."));
}

void MainWindow::on_action_Logout_triggered()
{
    qInfo() << Q_FUNC_INFO;
    m_Drive->setToken("");
    m_Drive->setRefreshToken("");
    clearModel();
    setUiEnable(false);
    this->statusBar()->showMessage(tr("Account logout."));
}

void MainWindow::on_action_Refresh_Token_triggered()
{
    // refresh token menu is only enable when user has login, no need to update UI
    qDebug() << Q_FUNC_INFO;
    m_Drive->refreshAccessToken();
    this->statusBar()->showMessage(tr("Refresh token..."));
}

void MainWindow::onGDrive_tokenChanged(const QString &token)
{
    /* m_currentOAuthToken = {}, token = "..." => login
     * m_currentOAuthToken = "a...", token = "b..." => refresh token or switch account
     * m_currentOAuthToken = "...", token = {} => logout */
    if (m_currentOAuthToken.isEmpty() && !token.isEmpty()) {
        qDebug() << "m_currentOAuthToken isEmpty() && token !isEmpty() -> login";
        setUiEnable(true);
        this->statusBar()->showMessage(tr("Account login."));
    }
    m_currentOAuthToken = token; /*change current token*/
}

void MainWindow::onGDrive_error(const QString &error, const QString &errorDescription, const QUrl &uri)
{
    QVariantMap info;
    info.insert(QStringLiteral("Error"),error);
    info.insert(QStringLiteral("Description"),errorDescription);
    info.insert(QStringLiteral("Uri"),uri);
    qDebug() << Q_FUNC_INFO << info;
}

void MainWindow::on_action_About_User_triggered()
{
    qInfo() << Q_FUNC_INFO;
    clearModel();
    aboutGet();
}

void MainWindow::on_action_Download_file_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogDownload->exec() == QDialog::Accepted){
        clearModel();
        fileDownload(m_dialogDownload->getDownloadFilePath(),
                     m_dialogDownload->getFileId());
    }else { // QDialog::Rejected
        consoleInfo(tr("Download cancled."));
    }
}

void MainWindow::on_action_Search_file_folder_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogSearch->exec() == QDialog::Accepted){
        clearModel();
        const QUrlQuery args = GDriveFileSearch::buildUrlArgs(
                    m_dialogSearch->getCorpora(),
                    m_dialogSearch->getDriveId(),
                    m_dialogSearch->getFields(),
                    m_dialogSearch->getIncludeItemsFromAllDrives(),
                    m_dialogSearch->getOrderBy(),
                    m_dialogSearch->getPageSize(),
                    m_dialogSearch->getPageToken(),
                    m_dialogSearch->getQ(),
                    m_dialogSearch->getSpaces(),
                    m_dialogSearch->getSupportsAllDrives());
        fileSearch(args);
    }else {
        consoleInfo(tr("Search cancled."));
    }
}

void MainWindow::on_action_Get_file_matadata_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogFileMataData->exec() == QDialog::Accepted){
        clearModel();
        const QUrlQuery args = GDriveFileGet::buildUrlArgs(
                    m_dialogFileMataData->getFields(),
                    m_dialogFileMataData->getSupportsAllDrives());
        fileGetMatadata(m_dialogFileMataData->getFileID(),args);
    }else {
        consoleInfo(tr("Get matadata cancled."));
    }
}

void MainWindow::on_action_Update_file_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogUpdate->exec() == QDialog::Accepted){
        clearModel();
        // upload type in args doesnt infected here
        const QUrlQuery args = GDriveFileSimpleUpdate::buildUrlArgs(
                    m_dialogUpdate->getAddParents(),
                    m_dialogUpdate->getEnforceSingleParent(),
                    m_dialogUpdate->getKeepRevisionForever(),
                    m_dialogUpdate->getOcrLanguage(),
                    m_dialogUpdate->getRemoveParents(),
                    m_dialogUpdate->getSupportsAllDrives(),
                    m_dialogUpdate->getUseContentAsIndexableText());
        int uploadtype = m_dialogUpdate->getUploadType();
        if(uploadtype == 0){
            fileSimpleUpdate(m_dialogUpdate->getFilePath(),m_dialogUpdate->getFileID(),args);
        }else if (uploadtype ==1) {
            fileMultipartUpdate(m_dialogUpdate->getFilePath(),m_dialogUpdate->getFileID(),args);
        }else if (uploadtype == 2) {
            fileResumableUpdate(m_dialogUpdate->getFilePath(),m_dialogUpdate->getFileID(),args);
        }
    }else {
        consoleInfo(tr("Update cancled."));
    }
}

void MainWindow::on_action_Upload_File_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogUpload->exec() == QDialog::Accepted){
        clearModel();
        const QUrlQuery args = GDriveFileSimpleCreate::buildUrlArgs(
                    m_dialogUpload->getEnforceSingleParent(),
                    m_dialogUpload->getIgnoreDefaultVisibility(),
                    m_dialogUpload->getKeepRevisionForever(),
                    m_dialogUpload->getOcrLanguage(),
                    m_dialogUpload->getSupportsAllDrives(),
                    m_dialogUpload->getUseContentAsIndexableText());

        int uploadtype = m_dialogUpload->getUploadType();
        if(uploadtype == 0){
            fileSimpleUpload(m_dialogUpload->getFilePath(),args);
        }else if (uploadtype == 1) {
            fileMultipartUpload(m_dialogUpload->getFilePath(),args);
        }else if (uploadtype == 2) {
            fileResumableUpload(m_dialogUpload->getFilePath(),args);
        }
    }else {
        consoleInfo(tr("Upload cancled."));
    }
}
