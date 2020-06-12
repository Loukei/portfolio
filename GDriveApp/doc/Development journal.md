# Develop Journal

## Note

### 5/30

本想做一個類似Google網頁中登入帳號後會顯示名稱與簡易選項的UI，考慮之後決定使用簡易的QMenu來做。
在QMenu中可以插入QWidget來顯示，需要透過QWidgetAction來完成。

``` c++
CustomWidget *cwidget = new CustomWidget(menu);
QWidgetAction *action = new QWidgetAction(menu);
action->setDefaultWidget(cwidget);
```

需要注意的是，選擇插入的widget發生大小變化時，所在的QMenu本身不會跟著適應大小變化。
也就是說，假如cwidget內部元件hide時，本身在menu裡大小不會改變

針對這個問題，我個人嘗試過`QWidget::adjustSize()` `QWidget::updateGeometry()`等，均沒有用，後來發現需要個別加入widget,即**每一個label，都需要加入一個widgetaction，如此一來每個action單獨hide時，對應的label就會被正確的隱藏並更新Menu的大小**

``` c++
label_UserIcon = new QLabel(parrent);
label_UserName = new QLabel(parrent);
label_UserEmail = new QLabel(parrent);

action_UserIcon = new QWidgetAction(parrent);
action_UserName = new QWidgetAction(parrent);
action_UserEmail = new QWidgetAction(parrent);

action_UserIcon->setDefaultWidget(label_UserIcon);
action_UserName->setDefaultWidget(label_UserName);
action_UserEmail->setDefaultWidget(label_UserEmail);

action_UserName->setVisible(false);
action_UserEmail->setVisible(false);
```

### 6/9

修改`GDriveService`類別，必須把OAuth相關的參數開個介面，由類別外部來設定。
而`QAbstractOAuth::setModifyParametersFunction(const QAbstractOAuth::ModifyParametersFunction &modifyParametersFunction)`這個部分需要一個靜態函數，但是這個函數的內容需要在OAuth參數設定之後才能取得，換言之，只有在`GDriveService`類別實體建構後才能得知`ModifyParametersFunction`的內容。

解決辦法是利用一個類別成員構建lambda function，而此構建函式直到取得Client Id與 Client Secreat之後才呼叫

``` c++
QAbstractOAuth::ModifyParametersFunction GDriveService::buildModifyParametersFunction()
{
    const QUrl clientIdentifier = m_google->clientIdentifier();
    const QUrl clientIdentifierSharedKey = m_google->clientIdentifierSharedKey();
    return [clientIdentifier,clientIdentifierSharedKey]
            (QAbstractOAuth::Stage stage, QVariantMap *parameters){
        if(stage == QAbstractOAuth::Stage::RequestingAuthorization){
            /* setup Authoriation Url here */
            parameters->insert("access_type","offline"); /* Request refresh token*/
            parameters->insert("prompt","consent"); /* force user check scope again */
        }
        if(stage == QAbstractOAuth::Stage::RefreshingAccessToken){
            parameters->insert("client_id",clientIdentifier);
            parameters->insert("client_secret",clientIdentifierSharedKey);
        }
    };
}
```

### 6/11

花了兩天的時間把所有的task參數的介面進行修改，在這裡先提一下背景。

Google Drive Api使用HTTP socket的方式操作，開發者將操作指令依據Google提供的文件包裝成Http socket發出，並根據回傳的內容進一步處理。
我使用類別的方式包裝所有Google Drive的操作，試圖利用簡單的參數傳遞方式完成所有上傳/下載/查詢的需求。

`GDriveFileSimpleCreate`類別負責上傳檔案，因此，最簡易的操作方法如下
`GDriveFileSimpleCreate(const QString &filepath,QOAuth2AuthorizationCodeFlow *parent)`

這裡提供了一個檔案路徑變數用以指定上傳的檔案，以及一個`QOAuth2AuthorizationCodeFlow`做為parent object，同時Http socket也是由它負責發送，因為上傳需要附加access token等參數。

但是上傳檔案的參數其實不只這些，不含檔案路徑，上傳檔案需要8個URL參數，其中6個是可選參數，如果將所有參數都要求API使用者提供會造成使用者記憶困難，同時大量的參數會欄位會降低閱讀程式碼的意願。

<API design for c++>一書揭示了一個重要原則: 針對一般使用者提供簡單易用API，對專家使用者提供進階的API，而前者通常是後者的封裝版本。

為了包裝上傳檔案的URL參數，我使用`QUrlQuery`這個現成類別，因為它本身是Qt的自然型別，具有容納各種URL參數的能力，同時也可以供使用者檢視內容與方便設定。
因此我的`GDriveFileSimpleCreate`就分成兩個部分

``` c++
GDriveFileSimpleCreate(const QString &filepath,const QUrlQuery &args,QOAuth2AuthorizationCodeFlow *parent);

static QUrlQuery buildUrlArgs(const bool enforceSingleParent = false,
                            const bool ignoreDefaultVisibility = false,
                            const bool keepRevisionForever = false,
                            const QString &ocrLanguage = QString(),
                            const bool supportsAllDrives = false,
                            const bool useContentAsIndexableText = false);
```

利用`QUrlQuery`保存可選的URL參數，並提供一個靜態方法讓使用者快速構建所需的`QUrlQuery`項目。

這裡有幾點注意:(1)OAuth的參數(api key,access_token)不能包含在內，因為只有執行時才能確定(2)必填的參數因為不能省略，使用者可能忘記瑱入args，比如fileID(3)固定值的參數不適合出現在建構式的參數列內，比如alt=media等。

實際的Url內容透過`QUrl buildUrl(const QString &uploadType,const QString &access_token,QUrlQuery args) const`產生，這裡會填入比如uploadtype與access_token等缺少的內容，必要的URL參數會不會隱藏在buildUrl()內部，儘管我們可以這麼做，但是顯示在函式簽名上可以減少後續修改產生的bug。

## Todo

* 使用login/logot signal取代 tokenchanged
* 對任務提供progress/abort/pause 方法
* 把Dialog裡的treeview結合到MainWindow裡
* 考慮將task模組化:建構Url、建構request、處理reply、
* 考慮提供[sync方法](https://www.devbean.net/2013/11/qt-study-road-2-access-network-4/)
* Login的按鈕在等待認證時應該暫時鎖住，直到認證通過/不通過才回復，觀察一下是否有對應的signal

## Reference

https://developers.google.com/drive/api/v3/reference/files/create