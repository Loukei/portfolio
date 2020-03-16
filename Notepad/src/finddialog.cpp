#include "finddialog.h"
#include "ui_finddialog.h"
#include "texteditor.h"
#include <QDebug>

FindDialog::FindDialog(QWidget *parent,TextEditor *_editor) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    editor = _editor;   //! find dialog does not own editor
    ui->label_Status->setText(tr("Ready."));
}

FindDialog::~FindDialog()
{
    delete ui;
}

bool FindDialog::isNullEditor()
{
    return editor == nullptr;
}

void FindDialog::on_pushButton_Find_clicked()
{
    qDebug() << "FindDialog::on_pushButton_Find_clicked";
    if(!editor) //! editor not found
        return;

    //! 確保editor存在才能進行下列操作
    editor->clearExtraSelections();
    const QString &message = dispatchSearchFunc(false);
    ui->label_Status->setText(message);
    updateCombox(ui->comboBox_Find);
}

void FindDialog::on_pushButton_FindPrev_clicked()
{
    qDebug() << "on_pushButton_FindPrev_clicked";
    if(!editor)
        return;

    //! 確保editor存在才能進行下列操作
    editor->clearExtraSelections();
    const QString &message = dispatchSearchFunc(true);
    ui->label_Status->setText(message);
    updateCombox(ui->comboBox_Find);
}

void FindDialog::on_pushButton_Replace_clicked()
{
    qDebug() << "on_pushButton_Replace_clicked";

    if(!editor)
        return;

    editor->clearExtraSelections();

    QTextCursor cursor = editor->textCursor();
    const QString &replaceStr = ui->comboBox_Replace->currentText();

    if(cursor.hasSelection() && cursor.selectedText() == replaceStr)
    {   //! 如果搜尋成功，則目前cursor選取的是必須取代的字串
        //! 利用cursor執行取代字串的動作
        editor->replace(replaceStr,&cursor);
    }
    else
    {
        dispatchSearchFunc(false);  //! 如果cursor沒有選中，執行一次搜尋
        cursor = editor->textCursor();
        editor->replace(replaceStr,&cursor);
    }
    editor->setTextCursor(cursor);

    ui->label_Status->setText(tr("Replaced."));
    updateCombox(ui->comboBox_Find);
    updateCombox(ui->comboBox_Replace);
}

void FindDialog::on_pushButton_ReplaceAll_clicked()
{
    qDebug() << "FindDialog::on_pushButton_ReplaceAll_clicked";

    if(!editor)
        return;

    const QString &findStr = ui->comboBox_Find->currentText();
    if(findStr.isEmpty())
    {
        ui->label_Status->setText(tr("Error : No find string found."));
        return;
    }

    const QString &message = dispatchReplaceAllFunc();

    ui->label_Status->setText(message);
    updateCombox(ui->comboBox_Find);
    updateCombox(ui->comboBox_Replace);
}

void FindDialog::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    //! 關閉視窗時清除搜尋結果的反白
    if(editor)
    {
        qDebug()<< "FindDialog::closeEvent";
        editor->clearExtraSelections();
    }
    //! keep continue closeEvent
    QDialog::closeEvent(e);
}

void FindDialog::updateCombox(QComboBox *box)
{
    if(box->findText(box->currentText())==-1)
    {
       box->addItem(box->currentText());//!確保搜尋的字串在combox是唯一的
    }
}

QString FindDialog::dispatchSearchFunc(bool isSearchBackward)
{
    //! 根據目前dialog勾選的設定來執行對應的搜尋函數
    QTextDocument::FindFlags flags;
    if(ui->checkBox_WholeWord->isChecked())
        flags |= QTextDocument::FindWholeWords;
    if(ui->checkBox_CaseSensitive->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if(isSearchBackward)    //! 往上搜尋
        flags |= QTextDocument::FindBackward;

    //! 分派給不同搜尋函數
    int searchCase = 0;
    if(ui->checkBox_RegularExpression->isChecked())
    {
        if(ui->checkBox_ShowAllMatch->isChecked())
            searchCase = 1;
        else
            searchCase = 2;
    }
    else
    {
        if(ui->checkBox_ShowAllMatch->isChecked())
            searchCase = 3;
        else
            searchCase = 4;
    }

    QRegularExpression expr(ui->comboBox_Find->currentText());
    int count = 0;  //! number of search result
    switch (searchCase) {
    case 1:
        count = editor->searchAll(expr,flags);
        break;
    case 2:
        count = editor->search(expr,flags);
        break;
    case 3:
        count = editor->searchAll(ui->comboBox_Find->currentText(),flags);
        break;
    case 4:
        count = editor->search(ui->comboBox_Find->currentText(),flags);
        break;
    default:
        count = 0;
        break;
    }

    //! return search result statistics
    return tr("%1 search results.").arg(count);
}

QString FindDialog::dispatchReplaceAllFunc()
{
    //! 根據目前dialog勾選的設定來執行對應的搜尋函數
    QTextDocument::FindFlags flags;
    if(ui->checkBox_WholeWord->isChecked())
        flags |= QTextDocument::FindWholeWords;
    if(ui->checkBox_CaseSensitive->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    const QString &findStr = ui->comboBox_Find->currentText();
    const QString &replaceStr = ui->comboBox_Replace->currentText();
    if(ui->checkBox_RegularExpression->isChecked())
    {
        QRegularExpression expr(findStr);
        editor->replaceAll(expr,replaceStr,flags);
    }
    else
    {
        editor->replaceAll(findStr,replaceStr,flags);
    }

    return tr("Replace All.");
}

void FindDialog::on_pushButton_clicked()
{
    if(editor)
    {
        QString replaceStr("******Test insertText******");
        QTextCursor cursor = editor->textCursor();
        cursor.insertText(replaceStr);
        cursor.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor,replaceStr.size());
        editor->setTextCursor(cursor);
    }
}
