# Notepad project

###### tags: `C++11` `Qt5`

## 螢幕截圖 Screenshot

![image][Screenshot.png]

## 功能 Features

1. 多分頁編輯
2. 視窗標題(window title)跟隨分頁切換
3. 若當前的分頁被修改，會在視窗標題增加修改標示符"*"
4. 開啟新檔時會自動選擇最近開啟過的檔案
5. 使用`QSettings`記錄上一次的視窗大小與位置
6. 編輯功能: Undo,Redo,cut,copy,paste,delete,selectAll
7. Recent Files: 記錄10筆最近開啟的檔案
8. Drag to Load: 拖曳檔案至視窗讀取
9. Encoding: 針對不同編碼文件可以選擇"Reopen with Encoding"
10. Find & Replace: 搜尋 & 取代

## 施工中

- 雲端同步檔案
- 側邊欄檔案總管
- 行數顯示
- 自訂UI外觀(QSS stylesheet)
- 拖曳分頁開啟新視窗
- tabWidget功能(新增分頁鈕、平均分配分頁大小)
- language highlight: 支援.cpp,.java,.md等語法高亮
- UI多國語言翻譯
- terminal

## 編譯環境

- C++ 11
- Qt5.13.0 MinGW 32bit
- win7 32bit

## 參考 Reference

[Screenshot.png]:https://raw.githubusercontent.com/Loukei/portfolio/master/Notepad/DemoImage/Screenshot.png
