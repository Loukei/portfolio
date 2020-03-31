#include "filematadatadialog.h"
#include "ui_filematadatadialog.h"
#include "QJsonModel/qjsonmodel.h"
#include "GDriveLib/googledriveservice.h"

FileMataDataDialog::FileMataDataDialog(QWidget *parent, const QString &fileID, const QString &fields) :
    QDialog(parent),
    ui(new Ui::FileMataDataDialog)
{
    ui->setupUi(this);
    m_model = new QJsonModel(this);
    ui->treeView->setModel(m_model);
    ui->lineEdit_FileId->setText(fileID);
    ui->lineEdit_Fileds->setText(fields);
}

FileMataDataDialog::~FileMataDataDialog()
{
    delete ui;
}

QString FileMataDataDialog::getFileID() const
{
    return ui->lineEdit_FileId->text();
}

QString FileMataDataDialog::getFields() const
{
    return ui->lineEdit_Fileds->text();
}

void FileMataDataDialog::onFileGet_finished()
{
    qInfo() << Q_FUNC_INFO;
    auto task = qobject_cast<GDrive::GDriveFileGet*>(sender());
    if(task->isComplete() && !task->isFailed()){
        //! update treeView to show message
        m_model->loadJson(task->getReplyString());
    }else {
        m_model->loadJson(task->getReplyString());
    }
    task->deleteLater();
}

void FileMataDataDialog::on_pushButton_Get_clicked()
{
    qInfo() << Q_FUNC_INFO;
    emit query(ui->lineEdit_FileId->text(),
               ui->lineEdit_Fileds->text());
}
