#include "gdrivefiledownloaddialog.h"
#include "ui_gdrivefiledownloaddialog.h"
#include <QFileDialog>

GDriveFileDownloadDialog::GDriveFileDownloadDialog(QWidget *parent,
                                                   const QString &downloadPath,
                                                   const QString &fileID) :
    QDialog(parent),
    ui(new Ui::GDriveFileDownloadDialog)
{
    ui->setupUi(this);
    ui->lineEdit_DownloadPath->setText(downloadPath);
    ui->lineEdit_FileID->setText(fileID);
}

GDriveFileDownloadDialog::~GDriveFileDownloadDialog()
{
    delete ui;
}

QString GDriveFileDownloadDialog::getDownloadFilePath()
{
    return ui->lineEdit_DownloadPath->text();
}

QString GDriveFileDownloadDialog::getFileId()
{
    return ui->lineEdit_FileID->text();
}

void GDriveFileDownloadDialog::accept()
{
    emit downloadParam(ui->lineEdit_DownloadPath->text(),ui->lineEdit_FileID->text());
    QDialog::accept();
}

void GDriveFileDownloadDialog::on_pushButton_DownloadPath_clicked()
{
    QString save = QFileDialog::getSaveFileName(this,
                                                tr("Save file"),
                                                "/home");
    if(!save.isEmpty()){
        ui->lineEdit_DownloadPath->setText(save);
    }
}
