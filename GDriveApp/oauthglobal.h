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
///file scope only able to download file form api uploaded
inline const QString keyScope(){
    /* https://developers.google.com/drive/api/v3/about-auth#select_scopes_for_a_new_app */
    return "https://www.googleapis.com/auth/drive.file";
}
///auth_uri
inline const QUrl keyAuthUri(){
    return QUrl("https://accounts.google.com/o/oauth2/auth");
}
///token_uri
inline const QUrl keyTokenUri(){
    return QUrl("https://oauth2.googleapis.com/token");
}
} //namespace OAuth
#endif // OAUTHGLOBAL_H
