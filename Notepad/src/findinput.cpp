#include "findinput.h"
#include <QPushButton>
#include <QHBoxLayout>

FindInput::FindInput(QWidget *parent):
    QLineEdit (parent)
{
    button_WholeWords = nullptr;
    button_CaseSensitively = nullptr;
    button_RegularExpression = nullptr;

    createIconButtons();
//    createTextButtons();
    addButtons();
}

FindInput::FindInput(const QString &contents, QWidget *parent):
    QLineEdit (contents,parent)
{
    button_WholeWords = nullptr;
    button_CaseSensitively = nullptr;
    button_RegularExpression = nullptr;

    createIconButtons();
//    createTextButtons();
    addButtons();
}

//! 回傳對應的按鈕是否按下 >>
bool FindInput::isCaseSensitively()
{
    if(button_CaseSensitively)
        return button_CaseSensitively->isChecked();
    else
        return false;
}

bool FindInput::isWholeWords()
{
    if(button_WholeWords)
        return button_WholeWords->isChecked();
    else
        return false;
}

bool FindInput::isRegularExpression()
{
    if(button_RegularExpression)
        return button_RegularExpression->isChecked();
    else
        return false;
}
//! << 回傳對應的按鈕是否按下

void FindInput::createIconButtons()
{
    /* CaseSensitively */
    button_CaseSensitively = new QPushButton(this);
    QIcon icon_CaseSensitively(QStringLiteral(":/Images/icon_Find_CaseSensitive"));
//    icon_CaseSensitively.addFile(":/Images/Icons/icon_CaseSensitive_9.svg",
//                                 QSize(32,32),
//                                 QIcon::Normal,
//                                 QIcon::On);      //! 按下時的圖片
    button_CaseSensitively->setToolTip(tr("Match Case Sensitively"));
    button_CaseSensitively->setIcon(icon_CaseSensitively);
    setIconButton(button_CaseSensitively);

    /* WholeWords */
    button_WholeWords = new QPushButton(this);
    QIcon icon_WholeWords(QStringLiteral(":/Images/icon_Find_MatchWholeWord"));
    button_WholeWords->setIcon(icon_WholeWords);
    button_WholeWords->setToolTip(tr("Match Whole Words"));
    setIconButton(button_WholeWords);

    /* Regular Expression */
    button_RegularExpression = new QPushButton(this);
    QIcon icon_RegularExpression(QStringLiteral(":/Images/icon_Find_RegularExpression"));
    button_RegularExpression->setIcon(icon_RegularExpression);
    button_RegularExpression->setToolTip(tr("Use Regular Expression"));
    setIconButton(button_RegularExpression);
}

void FindInput::createTextButtons()
{
    /* CaseSensitively */
    button_CaseSensitively = new QPushButton(this);
    button_CaseSensitively->setText("Aa");
    button_CaseSensitively->setToolTip(tr("Match Case Sensitively"));
    setTextButton(button_CaseSensitively);

    /* WholeWords */
    button_WholeWords = new QPushButton(this);
    button_WholeWords->setText("“”");
    button_WholeWords->setToolTip(tr("Match Whole Words"));
    setTextButton(button_WholeWords);

    /* Regular Expression */
    button_RegularExpression = new QPushButton(this);
    button_RegularExpression->setText(".*");
    button_RegularExpression->setToolTip(tr("Use Regular Expression"));
    setTextButton(button_RegularExpression);
}

//! set buttons and size 
static void setButtonSize(QPushButton *button,int width)
{
    //! 幫助函數，設定button為固定大小，並指定輸入內容與邊框間距
    QSizePolicy policy = button->sizePolicy();
    const int buttonMargin = 3; //! 按钮和输入内容的边距
    policy.setHorizontalPolicy(QSizePolicy::Fixed);
    button->setSizePolicy(policy);
    button->setFixedWidth(width + buttonMargin * 2);
}

void FindInput::setIconButton(QPushButton *button)
{
    if(!button)
    {
        qDebug("No Button Input.");
        return;
    }
    setButtonSize(button,button->iconSize().width());
    setButtonStyleSheet(button);
    // 设置背景和边框在非点击时不可见
    //button->setFlat(true);
}

void FindInput::setTextButton(QPushButton *button)
{
    if(!button)
    {
        qDebug("No Button Input.");
        return;
    }
    //! 取得字體寬度
    const int width = QWidget::fontMetrics().width(button->text());
    setButtonSize(button,width);
}

void FindInput::setButtonStyleSheet(QPushButton *button)
{
    QString btnStyleSheet =
            "/*PushButton一旦設定背景色，邊框就會失效，因此一定要設定邊框型態、粗細、顏色等*/"
            "QPushButton {   					/*正常狀態*/"
            "	background-color: rgba(255, 255, 255, 0); 	/*透明背景*/"
            "	border: none;								/*無邊框*/"
            "}"
            "QPushButton:hover{  				/*滑鼠懸停*/"
            "	background-color: rgba(182, 182, 182, 180);	/*深灰半透明*/"
            "	border: none;"
            "}"
            "QPushButton:checked{				/*壓下不動，與pressed不同*/"
            "	background-color: rgba(182, 182, 182, 180);	"
            "	border: solid; 								/*有邊框*/"
            "	border-color: rgb(0, 0, 0);					/*黑色*/"
            "	border-width: 1px;							/*邊框寬度*/"
            "}";
    button->setStyleSheet(btnStyleSheet);
}
//! set buttons and size

static void setAddedButton(QPushButton *button)
{   //! 設定按鈕加入時所需的各種屬性
    button->setCheckable(true);         //! 按鈕無法自動跳起
    button->setFocusPolicy(Qt::NoFocus);//! 按鈕已經是edit的一部分了，不應該再能被單獨聚焦
    button->setCursor(Qt::ArrowCursor); //! 設定游標，否則按鈕會顯示輸入游標
}

void FindInput::addButtons()
{
    if(!button_CaseSensitively | !button_WholeWords | !button_RegularExpression)
        return;

    /* CaseSensitively */
    connect(button_CaseSensitively,&QPushButton::clicked,
            this,&FindInput::CaseSensitivelyClicked);
    setAddedButton(button_CaseSensitively);

    /* WholeWords */
    connect(button_WholeWords,&QPushButton::clicked,
            this,&FindInput::WholeWordsClicked);
    setAddedButton(button_WholeWords);

    /* Regular Expression */
    connect(button_RegularExpression,&QPushButton::clicked,
            this,&FindInput::RegularExpressionClicked);
    setAddedButton(button_RegularExpression);

    /* set Layout */
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->addStretch();    //! 添加空白彈簧條
    btnLayout->addWidget(button_CaseSensitively);
    btnLayout->addWidget(button_WholeWords);
    btnLayout->addWidget(button_RegularExpression);
    btnLayout->setAlignment(Qt::AlignRight);
    btnLayout->setContentsMargins(0,0,0,0);
    btnLayout->setSpacing(0); //! 按鈕之間沒有空格
    setLayout(btnLayout);

    //! 設定輸入區域的範圍,從edit最左到buttons最左方
    const int width =
            button_CaseSensitively->width()
            + button_WholeWords->width()
            + button_RegularExpression->width();
    setTextMargins(0,0,width,0);
}
