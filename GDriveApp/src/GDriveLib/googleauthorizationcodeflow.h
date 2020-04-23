#ifndef GOOGLEAUTHORIZATIONCODEFLOW_H
#define GOOGLEAUTHORIZATIONCODEFLOW_H
#include <QOAuth2AuthorizationCodeFlow>

class GoogleAuthorizationCodeFlow : public QOAuth2AuthorizationCodeFlow
{
public:
    explicit GoogleAuthorizationCodeFlow(QObject *parent = nullptr);
    explicit GoogleAuthorizationCodeFlow(QNetworkAccessManager *manager,
                                          QObject *parent = nullptr);

    GoogleAuthorizationCodeFlow(const QString &clientIdentifier,
                                 QNetworkAccessManager *manager,
                                 QObject *parent = nullptr);

    GoogleAuthorizationCodeFlow(const QUrl &authorizationUrl,
                                 const QUrl &accessTokenUrl,
                                 QNetworkAccessManager *manager,
                                 QObject *parent = nullptr);

    GoogleAuthorizationCodeFlow(const QString &clientIdentifier,
                                 const QUrl &authorizationUrl,
                                 const QUrl &accessTokenUrl,
                                 QNetworkAccessManager *manager,
                                 QObject *parent = nullptr);

    virtual ~GoogleAuthorizationCodeFlow() override;

protected:
    /// Builds an authentication URL using url and parameters.
    /// This function emits an authorizeWithBrowser() signal to require user interaction.
    /// To handle refresh token, parameters has been insert "access_type = offline"
    virtual void resourceOwnerAuthorization(const QUrl &url, const QVariantMap &parameters = QVariantMap()) override;

};

#endif // GOOGLEAUTHORIZATIONCODEFLOW_H
