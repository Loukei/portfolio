#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
namespace Ui {
class MainWindow;
}

/*foward declaration*/
class TabWidget;
class TextEditor;
class QSettings;
class RecentFiles;
class SearchWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    int openFile(const QString &fpath);

protected:
    void closeEvent(QCloseEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override; //! drag to load
    virtual void dropEvent(QDropEvent *event) override; //! drag to load

private:
    Ui::MainWindow *ui;

    /*child widget*/
    TabWidget *tabwidget;
    SearchWidget *searchwidget;

    /*Variables*/
    const char *apptitle = "NotePad";
    QSettings *settings;
    enum {maxRecent = 10};  //! 決定recent file列表的大小
    QSharedPointer<RecentFiles> recentFiles;

    /*Actions*/
    QAction* action_SubMenuRecentFile;
    QAction* action_RecentFileArray[maxRecent];

    /*Function*/
    void initialActions();
    void initialMenus();
    void initialToolBars();
    void initialStatusBar();
    void initialTabWidget();
    void initailSearchWidget();
    TextEditor* createEditor(QWidget *parent);

    //! File I/O
    bool maybeSave(TextEditor *editor);
    //! File I/O

    //! QSettings
    void loadSettings();
    void saveSettings();
    //! QSettings

    QString getCurrentFolder();
    void resetMainWindowTitle(const QString &fpath);
    void updateMenus();
    TextEditor* currentTextEditor();

private slots:
    //! File I/O
    void on_newFile();
    void on_openFile();
    void on_saveFile();
    void on_saveAsFile();
    //! File I/O

    void on_TabWidget_tabCloseRequested(int index); //!tab關閉按鈕按下
    void on_TabWidget_currentChanged(int index);    //!tabwidget切換tab
    void on_documentWasModified();                  //!檔案內容修改

    //! click board
    void on_UndoAct_triggered();
    void on_RedoAct_triggered();
    void on_cutAct_triggered();
    void on_copyAct_triggered();
    void on_pasteAct_triggered();
    void on_deleteAct_triggered();
    void on_selectAllAct_triggered();
    //! click board

    //! recent File
    void on_openRecentFile();
    void on_menuRecentFile_aboutToShow();
    void on_clearRecentFileAct_triggered();
    //! recent File

    //! Find & Replace
    void on_FindOrReplaceAct_triggered();
    void on_SearchWidget_findNextButtonClicked();
    void on_SearchWidget_findPreviousButtonClicked();
    void on_SearchWidget_replaceButtonClicked();
    void on_SearchWidget_replaceAllButtonClicked();
    //! Find & Replace

    void on_setFont_triggered(); //! set editor textfont

    void on_reopenEncoding();
    void on_saveEncoding();
};
#endif // MAINWINDOW_H

//! 功能與對應函數說明：
//! 1.主要構成：MenuBar,ToolBar,TabWidget,TextEditor.
//!     TabWidget為主元件當做centralWidget,由tabwidget.h定義,本身並沒有特化去控制底下的TextEditor.
//!     TabWidget每個tab控制一個TextEditor實現多文本檢視.
//!     TextEditor實現基本的編輯功能:開啟/保存/另存 檔案，搜尋/取代。但不負責實現這些UI邏輯
//!
//! 2.新建/開啟/保存/另存 檔案功能，由on_newFile(); on_openFile(); on_saveFile(); on_saveAsFile();
//!     四個函數提供，maybeSave()則在關閉tab時詢問使用者是否存檔
//!
//! 3.視窗標題會同步顯示目前tab的 (1)檔案路徑 (2)修改標識符'*'，這部分可參考
//!     (1)on_TabWidget_currentChanged()，resetMainWindowTitle()
//!     (2)setWindowModified()，on_documentWasModified()
//!
//! 4.getCurrentFolder()負責在開檔/另存的時候提供最近開啟過的路徑，如果沒有則提供桌面路徑
//!
//! 5.QSettings負責保存上一次執行結果，比如視窗大小等
//!     相關的函式可以參考loadSettings()、saveSettings()
//!
//! 6.編輯功能(Undo,Redo,cut,copy,paste,delete,selectAll)
//!     qTextEditor本身就有實做這部分功能，詳見
//!     on_UndoAct_triggered(),on_RedoAct_triggered(),
//!     on_cutAct_triggered(),on_copyAct_triggered(),
//!     on_pasteAct_triggered(),on_deleteAct_triggered(),
//!     on_selectAllAct_triggered(),
//!     同時，切換tab時根據editor情況不同，比如復原&重複需要editor曾經有修改的痕跡才可執行，
//!     其他的編輯行為需要文字游標有選取才能動作，這些都寫在updateMenus()方法理
//!
//! 7.紀錄最近的檔案(Recent Files)功能
//!     參考QT範例的mdi修改而來，可以紀錄開起過的檔案並在重新開啟時仍保存紀錄
//!     實作方法是在程式初始化的時候加入一個QAction[MAX],用來存放最多MAX大小的action，
//!     實際的內容由recentFiles保存
//!     on_menuRecentFile_aboutToShow()會在選到視窗的時候提取recentFiles來更新QAction[MAX]
//!     on_openRecentFile()會在按下選單內容時依照sender開啟對應檔案
//!     RecentFiles類別繼承QStringList,並提供讀寫QSettings內容到物件本身的方法，以及
//!     加入檔案路徑的方法等
//!     recentFiles會在on_openFile()及on_saveAsFile()更新,on_clearRecentFileAct_triggered清除
//!
//! 8.drag to load 拖曳檔案讀取
//!     首先開啟視窗的setAcceptDrops屬性，並關閉子視窗的屬性，最後複寫dragEnterEvent()與dropEvent()
//!
//! 9.find & replace
//!     SearchWidget負責搜尋/取代的UI邏輯，MainWindow負責創建SearchWidget並在SearchWidget按鈕按下時
//!     呼叫對應的函數(PS:因為必須按下同時取得當前的Editor，這部分無法由searchWidget實現)。
//!     核心的搜尋取代功能由TextEditor類實現
//!
//! 10.字型
//!     QT本身就有提供很好的UI元件:QFontDialog，考慮到專案還沒規劃到RichText，
//!     每次修改字型都會變動到整個editor的文字
//!
//! 11.Encoding
//!     Qt的標準支援使用Unicode，包含QTextEdit也是
//!     要想使用不同編碼進行讀寫檔案，參考QTextCodec類toUnicode/fromUnicode
//!     QTextStream支援setCodec方法可以設置讀寫的編碼
//!     這部分功能在TextEditor實現，
//!     參考 on_reopenEncoding() on_saveEncoding()
//!
//! 12.行數顯示
//!
//! 13.檔案總管功能
//!
//! 14.QSS載入(自訂外觀)
//!
//! 15.拖曳分頁開啟新視窗
//!
//! 16.tabWidget功能(新增分頁鈕、平均分配分頁大小)
//!
//! 雲端硬碟同步(進階)
//!

/* Bug:
 * 說明：
 * 2.Settings:保存設定，確保下次開啟時使用者有相同體驗
 *  - 目前保存的內容有以下:
 *  - 視窗的大小、位置  O
 *  - 須提供"回復預設"的選項 X
 * 3.select all menus enabled 沒有同步(沒有找到適合的signal表示目前的editor有內容)
*/
