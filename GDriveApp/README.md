# GDriveApp project

###### tags: `C++11` `Qt5` `OAuth 2.0`

## 簡介(Introduction)

目前Google Drive Api並沒有官方支援的C++ Lib，雖然網路上也有一些不錯的專案比如[googleQt](https://github.com/osoftteam/googleQt)或是[o2](https://github.com/pipacs/o2)等...。

一方面我們不能滿足於都用別人寫好的工具，另一方面我也想透過這個專案學習親手操作網路Api。

透過Qt OAuth2模組，利用Socket操作google drive api對本地上傳或下載檔案。

## 螢幕截圖(Screenshot)

![image][Demo_mainwindow.png]

## 功能(Features)

- OAuth2 帳號登入(須開啟瀏覽器)
![image][Demo_Login.gif]

- 取得使用者名稱與email

- 上傳檔案
![image][Demo_Upload.gif]

- 下載檔案，需要輸入fileID
![image][Demo_Download.gif]

## 自行編譯(Building App)

1. 下載程式碼(src)
2. 安裝Qt5.13或更高版本，建議使用Qt creater
3. 請先確保Qt的網路模組可順利執行，您應該在步驟2中安裝Qt WebEngine與Qt NetWork Authorization模組
4. 要開發OAuth 2 App，您應該[向google申請自己的一組App](https://support.google.com/a/answer/162106?hl=zh-Hant)，並在[Google Api Console](https://console.developers.google.com/apis)下載Client_secret.json
5. 要使用OAuth，必須提供Client_secret.json中的參數，請將參數填入oauthglobal.h
6. 修改程式碼，在`googledriveservice.cpp`中include`oauthglobal.h`
7. 執行編譯

參數名稱(.json)     | 對應函數(.h)          | 說明
--------------------|:---------------------:|:---------------------
client_id           | keyClientId()         | 提供google辨識請求App
client_secret       | keyClientSecert()     | 
redirect_uris       | keyRedirectUri()      | 網頁重新導向的路徑位址

## 如何使用(How to Use)

專案的主要目的是開發可操作Google Drive的C++庫，因此介面的設計較為簡單，讓使用者更易理解類別的操作方式。

## 未完成功能

- 記憶上次登入的帳號
- 斷點續傳功能
- 瀏覽google drive 文件目錄

## 編譯環境(參考)

- C++ 11
- Qt5.13.0 MinGW 32bit
- win7 32bit

## 參考(Reference)

[Connecting your Qt application with Google Services using OAuth 2.0](https://www.qt.io/blog/2017/01/25/connecting-qt-application-google-services-using-oauth-2-0)

[Demo_mainwindow.png]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_mainwindow.png

[Demo_Login.gif]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Login.gif

[Demo_Upload.gif]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Upload.gif

[Demo_Download.gif]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Download.gif