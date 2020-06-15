#include "filematadatadialog.h"
#include "ui_filematadatadialog.h"

FileMataDataDialog::FileMataDataDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::FileMataDataDialog)
{
    ui->setupUi(this);
}

FileMataDataDialog::~FileMataDataDialog()
{
    delete ui;
}

QString FileMataDataDialog::getFileID() const
{
    return ui->lineEdit_FileId->text();
}

void FileMataDataDialog::setFileID(const QString &fileID)
{
    ui->lineEdit_FileId->setText(fileID);
}

QString FileMataDataDialog::getFields() const
{
    return ui->lineEdit_Fileds->text();
}

void FileMataDataDialog::setFields(const QString &fields)
{
    ui->lineEdit_Fileds->setText(fields);
}

bool FileMataDataDialog::getSupportsAllDrives() const
{
    return ui->comboBox_supportsAllDrives->currentIndex();
}

void FileMataDataDialog::setSupportsAllDrives(bool supportsAllDrives)
{
    ui->comboBox_supportsAllDrives->setCurrentIndex(supportsAllDrives);
}
