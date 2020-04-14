#include "uploaddialog.h"
#include "ui_uploaddialog.h"
#include <QFileDialog>

UploadDialog::UploadDialog(QWidget *parent, const QString &caption, const QString &filePath) :
    QDialog(parent),
    ui(new Ui::UploadDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(caption);
    ui->lineEdit_FilePath->setText(filePath);
}

UploadDialog::~UploadDialog()
{
    delete ui;
}

QString UploadDialog::getFilePath() const
{
    return ui->lineEdit_FilePath->text();
}

int UploadDialog::getUploadType() const
{
    return ui->comboBox_uploadType->currentIndex();
}

QString UploadDialog::getUploadTypeStr() const
{
    return ui->comboBox_uploadType->currentText();
}

void UploadDialog::on_pushButton_FilePath_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    tr("Open file"),
                                                    "/home");
    if(!filepath.isEmpty()){
        ui->lineEdit_FilePath->setText(filepath);
    }
}

bool UploadDialog::getUseContentAsIndexableText() const
{
    return ui->comboBox_useContentAsIndexableText->currentIndex();
}

void UploadDialog::setUseContentAsIndexableText(bool useContentAsIndexableText)
{
    ui->comboBox_useContentAsIndexableText->setCurrentIndex(useContentAsIndexableText);
}

QString UploadDialog::getOcrLanguage() const
{
    return ui->lineEdit_ocrLanguage->text();
}

void UploadDialog::setOcrLanguage(const QString &ocrLanguage)
{
    ui->lineEdit_ocrLanguage->setText(ocrLanguage);
}

bool UploadDialog::getKeepRevisionForever() const
{
    return ui->comboBox_keepRevisionForever->currentIndex();
}

void UploadDialog::setKeepRevisionForever(bool keepRevisionForever)
{
    ui->comboBox_keepRevisionForever->setCurrentIndex(keepRevisionForever);
}

bool UploadDialog::getIgnoreDefaultVisibility() const
{
    return ui->comboBox_ignoreDefaultVisibility->currentIndex();
}

void UploadDialog::setIgnoreDefaultVisibility(bool ignoreDefaultVisibility)
{
    ui->comboBox_ignoreDefaultVisibility->setCurrentIndex(ignoreDefaultVisibility);
}

bool UploadDialog::getEnforceSingleParent() const
{
    return ui->comboBox_enforceSingleParent->currentIndex();
}

void UploadDialog::setEnforceSingleParent(bool enforceSingleParent)
{
    ui->comboBox_enforceSingleParent->setCurrentIndex(enforceSingleParent);
}
