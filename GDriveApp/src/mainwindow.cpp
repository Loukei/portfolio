#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "accountwidgetacts.h"
#include "uploaddialog.h"
#include "downloaddialog.h"
#include "searchdialog.h"
#include "filematadatadialog.h"
#include "updatedialog.h"

#include "GDriveLib/googledriveservice.h"
#include "QJsonModel/qjsonmodel.h"
#include "networkimgloader.h" // load image from url
#include "mainwindow_settings.h" // namespace Settings
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
    //! Read Settings
    m_settings = new QSettings(QStringLiteral("GDriveApp_Settings.ini"),QSettings::IniFormat,this);
    m_settings->setIniCodec("UTF-8");
    this->setGeometry(Settings::MainWindow_Geometry(m_settings));
    //! Initialize Account widget actions
    m_accountActs->setupUi(ui->menu_Acount);
    ui->menu_Acount->insertActions(ui->action_Login,m_accountActs->actions());
    //! Setup upload dialog
    m_dialogUpload = new UploadDialog(this,
                                      tr("Upload File"),
                                      Settings::Upload_FilePath(m_settings));
    //! Setup download dialog
    m_dialogDownload = new DownloadDialog(this,
                                          tr("Download File"),
                                          Settings::Download_FilePath(m_settings),
                                          Settings::Download_FileID(m_settings));
    //! Setup search dialog
    m_dialogSearch = new SearchDialog(this);
    connect(m_dialogSearch,&SearchDialog::query,
            this,&MainWindow::onSearchDialog_query);
    //! Setup File::Get dialog
    m_dialogFileMataData = new FileMataDataDialog(this,
                                                  Settings::FileGet_FileID(m_settings),
                                                  Settings::FileGet_Fields(m_settings));
    connect(m_dialogFileMataData,&FileMataDataDialog::query,
            this,&MainWindow::onFileMataDataDialog_query);
    //! Setup file update dialog
    m_dialogUpdate = new UpdateDialog(this,
                                      Settings::Update_FileID(m_settings),
                                      Settings::Update_FilePath(m_settings));
    //! Setup Reply TreeView and model
    m_model = new QJsonModel(this);
    ui->treeView_Reply->setModel(m_model);
    //! Create Google Drive Serviece instance
    m_Drive = new GDriveService(this);
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

void MainWindow::updateAccountActs()
{
    GDriveAbout* userAbout = m_Drive->getAbout(QStringLiteral("user"));
    auto onAboutFinished = [userAbout,this](){
        if(userAbout->isComplete() && !userAbout->isFailed()){
            GDriveAboutResource resource = userAbout->getResource();
            m_accountActs->setUserName(resource.user_displayName());
            m_accountActs->setEmailAddress(resource.user_emailAddress());
            m_accountActs->action_UserName->setVisible(true);
            m_accountActs->action_UserEmail->setVisible(true);
            loadUserIcon(m_accountActs,resource.user_photoLink(),m_Drive->networkAccessManager());
        }else {
            m_accountActs->resetUi();
            ui->plainTextEdit->appendPlainText("About message Error.\n");
        }
        m_model->loadJson(userAbout->getReplyString());
        userAbout->deleteLater();
    };
    connect(userAbout,&GDriveAbout::finished,
            this,onAboutFinished);
}

void MainWindow::accountAbout()
{
    GDriveAbout* userAbout = m_Drive->getAbout(GDriveAbout::DISPLAYNAME|GDriveAbout::EMAILADDRESS);
    auto onAboutFinished = [userAbout,this](){
        if((!userAbout->isComplete()) | userAbout->isFailed()){
            ui->plainTextEdit->appendPlainText("About message Error.\n");
        }
        m_model->loadJson(userAbout->getReplyString());
        userAbout->deleteLater();
    };
    connect(userAbout,&GDriveAbout::finished,
            this,onAboutFinished);
}

void MainWindow::fileSimpleUpload(const QString &filepath, const FileCreateArgs &args)
{
    auto task = m_Drive->fileSimpleCreate(filepath,args);
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){ //! 上傳任務成功必須(1)完成且(2)沒有失敗
            ui->plainTextEdit->appendPlainText(filepath + " Simple Upload Success.\n");
        }else {
            ui->plainTextEdit->appendPlainText(filepath + " Simple Upload error:" + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileTask::finished,
            this,onUploadreceive);
}

void MainWindow::fileMultipartUpload(const QString &filepath, const FileCreateArgs &args)
{
    auto task = m_Drive->fileMultipartCreate(filepath,args);
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            ui->plainTextEdit->appendPlainText(filepath + " Multipart Upload Success.\n");
        }else {
            ui->plainTextEdit->appendPlainText(filepath + " Upload error:" + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileTask::finished,
            this,onUploadreceive);
}

void MainWindow::fileResumableUpload(const QString &filepath, const FileCreateArgs &args)
{
    auto task = m_Drive->fileResumableCreate(filepath,args);
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            ui->plainTextEdit->appendPlainText(filepath + " Resumable Upload Success.\n");
        }else {
            ui->plainTextEdit->appendPlainText(filepath + " Upload error:" + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileTask::finished,
            this,onUploadreceive);
}

void MainWindow::fileSimpleUpdate(const QString &filepath, const FileUpdateArgs &args)
{
    auto task = m_Drive->fileSimpleUpdate(filepath,args);
    auto onUpdatereceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            ui->plainTextEdit->appendPlainText(filepath + " Simple Update Success.\n");
        }else {
            ui->plainTextEdit->appendPlainText(filepath + " Update error:" + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileTask::finished,
            this,onUpdatereceive);
}

void MainWindow::fileMultipartUpdate(const QString &filepath, const FileUpdateArgs &args)
{
    auto task = m_Drive->fileMultipartUpdate(filepath,args);
    auto onUpdatereceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            ui->plainTextEdit->appendPlainText(filepath + " Multipart Update Success.\n");
        }else {
            ui->plainTextEdit->appendPlainText(filepath + " Multipart Update error:" + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileTask::finished,
            this,onUpdatereceive);
}

void MainWindow::fileResumableUpdate(const QString &filepath, const FileUpdateArgs &args)
{
    auto task = m_Drive->fileResumableUpdate(filepath,args);
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            ui->plainTextEdit->appendPlainText(filepath + " Resumable Update Success.\n");
        }else {
            ui->plainTextEdit->appendPlainText(filepath + " Resumable Update error:" + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    };
    connect(task,&GDriveFileTask::finished,
            this,onUploadreceive);
}

void MainWindow::fileDownload(const QString &downloadFilePath, const QString &fileId)
{
    QSharedPointer<QFile> writeFile(new QFile(downloadFilePath,this),&QFile::deleteLater);
    auto task = m_Drive->fileDownload(fileId,"id,name",writeFile);
    connect(task,&GDriveFileTask::finished,
            this,[task,this,downloadFilePath](){
        if(task->isComplete() && !task->isFailed()){
            ui->plainTextEdit->appendPlainText(downloadFilePath + " Download complete.\n");
        }else {
            ui->plainTextEdit->appendPlainText(downloadFilePath + " Download error:" + task->errorString());
        }
        m_model->loadJson(task->getReplyString());
        task->deleteLater();
    });
}

void MainWindow::writeSettings()
{
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
        m_settings->setValue(Settings::key_OAuth_RefreshToken,m_Drive->refreshToken());
    }else {
        m_settings->remove(Settings::key_OAuth_UserEmail);
        m_settings->remove(Settings::key_OAuth_RefreshToken);
    }
}

void MainWindow::loadUserAccount(const QSettings &settings)
{
    qDebug() << Q_FUNC_INFO; // "Status: NotAuthenticated"
    const QString refreshToken = Settings::OAuth_RefreshToken(&settings);
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

void MainWindow::on_action_Login_triggered()
{
    qInfo() << Q_FUNC_INFO;
    m_Drive->grant();
    this->statusBar()->showMessage(tr("Login from browser..."));
}

void MainWindow::onGDrive_granted()
{
    qDebug() << Q_FUNC_INFO;
    // request user info
    const QString info = QString("Token: %1\nRefresh Token: %2\n")
            .arg(m_Drive->token()).arg(m_Drive->refreshToken());
    ui->plainTextEdit->appendPlainText(info);
    this->statusBar()->showMessage(tr("Access Token received."));
}

void MainWindow::on_action_Logout_triggered()
{
    qInfo() << Q_FUNC_INFO;
    m_Drive->logout();
    clearModel();
    /*Set ui->menu_Acount to logout*/
    m_accountActs->resetUi();
    ui->action_Login->setEnabled(true);
    ui->action_Login->setVisible(true);
    ui->action_Logout->setVisible(false);
    ui->action_Remember_me->setVisible(false);
    ui->action_Remember_me->setChecked(false);
    ui->action_Refresh_Token->setVisible(false);
    /*Close all OAuth actions*/
    ui->menu_About->setEnabled(false);
    ui->menu_File->setEnabled(false);
    this->statusBar()->showMessage(tr("Account logout."));
}

void MainWindow::on_action_Refresh_Token_triggered()
{
    /// refresh token menu is only enable when user has login, no need to update UI
    qDebug() << Q_FUNC_INFO;
    m_Drive->refreshAccessToken();
    this->statusBar()->showMessage(tr("Refresh token..."));
}

void MainWindow::onGDrive_tokenChanged(const QString &token)
{
    /// m_currentOAuthToken = {}, token = "..." => login
    /// m_currentOAuthToken = "a...", token = "b..." => refresh token or switch account
    /// m_currentOAuthToken = "...", token = {} => logout
    qDebug() << Q_FUNC_INFO;
    if(token.isEmpty()){
        qDebug() << "token isEmpty -> logout";
    }else if (m_currentOAuthToken.isEmpty()) {
        qDebug() << "m_currentOAuthToken isEmpty() && token !isEmpty() -> login";
        updateAccountActs();
        ui->action_Login->setVisible(false);
        ui->action_Logout->setVisible(true);
        ui->action_Refresh_Token->setVisible(true);
        ui->action_Remember_me->setVisible(true);
        ui->action_Remember_me->setChecked(true);
        ui->menu_About->setEnabled(true);
        ui->menu_File->setEnabled(true);
    }else if (token != m_currentOAuthToken) {
        qDebug() << "token != m_currentOAuthToken -> refresh or switch account";
    }
    m_currentOAuthToken = token;
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
    accountAbout();
}

void MainWindow::on_action_Download_file_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogDownload->exec() == QDialog::Accepted){
        clearModel();
        fileDownload(m_dialogDownload->getDownloadFilePath(),
                     m_dialogDownload->getFileId());
    }else { //! QDialog::Rejected
        ui->plainTextEdit->appendPlainText("Download cancled.\n");
    }
}

void MainWindow::on_action_Search_file_folder_triggered()
{
    qInfo() << Q_FUNC_INFO;
    m_dialogSearch->exec();
}

void MainWindow::onSearchDialog_query(const QString &corpora,
                                      const QString &driveId,
                                      const QString &fields,
                                      const QString &orderBy,
                                      int pageSize,
                                      const QString &pageToken,
                                      const QString &q,
                                      const QString &spaces)
{
    qInfo() << Q_FUNC_INFO;
    const GDrive::FileListArgs args = {corpora,driveId,fields,orderBy,pageSize,pageToken,q,spaces};
    auto task = m_Drive->fileList(args);
    connect(task,&GDriveFileSearch::finished,
            m_dialogSearch,&SearchDialog::onFileSearch_finished);
}

void MainWindow::on_action_Get_file_matadata_triggered()
{
    qInfo() << Q_FUNC_INFO;
    m_dialogFileMataData->exec();
}

void MainWindow::onFileMataDataDialog_query(const QString &fileID, const QString &fields)
{
    qInfo() << Q_FUNC_INFO;
    GDrive::FileGetArgs args = {fileID,false,fields};
    auto task = m_Drive->fileGet(args);
    connect(task,&GDriveFileGet::finished,
            m_dialogFileMataData,&FileMataDataDialog::onFileGet_finished);
}

void MainWindow::on_action_Update_file_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogUpdate->exec() == QDialog::Accepted){
        clearModel();
        //! upload type in args doesnt infected here
        const GDrive::FileUpdateArgs args = {m_dialogUpdate->getFileID(),
                                             GDrive::UrlArgs::UploadType::MEDIA,
                                             m_dialogUpdate->getAddParents(),
                                             m_dialogUpdate->getEnforceSingleParent(),
                                             m_dialogUpdate->getKeepRevisionForever(),
                                             m_dialogUpdate->getOcrLanguage(),
                                             m_dialogUpdate->getRemoveParents(),
                                             m_dialogUpdate->getUseContentAsIndexableText()};
        int uploadtype = m_dialogUpdate->getUploadType();
        if(uploadtype == 0){
            fileSimpleUpdate(m_dialogUpdate->getFilePath(),args);
        }else if (uploadtype ==1) {
            fileMultipartUpdate(m_dialogUpdate->getFilePath(),args);
        }else if (uploadtype == 2) {
            fileResumableUpdate(m_dialogUpdate->getFilePath(),args);
        }
    }else {
        ui->plainTextEdit->appendPlainText("Update cancled.\n");
    }
}

void MainWindow::on_action_Upload_File_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogUpload->exec() == QDialog::Accepted){
        clearModel();

        GDrive::FileCreateArgs args = { GDrive::UrlArgs::UploadType::MEDIA,
                                      m_dialogUpload->getEnforceSingleParent(),
                                      m_dialogUpload->getIgnoreDefaultVisibility(),
                                      m_dialogUpload->getKeepRevisionForever(),
                                      m_dialogUpload->getOcrLanguage(),
                                      m_dialogUpload->getUseContentAsIndexableText()};
        int uploadtype = m_dialogUpload->getUploadType();
        if(uploadtype == 0){
            fileSimpleUpload(m_dialogUpload->getFilePath(),args);
        }else if (uploadtype == 1) {
            fileMultipartUpload(m_dialogUpload->getFilePath(),args);
        }else if (uploadtype == 2) {
            fileResumableUpload(m_dialogUpload->getFilePath(),args);
        }
    }else {
        ui->plainTextEdit->appendPlainText("Upload cancled.\n");
    }
}

void MainWindow::loadUserIcon(AccountWidgetActs *accountActs, const QUrl &url, QNetworkAccessManager *manager)
{
    NetworkImgLoader *loader = new NetworkImgLoader(url,manager);
    connect(loader,&NetworkImgLoader::dataReceived,
            this,[accountActs,loader](){
       accountActs->loadUserIconFromData(loader->data());
       loader->deleteLater();
    });
}
