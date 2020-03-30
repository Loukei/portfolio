#include "filematadatadialog.h"
#include "ui_filematadatadialog.h"
#include "QJsonModel/qjsonmodel.h"
#include "GDriveLib/googledriveservice.h"

FileMataDataDialog::FileMataDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileMataDataDialog)
{
    ui->setupUi(this);
    m_model = new QJsonModel(this);
    ui->treeView->setModel(m_model);
}

FileMataDataDialog::~FileMataDataDialog()
{
    delete ui;
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
