# GDriveApp project

###### tags: `C++11` `Qt5` `OAuth 2.0`

## 簡介 Introduction

目前Google Drive Api並沒有官方支援的C++ Lib，雖然網路上也有一些不錯的專案比如[googleQt](https://github.com/osoftteam/googleQt)或是[o2](https://github.com/pipacs/o2)等...。

一方面我們不能滿足於都用別人寫好的工具，另一方面我也想透過這個專案學習親手操作網路Api。

透過Qt OAuth2模組，利用Socket操作google drive api對本地上傳或下載檔案。

## 螢幕截圖 Screenshot

![image][Demo_mainwindow.png]

## 功能 Features

- OAuth2 帳號登入(須開啟瀏覽器)
![image][Demo_Login.gif]

- 取得使用者名稱與email

- 上傳檔案
![image][Demo_Upload.gif]

- 下載檔案，需要輸入fileID
![image][Demo_Download.gif]

## 自行編譯 Building App

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

## 如何使用 How to Use

專案的主要目的是開發可操作Google Drive的C++庫，因此介面的設計較為簡單，讓使用者更易理解類別的操作方式。

要在您的程式中使用GDrive Api，將GDriveLib加入專案中，按照[編譯](#自行編譯  Building App)的內容添加`oauthglobal.h`。

### 宣告類別實體

- 為了使用GDrive Api,在你的MainWindow中添加`GDrive::GDriveService class`
- 當您創建了一個`GDriveService`實體，它會立即依照`keyRedirectPort()`監聽對應的port，並在執行使用者登入的時候打開瀏覽器。

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
//! Step1: 建立一個取得認證後的slot函數
void MainWindow::onGDrive_granted(){
    QString msg = m_Drive->showInfo(); //回傳token
    // 其他UI操作...
}
//! Step2: 連接GDriveService::granted()信號
connect(m_Drive,&GDriveService::granted,
        this,&MainWindow::onGDrive_granted);
//! Step3: 在適當時機使用GDriveService::start()
m_Drive->start();
```

### 取得使用者名稱

``` c++
//! 1.使用getAbout()取得使用者資訊，在這裡是取得使用者名稱與email帳號
GDriveAbout* userAbout = m_Drive->getAbout(GDriveAbout::DISPLAYNAME|GDriveAbout::EMAILADDRESS);
//! 2.發出命令後，等待GDriveAbout發出received信號
connect(userAbout,&GDriveAbout::received,
        [userAbout,this](bool success){
    if(success){
        //! 3.透過GDriveAbout取得實際內容
        GDriveAboutResource resource = userAbout->getResource();
        ui->label->setText("Account: " + resource.displayName());
    }else {
        ui->label->setText("Unknown");
        ui->plainTextEdit->appendPlainText("About message Error.\n");
    }
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

## 未完成功能

- 記憶上次登入的帳號
- 斷點續傳功能
- 瀏覽google drive 文件目錄

## 編譯環境

- C++ 11
- Qt5.13.0 MinGW 32bit
- win7 32bit

## 參考 Reference

[Connecting your Qt application with Google Services using OAuth 2.0](https://www.qt.io/blog/2017/01/25/connecting-qt-application-google-services-using-oauth-2-0)

[Demo_mainwindow.png]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_mainwindow.png

[Demo_Login.gif]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Login.gif

[Demo_Upload.gif]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Upload.gif

[Demo_Download.gif]: https://raw.githubusercontent.com/Loukei/portfolio/master/GDriveApp/Demo_Image/Demo_Download.gif