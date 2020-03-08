# GDriveApp project
目前Google Drive Api並沒有官方支援的C++ Lib，雖然網路上也有一些不錯的專案比如[googleQt](https://github.com/osoftteam/googleQt)或是[o2](https://github.com/pipacs/o2)等...。

一方面我們不能滿足於都用別人寫好的工具，另一方面我也想透過這個專案學習親手操作網路Api。

透過Qt OAuth2模組，利用Socket操作google drive api對本地上傳或下載檔案。
![image][Demo_mainwindow.png]

## 功能
- OAuth2 帳號登入(須開啟瀏覽器)
![image][Demo_Login.gif]

- 取得使用者名稱與email

- 上傳檔案
![image][Demo_Upload.gif]

- 下載檔案，需要輸入fileID
![image][Demo_Download.gif]

## 未完成功能
- 記憶上次登入的帳號
- 斷點續傳功能
- 瀏覽google drive 文件目錄

## 編譯環境
- C++ 11
- Qt5.13.0 MinGW 32bit
- win7 32bit

[Demo_mainwindow.png]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_mainwindow.png

[Demo_Login.gif]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Login.gif

[Demo_Upload.gif]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Upload.gif

[Demo_Download.gif]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Download.gif