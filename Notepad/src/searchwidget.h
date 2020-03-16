#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>

namespace Ui {
class SearchWidget;
}

class TextEditor;

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget *parent = nullptr,Qt::WindowFlags f = Qt::Widget);
    ~SearchWidget();

    void setCurrentEditor(TextEditor *editor = nullptr);
    TextEditor* CurrentEditor();

    void findNext(TextEditor *editor);
    void findPrev(TextEditor *editor);
    void replace(TextEditor *editor);
    void replaceAll(TextEditor *editor);

signals:
    void closeButtonClicked();
    void findNextButtonClicked();
    void findPreviousButtonClicked();
    void replaceButtonClicked();
    void replaceAllButtonClicked();

public slots:
    void receiveEditor(TextEditor *_editor);

private:
    Ui::SearchWidget *ui;
    TextEditor *currentEditor; //! 指向目前操作的文字編輯器類別

    void callEditorFindFunc(bool isRegExp = false);

private slots:
    void on_FindNext();
    void on_FindPrevious();
    void on_Replace();
    void on_ReplaceAll();
};

//! 長寬似乎被設計成固定大小，原因不明，修改.ui也沒有效果
//! 試驗結果，QLineEdit使用addAction似乎無法使按鈕設置checkable屬性
//! 而QLineEdit結合QPushButton
//!
//! 設置背景顏色
//! QPalette pal(m_pWidget->palette());
//! pal.setColor(QPalette::Background, Qt::black);
//! m_pWidget->setAutoFillBackground(true); //不使用的話自己的顏色就只能從父元件繼承,palette不會生效
//! m_pWidget->setPalette(pal);

#endif // SEARCHWIDGET_H
