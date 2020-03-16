#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

/*Features*/
//* new/open/save/saveas file
//* drag to open file
//* cut/copy/post/
//* remarber opened file
//* new window
//* tab view
//* search text

/*Bugs*/
//*開啟檔案的過程中不會記得上一次的檔案開啟路徑

/*支援分頁、存讀檔、記憶以前的舊檔(關閉重開)功能、文字編輯(複製貼上)
            、字形大小及顏色、回復動作、字串搜尋、自動存檔、多國語言介面、雲端備份(高級)*/
