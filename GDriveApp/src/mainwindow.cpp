#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloaddialog.h"
#include "searchdialog.h"
#include "filematadatadialog.h"
#include "GDriveLib/googledriveservice.h"
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QFileDialog>

using namespace GDrive;

namespace Settings {
inline QRect MainWindow_Geometry(QSettings *settings)
{
    return settings->value("MainWindow/Geometry",QRect(0,0,630,495)).toRect();
}

inline QString Upload_FilePath(QSettings *settings)
{
    return settings->value("MainWindow/DialogUpload/FilePath","/home").toString();
}

inline QString Download_FilePath(QSettings *settings)
{
    return settings->value("MainWindow/DialogDownload/FilePath","/home").toString();
}

inline QString Download_FileID(QSettings *settings)
{
    return settings->value("MainWindow/DialogDownload/FileID","YOUR_FILE_ID").toString();
}

inline QString FileGet_FileID(QSettings *settings)
{
    return settings->value("MainWindow/DialogFileMataData/FileID","YOUR_FILE_ID").toString();
}

inline QString FileGet_Fields(QSettings *settings)
{
    return settings->value("MainWindow/DialogFileMataData/Fields","YOUR_FILE_ID").toString();
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
            ui->label->setText("Account: " + resource.user_displayName());
        }else {
            ui->label->setText("Unknown");
            ui->plainTextEdit->appendPlainText("About message Error.\n");
        }
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
        task->deleteLater();
    });
}

void MainWindow::writeSettings()
{
    m_settings->beginGroup("MainWindow");
    m_settings->setValue("Geometry",this->geometry());
    m_settings->setValue("DialogDownload/FileID",m_dialogDownload->getFileId());
    m_settings->setValue("DialogDownload/FilePath",m_dialogDownload->getDownloadFilePath());
    m_settings->setValue("DialogUpload/FilePath",m_currentUploadFilePath);
    m_settings->setValue("DialogFileMataData/FileID",m_dialogFileMataData->getFileID());
    m_settings->setValue("DialogFileMataData/Fields",m_dialogFileMataData->getFields());
    m_settings->endGroup();
}

void MainWindow::on_actionLogin_account_triggered()
{
    qInfo() << Q_FUNC_INFO;
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
    accountLogout();
    ui->label->clear();
    ui->plainTextEdit->appendPlainText("Account logout.\n");
    ui->actionAbout->setEnabled(false);
    ui->menuUpload_file->setEnabled(false);
    ui->actionDownload_file->setEnabled(false);
    ui->action_Search_file_folder->setEnabled(false);
    ui->actionGet_file_matadata->setEnabled(false);
}

void MainWindow::on_actionAbout_triggered()
{
    accountAbout();
}

void MainWindow::on_actionDownload_file_triggered()
{
    qInfo() << Q_FUNC_INFO;
    if(m_dialogDownload->exec() == QDialog::Accepted){
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
