#include "googledriveservice.h"
#include <QOAuthHttpServerReplyHandler>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QDesktopServices>
#include <QDebug>

using namespace GDrive;

GDriveService::GDriveService(const QUrl &authorizationUrl,
                             const QUrl &accessTokenUrl,
                             const QString &clientIdentifier,
                             const QString &clientIdentifierSharedKey,
                             const QString &scope,
                             quint16 port,
                             QObject *parent)
    : QObject(parent)
{
    /* Set google app OAuth2 setting */
    m_manager = new QNetworkAccessManager(this);
    m_google = new QOAuth2AuthorizationCodeFlow(m_manager,this);
    m_google->setScope(scope);
    m_google->setAuthorizationUrl(authorizationUrl);
    m_google->setClientIdentifier(clientIdentifier);
    m_google->setAccessTokenUrl(accessTokenUrl);
    m_google->setClientIdentifierSharedKey(clientIdentifierSharedKey);
    /* reset Oauth parameter to fit Google OAuth situation */
    m_google->setModifyParametersFunction(buildModifyParametersFunction());

    auto replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    m_google->setReplyHandler(replyHandler);

    connect(m_google,&QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            this,&QDesktopServices::openUrl);
    connect(m_google,&QOAuth2AuthorizationCodeFlow::granted,
            this,&GDriveService::granted);
    connect(m_google,&QOAuth2AuthorizationCodeFlow::tokenChanged,
            this,&GDriveService::tokenChanged);
    connect(m_google,&QOAuth2AuthorizationCodeFlow::refreshTokenChanged,
            this,&GDriveService::refreshTokenChanged);
    connect(m_google,&QOAuth2AuthorizationCodeFlow::expirationAtChanged,
            this,&GDriveService::expirationAtChanged);
    connect(m_google,&QOAuth2AuthorizationCodeFlow::error,
            this,&GDriveService::error);
}

GDriveService::~GDriveService()
{
    /* delete none parent pointer here... */
}

QDateTime GDriveService::expirationAt() const
{
    return m_google->expirationAt();
}

void GDriveService::grant()
{
    return m_google->grant();
}

void GDriveService::refreshAccessToken()
{
    return m_google->refreshAccessToken();
}

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

QString GDriveService::token() const
{
    return m_google->token();
}

void GDriveService::setToken(const QString &token)
{
    return m_google->setToken(token);
}

QString GDriveService::refreshToken() const
{
    return m_google->refreshToken();
}

void GDriveService::setRefreshToken(const QString &refreshToken)
{
    return m_google->setRefreshToken(refreshToken);
}

QNetworkAccessManager *GDriveService::networkAccessManager() const
{
    return m_google->networkAccessManager();
}

GDriveAbout *GDriveService::about(const QString &fields)
{
    return new GDriveAbout(m_google,fields);
}

GDriveFileSimpleCreate *GDriveService::fileSimpleCreate(const QString &filepath)
{
    return new GDriveFileSimpleCreate(filepath,m_google);
}

GDriveFileSimpleCreate *GDriveService::fileSimpleCreate(const QString &filepath, const QUrlQuery &args)
{
    return new GDriveFileSimpleCreate(filepath,args,m_google);
}

GDriveFileMultipartCreate *GDriveService::fileMultipartCreate(const QString &filepath)
{
    return new GDriveFileMultipartCreate(filepath,m_google);
}

GDriveFileMultipartCreate *GDriveService::fileMultipartCreate(const QString &filepath, const QUrlQuery &args)
{
    return new GDriveFileMultipartCreate(filepath,args,m_google);
}

GDriveFileResumableCreate *GDriveService::fileResumableCreate(const QString &filepath)
{
    return new GDriveFileResumableCreate(filepath,m_google);
}

GDriveFileResumableCreate *GDriveService::fileResumableCreate(const QString &filepath, const QUrlQuery &args)
{
    return new GDriveFileResumableCreate(filepath,args,m_google);
}

GDriveFileSimpleUpdate *GDriveService::fileSimpleUpdate(const QString &filepath, const QString &fileID)
{
    return new GDriveFileSimpleUpdate(filepath,fileID,m_google);
}

GDriveFileSimpleUpdate *GDriveService::fileSimpleUpdate(const QString &filepath, const QString &fileID, const QUrlQuery &args)
{
    return new GDriveFileSimpleUpdate(filepath,fileID,args,m_google);
}

GDriveFileMultipartUpdate *GDriveService::fileMultipartUpdate(const QString &filepath, const QString &fileID)
{
    return new GDriveFileMultipartUpdate(filepath,fileID,m_google);
}

GDriveFileMultipartUpdate *GDriveService::fileMultipartUpdate(const QString &filepath, const QString &fileID, const QUrlQuery &args)
{
    return new GDriveFileMultipartUpdate(filepath,fileID,args,m_google);
}

GDriveFileResumableUpdate *GDriveService::fileResumableUpdate(const QString &filepath, const QString &fileID)
{
    return new GDriveFileResumableUpdate(filepath,fileID,m_google);
}

GDriveFileResumableUpdate *GDriveService::fileResumableUpdate(const QString &filepath, const QString &fileID, const QUrlQuery &args)
{
    return new GDriveFileResumableUpdate(filepath,fileID,args,m_google);
}

GDriveFileGet *GDriveService::fileGet(const QString &fileId,const QUrlQuery &args)
{
    return new GDriveFileGet(fileId,args,m_google);
}

GDriveFileGet *GDriveService::fileGet(const QString &fileId, const QString &fields)
{
    return new GDriveFileGet(fileId,fields,m_google);
}

GDriveFileSearch *GDriveService::fileList(const QUrlQuery &args)
{
    return new GDriveFileSearch(args,m_google);
}

GDriveFileDownloader *GDriveService::fileDownload(const QString &fileId, const QString &fields, QSharedPointer<QFile> file)
{
    return new GDriveFileDownloader(fileId,fields,file,m_google);
}

GDriveFileDownloader *GDriveService::fileDownload(const QString &fileId, const QUrlQuery &args, QSharedPointer<QFile> file)
{
    return new GDriveFileDownloader(fileId,args,file,m_google);
}
