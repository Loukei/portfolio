#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uploaddialog.h"
#include "downloaddialog.h"
#include "searchdialog.h"
#include "filematadatadialog.h"
#include "updatedialog.h"
#include "GDriveLib/googledriveservice.h"
#include "QJsonModel/qjsonmodel.h"
#include "mainwindow_settings.h" // namespace Settings
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QFileDialog>

using namespace GDrive;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //! Read Settings
    m_settings = new QSettings(QStringLiteral("GDriveApp_Settings.ini"),QSettings::IniFormat,this);
    m_settings->setIniCodec("UTF-8");
    this->setGeometry(Settings::MainWindow_Geometry(m_settings));
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
    connect(m_Drive,&GDriveService::granted,
            this,&MainWindow::onGDrive_granted);
    connect(m_Drive,&GDriveService::statusChanged,
            this,&MainWindow::onGDrive_statusChanged);
    //!
//    m_Drive->setToken(Settings::OAuth_CurrentToken(m_settings));
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::accountLogin()
{
    return m_Drive->grant();   //! see MainWindow::onGDrive_granted after granted emit
}

void MainWindow::accountLogout()
{
    m_Drive->logout();
}

void MainWindow::accountAbout()
{
    GDriveAbout* userAbout = m_Drive->getAbout(GDriveAbout::DISPLAYNAME|GDriveAbout::EMAILADDRESS);
    auto onAboutFinished = [userAbout,this](){
        if(userAbout->isComplete() && !userAbout->isFailed()){
            GDriveAboutResource resource = userAbout->getResource();
            ui->label_UserName->setText("Account: " + resource.user_displayName());
        }else {
            ui->label_UserName->setText("Unknown");
            ui->plainTextEdit->appendPlainText("About message Error.\n");
        }
        updateModel(userAbout->getReplyString());
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
        updateModel(task->getReplyString());
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
        updateModel(task->getReplyString());
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
        updateModel(task->getReplyString());
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
        updateModel(task->getReplyString());
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
        updateModel(task->getReplyString());
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
        updateModel(task->getReplyString());
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
        updateModel(task->getReplyString());
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
    m_settings->setValue(Settings::key_OAuth_CurrentToken,m_Drive->token());
}

void MainWindow::updateModel(const QByteArray &json)
{
    m_model->loadJson(json);
}

void MainWindow::clearModel()
{
    m_model->loadJson("{}");
}

void MainWindow::on_actionLogin_account_triggered()
{
    qInfo() << Q_FUNC_INFO;
    clearModel();
    accountLogin();
}

void MainWindow::onGDrive_granted()
{
    ui->plainTextEdit->appendPlainText(QStringLiteral("Receive Token: ") + m_Drive->token());
    //! Open other UI menu
    ui->actionAbout->setEnabled(true);
    ui->actionUpload_File->setEnabled(true);
    ui->actionUpdate_file->setEnabled(true);
    ui->actionDownload_file->setEnabled(true);
    ui->action_Search_file_folder->setEnabled(true);
    ui->actionGet_file_matadata->setEnabled(true);
}

void MainWindow::on_action_Logout_Account_triggered()
{
    qInfo() << Q_FUNC_INFO;
    clearModel();
    accountLogout();
    ui->label_UserName->clear();
    ui->plainTextEdit->appendPlainText("Account logout.\n");
    ui->actionAbout->setEnabled(false);
    ui->actionUpload_File->setEnabled(false);
    ui->actionUpdate_file->setEnabled(false);
    ui->actionDownload_file->setEnabled(false);
    ui->action_Search_file_folder->setEnabled(false);
    ui->actionGet_file_matadata->setEnabled(false);
}

void MainWindow::on_actionAbout_triggered()
{
    qInfo() << Q_FUNC_INFO;
    clearModel();
    accountAbout();
}

void MainWindow::on_actionDownload_file_triggered()
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

void MainWindow::on_actionGet_file_matadata_triggered()
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

void MainWindow::on_actionUpdate_file_triggered()
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

void MainWindow::on_actionUpload_File_triggered()
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

void MainWindow::onGDrive_statusChanged(QAbstractOAuth::Status status)
{
    QString info("Status: ");
    switch (status) {
    case QAbstractOAuth::Status::NotAuthenticated:
        info += "NotAuthenticated";
        break;
    case QAbstractOAuth::Status::TemporaryCredentialsReceived:
        info += "TemporaryCredentialsReceived";
        break;
    case QAbstractOAuth::Status::Granted:
        info += "Granted";
        break;
    case QAbstractOAuth::Status::RefreshingToken:
        info += "RefreshingToken";
        break;
    }
    qInfo() << Q_FUNC_INFO << info;
}
