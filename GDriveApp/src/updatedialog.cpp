#include "updatedialog.h"
#include "ui_updatedialog.h"
#include <QFileDialog>

UpdateDialog::UpdateDialog(QWidget *parent,
                           const QString &fileID,
                           const QString &filePath):
    QDialog(parent),
    ui(new Ui::UpdateDialog)
{
    ui->setupUi(this);
    ui->lineEdit_FileID->setText(fileID);
    ui->lineEdit_FilePath->setText(filePath);
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

QString UpdateDialog::getFileID() const
{
    return ui->lineEdit_FileID->text();
}

QString UpdateDialog::getFilePath() const
{
    return ui->lineEdit_FilePath->text();
}

int UpdateDialog::getUploadType() const
{
    return ui->comboBox_uploadType->currentIndex();
}

QString UpdateDialog::getUploadTypeStr() const
{
    return ui->comboBox_uploadType->currentText();
}

void UpdateDialog::setAddParents(const QString &addParents)
{
    ui->lineEdit_addParents->setText(addParents);
}

QString UpdateDialog::getAddParents() const
{
    return ui->lineEdit_addParents->text();
}

void UpdateDialog::setEnforceSingleParent(bool enforceSingleParent)
{
    //! Turn input value to set CurrentIndex
    //! true -> 1, false ->0
    ui->comboBox_enforceSingleParent->setCurrentIndex(enforceSingleParent);
}

bool UpdateDialog::getEnforceSingleParent() const
{
    return ui->comboBox_enforceSingleParent->currentIndex();
}

void UpdateDialog::setKeepRevisionForever(bool keepRevisionForever)
{
    ui->comboBox_keepRevisionForever->setCurrentIndex(keepRevisionForever);
}

bool UpdateDialog::getKeepRevisionForever() const
{
    return ui->comboBox_keepRevisionForever->currentIndex();
}

void UpdateDialog::setOcrLanguage(const QString &ocrLanguage)
{
    ui->lineEdit_ocrLanguage->setText(ocrLanguage);
}

void UpdateDialog::setRemoveParents(const QString &removeParents)
{
    ui->lineEdit_removeParents->setText(removeParents);
}

QString UpdateDialog::getRemoveParents() const
{
    return ui->lineEdit_removeParents->text();
}

void UpdateDialog::setUseContentAsIndexableText(bool useContentAsIndexableText)
{
    ui->comboBox_useContentAsIndexableText->setCurrentIndex(useContentAsIndexableText);
}

bool UpdateDialog::getUseContentAsIndexableText() const
{
    return ui->comboBox_useContentAsIndexableText->currentIndex();
}

void UpdateDialog::on_pushButton_FilePath_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    tr("Open file"),
                                                    "/home");
    if(!filepath.isEmpty()){
        ui->lineEdit_FilePath->setText(filepath);
    }
}

QString UpdateDialog::formBool(bool value)
{
    if(value)   return "True";
    else        return "False";
}

bool UpdateDialog::formString(const QString &value)
{
    if( (value == "True") | (value == "true") ){
        return true;
    }else if ( (value == "False") | (value == "false") ) {
        return false;
    }else {
        return false;
    }
}
