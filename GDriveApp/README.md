# GDriveApp project

###### tags: `C++11` `Qt5` `OAuth 2.0`

## 簡介 Introduction

目前Google Drive Api並沒有官方支援的C++ Lib，雖然網路上也有一些不錯的專案比如[googleQt][]或是[o2][]等...。

一方面我們不能滿足於都用別人寫好的工具，另一方面我也想透過這個專案學習親手操作網路API。

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

## 安裝環境

1. 安裝Qt5.13或更高版本
2. 確保Qt的網路模組可順利執行，安裝[Qt WebEngine][]與[Qt NetWork Authorization][]模組
3. 要開發Google OAuth 2 App，您應該[向google申請自己的一組App][Enable the Drive API]，並在[Google Api Console][]下載Client_secret.json
4. 將`src/GDriveLib`資料夾加入你的專案

[可選] 要使用OAuth，必須提供Client_secret.json中的參數，請將參數填入[oauthglobal.h][]，用來初始化

參數名稱(.json)     | 對應函數(.h)          | 說明
--------------------|:---------------------:|:---------------------
client_id           | keyClientId()         | 提供google辨識請求App
client_secret       | keyClientSecert()     | 
redirect_uris       | keyRedirectUri()      | 網頁重新導向的路徑位址

## 如何使用GDriveLib How to Use library

### 範例 Example

1. 在專案內加入必要的模組`QT += core gui network networkauth`
2. 將`src/GDriveLib`資料夾加入專案

``` C++
#include "GDriveLib/googledriveservice.h"

ExampleDialog::ExampleDialog(QWidget *parent)
    : QDialog(parent),m_currentOAuthToken(QString())
{
    m_Drive = new GDriveService(
        OAuth::keyAuthUri(),
        OAuth::keyTokenUri(),
        OAuth::keyClientId(),
        OAuth::keyClientSecert(),
        OAuth::keyScope(),
        OAuth::keyRedirectPort(),
        this);
    connect(m_Drive,&GDriveService::granted,
            this,&ExampleDialog::onGDrive_granted);
    connect(m_Drive,&GDriveService::error,
            this,&ExampleDialog::onGDrive_error);
    connect(m_Drive,&GDriveService::tokenChanged,
            this,&ExampleDialog::onGDrive_tokenChanged);
    // setup UI...
}
// login user
void ExampleDialog::login()
{
    m_Drive->grant();
}
// logout
void ExampleDialog::logout()
{
    m_Drive->setToken("");
    m_Drive->setRefreshToken("");
}
// simple upload local file
void ExampleDialog::uploadFile()
{
    const QString filepath = "D:/Download/testdata/soviet example.txt";
    auto task = m_Drive->fileSimpleCreate(filepath);
    if(!task->start()){
        /* For upload/download/update task, use start() to check the relate file has open or not. */
        m_textbrowser->append(filepath + " Simple Upload error:" + task->errorString());
        task->deleteLater();
        return;
    }
    auto onUploadreceive = [task,this,filepath](){
        if(task->isComplete() && !task->isFailed()){
            m_textbrowser->append(filepath + " Simple Upload Success.\n");
        }else {
            m_textbrowser->append(filepath + " Simple Upload error:" + task->errorString());
        }
        task->deleteLater();
    };
    connect(task,&GDriveFileTask::finished,
            this,onUploadreceive);
}
void ExampleDialog::onGDrive_granted()
{
    qDebug() << "Token: " << m_Drive->token()
            << "Refresh token:" << m_Drive->refreshToken();
}
void ExampleDialog::onGDrive_error(const QString &error, const QString &errorDescription, const QUrl &uri)
{
    /* handle error here */
    QVariantMap info;
    info.insert(QStringLiteral("Error"),error);
    info.insert(QStringLiteral("Description"),errorDescription);
    info.insert(QStringLiteral("Uri"),uri);
    qDebug() << info;
}
void ExampleDialog::onGDrive_tokenChanged(const QString &token)
{
    /* m_currentOAuthToken = {}, token = "..." => login
     * m_currentOAuthToken = "a...", token = "b..." => refresh token or switch account
     * m_currentOAuthToken = "...", token = {} => logout */
    if(token.isEmpty()){
        qDebug() << "token isEmpty -> logout";
    }else if (m_currentOAuthToken.isEmpty()) {
        qDebug() << "m_currentOAuthToken isEmpty() && token !isEmpty() -> login";
    }else if (token != m_currentOAuthToken) {
        qDebug() << "token != m_currentOAuthToken -> refresh or switch account";
    }
    m_currentOAuthToken = token; /*change current token*/
}
```

### 說明

#### 設定OAuth api

建立一個`GDriveService`物件需要以下幾個參數:

``` c++
QUrl authorizationUrl
QUrl accessTokenUrl
QString clientIdentifier
QString clientIdentifierSharedKey
QString scope
quint16 port
```

可以從你的google api設定裡填入，或是使用[oauthglobal.h][]幫助

#### 登入

``` c++
void GDriveService::grant();
```

#### 登出

``` c++
// 清除Access token與Refresh token
GDriveService::setToken("");
GDriveService::setRefreshToken("");
```

#### 上傳檔案

``` c++
GDriveFileSimpleCreate *GDriveService::fileSimpleCreate(const QString &filepath)
```

API使用任務導向操作，物件內部會發出http socket、處理上傳流程與回傳結果

``` c++
auto task = m_Drive->fileSimpleCreate(filepath);
```

針對檔案上傳/下載/更新的動作需要開啟本地端檔案，為了確保在網路請求發出前做出確認，這類Task使用`bool start()`進行檢查，如果傳入的檔案路徑開啟成功就會送出請求，反之會失敗並回傳false

``` c++
if(!task->start()){
    // 印出失敗訊息...
    qDebug() << "file open fail";
    task->deleteLater();
    return;
}
```

連接`finished()`信號，準備接收回傳結果，當任務完成或失敗都會產生信號

``` c++
connect(task,&GDriveFileTask::finished,this,onUploadreceive);
```

準備一個函式負責處理task的結果

`isComplete()`表示任務是否完成

`isFailed()`表示任務是否失敗

使用`QByteArray getReplyString()`接收回傳內容
> 當任務完成後，使用`QObject::deleteLater()`刪除資料，不要使用`delete`

``` c++
auto onUploadreceive = [task,this,filepath](){
    if(task->isComplete() && !task->isFailed()){
        qDebug() << filepath + " Simple Upload Success.";
    }else {
        qDebug() << filepath + " Simple Upload error:" + task->errorString();
    }
    task->deleteLater();
};
```

額外: 如果需要顯示上傳/下載進度，參考[QNetworkReply::uploadProgress][]

``` c++
void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
```

以範例中的情況就是使用 `GDriveFileSimpleCreate::uploadProgress`

#### 信號處理

- 當認證流程成功或者access token更新，`GDriveService::granted()`會發出，用來確認可以執行OAuth操作。

- 發生認證問題時使用`GDriveService::error(const QString &error, const QString &errorDescription, const QUrl &uri)`信號處裡。

- 利用`GDriveService::tokenChanged(const QString &token)`處理access token的變化。

- `GDriveService`相關的信號說明可以參照[QOAuth2AuthorizationCodeFlow][]

#### 偵測登入/登出/更新Token

OAuth 2並沒有設定登出的機制，實際上要取消登入只要刪除本地端的Refresh token與access token即可。

開發者不會希望在沒有access token的情況下呼叫網路作業，這會導致往程式錯誤甚至崩潰。

可以使用`GDriveService::tokenChanged()`以及紀錄前一個token的內容來開啟/關閉網路操作。

當token改變時，通過與前一個token之間的比較我們可以了解目前的狀態

- token = {}, 則為logout，關閉相關的網路操作，並開啟login選項。
- token != {} 且 current_token != {}，代表token更新
- token != {} 且 current_token = {}，代表現在已登入，關閉login選項

> 不要使用`GDriveService::granted()`判定是否登入，因為更新access token時也會觸發此信號

#### 保存使用者帳號，簡化登入操作

為了方便APP使用者無須重複登入google取得授權，你可以選擇保存[refresh token][]在本地端並加密內容，下一次程式開啟時載入refresh token，並呼叫`GDriveService::refreshAccessToken()`取得access token，完成登入操作。

``` c++
m_Drive->setRefreshToken(refreshToken);
m_Drive->refreshAccessToken(); // use refresh token to login
```

## 編譯環境 My compile environment

- C++ ver: C++ 11
- Qt kits: Qt5.13.2 MinGW 32bit
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

[QOAuth2AuthorizationCodeFlow]: https://doc.qt.io/qt-5/qoauth2authorizationcodeflow.html

[refresh token]: https://developers.google.com/identity/protocols/oauth2/web-server#creatingclient

[QNetworkReply::uploadProgress]: https://doc.qt.io/qt-5/qnetworkreply.html#uploadProgress
