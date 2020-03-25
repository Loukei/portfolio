#include "searchdialog.h"
#include "ui_searchdialog.h"
#include "GDriveLib/googledriveservice.h"
#include "QJsonModel/qjsonmodel.h"

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
    m_model = new QJsonModel(this);
    ui->treeView->setModel(m_model);
    m_model->loadJson("{ \"kind\": \"drive#fileList\","
                      " \"incompleteSearch\": false,"
                      " \"files\": [  "
                      "{   "
                      "\"kind\": \"drive#file\","
                      "   \"id\": \"EXAMPLE_FILE_ID\","
                      "   \"name\": \"Untitled\","
                      "   \"mimeType\": \"text/plain\"  "
                      "} "
                      "]"
                      "}");
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::onFileSearch_finished()
{
    qInfo() << Q_FUNC_INFO;
    auto task = qobject_cast<GDrive::GDriveFileSearch*>(sender());
    if(task->isComplete() && !task->isFailed()){
        //! update treeView to show message
        m_model->loadJson(task->getReplyString());
    }else {
        m_model->loadJson(task->getReplyString());
    }
    task->deleteLater();
}


void SearchDialog::on_pushButton_search_clicked()
{
    qInfo() << Q_FUNC_INFO;
    emit query(ui->lineEdit_Q->text(),
               ui->lineEdit_pageToken->text());
}
