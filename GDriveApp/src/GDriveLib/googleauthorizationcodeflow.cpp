#include "googleauthorizationcodeflow.h"
#include <QDebug>

GoogleAuthorizationCodeFlow::GoogleAuthorizationCodeFlow(QObject *parent)
    : QOAuth2AuthorizationCodeFlow(parent)
{

}

GoogleAuthorizationCodeFlow::GoogleAuthorizationCodeFlow(QNetworkAccessManager *manager, QObject *parent)
    : QOAuth2AuthorizationCodeFlow(manager,parent)
{

}

GoogleAuthorizationCodeFlow::GoogleAuthorizationCodeFlow(const QString &clientIdentifier,
                                                         QNetworkAccessManager *manager, QObject *parent)
    : QOAuth2AuthorizationCodeFlow(clientIdentifier,manager,parent)
{

}

GoogleAuthorizationCodeFlow::GoogleAuthorizationCodeFlow(const QUrl &authorizationUrl,
                                                         const QUrl &accessTokenUrl,
                                                         QNetworkAccessManager *manager,
                                                         QObject *parent)
    : QOAuth2AuthorizationCodeFlow(authorizationUrl,accessTokenUrl,manager,parent)
{

}

GoogleAuthorizationCodeFlow::GoogleAuthorizationCodeFlow(const QString &clientIdentifier,
                                                         const QUrl &authorizationUrl,
                                                         const QUrl &accessTokenUrl,
                                                         QNetworkAccessManager *manager,
                                                         QObject *parent)
    : QOAuth2AuthorizationCodeFlow(clientIdentifier,authorizationUrl,accessTokenUrl,manager,parent)
{

}

GoogleAuthorizationCodeFlow::~GoogleAuthorizationCodeFlow()
{

}

void GoogleAuthorizationCodeFlow::resourceOwnerAuthorization(const QUrl &url, const QVariantMap &parameters)
{
    qInfo() << Q_FUNC_INFO
            << "url: " << url
            << "parameters: " << parameters;
    //! for get refersh token, Insert parameter below:
    QVariantMap param(parameters);
    param.insert("access_type","offline"); //online / offline，為了取得 refresh token，需要取得offline權限
    param.insert("prompt","consent"); //是否每次授權時，都跳出授權視窗，在此範例建議開啟
    return QOAuth2AuthorizationCodeFlow::resourceOwnerAuthorization(url,param);
}
