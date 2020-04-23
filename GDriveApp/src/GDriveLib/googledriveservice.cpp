#include "googledriveservice.h"
#include <QOAuthHttpServerReplyHandler>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QDesktopServices>
#include <QDebug>
#include "Secret/oauthglobal.h" // OAuth parameter

using namespace GDrive;

GDriveService::GDriveService(QObject *parent)
    : QObject(parent)
{
    //! set google app OAuth2 setting
    m_manager = new QNetworkAccessManager(this);
    m_google = new QOAuth2AuthorizationCodeFlow(m_manager,this);
    m_google->setScope(OAuth::keyScope());
    m_google->setAuthorizationUrl(OAuth::keyAuthUri());
    m_google->setClientIdentifier(OAuth::keyClientId());
    m_google->setAccessTokenUrl(OAuth::keyTokenUri());
    m_google->setClientIdentifierSharedKey(OAuth::keyClientSecert());

    auto replyHandler = new QOAuthHttpServerReplyHandler(OAuth::keyRedirectPort(), this);
    m_google->setReplyHandler(replyHandler);

    connect(m_google,&QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            this,&QDesktopServices::openUrl);
    connect(m_google,&QOAuth2AuthorizationCodeFlow::granted,
            this,&GDriveService::granted);
    connect(m_google,&QOAuth2AuthorizationCodeFlow::statusChanged,
            this,&GDriveService::statusChanged);
    connect(m_google,&QOAuth2AuthorizationCodeFlow::expirationAtChanged,
            this,&GDriveService::expirationAtChanged);
    connect(m_google,&QOAuth2AuthorizationCodeFlow::error,
            this,&GDriveService::error);
}

GDriveService::~GDriveService()
{
    /// delete none parent pointer here...
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

void GDriveService::logout()
{
    m_google->setToken(QString());
    m_google->setRefreshToken(QString());
}

QString GDriveService::token() const
{
    return m_google->token();
}

void GDriveService::setToken(const QString &token)
{
    return m_google->setToken(token);
}

QAbstractOAuth::Status GDriveService::status() const
{
    return m_google->status();
}

QString GDriveService::refreshToken() const
{
    return m_google->refreshToken();
}

void GDriveService::setRefreshToken(const QString &refreshToken)
{
    return m_google->setRefreshToken(refreshToken);
}

GDriveAbout* GDriveService::getAbout(GDriveAbout::AboutArgs args)
{
    return new GDriveAbout(m_google,args);
}

GDriveFileSimpleCreate *GDriveService::fileSimpleCreate(const QString &filepath, const FileCreateArgs &args)
{
    return new GDriveFileSimpleCreate(m_google,filepath,args);
}

GDriveFileMultipartCreate *GDriveService::fileMultipartCreate(const QString &filepath, const FileCreateArgs &args)
{
    return new GDriveFileMultipartCreate(m_google,filepath,args);
}

GDriveFileResumableCreate *GDriveService::fileResumableCreate(const QString &filepath, const FileCreateArgs &args)
{
    return new GDriveFileResumableCreate(m_google,filepath,args);
}

GDriveFileSimpleUpdate *GDriveService::fileSimpleUpdate(const QString &filepath, const FileUpdateArgs &args)
{
    return new GDriveFileSimpleUpdate(m_google,filepath,args);
}

GDriveFileMultipartUpdate *GDriveService::fileMultipartUpdate(const QString &filepath, const FileUpdateArgs &args)
{
    return new GDriveFileMultipartUpdate(m_google,filepath,args);
}

GDriveFileResumableUpdate *GDriveService::fileResumableUpdate(const QString &filepath, const FileUpdateArgs &args)
{
    return new GDriveFileResumableUpdate(m_google,filepath,args);
}

GDriveFileGet *GDriveService::fileGet(const FileGetArgs &args)
{
    return new GDriveFileGet(m_google,args);
}

GDriveFileSearch *GDriveService::fileList(const FileListArgs &args)
{
    return new GDriveFileSearch(m_google,args);
}

GDriveFileDownloader *GDriveService::fileDownload(const QString &fileId, const QString &fields, QSharedPointer<QFile> file)
{
    return new GDriveFileDownloader(m_google,fileId,fields,file);
}
