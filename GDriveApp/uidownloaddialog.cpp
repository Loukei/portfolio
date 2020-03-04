#include "uidownloaddialog.h"
#include "ui_uidownloaddialog.h"
#include <QFileDialog>

UIDownloadDialog::UIDownloadDialog(QWidget *parent,
                                   const QString &downloadPath,
                                   const QString &fileID) :
    QDialog(parent),
    ui(new Ui::UIDownloadDialog)
{
    ui->setupUi(this);
    ui->lineEdit_DownloadPath->setText(downloadPath);
    ui->lineEdit_FileID->setText(fileID);
}

UIDownloadDialog::~UIDownloadDialog()
{
    delete ui;
}

QString UIDownloadDialog::getDownloadFilePath()
{
    return ui->lineEdit_DownloadPath->text();
}

QString UIDownloadDialog::getFileId()
{
    return ui->lineEdit_FileID->text();
}

void UIDownloadDialog::accept()
{
    emit downloadParam(ui->lineEdit_DownloadPath->text(),ui->lineEdit_FileID->text());
    QDialog::accept();
}

void UIDownloadDialog::on_pushButton_DownloadPath_clicked()
{
    QString save = QFileDialog::getSaveFileName(this,
                                                tr("Save file"),
                                                "/home");
    if(!save.isEmpty()){
        ui->lineEdit_DownloadPath->setText(save);
    }
}
