#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloaddialog.h"
#include "searchdialog.h"
#include "filematadatadialog.h"
#include "updatedialog.h"
#include "GDriveLib/googledriveservice.h"
#include "QJsonModel/qjsonmodel.h"
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QFileDialog>

using namespace GDrive;

/*!
 * \namespace Settings
 * \brief Settings is used for collect all settings key and read settings
 */
namespace Settings {
const QString key_Geometry = "MainWindow/Geometry";
const QString key_Upload_FilePath = "MainWindow/DialogUpload/FilePath";
const QString key_Download_FilePath = "MainWindow/DialogDownload/FilePath";
const QString key_Download_FileID = "MainWindow/DialogDownload/FileID";
const QString key_FileGet_FileID = "MainWindow/DialogFileMataData/FileID";
const QString key_FileGet_Fields = "MainWindow/DialogFileMataData/Fields";
const QString key_Update_FilePath = "MainWindow/DialogUpdate/FilePath";
const QString key_Update_FileID = "MainWindow/DialogUpdate/FileID";

inline QRect MainWindow_Geometry(QSettings *settings)
{
    return settings->value(key_Geometry,QRect(0,0,630,495)).toRect();
}

inline QString Upload_FilePath(QSettings *settings)
{
    return settings->value(key_Upload_FilePath,"/home").toString();
}

inline QString Download_FilePath(QSettings *settings)
{
    return settings->value(key_Download_FilePath,"/home").toString();
}

inline QString Download_FileID(QSettings *settings)
{
    return settings->value(key_Download_FileID,"YOUR_FILE_ID").toString();
}

inline QString FileGet_FileID(QSettings *settings)
{
    return settings->value(key_FileGet_FileID,"YOUR_FILE_ID").toString();
}

inline QString FileGet_Fields(QSettings *settings)
{
    return settings->value(key_FileGet_Fields,"YOUR_FILE_ID").toString();
}

inline QString Update_FilePath(QSettings *settings)
{
    return settings->value(key_Update_FilePath,"/home").toString();
}

inline QString Update_FileID(QSettings *settings)
{
    return settings->value(key_Update_FileID,"YOUR_FILE_ID").toString();
}
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //! Read Settings
    m_settings = new QSettings("GDriveApp_Settings.ini",QSettings::IniFormat,this);
    m_settings->setIniCodec("UTF-8");
    this->setGeometry(Settings::MainWindow_Geometry(m_settings));
    m_currentUploadFilePath = Settings::Upload_FilePath(m_settings);
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
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::accountLogin()
{
    m_Drive->start();   //! see MainWindow::onGDrive_granted after granted emit
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

void MainWindow::fileSimpleUpload(const QString &filepath)
{
    auto task = m_Drive->fileSimpleCreate(filepath);
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

void MainWindow::fileMultipartUpload(const QString &filepath)
{
    auto task = m_Drive->fileMultipartCreate(filepath);
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

void MainWindow::fileResumableUpload(const QString &filepath)
{
    auto task = m_Drive->fileResumableCreate(filepath);
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

void MainWindow::fileSimpleUpdate(const QString &filepath, const QString &fileID)
{
    auto task = m_Drive->fileSimpleUpdate(filepath,fileID);
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            ui->plainTextEdit->appendPlainText(filepath + " Simple Update Success.\n");
        }else {
            ui->plainTextEdit->appendPlainText(filepath + " Update error:" + task->errorString());
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
    m_settings->setValue(Settings::key_Upload_FilePath,m_currentUploadFilePath);
    m_settings->setValue(Settings::key_FileGet_FileID,m_dialogFileMataData->getFileID());
    m_settings->setValue(Settings::key_FileGet_Fields,m_dialogFileMataData->getFields());
    m_settings->setValue(Settings::key_Update_FilePath,m_dialogUpdate->getFilePath());
    m_settings->setValue(Settings::key_Update_FileID,m_dialogUpdate->getFileID());
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
    ui->plainTextEdit->appendPlainText(m_Drive->receivedToken());
    //! Open other UI menu
    ui->actionAbout->setEnabled(true);
    ui->menuUpload_file->setEnabled(true);
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
    ui->menuUpload_file->setEnabled(false);
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

void MainWindow::on_actionSimple_Upload_triggered()
{
    qInfo() << Q_FUNC_INFO;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Upload File"),
                                                    m_currentUploadFilePath);
    if (!fileName.isEmpty()) {
        clearModel();
        m_currentUploadFilePath = fileName;
        fileSimpleUpload(fileName);
    }else {
        ui->plainTextEdit->appendPlainText("upload cancled.\n");
    }
}

void MainWindow::on_actionMultipart_Upload_triggered()
{
    qInfo() << Q_FUNC_INFO;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Upload File"),
                                                    m_currentUploadFilePath);
    if (!fileName.isEmpty()) {
        clearModel();
        m_currentUploadFilePath = fileName;
        fileMultipartUpload(fileName);
    }else {
        ui->plainTextEdit->appendPlainText("upload cancled.\n");
    }
}

void MainWindow::on_actionResumable_Upload_triggered()
{
    qInfo() << Q_FUNC_INFO;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Upload File"),
                                                    m_currentUploadFilePath);
    if (!fileName.isEmpty()) {
        clearModel();
        m_currentUploadFilePath = fileName;
        fileResumableUpload(fileName);
    }else {
        ui->plainTextEdit->appendPlainText("upload cancled.\n");
    }
}

void MainWindow::on_action_Search_file_folder_triggered()
{
    qInfo() << Q_FUNC_INFO;
    m_dialogSearch->exec();
}

void MainWindow::onSearchDialog_query(const QString &q, const QString &pageToken)
{
    qInfo() << Q_FUNC_INFO;
    auto task = m_Drive->fileList(q,pageToken);
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
    auto task = m_Drive->fileGet(fileID,fields);
    connect(task,&GDriveFileGet::finished,
            m_dialogFileMataData,&FileMataDataDialog::onFileGet_finished);
}

void MainWindow::on_actionUpdate_file_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogUpdate->exec() == QDialog::Accepted){
        clearModel();
        int uploadtype = m_dialogUpdate->getUploadType();
        if(uploadtype == 0){
            fileSimpleUpdate(m_dialogUpdate->getFilePath(),m_dialogUpdate->getFileID());
        }
    }else {
        ui->plainTextEdit->appendPlainText("Update cancled.\n");
    }
}
