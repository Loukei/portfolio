#ifndef OAUTHGLOBAL_H
#define OAUTHGLOBAL_H

#include <QString>
#include <QUrl>
/// OAuth app settings
namespace OAuth {
///client_id
inline const QString keyClientId(){
    return "YOUR_CLIENT_ID";
}
///client_secret
inline const QString keyClientSecert(){
    return "YOUR_CLIENT_SECERT";
}
///redirect_uris
inline const QUrl keyRedirectUri(){
    return QUrl("YOUR_REDIRECT_URI");
}

inline quint16 keyRedirectPort(){
    return static_cast<quint16>(keyRedirectUri().port());
}

inline const QString keyScope(){
    //! file scope only able to download file form api uploaded
    //! refernece: https://developers.google.com/drive/api/v3/about-auth
    return "https://www.googleapis.com/auth/drive.file";
}
///auth_uri
inline const QString keyAuthUri(){
    return "https://accounts.google.com/o/oauth2/auth";
}
///token_uri
inline const QString keyTokenUri(){
    return "https://oauth2.googleapis.com/token";
}
} //namespace OAuth
#endif // OAUTHGLOBAL_H
