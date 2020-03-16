#ifndef FINDINPUT_H
#define FINDINPUT_H
#include <QLineEdit>

class QPushButton;

class FindInput : public QLineEdit
{
    Q_OBJECT
public:
    explicit FindInput(QWidget *parent = nullptr);
    explicit FindInput(const QString &contents,QWidget *parent = nullptr);
    ~FindInput() = default;

    //! 回傳對應的按鈕是否按下
    bool isCaseSensitively();
    bool isWholeWords();
    bool isRegularExpression();

signals:
    //! 按鈕按下的信號
    void CaseSensitivelyClicked(bool pressed);
    void WholeWordsClicked(bool pressed);
    void RegularExpressionClicked(bool pressed);

private:
    QPushButton *button_CaseSensitively;
    QPushButton *button_WholeWords;
    QPushButton *button_RegularExpression;

    void createIconButtons();
    void createTextButtons();
    void setIconButton(QPushButton *button);
    void setTextButton(QPushButton *button);
    void setButtonStyleSheet(QPushButton *button);
    void addButtons();
};
#endif // FINDINPUT_H

//! 說明:
//! 這個類別繼承QLineEdit,並使用QPushButton新增三個搜尋相關選項:
//! 大小寫匹配(CaseSensitively)
//! 全文匹配(WholeWords)
//! 正則表達式(RegularExpression)
//!
//! FindInput不提供搜尋的實做功能，上述的三個選項可以透過 void XXXClicked(bool pressed) 信號
//! 或者 bool isXXX()取得，搜尋的實做請參考QTextDocument的搜尋功能
//!
//! 類別裡的按鈕均設置為toggle button，詳情參考QPushButton的Checkable屬性
//! 為了後續修改的靈活性，提供了兩種設置按鈕的方法setIconButton()及setTextButton()
//! 兩者不可混用。從結果上來看，
//! 使用Icon優點是圖片比較漂亮，大小也很好掌握。缺點是不好用stylesheet操作效果
//! 使用text優點是可以操作stylesheet控制字型粗細、顏色...。缺點是按鈕的大小取決於文字長度反而不好操作。
//!
//! QPushButton是ㄧ個狀態機，Normal/hover/checked等,...參考[2]
//! 要修改Button各個狀態下的Icon，參考[3]
//!
//! 參考文章:
//! [1]三种方法为QLineEdit添加清除内容按钮
//! <https://www.cnblogs.com/apocelipes/p/10367521.html#plan1>
//! [2]QPushButton details
//! <https://doc.qt.io/qt-5/qpushbutton.html#details>
//! [3]QIcon Mode
//! <https://doc.qt.io/qt-5/qicon.html#Mode-enum>
