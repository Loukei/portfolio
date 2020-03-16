#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tabwidget.h"
#include "texteditor.h"
#include "recentfiles.h"
#include "searchwidget.h"
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QCloseEvent>
//! Darg & drop
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
//! Font
#include <QFontDialog>
//! Encoding
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tabwidget(nullptr),
    searchwidget(nullptr)
{
   ui->setupUi(this);
   recentFiles = QSharedPointer<RecentFiles>(new RecentFiles);
   loadSettings(); //! 這段程式碼有讀取recent files，一定要放在recent files初始化之後

   initialActions();
   initialMenus();
   initialToolBars();
   tabwidget = ui->tabWidget;
   initialTabWidget();
   searchwidget = ui->searchWidget;
   initailSearchWidget();
   initialStatusBar();
   updateMenus();

   setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialActions()
{
    //! File IO : 選單 Menu > File
    ui->action_New_File->setShortcut(QKeySequence::New);
    ui->action_New_File->setStatusTip(tr("Create new file"));
    connect(ui->action_New_File,&QAction::triggered,this,&MainWindow::on_newFile);

    ui->action_Open_File->setShortcut(QKeySequence::Open);
    ui->action_Open_File->setStatusTip(tr("Open exist File"));
    connect(ui->action_Open_File,&QAction::triggered,this,&MainWindow::on_openFile);

    ui->action_Save_File->setShortcut(QKeySequence::Save);
    ui->action_Save_File->setStatusTip(tr("Save File"));
    connect(ui->action_Save_File,&QAction::triggered,this,&MainWindow::on_saveFile);

    ui->action_Save_As->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S); //! 示範使用自定義按鍵
    ui->action_Save_As->setStatusTip(tr("Save as new File"));
    connect(ui->action_Save_As,&QAction::triggered,this,&MainWindow::on_saveAsFile);

    ui->action_Exit->setShortcut(QKeySequence::Close);
    ui->action_Exit->setStatusTip(tr("Close window"));
    connect(ui->action_Exit,&QAction::triggered,
            this,&MainWindow::close); //! Exit動作觸發主程式關閉，並執行closeEvent()
    //! File IO

    //! <Open Recent subMenu>
    //!     開啟Open recent視窗前，讀取recent file列表來載入
    connect(ui->menu_File_Open_Recent,&QMenu::aboutToShow,
            this,&MainWindow::on_menuRecentFile_aboutToShow);
    //!     清除recent files
    connect(ui->action_Clear_Items,&QAction::triggered,
            this,&MainWindow::on_clearRecentFileAct_triggered);
    //!     初始化 recent file 為空
    for(int i=0;i<maxRecent;++i)
    {
        action_RecentFileArray[i] =
                ui->menu_File_Open_Recent->addAction("",this,&MainWindow::on_openRecentFile);
        action_RecentFileArray[i]->setVisible(false);
    }
    //! </Open Recent subMenu>

    //! Reopen encoding
    ui->action_Code_Unicode->setData(QString("Unicode"));
    ui->action_Code_Big5->setData(QString("Big5"));
    ui->action_Code_EUC_JP->setData(QString("EUC-JP"));
    ui->action_Code_ISO_2022_JP->setData(QString("ISO 2022-JP"));
    ui->action_Code_Shift_JIS->setData(QString("Shift-JIS"));
    ui->action_Code_GB18030->setData(QString("GB18030"));
    //! Reopen encoding

    //! <Edit Menu>
    ui->action_Undo->setShortcut(QKeySequence::Undo);
    ui->action_Undo->setStatusTip(tr("Back to previous Modified"));
    ui->action_Undo->setDisabled(true);
    connect(ui->action_Undo,&QAction::triggered,this,&MainWindow::on_UndoAct_triggered);

    ui->action_Redo->setShortcut(QKeySequence::Redo);
    ui->action_Redo->setStatusTip(tr("Go to next Modified"));
    ui->action_Redo->setDisabled(true);
    connect(ui->action_Redo,&QAction::triggered,this,&MainWindow::on_RedoAct_triggered);

#ifndef QT_NO_CLIPBOARD
    ui->action_Cut->setShortcut(QKeySequence::Cut);
    ui->action_Cut->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(ui->action_Cut,&QAction::triggered,this,&MainWindow::on_cutAct_triggered);

    ui->action_Copy->setShortcut(QKeySequence::Copy);
    ui->action_Copy->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(ui->action_Copy,&QAction::triggered,this,&MainWindow::on_copyAct_triggered);

    ui->action_Paste->setShortcut(QKeySequence::Paste);
    ui->action_Paste->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(ui->action_Paste,&QAction::triggered,this,&MainWindow::on_pasteAct_triggered);
#endif

    ui->action_Delete->setShortcut(QKeySequence::Delete);
    ui->action_Delete->setStatusTip(tr("Clear all content"));
    connect(ui->action_Delete,&QAction::triggered,this,&MainWindow::on_deleteAct_triggered);

    ui->action_Select_All->setShortcut(QKeySequence::SelectAll);
    ui->action_Select_All->setStatusTip(tr("Select All text in editor"));
    connect(ui->action_Select_All,&QAction::triggered,this,&MainWindow::on_selectAllAct_triggered);
    //! </Edit Menu>

    ui->action_FindOrReplace->setShortcut(Qt::CTRL + Qt::Key_H);
    ui->action_FindOrReplace->setStatusTip(tr("Search or Replace word in document."));
    connect(ui->action_FindOrReplace,&QAction::triggered,
            this,&MainWindow::on_FindOrReplaceAct_triggered);

    connect(ui->action_Font,&QAction::triggered,
            this,&MainWindow::on_setFont_triggered);

    //! encoding
    ui->action_Code_Unicode->setData(QByteArray("Unicode"));
    ui->action_Code_Big5->setData(QByteArray("Big5"));
    ui->action_Code_EUC_JP->setData(QByteArray("EUC-JP"));
    ui->action_Code_ISO_2022_JP->setData(QByteArray("ISO 2022-JP"));
    ui->action_Code_Shift_JIS->setData(QByteArray("Shift-JIS"));
    ui->action_Code_GB18030->setData(QByteArray("GB18030"));

    connect(ui->action_Code_Unicode,&QAction::triggered,
            this,&MainWindow::on_reopenEncoding);
    connect(ui->action_Code_Big5,&QAction::triggered,
            this,&MainWindow::on_reopenEncoding);
    connect(ui->action_Code_EUC_JP,&QAction::triggered,
            this,&MainWindow::on_reopenEncoding);
    connect(ui->action_Code_ISO_2022_JP,&QAction::triggered,
            this,&MainWindow::on_reopenEncoding);
    connect(ui->action_Code_Shift_JIS,&QAction::triggered,
            this,&MainWindow::on_reopenEncoding);
    connect(ui->action_Code_GB18030,&QAction::triggered,
            this,&MainWindow::on_reopenEncoding);

    ui->action_SaveCode_Big5->setData(QByteArray("Big5"));
    ui->action_SaveCode_EUC_JP->setData(QByteArray("EUC-JP"));
    ui->action_SaveCode_ISO_2022_JP->setData(QByteArray("ISO 2022-JP"));
    ui->action_SaveCode_Shift_JIS->setData(QByteArray("Shift-JIS"));
    ui->action_SaveCode_GB18030->setData(QByteArray("GB18030"));

    connect(ui->action_SaveCode_Big5,&QAction::triggered,
            this,&MainWindow::on_saveEncoding);
    connect(ui->action_SaveCode_EUC_JP,&QAction::triggered,
            this,&MainWindow::on_saveEncoding);
    connect(ui->action_SaveCode_ISO_2022_JP,&QAction::triggered,
            this,&MainWindow::on_saveEncoding);
    connect(ui->action_SaveCode_Shift_JIS,&QAction::triggered,
            this,&MainWindow::on_saveEncoding);
    connect(ui->action_SaveCode_GB18030,&QAction::triggered,
            this,&MainWindow::on_saveEncoding);
    //! encoding
}

void MainWindow::initialMenus()
{
//    QMenu *fileMenu = ui->menuFile;
}

void MainWindow::initialToolBars()
{
    //! <File ToolBar>
    QToolBar *fileToolBar = ui->mainToolBar;
    fileToolBar->setWindowTitle(tr("&File"));
    fileToolBar->setMovable(false);
    //! </File ToolBar>
}

void MainWindow::initialTabWidget()
{
    tabwidget->setTabsClosable(true);
    tabwidget->setMovable(true);
    connect(tabwidget,&TabWidget::tabCloseRequested,
            this,&MainWindow::on_TabWidget_tabCloseRequested);
    connect(tabwidget,&TabWidget::currentChanged,
            this,&MainWindow::on_TabWidget_currentChanged);

    //! tabwidget 已經建立，但一開始我們先開一個空白分頁
    on_newFile();
    resetMainWindowTitle(QStringLiteral(""));
}

void MainWindow::initailSearchWidget()
{
    connect(searchwidget,&SearchWidget::closeButtonClicked,
            searchwidget,&SearchWidget::hide);
    connect(searchwidget,&SearchWidget::findNextButtonClicked,
            this,&MainWindow::on_SearchWidget_findNextButtonClicked);
    connect(searchwidget,&SearchWidget::findPreviousButtonClicked,
            this,&MainWindow::on_SearchWidget_findPreviousButtonClicked);
    connect(searchwidget,&SearchWidget::replaceButtonClicked,
            this,&MainWindow::on_SearchWidget_replaceButtonClicked);
    connect(searchwidget,&SearchWidget::replaceAllButtonClicked,
            this,&MainWindow::on_SearchWidget_replaceAllButtonClicked);
    searchwidget->hide();
}

void MainWindow::initialStatusBar()
{
    ui->statusBar->showMessage(tr("Ready"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //! 關閉視窗時詢問是否保存檔案
    bool is_allTab_saved = true;
    if(is_allTab_saved)
    {
        event->accept();
        //! accept close event, save Settings
        saveSettings();
    }
    else
    { event->ignore(); }
}

void MainWindow::loadSettings()
{
    settings = new QSettings(QApplication::applicationDirPath() + "NotePadsettings.ini",
                             QSettings::IniFormat, this);

    //! load window size and position from last time
    settings->beginGroup("MainWindow");
    resize(settings->value("size", QSize(800, 600)).toSize());
    move(settings->value("pos", QPoint(200, 200)).toPoint());
    settings->endGroup();

    recentFiles->readSettings(settings);    //! read settings to recent files
}

void MainWindow::saveSettings()
{
    if(!settings)
        return;

    qDebug() << "save sattings to:" << settings->fileName();
    settings->beginGroup("MainWindow");
    settings->setValue("size",this->size());
    settings->setValue("pos", this->pos());
    settings->endGroup();

    recentFiles->writeSettings(settings);   //! write recent files to settings
}

QString MainWindow::getCurrentFolder()
{
    //* \brief return the folder path from tab or return desktop
    QString fileFolder = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if(tabwidget->currentWidget() == nullptr) { return fileFolder; }
    else
    {
        QString currentFolder = QFileInfo(tabwidget->currentWidget()->windowFilePath()).path();
        if(currentFolder.isEmpty()) return fileFolder;
        else                        return currentFolder;
    }
}

void MainWindow::resetMainWindowTitle(const QString &fpath)
{
    QString wintitle;
    if(fpath.isEmpty()) { wintitle = tr("UnTitled") + "[*] - " + tr(apptitle); }
    else                { wintitle = fpath + "[*] - " + tr(apptitle); }
    this->setWindowTitle(wintitle);
}

void MainWindow::updateMenus()
{
    //! 目前若沒有editor -> 關閉存檔有關選項
    bool hasCurrentTextEditor = currentTextEditor();
    ui->action_Save_File->setEnabled(hasCurrentTextEditor);
    ui->action_Save_As->setEnabled(hasCurrentTextEditor);
    //saveAllAct->setEnabled(hasCurrentTextEditor);
#ifndef QT_NO_CLIPBOARD
    ui->action_Paste->setEnabled(hasCurrentTextEditor);
#endif

    //! 針對目前切換到的editor，同步Edit有關選單
    if(TextEditor *editor = currentTextEditor())
    {
        ui->action_Undo->setEnabled(editor->document()->isUndoAvailable());
        ui->action_Redo->setEnabled(editor->document()->isRedoAvailable());
#ifndef QT_NO_CLIPBOARD
        bool hasSelection = editor->textCursor().hasSelection();
        ui->action_Cut->setEnabled(hasSelection);
        ui->action_Copy->setEnabled(hasSelection);
        ui->action_Delete->setEnabled(hasSelection);
#endif
    }
}

TextEditor *MainWindow::currentTextEditor()
{
    //! return tabwidget current text editor, return 0 if no tabs
    if(tabwidget->count())
        return qobject_cast<TextEditor*>(tabwidget->currentWidget());
    else
        return nullptr;
}

//static void setEditorStyleSheet(TextEditor *editor)
//{
//    QString style = "TextEditor { "
//                    "background: lightGray"
//                    "selection-background-color: green "
//                    "selection-color: white"
//                    "}";
//    editor->setStyleSheet(style);
//}

//! 利用QPalette設定editor背景與文字顏色
static void setEditorPalette(TextEditor *editor)
{
    QPalette palette = editor->palette();
    palette.setColor(QPalette::Base,QColor(51, 102, 102));  // set background color
    palette.setColor(QPalette::Text,QColor(240, 240, 240)); // set text color
    //! bugs:修改背景跟文字顏色時要注意選取背景色(QPalette::Highlight)跟
    //! 選取文字色(QPalette::HighlightedText)不能相近,否則選取出來會看不清
    //! QPalette::AlternateBase
    //! 同理，find選中的片段也一樣
    editor->setPalette(palette);
}

TextEditor* MainWindow::createEditor(QWidget *parent)
{
    //! 創建一個texteditor，並連結必要的slot
    TextEditor *editor = new TextEditor(parent);
    connect(editor->document(),&QTextDocument::modificationChanged,
            this,&MainWindow::on_documentWasModified);  //!同步顯示目前的檔案是否被修改
    connect(editor,&TextEditor::undoAvailable,ui->action_Redo,&QAction::setEnabled);    //!
    connect(editor,&TextEditor::redoAvailable,ui->action_Redo,&QAction::setEnabled);
#ifndef QT_NO_CLIPBOARD
    connect(editor,&TextEditor::copyAvailable,ui->action_Cut,&QAction::setEnabled);
    connect(editor,&TextEditor::copyAvailable,ui->action_Copy,&QAction::setEnabled);
    connect(editor,&TextEditor::copyAvailable,ui->action_Delete,&QAction::setEnabled);
#endif
    editor->setAcceptDrops(false); //set edit not accept drop event
//    setEditorStyleSheet(editor);  //不明原因無法設定QSS
    setEditorPalette(editor);
    return editor;
}

//! File I/O
void MainWindow::on_newFile()
{
    TextEditor *editor = createEditor(tabwidget);
    int curindex = tabwidget->addTab(editor,tr("Untitled"));
    tabwidget->setCurrentIndex(curindex);   //! 新增tab後自動切換到此tab
}

void MainWindow::on_openFile()
{
    QString fpath = QFileDialog::getOpenFileName(this,
                                                 tr("Open File"),       /*title*/
                                                 getCurrentFolder(),        /*working directory*/
                                                 tr("Text files (*.txt)")); /*selectedFilter*/
    if(!fpath.isEmpty())
    {
        TextEditor *editor = createEditor(tabwidget);
        editor->openFile(fpath);
        int curindex = tabwidget->addTab(editor,QFileInfo(fpath).fileName());
        tabwidget->setCurrentIndex(curindex);
        recentFiles->prepend(fpath);    //! add to recent File list
    }
}

int MainWindow::openFile(const QString &fpath)
{
    TextEditor *editor = createEditor(tabwidget);
    editor->openFile(fpath);
    int curindex = tabwidget->addTab(editor,QFileInfo(fpath).fileName());
    tabwidget->setCurrentIndex(curindex);
    return curindex;
}

void MainWindow::on_saveFile()
{
    if(TextEditor *editor = currentTextEditor())
    {   //! currentTextEditor() get null if tabwidget is empty
        if(editor->isUnTitled())
            return on_saveAsFile();
        else
            editor->saveFile(editor->windowFilePath());
    }
}

void MainWindow::on_saveAsFile()
{
    TextEditor *editor = currentTextEditor();
    if(editor == nullptr)   return;

    QString fpath = QFileDialog::getSaveFileName(
                this,                       /*parrent*/
                tr("Save File"),            /*title*/
                getCurrentFolder(),         /*working directory*/
                tr("Text files (*.txt)")    /*selectedFilter*/
                );
    if(!fpath.isEmpty())
    {
        editor->saveFile(fpath);
        tabwidget->setTabText(tabwidget->currentIndex(),QFileInfo(fpath).fileName());
        resetMainWindowTitle(fpath);
        recentFiles->prepend(fpath);
    }
}
//! File I/O

bool MainWindow::maybeSave(TextEditor *editor)
{
    if(editor->document()->isModified())
    {
        //! save changes?
        int ret = QMessageBox::warning(
                    this,
                    tr("Save Changes?"),                                            //title
                    tr("File has been modified,save changes?"),                     //text
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, //standard box
                    QMessageBox::Save                                               //default button
                    );
        switch(ret)
        {
        case QMessageBox::Save:
//            on_saveFile();
            editor->saveFile(editor->windowFilePath());
            return true;
        case QMessageBox::Discard:
            return true;
        case QMessageBox::Cancel:
            return false; //! cancle operaction,dont do anything
        default:
            //! shouls not reached
            break;
        }
    }
    return true; //! when textedit not modified
}

void MainWindow::on_TabWidget_tabCloseRequested(int index)
{
    if(index+1) //! tab index為-1表示不合法
    {
        TextEditor *select_editor = qobject_cast<TextEditor*>(tabwidget->widget(index));
        if(maybeSave(select_editor))    //! 關閉tab前詢問是否存檔
            tabwidget->removeTab(index);
//        if(tabwidget->isEmpty())    //! 如果關閉tab後tabwidget為空，新增一個tab
//            on_newFile();
    }
}

void MainWindow::on_TabWidget_currentChanged(int index)
{
    if(index+1) //! index is -1 if there isn't a new one
    {   //! 當tabwidget切換目前的tab時
        TextEditor *editor = currentTextEditor();
        setWindowModified(editor->document()->isModified());    //! 將視窗modified值同步
        resetMainWindowTitle(editor->windowFilePath()); //! 視窗標題同步檔案路徑
        updateMenus();  //! 透過這個函數可以切換menu裡針對editor同步的action
    }
}

void MainWindow::on_documentWasModified()
{
    //! 每一個editor在文件被修改時都會送出modificationChanged信號
    //! 但只有目前使用者選取的Tab才會影響視窗標題，使標題同步標示檔案是否修改
    TextEditor *editor = currentTextEditor();
    if(editor == nullptr)   //protect editor->document()
        return;
    else if(sender() == editor->document())
        setWindowModified(editor->document()->isModified());
}

//! click board
void MainWindow::on_UndoAct_triggered()
{
    if(TextEditor *editor = currentTextEditor()) editor->undo();
}

void MainWindow::on_RedoAct_triggered()
{
    if(TextEditor *editor = currentTextEditor()) editor->redo();
}

void MainWindow::on_cutAct_triggered()
{
    if(TextEditor *editor = currentTextEditor()) editor->cut();
}

void MainWindow::on_copyAct_triggered()
{
    if(TextEditor *editor = currentTextEditor()) editor->copy();
}

void MainWindow::on_pasteAct_triggered()
{
    if(TextEditor *editor = currentTextEditor()) editor->paste();
}

void MainWindow::on_deleteAct_triggered()
{
    if(TextEditor *editor = currentTextEditor()) editor->deleteSelectChar();
}

void MainWindow::on_selectAllAct_triggered()
{
    if(TextEditor *editor = currentTextEditor()) editor->selectAll();
}
//! click board

static inline bool isFileExist(const QString &filepath)
{   //!確保路徑: (1)存在 (2)是ㄧ個檔案
    return QFileInfo::exists(filepath) && QFileInfo(filepath).isFile();
}

//! Recent File
void MainWindow::on_openRecentFile()
{
   if(const QAction *act = qobject_cast<QAction*>(sender()))
   {
       const QString &filePath =  act->data().toString();
       if(isFileExist(filePath)){
           openFile(filePath);
       }else {
            QMessageBox::information(this,tr("Recent file."),
                                     tr("File has removed."),QMessageBox::Ok);
       }
   }
}

void MainWindow::on_menuRecentFile_aboutToShow()
{
    if(recentFiles->isEmpty()){
        //! 小心recent file被清空之後，settings裡面仍有資料(清空recentFiles有可能是使用者行為)
//        recentFiles->readSettings(settings);
        qDebug("MainWindow::on_menuRecentFile_aboutToShow() [Message]"
               "No recent Files in string list,check settings.");
    }

    const int count = qMin(int(maxRecent),recentFiles->size());
    int i=0;
    for (; i < count; ++i) {
        const QString &filepath = recentFiles->at(i);
        action_RecentFileArray[i]->setText(tr("&%1 %2").arg(i+1).arg(filepath));
        action_RecentFileArray[i]->setData(filepath);
        action_RecentFileArray[i]->setVisible(true);
    }
    for (;i<maxRecent;++i) {
        action_RecentFileArray[i]->setVisible(false);
    }
}

void MainWindow::on_clearRecentFileAct_triggered()
{
    recentFiles->clear(); //! clear recent file list
    on_menuRecentFile_aboutToShow();    //! clear open recent menu
    //    recentFiles->writeSettings(settings);
}
//! Recent File

//! Find & Replace
void MainWindow::on_FindOrReplaceAct_triggered()
{
    searchwidget->show();
}

void MainWindow::on_SearchWidget_findNextButtonClicked()
{
    TextEditor *editor = currentTextEditor();
    if(editor)
        searchwidget->findNext(editor);
}

void MainWindow::on_SearchWidget_findPreviousButtonClicked()
{
    TextEditor *editor = currentTextEditor();
    if(editor)
        searchwidget->findPrev(editor);
}

void MainWindow::on_SearchWidget_replaceButtonClicked()
{
    TextEditor *editor = currentTextEditor();
    if(editor)
        searchwidget->replace(editor);
}

void MainWindow::on_SearchWidget_replaceAllButtonClicked()
{
    TextEditor *editor = currentTextEditor();
    if(editor)
        searchwidget->replaceAll(editor);
}
//! Find & Replace

void MainWindow::on_setFont_triggered()
{
    TextEditor *editor = currentTextEditor();
    if(editor)
        editor->setFont(QFontDialog::getFont(nullptr, editor->font()));
}

//! reopen file with selected code
void MainWindow::on_reopenEncoding()
{
    if(const QAction *act = qobject_cast<QAction*>(sender()))
    {
        TextEditor *editor = currentTextEditor();
        if(!editor){
            qDebug() << "MainWindow::on_reopenEncoding:[Err]"
                     << "No current editor";
            ui->statusBar->showMessage(tr("No current editor"));
            return;
        } else if(editor->isUnTitled() | editor->document()->isModified()){
            qDebug() << "MainWindow::on_reopenEncoding:[Err]"
                     << "Only saved file could be encode.";
            ui->statusBar->showMessage(tr("Only saved file could be encode."));
            return;
        }

        const QString &filePath = editor->windowFilePath();
        const QByteArray &codeName = act->data().toByteArray();
        bool isSuccess = false;
        if(codeName == "Unicode")
            isSuccess = editor->openFile(filePath);
        else
            isSuccess = editor->openFile(filePath,
                                         QTextCodec::codecForName(codeName));

        if(isSuccess)
            ui->statusBar->showMessage(tr("Reopen with " + codeName));
    }
}

void MainWindow::on_saveEncoding()
{
    const QAction *act = qobject_cast<QAction*>(sender());
    if(!act)    return;

    TextEditor *editor = currentTextEditor();
    if(!editor){
        qDebug() << "MainWindow::on_saveEncoding:[Err]"
                 << "No current editor";
        ui->statusBar->showMessage(tr("No current editor"));
        return;
    }

    QString filePath = editor->windowFilePath();
    if(filePath.isEmpty()){
            filePath = QFileDialog::getSaveFileName(this,tr("Save As File"),
                                                    "D:/",tr("Text Files (*.txt)"));
    }

    const QByteArray codeName = act->data().toByteArray();
    bool isSuccess = editor->saveFile(filePath, QTextCodec::codecForName(codeName));

    if(isSuccess){
        tabwidget->setTabText(tabwidget->currentIndex(),QFileInfo(filePath).fileName());
        resetMainWindowTitle(filePath);
        recentFiles->prepend(filePath);
        ui->statusBar->showMessage(tr("Save file with code" + codeName));
    }
}

//! drag to load
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    //! brief: this call by mouse drag enter mainwindow
    //! check if files is text,then accept
    qDebug("MainWindow::dragEnterEvent()");
    //event->mimeData()->hasFormat("text/uri-list") //! 測試mimeData是否為文字型別
    const QString &fpath = event->mimeData()->urls().first().toLocalFile();
    if(QFileInfo(fpath).suffix() == QStringLiteral("txt"))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    qDebug("MainWindow::dropEvent()");
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())  return; //! 沒有拖曳檔案

    const QString &fpath = urls.first().toLocalFile();//! dragEnterEvent已經判定過
    openFile(fpath);
}
//! drag to load
