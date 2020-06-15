#include "searchdialog.h"
#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
//    m_model->loadJson("{ \"kind\": \"drive#fileList\","
//                      " \"incompleteSearch\": false,"
//                      " \"files\": [  "
//                      "{   "
//                      "\"kind\": \"drive#file\","
//                      "   \"id\": \"EXAMPLE_FILE_ID\","
//                      "   \"name\": \"Untitled\","
//                      "   \"mimeType\": \"text/plain\"  "
//                      "} "
//                      "]"
//                      "}");
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

QString SearchDialog::getCorpora() const
{
    return ui->lineEdit_corpora->text();
}

QString SearchDialog::getDriveId() const
{
    return ui->lineEdit_driveId->text();
}

QString SearchDialog::getFields() const
{
    return ui->lineEdit_fields->text();
}

bool SearchDialog::getIncludeItemsFromAllDrives() const
{
    return ui->comboBox_includeItemsFromAllDrives->currentIndex();
}

QString SearchDialog::getOrderBy() const
{
    return ui->lineEdit_orderBy->text();
}

int SearchDialog::getPageSize() const
{
    return ui->spinBox_pageSize->value();
}

QString SearchDialog::getPageToken() const
{
    return ui->lineEdit_pageToken->text();
}

QString SearchDialog::getQ() const
{
    return ui->lineEdit_Q->text();
}

QString SearchDialog::getSpaces() const
{
    return ui->lineEdit_spaces->text();
}

bool SearchDialog::getSupportsAllDrives() const
{
    return ui->comboBox_supportsAllDrives->currentIndex();
}
