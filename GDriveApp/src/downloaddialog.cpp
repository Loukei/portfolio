#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include <QFileDialog>

// https://developers.google.com/drive/api/v3/reference/files/get#parameters.
const QString DownloadDialog::instructions_String =
         QStringLiteral("Use fileID to specify download file in Drive.\n"
                        "Set file name and path by click Browse Button.\n");

DownloadDialog::DownloadDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::DownloadDialog)
{
    ui->setupUi(this);
    // set default message to status label
    ui->label_Status->setText(instructions_String);
    ui->label_Status->setOpenExternalLinks(true); // able to click link
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

QString DownloadDialog::getDownloadFilePath() const
{
    return ui->lineEdit_DownloadPath->text();
}

void DownloadDialog::setDownloadFilePath(const QString &downloadPath)
{
    ui->lineEdit_DownloadPath->setText(downloadPath);
}

QString DownloadDialog::getFileId() const
{
    return ui->lineEdit_FileID->text();
}

void DownloadDialog::setFileId(const QString &fileID)
{
    ui->lineEdit_FileID->setText(fileID);
}

bool DownloadDialog::getAcknowledgeAbuse() const
{
    return ui->comboBox_acknowledgeAbuse->currentIndex();
}

bool DownloadDialog::getSupportsAllDrives() const
{
    return ui->comboBox_supportsAllDrives->currentIndex();
}

void DownloadDialog::on_pushButton_DownloadPath_clicked()
{
    const QString oldpath = ui->lineEdit_DownloadPath->text();
    QString save = QFileDialog::getSaveFileName(this,
                                                tr("Save file"),
                                                oldpath);
    if(!save.isEmpty()){
        ui->lineEdit_DownloadPath->setText(save);
    }
}
