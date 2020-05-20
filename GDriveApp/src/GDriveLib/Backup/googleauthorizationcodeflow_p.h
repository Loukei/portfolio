#ifndef GOOGLEAUTHORIZATIONCODEFLOW_P_H
#define GOOGLEAUTHORIZATIONCODEFLOW_P_H

#include "GDriveLib/googleauthorizationcodeflow.h"
#include <QtNetworkAuth/private/qoauth2authorizationcodeflow_p.h>

class GoogleAuthorizationCodeFlowPrivate : public QOAuth2AuthorizationCodeFlowPrivate
{
    Q_DECLARE_PUBLIC(GoogleAuthorizationCodeFlow)
public:
    GoogleAuthorizationCodeFlowPrivate(const QUrl &authorizationUrl,
                                        const QUrl &accessTokenUrl,
                                        const QString &clientIdentifier,
                                        QNetworkAccessManager *manager = nullptr);

    QString m_prompt = QString();
    QString m_access_type = QString();
};

#endif // GOOGLEAUTHORIZATIONCODEFLOW_P_H
