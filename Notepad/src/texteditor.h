#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H
#include <QTextEdit>

class TextEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

    bool openFile(const QString &fname, QTextCodec *codec = nullptr);
    bool saveFile(const QString &fname, QTextCodec *codec = nullptr);
    bool isUnTitled();
    void deleteSelectChar();

    void clearExtraSelections();

    QTextCursor findFunc(const QString &subString,
                         const QTextCursor &cursor,
                         QTextDocument::FindFlags flags,bool isReg);
    QList<QTextEdit::ExtraSelection> searchAll(const QString &subString,
                                               QTextDocument::FindFlags flags,
                                               const QBrush &brush = Qt::green);
    QList<QTextEdit::ExtraSelection> searchAll(const QRegularExpression &expr,
                                               QTextDocument::FindFlags flags,
                                               const QBrush &brush = Qt::green);

    bool replace(const QString &subString,QTextCursor *cursor);
    int replaceAll(const QString &findStr,const QString &replaceStr,
                   QTextDocument::FindFlags flags);
    int replaceAll(const QRegularExpression &findExpr,const QString &replaceStr,
                   QTextDocument::FindFlags flags);

private:
    bool load(const QString &fname);
    bool load(const QString &fname, QTextCodec *codec);
    bool save(const QString &fname);
    bool save(const QString &fname, QTextCodec *codec);
};
#endif // TEXTEDITOR_H
