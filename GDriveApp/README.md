# GDriveApp project

###### tags: `C++11` `Qt5` `OAuth 2.0`

## 簡介 Introduction

目前Google Drive Api並沒有官方支援的C++ Lib，雖然網路上也有一些不錯的專案比如[googleQt][]或是[o2][]等...。

一方面我們不能滿足於都用別人寫好的工具，另一方面我也想透過這個專案學習親手操作網路Api。

透過Qt OAuth2模組，利用Socket操作google drive api對本地上傳或下載檔案。

## 螢幕截圖 Screenshot

<img src = "https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_mainwindow(Login).png" width="400" height="300" />

## 功能 Features

- OAuth2 帳號登入(須開啟瀏覽器)

- 取得使用者名稱與email

<img src = "https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Login.gif" width="400" height="300" />

- 上傳檔案

<img src = "https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_UploadFile.gif" width="400" height="300" />

- 下載檔案，需要輸入fileID

<img src = "https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_DownloadFile.gif" width="400" height="300" />

- 搜尋，取得檔案fileID

<img src = "https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_SearchFile.gif" width="400" height="300" />

## 自行編譯 Building App

1. 下載程式碼(src)
2. 安裝Qt5.13或更高版本，建議使用Qt creater
3. 請先確保Qt的網路模組可順利執行，您應該在步驟2中安裝[Qt WebEngine][]與[Qt NetWork Authorization][]模組
4. 要開發OAuth 2 App，您應該[向google申請自己的一組App][Enable the Drive API]，並在[Google Api Console][]下載Client_secret.json
5. 將`src/GDriveLib`資料夾加入你的專案
[可選] 要使用OAuth，必須提供Client_secret.json中的參數，請將參數填入[oauthglobal.h][]，用來初始化

參數名稱(.json)     | 對應函數(.h)          | 說明
--------------------|:---------------------:|:---------------------
client_id           | keyClientId()         | 提供google辨識請求App
client_secret       | keyClientSecert()     | 
redirect_uris       | keyRedirectUri()      | 網頁重新導向的路徑位址

## 如何使用 How to Use

專案的主要目的是開發可操作Google Drive的C++庫，因此介面的設計較為簡單，讓使用者更易理解類別的操作方式。

要在您的程式中使用GDrive Api，將GDriveLib加入專案中，按照[編譯](#自行編譯  Building App)的內容添加`oauthglobal.h`。

### 宣告類別實體

- 為了使用GDrive Api,在你的MainWindow中添加`GDrive::GDriveService class`
- 創建一個`GDriveService`實體，它會立即依照`OAuth::keyRedirectPort()`監聽對應的port，並在執行使用者登入的時候打開瀏覽器。

``` c++
// mainwindow.h
class MainWindow{
private:
    GDrive::GDriveService *m_Drive;
}
// mainwindow.cpp
MainWindow::MainWindow(QWidget *parent){
    m_Drive = new GDriveService(this);
}
```

### 執行認證

``` c++
// Step1: 建立一個取得認證後的slot函數
void MainWindow::onGDrive_granted(){
    const QString token = m_Drive->token(); //回傳token
    const QString refreshToken = m_Drive->refreshToken(); //回傳Refresh token
    // 其他UI操作...
}
// Step2: 連接GDriveService::granted() signal
connect(m_Drive,&GDriveService::granted,
        this,&MainWindow::onGDrive_granted);
// Step3: 在適當時機使用GDriveService::start()，觸發GDriveService::granted() signal
m_Drive->start();
```

### 取得使用者名稱

``` c++
// 1.使用getAbout()取得使用者資訊
GDriveAbout* userAbout = m_Drive->getAbout(QString("user"));
// 2.發出命令後，等待GDriveAbout發出received信號
connect(userAbout,&GDriveAbout::received,
        [userAbout,this](bool success){
    if(success){
        // 3.透過GDriveAbout取得實際內容
        GDriveAboutResource resource = userAbout->getResource();
        const QString displayName = resource.user_displayName();
        const QString emailAddress = resource.user_emailAddress();
        const QString photoLink = resource.user_photoLink();
        // 3.2. 你也可以使用getReplyString()取得完整的Json回傳，自行處理
        QByteArray reply = userAbout->getReplyString();
    }else {
        ui->plainTextEdit->appendPlainText("About message Error.\n");
    }
    // 4. 使用deleteLater()刪除任務
    userAbout->deleteLater();
});
```

### 上傳檔案

1.透過fileXXXCreate來執行上傳任務，這個指令會在雲端開新檔案

``` c++
GDriveFileSimpleCreate *GDriveService::fileSimpleCreate(const QString &filepath);
GDriveFileMultipartCreate *GDriveService::fileMultipartCreate(const QString &filepath);
GDriveFileResumableCreate *GDriveService::fileResumableCreate(const QString &filepath);
```

以上三個回傳都使用`GDriveFileTask`作為父類別

2.`GDriveFileTask`物件一旦生成就會開始執行檔案相關的任務

``` c++
/// GDriveFileTask class
public:
    QString errorString() const;    ///回傳可讀的錯誤訊息
    bool isComplete() const;        ///查詢任務是否完成，預設為false
    bool isFailed() const;          ///查詢任務是否失敗，預設為false
signals:
    void finished(); ///任務完成或失敗時都會發出signal
```

3.以`getResource()`方法取得檔案上傳後的matadata，`GDriveFileResource`是ㄧ個純資料的類別

``` c++
GDriveFileResource getResource() const;
```

4.`GDriveFileTask`物件完成任務後，請在對應的slot使用`deleteLater()`刪除

#### 上傳範例

``` c++
void MainWindow::fileSimpleUpload(const QString &filepath)
{
    auto task = m_Drive->fileSimpleCreate(filepath);
    auto onUploadreceive = [task,filepath](){
        if(task->isComplete() && !task->isFailed()){
            qDebug() << filepath + " Simple Upload Success.\n";
        }else {
            qDebug() << filepath + " Simple Upload error:" + task->errorString();
        }
        task->deleteLater();
    };
    connect(task,&GDriveFileTask::finished,
            this,onUploadreceive);
}
```

### 下載檔案

Google Drive Api對檔案的辨識不是依靠檔名或路徑，而是依靠fileId參數。
要取得fileId最簡單的做法是選取欲下載的檔案>點選右鍵>取得檔案共用連結>選擇URL中id="..."的部份

1.創建`QFile`實體並指定本地端儲存路徑

2.使用`fileDownload()`方法下載檔案

``` c++
GDriveFileDownloader *GDriveService::fileDownload(const QString &fileId, const QString &fields, QSharedPointer<QFile> file)
```

`fileID`表示指定的雲端檔案。
`fields`表示接受的回傳元資料，例如"id,name"表示回傳fileID與檔名。
`file`表示下載的檔案，使用`QSharedPointer`傳入

3.等待`GDriveFileDownloader`發出`finished`信號即結束下載。

#### 下載範例

``` c++
void MainWindow::fileDownload(const QString &downloadFilePath, const QString &fileId)
{
    QSharedPointer<QFile> writeFile(new QFile(downloadFilePath,this),&QFile::deleteLater);
    auto task = m_Drive->fileDownload(fileId,"id,name",writeFile);
    connect(task,&GDriveFileTask::finished,
            this,[task,downloadFilePath](){
        if(task->isComplete() && !task->isFailed()){
            qDebug() << downloadFilePath + " Download complete.\n";
        }else {
            qDebug() << downloadFilePath + " Download error:" + task->errorString();
        }
        task->deleteLater();
    });
}
```

### 搜尋檔案&資料夾

對應Google Drive Api的`Files: list`方法

1. 使用`GDriveFileSearch *GDriveService::fileList(QString &q, QString &pageToken)`搜尋檔案。

參數名稱    | 用途
------------|:-------------------------------------------
q           | 過濾搜尋的內容
pageToken   | 如果搜尋內容過多，會以此參數顯示下一筆搜尋內容

## 編譯環境 My compile environment

- C++ ver: C++ 11
- Qt kits: Qt5.13.0 MinGW 32bit
- OS: win7 32bit

## Credit

- [QJsonModel](https://github.com/dridk/QJsonModel)

- [SimpleCrypt](https://wiki.qt.io/Simple_encryption_with_SimpleCrypt)

## 參考 Reference

1. [Connecting your Qt application with Google Services using OAuth 2.0](https://www.qt.io/blog/2017/01/25/connecting-qt-application-google-services-using-oauth-2-0)

2. [Download files](https://developers.google.com/drive/api/v3/manage-downloads)

3. [Upload files](https://developers.google.com/drive/api/v3/manage-uploads)

4. [Files: list](https://developers.google.com/drive/api/v3/reference/files/list)

5. [Search for files and folders](https://developers.google.com/drive/api/v3/search-files)

[googleQt]: https://github.com/osoftteam/googleQt

[o2]: https://github.com/pipacs/o2

[Qt WebEngine]: https://doc.qt.io/qt-5/qtwebengine-index.html

[Qt NetWork Authorization]: https://doc.qt.io/qt-5/qtnetworkauth-index.html

[Enable the Drive API]: https://developers.google.com/drive/api/v3/enable-drive-api

[Google Api Console]: https://console.developers.google.com/apis

[oauthglobal.h]: https://github.com/Loukei/portfolio/blob/master/GDriveApp/oauthglobal.h
