#include "uploaddialog.h"
#include "ui_uploaddialog.h"
#include <QFileDialog>

UploadDialog::UploadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UploadDialog)
{
    ui->setupUi(this);
}

UploadDialog::~UploadDialog()
{
    delete ui;
}

QString UploadDialog::getFilePath() const
{
    return ui->lineEdit_FilePath->text();
}

void UploadDialog::setFilePath(const QString &filepath)
{
    ui->lineEdit_FilePath->setText(filepath);
}

int UploadDialog::getUploadType() const
{
    return ui->comboBox_uploadType->currentIndex();
}

void UploadDialog::on_pushButton_FilePath_clicked()
{
    QString oldpath = ui->lineEdit_FilePath->text();
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    tr("Open file"),
                                                    oldpath);
    if(!filepath.isEmpty()){
        ui->lineEdit_FilePath->setText(filepath);
    }
}

bool UploadDialog::getUseContentAsIndexableText() const
{
    return ui->comboBox_useContentAsIndexableText->currentIndex();
}

QString UploadDialog::getOcrLanguage() const
{
    return ui->lineEdit_ocrLanguage->text();
}

bool UploadDialog::getSupportsAllDrives() const
{
    return ui->comboBox_supportsAllDrives->currentIndex();
}

bool UploadDialog::getKeepRevisionForever() const
{
    return ui->comboBox_keepRevisionForever->currentIndex();
}

bool UploadDialog::getIgnoreDefaultVisibility() const
{
    return ui->comboBox_ignoreDefaultVisibility->currentIndex();
}

bool UploadDialog::getEnforceSingleParent() const
{
    return ui->comboBox_enforceSingleParent->currentIndex();
}
