#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uidownloaddialog.h"
#include "GDriveLib/googledriveservice.h"
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
    /// Read Settings
    m_settings = new QSettings("GDriveApp_Settings.ini",QSettings::IniFormat,this);
    m_settings->setIniCodec("UTF-8");
    this->setGeometry(readGeometry(m_settings));
    m_currentUploadFilePath = readUploadFilePath(m_settings);
    /// Setup download dialog
    downloadDialog = new UIDownloadDialog(this,
                                          tr("Download File"),
                                          readDownloadFilePath(m_settings),
                                          readDownloadFileID(m_settings));
    /// Create Google Drive Serviece instance
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
    auto userAbout = m_Drive->getAbout(GDriveAbout::DISPLAYNAME|GDriveAbout::EMAILADDRESS);
    connect(userAbout,&GDriveAbout::received,
            [userAbout,this](bool success){
        if(success){
            GDriveAboutResource resource = userAbout->getResource();
            ui->label->setText("Account: " + resource.displayName());
        }else {
            ui->label->setText("Unknown");
            ui->plainTextEdit->appendPlainText("About message Error.\n");
        }
        userAbout->deleteLater();
    });
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

QRect MainWindow::readGeometry(QSettings *settings)
{
    return settings->value("MainWindow/Geometry",QRect(0,0,630,495)).toRect();
}

QString MainWindow::readUploadFilePath(QSettings *settings)
{
    return settings->value("MainWindow/UploadFilePath","/home").toString();
}

QString MainWindow::readDownloadFilePath(QSettings *settings)
{
    return settings->value("MainWindow/DownloadFilePath","/home").toString();
}

QString MainWindow::readDownloadFileID(QSettings *settings)
{
    return settings->value("MainWindow/DownloadFileID","YOUR_FILE_ID").toString();
}

void MainWindow::writeSettings()
{
    m_settings->beginGroup("MainWindow");
    m_settings->setValue("Geometry",this->geometry());
    m_settings->setValue("DownloadFileID",downloadDialog->getFileId());
    m_settings->setValue("DownloadFilePath",downloadDialog->getDownloadFilePath());
    m_settings->setValue("UploadFilePath",m_currentUploadFilePath);
    m_settings->endGroup();
}

void MainWindow::on_actionLogin_account_triggered()
{
    qInfo() << "MainWindow::on_actionLogin_account_triggered()";
    accountLogin();
}

void MainWindow::onGDrive_granted()
{
    ui->plainTextEdit->appendPlainText(m_Drive->showInfo());
    //!Open other UI menu
    ui->actionAbout->setEnabled(true);
    ui->menuUpload_file->setEnabled(true);
    ui->actionDownload_file->setEnabled(true);
}

void MainWindow::on_action_Logout_Account_triggered()
{
    qInfo() << "MainWindow::on_action_Logout_Account_triggered()";
    accountLogout();
    ui->label->clear();
    ui->plainTextEdit->appendPlainText("Account logout.\n");
    ui->actionAbout->setEnabled(false);
    ui->menuUpload_file->setEnabled(false);
    ui->actionDownload_file->setEnabled(false);
}

void MainWindow::on_actionAbout_triggered()
{
    accountAbout();
}

void MainWindow::on_actionDownload_file_triggered()
{
    qInfo() << "MainWindow::on_actionDownload_file_triggered()";
    if(downloadDialog->exec() == QDialog::Accepted){
        fileDownload(downloadDialog->getDownloadFilePath(),
                     downloadDialog->getFileId());
    }else { //! QDialog::Rejected
        ui->plainTextEdit->appendPlainText("Download cancled.\n");
    }
}

void MainWindow::on_actionSimple_Upload_triggered()
{
    qInfo() << "MainWindow::on_actionSimple_Upload_triggered()";
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
    qInfo() << "MainWindow::on_actionMultipart_Upload_triggered()";
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
    qInfo() << "MainWindow::on_actionResumable_Upload_triggered()";
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
