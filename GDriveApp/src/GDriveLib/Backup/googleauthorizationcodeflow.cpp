#include "googleauthorizationcodeflow.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QAbstractOAuthReplyHandler>
#include <QAuthenticator>

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

QString GoogleAuthorizationCodeFlow::prompt() const
{
    return m_prompt;
}

void GoogleAuthorizationCodeFlow::setPrompt(const QString &prompt)
{
    m_prompt = prompt;
}

QString GoogleAuthorizationCodeFlow::access_type() const
{
    return m_access_type;
}

void GoogleAuthorizationCodeFlow::setAccess_type(const QString &access_type)
{
    m_access_type = access_type;
}

void GoogleAuthorizationCodeFlow::refreshAccessToken()
{
    if(this->refreshToken().isEmpty()){
//        qCWarning(d->loggingCategory, "Cannot refresh access token. Empty refresh token");
        qWarning("Cannot refresh access token. Empty refresh token");
        return;
    }
    if(this->status() == Status::RefreshingToken){
//        qCWarning(d->loggingCategory, "Cannot refresh access token. "
//                                             "Refresh Access Token is already in progress");
        qWarning( "Cannot refresh access token. "
                  "Refresh Access Token is already in progress");
        return;
    }

    QNetworkRequest request(this->accessTokenUrl());
    /* Setup body */
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("client_id"),this->clientIdentifier());
    query.addQueryItem(QStringLiteral("client_secret"),this->clientIdentifierSharedKey());
    query.addQueryItem(QStringLiteral("grant_type"),QStringLiteral("refresh_token"));
//    query.addQueryItem("redirect_uri",);
    query.addQueryItem(QStringLiteral("refresh_token"),this->refreshToken());
    const QString data = query.toString(QUrl::FullyDecoded);
    /* Setup Header */
    request.setRawHeader(QByteArray("Host"),
                         this->accessTokenUrl().host().toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      data.length());
    /* Setup status */
    this->setStatus(Status::RefreshingToken);
    /* send request and connection slots */
    QNetworkReply *reply = this->networkAccessManager()->post(request,data.toUtf8());
    QAbstractOAuthReplyHandler *handler = replyHandler();
    connect(reply, &QNetworkReply::finished,
                [handler, reply]() { handler->networkReplyFinished(reply); });
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    connect(handler, &QAbstractOAuthReplyHandler::tokensReceived,
            this, &GoogleAuthorizationCodeFlow::accessTokenRequestFinished,
            Qt::UniqueConnection);
    connect(networkAccessManager(),&QNetworkAccessManager::authenticationRequired,
            this, &GoogleAuthorizationCodeFlow::authenticate,
            Qt::UniqueConnection);
}

void GoogleAuthorizationCodeFlow::resourceOwnerAuthorization(const QUrl &url, const QVariantMap &parameters)
{
    //! for get refersh token, Insert parameter below:
    QVariantMap param(parameters);
    if(!m_access_type.isEmpty()){
        param.insert("access_type","offline");
    }
    if(!m_prompt.isEmpty()){
        param.insert("prompt","consent");
    }
    return QOAuth2AuthorizationCodeFlow::resourceOwnerAuthorization(url,param);
}

void GoogleAuthorizationCodeFlow::accessTokenRequestFinished(const QVariantMap &values)
{
//    Q_Q(QOAuth2AuthorizationCodeFlow);
//    using Key = QAbstractOAuth2Private::OAuth2KeyString;
//    if (values.contains(Key::error)) {
//        const QString error = values.value(Key::error).toString();
//        qCWarning(loggingCategory, "Error: %s", qPrintable(error));
//        return;
//    }
//    bool ok;
//    const QString accessToken = values.value(Key::accessToken).toString();
//    tokenType = values.value(Key::tokenType).toString();
//    int expiresIn = values.value(Key::expiresIn).toInt(&ok);
//    if (!ok)
//        expiresIn = -1;
//    if (values.value(Key::refreshToken).isValid())
//        q->setRefreshToken(values.value(Key::refreshToken).toString());
//    scope = values.value(Key::scope).toString();
//    if (accessToken.isEmpty()) {
//        qCWarning(loggingCategory, "Access token not received");
//        return;
//    }
//    q->setToken(accessToken);
//    const QDateTime currentDateTime = QDateTime::currentDateTime();
//    if (expiresIn > 0 && currentDateTime.secsTo(expiresAt) != expiresIn) {
//        expiresAt = currentDateTime.addSecs(expiresIn);
//        Q_EMIT q->expirationAtChanged(expiresAt);
//    }
//    setStatus(QAbstractOAuth::Status::Granted);

}

void GoogleAuthorizationCodeFlow::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
{
    if (reply == currentReply){
        const auto url = reply->url();
        if (url == this->accessTokenUrl()) {
            authenticator->setUser(this->clientIdentifier());
            authenticator->setPassword(QString());
        }
    }
}

