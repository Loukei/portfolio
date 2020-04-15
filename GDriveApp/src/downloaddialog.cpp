#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include <QFileDialog>

DownloadDialog::DownloadDialog(QWidget *parent,
                               const QString &caption,
                               const QString &downloadPath,
                               const QString &fileID) :
    QDialog(parent),
    ui(new Ui::DownloadDialog)
{
    ui->setupUi(this);
    setWindowTitle(caption);
    ui->lineEdit_DownloadPath->setText(downloadPath);
    ui->lineEdit_FileID->setText(fileID);
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

QString DownloadDialog::getDownloadFilePath() const
{
    return ui->lineEdit_DownloadPath->text();
}

QString DownloadDialog::getFileId() const
{
    return ui->lineEdit_FileID->text();
}

void DownloadDialog::accept()
{
    emit downloadParam(ui->lineEdit_DownloadPath->text(),ui->lineEdit_FileID->text());
    QDialog::accept();
}

void DownloadDialog::on_pushButton_DownloadPath_clicked()
{
    QString oldpath = ui->lineEdit_DownloadPath->text();
    QString save = QFileDialog::getSaveFileName(this,
                                                tr("Save file"),
                                                oldpath);
    if(!save.isEmpty()){
        ui->lineEdit_DownloadPath->setText(save);
    }
}
