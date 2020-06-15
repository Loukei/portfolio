#include "updatedialog.h"
#include "ui_updatedialog.h"
#include <QFileDialog>

UpdateDialog::UpdateDialog(QWidget *parent):
    QDialog(parent),
    ui(new Ui::UpdateDialog)
{
    ui->setupUi(this);
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

QString UpdateDialog::getFileID() const
{
    return ui->lineEdit_FileID->text();
}

void UpdateDialog::setFileID(const QString &fileID)
{
    ui->lineEdit_FileID->setText(fileID);
}

QString UpdateDialog::getFilePath() const
{
    return ui->lineEdit_FilePath->text();
}

void UpdateDialog::setFilePath(const QString &filePath)
{
    ui->lineEdit_FilePath->setText(filePath);
}

int UpdateDialog::getUploadType() const
{
    return ui->comboBox_uploadType->currentIndex();
}

QString UpdateDialog::getUploadTypeStr() const
{
    return ui->comboBox_uploadType->currentText();
}

QString UpdateDialog::getAddParents() const
{
    return ui->lineEdit_addParents->text();
}

bool UpdateDialog::getEnforceSingleParent() const
{
    return ui->comboBox_enforceSingleParent->currentIndex();
}

bool UpdateDialog::getKeepRevisionForever() const
{
    return ui->comboBox_keepRevisionForever->currentIndex();
}

QString UpdateDialog::getOcrLanguage() const
{
    return ui->lineEdit_ocrLanguage->text();
}

QString UpdateDialog::getRemoveParents() const
{
    return ui->lineEdit_removeParents->text();
}

bool UpdateDialog::getSupportsAllDrives() const
{
    return ui->comboBox_supportsAllDrives->currentIndex();
}

bool UpdateDialog::getUseContentAsIndexableText() const
{
    return ui->comboBox_useContentAsIndexableText->currentIndex();
}

void UpdateDialog::on_pushButton_FilePath_clicked()
{
    QString oldpath = ui->lineEdit_FilePath->text();
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    tr("Open file"),
                                                    oldpath);
    if(!filepath.isEmpty()){
        ui->lineEdit_FilePath->setText(filepath);
    }
}
