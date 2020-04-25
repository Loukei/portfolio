#ifndef GOOGLEAUTHORIZATIONCODEFLOW_H
#define GOOGLEAUTHORIZATIONCODEFLOW_H
#include <QOAuth2AuthorizationCodeFlow>

/*!
 * \class GoogleAuthorizationCodeFlow
 * \brief The GoogleAuthorizationCodeFlow class provide google OAuth Authorization Code Flow
 *
 * - Google 的 OAuth
 * ## Reference
 * - [OAuth 2.0 for Web Server Applications](https://developers.google.com/identity/protocols/oauth2/web-server)
 */
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

    QString prompt() const;
    void setPrompt(const QString &prompt);

    QString access_type() const;
    void setAccess_type(const QString &access_type);

public slots:
    ///
    void refreshAccessToken();

protected:
    /// Builds an authentication URL using url and parameters.
    /// This function emits an authorizeWithBrowser() signal to require user interaction.
    /// To handle refresh token, parameters has been insert "access_type = offline"
    virtual void resourceOwnerAuthorization(const QUrl &url, const QVariantMap &parameters = QVariantMap()) override;

private:
    /// [Optional] A space-delimited, case-sensitive list of prompts to present the user.
    /// If you don't specify this parameter, the user will be prompted only the first time your project requests access.
    QString m_prompt = QString();
    /// [Recommended] Indicates whether your application can refresh access tokens when the user is not present at the browser.
    QString m_access_type = QString();

private slots:
    void on_refreshAccessToken_Finished();

};

#endif // GOOGLEAUTHORIZATIONCODEFLOW_H
