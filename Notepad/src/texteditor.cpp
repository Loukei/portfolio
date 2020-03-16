#include "texteditor.h"
#include <QFile>
#include <QFontInfo>
#include <QTextStream>
#include <QtWidgets>    //! setOverrideCursor
#include <QTextCodec>

TextEditor::TextEditor(QWidget *parent):
    QTextEdit (parent)
{

}

TextEditor::~TextEditor()
{

}

bool TextEditor::openFile(const QString &fname, QTextCodec *codec)
{
    if(fname.isEmpty())
    {
        return false;
    }
    else
    {
        bool isOpen = false;
        if(!codec)  isOpen = load(fname);        // unicode file
        else        isOpen = load(fname,codec);  // non-nuicode file

        if(isOpen){
            setWindowFilePath(fname);
            document()->setModified(false);
        }
        return isOpen;
    }
}

bool TextEditor::saveFile(const QString &fname, QTextCodec *codec)
{
    if(fname.isEmpty())
    {
        return false;
    }
    else
    {   //compare fname and windowFilePath is unefficient
        bool isSave = false;
        if(!codec)  isSave = save(fname);
        else        isSave = save(fname,codec);

        if(isSave){
            setWindowFilePath(fname); //work on save as file
            document()->setModified(false);
        }
        return isSave;
    }
}

//void TextEditor::openFile(const QString &dir)
//{
//    const QString fname = QFileDialog::getOpenFileName(this,
//                                                       tr("Open File"), /*title*/
//                                                       dir, /*working directory*/
//                                                       tr("Text files (*.txt)") /*file fliter*/);
//    if(!fname.isEmpty())
//    {
//        load(fname);
//        this->setWindowFilePath(fname);
//        this->document()->setModified(false);
//    }
//}

//void TextEditor::saveFile(const QString &dir)
//{
//    if(isUnTitled())
//    {
//        return saveAsFile(dir);
//    }
//    save(this->windowFilePath());
//    this->document()->setModified(false);
//}

//void TextEditor::saveAsFile(const QString &dir)
//{
//    const QString fname = QFileDialog::getSaveFileName(this,
//                                                       tr("Save As File"),
//                                                       dir,
//                                                       tr("Text files (*.txt)"));
//    if(!fname.isEmpty())
//    {
//        save(fname);
//        this->setWindowFilePath(fname);
//        this->document()->setModified(false);
//    }
//}

bool TextEditor::load(const QString &fname)
{
    QFile file(fname);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "TextEditor::load(): [Err]:"
                 << "Path " << fname << " can't open.\n"
                 << "Error String " << file.errorString();
        return false;
    }

    QTextStream in(&file);
 #ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
 #endif
    this->document()->setPlainText(in.readAll());
 #ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
 #endif

    file.close();
    textCursor().movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
    return true;
}

//! load file with non-unicode file
bool TextEditor::load(const QString &fname, QTextCodec *codec)
{
    QFile file(fname);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "TextEditor::load(): [Err]:"
                 << "Path " << fname << " can't open.\n"
                 << "Error String " << file.errorString();
        return false;
    }

    if(!codec)
    {
        qDebug("TextEditor::load():[Err] empty codec");
        return false;
    }

    this->clear(); //! clear before append
 #ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
 #endif
    QTextDecoder *decoder = codec->makeDecoder();
    while(!file.atEnd())
    {
       QByteArray line = file.readLine();
       this->append(decoder->toUnicode(line));
    }
 #ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
 #endif

    file.close();
    textCursor().movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
    return true;
}

bool TextEditor::save(const QString &fname)
{
    QFile file(fname);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "TextEditor::save(): [Err]:"
                 << "Path " << fname << " can't save.\n"
                 << "Error String " << file.errorString();
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    out << this->toPlainText();
#ifndef QT_NO_CURSOR
   QApplication::restoreOverrideCursor();
#endif

    file.close();
    return true;
}

bool TextEditor::save(const QString &fname, QTextCodec *codec)
{
    QFile file(fname);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "TextEditor::save(): [Err]:"
                 << "Path " << fname << " can't save.\n"
                 << "Error String " << file.errorString();
        return false;
    }

    QTextStream out(&file);
    out.setCodec(codec);
#ifndef QT_NO_CURSOR
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    out << this->toPlainText();
#ifndef QT_NO_CURSOR
   QApplication::restoreOverrideCursor();
#endif

    file.close();
    return true;
}

bool TextEditor::isUnTitled()
{
    return this->windowFilePath().isEmpty();
}

void TextEditor::deleteSelectChar()
{
    QTextCursor cursor(this->textCursor());
    if(cursor.hasSelection()) cursor.deleteChar();
}

void TextEditor::clearExtraSelections()
{
    //! 清除extra selection
    QList<QTextEdit::ExtraSelection> selections = extraSelections();
    for(int i=0;i<selections.size();++i)
    {
        selections[i].format.clearBackground();
    }
    setExtraSelections(selections);
}

//! Start Search function()
QTextCursor TextEditor::findFunc(const QString &subString,const QTextCursor &cursor,
                                 QTextDocument::FindFlags flags, bool isReg)
{
    //! 如果搜尋的字串或是正則表達式非法，呼叫的函數自己會回傳空Cursor
    //! 因此在這裡不檢查
    QTextCursor textCursor;
    if(isReg)
    {
        QRegularExpression expr(subString);
        textCursor = this->document()->find(expr,cursor,flags);
    }
    else
    {
        textCursor = this->document()->find(subString,cursor,flags);
    }
    return textCursor;
}

QList<QTextEdit::ExtraSelection>
TextEditor::searchAll(const QString &subString, QTextDocument::FindFlags flags,
                      const QBrush &brush)
{
    QList<QTextEdit::ExtraSelection> selections;
    if(subString.isEmpty())
        return selections;

    //! find how many matches
    QTextCursor visit(this->document());
    while(!visit.atEnd() && !visit.isNull())
    {
        visit = this->document()->find(subString,visit,flags);
        if(!visit.isNull())
        {
            QTextEdit::ExtraSelection extra;
            extra.cursor = visit;
            extra.format.setBackground(brush);
            selections.append(extra);
        }
    }
    return selections;
}

QList<QTextEdit::ExtraSelection>
TextEditor::searchAll(const QRegularExpression &expr, QTextDocument::FindFlags flags,
                      const QBrush &brush)
{
    QList<QTextEdit::ExtraSelection> selections;
    if(!expr.isValid())
        return selections;

    //! find how many matches
    QTextCursor visit(this->document());
    while(!visit.atEnd() && !visit.isNull())
    {
        visit = this->document()->find(expr,visit,flags);
        if(!visit.isNull())
        {
            QTextEdit::ExtraSelection extra;
            extra.cursor = visit;
            extra.format.setBackground(brush);
            selections.append(extra);
        }
    }
    return selections;
}
//! End Search function()

//! Start replace function
bool TextEditor::replace(const QString &subString, QTextCursor *cursor)
{
    if(cursor->hasSelection())
    {
        cursor->removeSelectedText();
        cursor->insertText(subString);
        return true;
    }
    return false;
}

int TextEditor::replaceAll(const QString &findStr, const QString &replaceStr,
                           QTextDocument::FindFlags flags)
{
    if(findStr.isEmpty())
    {
        qDebug("TextEditor::replaceAll >> input empty find string.");
        return 0;
    }

    QTextCursor visit(this->document());   //! 宣告一個TextCursor指向文件頭
    QList<QTextEdit::ExtraSelection> selections; //! 用來統計並標記修改後的字串

    while(!visit.isNull()) //! 搜尋不到符合的字串時會回傳null cursor
    {
        visit = this->document()->find(findStr,visit,flags);
        if(replace(replaceStr,&visit))  //! if replace success...
        {
            //! make replaceStr been selected
            visit.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor,
                               replaceStr.size());

            //! add visit to selections
            QTextEdit::ExtraSelection extra;
            extra.cursor = visit;
            extra.format.setBackground(Qt::green);
            selections.append(extra);
        }
    }

    if(!selections.isEmpty())
        this->setExtraSelections(selections);

    return selections.count();
}

int TextEditor::replaceAll(const QRegularExpression &findExpr, const QString &replaceStr,
                           QTextDocument::FindFlags flags)
{
    if(!findExpr.isValid())
    {
        qDebug("TextEditor::replaceAll >> input empty find string.");
        return 0;
    }

    QTextCursor visit(this->document());   //! 宣告一個TextCursor指向文件頭
    QList<QTextEdit::ExtraSelection> selections; //! 用來統計並標記修改後的字串

    while(!visit.isNull()) //! 搜尋不到符合的字串時會回傳null cursor
    {
        visit = this->document()->find(findExpr,visit,flags);
        if(replace(replaceStr,&visit))  //! if replace success...
        {
            //! make replaceStr been selected
            visit.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor,replaceStr.size());

            //! add visit to selections
            QTextEdit::ExtraSelection extra;
            extra.cursor = visit;
            extra.format.setBackground(Qt::green);
            selections.append(extra);
        }
    }

    if(!selections.isEmpty())
        this->setExtraSelections(selections);

    return selections.count();
}
//! End replace function
