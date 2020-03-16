#include "searchwidget.h"
#include "ui_searchwidget.h"
#include "texteditor.h"
#include <QAction>
#include <QRegularExpression>
#include <QRegExp>

SearchWidget::SearchWidget(QWidget *parent,Qt::WindowFlags f) :
    QWidget(parent,f),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);
    currentEditor = nullptr;

    connect(ui->pushButton_Close,&QPushButton::clicked,
            this,&SearchWidget::closeButtonClicked);

    //! 當使用者按下對應按鈕，發出signal通知
    connect(ui->pushButton_FindNext,&QPushButton::clicked,
            this,&SearchWidget::findNextButtonClicked);
    connect(ui->pushButton_FindPrevious,&QPushButton::clicked,
            this,&SearchWidget::findPreviousButtonClicked);
    connect(ui->pushButton_Replace,&QPushButton::clicked,
            this,&SearchWidget::replaceButtonClicked);
    connect(ui->pushButton_ReplaceAll,&QPushButton::clicked,
            this,&SearchWidget::replaceAllButtonClicked);

    ui->statusLabel->setText(tr("Ready."));
}

SearchWidget::~SearchWidget()
{
    delete ui;
}

inline void SearchWidget::setCurrentEditor(TextEditor *editor)
{
    currentEditor = editor;
}

inline TextEditor *SearchWidget::CurrentEditor()
{
    return currentEditor;
}

static QTextDocument::FindFlags getUIFindFlags(Ui::SearchWidget *ui,
                                               bool isFindBack = false)
{
    //! 幫助函數，取得UI的FindFlag

    QTextDocument::FindFlags flags;
    if(ui->findInput->isWholeWords())
        flags |= QTextDocument::FindWholeWords;
    if(ui->findInput->isCaseSensitively())
        flags |= QTextDocument::FindCaseSensitively;
    if(isFindBack)
        flags |= QTextDocument::FindBackward;

    return flags;
}

void SearchWidget::findNext(TextEditor *editor)
{
    editor->clearExtraSelections();  //! 先清除前面的搜尋內容

    //! 確保搜尋字串非空
    const QString &findStr = ui->findInput->text();
    if(findStr.isEmpty())
    {
        ui->statusLabel->setText(tr("Please enter your search string."));
        return;
    }

    //! 取得目前UI裡的findFlags(搜尋選項),findNext不使用往前搜尋
    QTextDocument::FindFlags flags = getUIFindFlags(ui,false);
    //! 按照使用者是否選用正則表達式呼叫對應的find函數
    QTextCursor cursor = editor->textCursor();
    cursor = editor->findFunc(findStr,cursor,flags,ui->findInput->isRegularExpression());

    //! 如果上一步有成功，會返回非空(null)的cursor
    if(!cursor.isNull()){
        editor->setTextCursor(cursor);
        ui->statusLabel->setText(tr("Find result."));
    }else{  //! 搜尋未成功則考慮是否達到頁尾
        cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor,1);  //!移動搜尋到頁首
        cursor = editor->findFunc(findStr,cursor,flags,
                                  ui->findInput->isRegularExpression());

        if(!cursor.isNull())
        {
            editor->setTextCursor(cursor);
            ui->statusLabel->setText(tr("Search at Footer,go to Header."));
        }else{
            ui->statusLabel->setText(tr("No result found."));
        }
    }
}

void SearchWidget::findPrev(TextEditor *editor)
{
    editor->clearExtraSelections();
    //! 確保搜尋字串非空
    const QString &findStr = ui->findInput->text();
    if(findStr.isEmpty())
    {
        ui->statusLabel->setText(tr("Please enter your search string."));
        return;
    }

    //! 取得目前UI裡的findFlags(搜尋選項),findPrev使用往前搜尋
    QTextDocument::FindFlags flags = getUIFindFlags(ui,true);
    //! 按照使用者是否選用正則表達式呼叫對應的find函數
    QTextCursor cursor = editor->textCursor();
    if(ui->findInput->isRegularExpression())
        cursor = editor->document()->find(QRegularExpression(findStr),cursor,flags);
    else
        cursor = editor->document()->find(findStr,cursor,flags);

    //! 如果上一步有成功，會返回非空(null)的cursor
    if(!cursor.isNull()){
        editor->setTextCursor(cursor);
        ui->statusLabel->setText(tr("Find result."));
    }else{
        cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);    //! 移動搜尋至頁尾
        cursor = editor->findFunc(findStr,cursor,flags,
                                  ui->findInput->isRegularExpression());

        if(!cursor.isNull())
        {
            editor->setTextCursor(cursor);
            ui->statusLabel->setText(tr("Search at Header,go to Footer."));
        }else{
            ui->statusLabel->setText(tr("No result found."));
        }
    }
}

static bool hasCursorSelectedTarget(Ui::SearchWidget *ui,QTextCursor *cursor)
{
    if(ui->findInput->isRegularExpression())
    {
        return QRegularExpression(ui->findInput->text())
                .match(cursor->selectedText()).hasMatch();
    }
    else
    {
        return cursor->selectedText() == ui->findInput->text();
    }
}

void SearchWidget::replace(TextEditor *editor)
{
    //! 1.清除上一次搜尋的extraSelection
    editor->clearExtraSelections();

    //! 2.取得目前的搜尋選項
    const QString &replaceStr = ui->lineEdit_Replace->text();

    //! 3.如果發現textCursor已經選中結果，執行replace
    QTextCursor cursor = editor->textCursor();
    if(cursor.hasSelection() && hasCursorSelectedTarget(ui,&cursor))
    {
        editor->replace(replaceStr,&cursor);
        ui->statusLabel->setText(tr("Replace 1 matches."));
    }
    else
    {   //! 4.如果沒有，執行一次findNext
        findNext(editor);
    }
}

//! 取代所有的目標字串，並標示取代結果
void SearchWidget::replaceAll(TextEditor *editor)
{
    //! 1.清除上一次搜尋的extraSelection
    editor->clearExtraSelections();

    //! 2.
    QTextDocument::FindFlags flags = getUIFindFlags(ui,false);
    const QString &replaceStr = ui->lineEdit_Replace->text();
    const QString &findStr = ui->findInput->text();
    if(findStr.isEmpty())
    {
        ui->statusLabel->setText(tr("Find string cant be empty."));
        return;
    }

    int match_count = 0;
    if(ui->findInput->isRegularExpression())
        match_count = editor->replaceAll(QRegularExpression(findStr),replaceStr,flags);
    else
        match_count = editor->replaceAll(findStr,replaceStr,flags);

    ui->statusLabel->setText(tr("Replace %1 matchs.").arg(match_count));
}

//! Find functions
void SearchWidget::on_FindNext()
{
}

void SearchWidget::on_FindPrevious()
{
}
//! Find functions

//! Search functions
void SearchWidget::on_Replace()
{
}

void SearchWidget::on_ReplaceAll()
{
}
//! Search functions

void SearchWidget::receiveEditor(TextEditor *editor)
{
    setCurrentEditor(editor);
}

