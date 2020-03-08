# 簡介

## How to start

1. Go to Google Api console and create your App
2. Download your client secreat file
3. Create `oauthglobal.h` file like below

``` c++
namespace OAuth {
const QString keyClientId(){
    return "YOUR_CLIENT_ID";
}
const QString keyClientSecert(){
    return "YOUR_CLIENT_SECERT";
}
const QUrl keyRedirectUri(){
    return QUrl("http://127.0.0.1:8080/");
}
quint16 keyRedirectPort(){
    return static_cast<quint16>(keyRedirectUri().port());
}
const QString keyScope(){
    return "https://www.googleapis.com/auth/drive.file";
}
const QString keyAuthUri(){
    return "https://accounts.google.com/o/oauth2/auth";
}
const QString keyTokenUri(){
    return "https://oauth2.googleapis.com/token";
}
} //namespace OAuth
```

4. `oauthglobal.h` should include in `googledriveservice.cpp` ,to provide OAuth2 parameter
