#include "googledriveservice.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QNetworkAccessManager>
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
}

GDriveService::~GDriveService()
{
  /// delete none parent pointer here...
}

void GDriveService::start()
{
    m_google->grant();
}

void GDriveService::logout()
{
    m_google->setToken(QString());
    m_google->setRefreshToken(QString());
}

QString GDriveService::receivedToken() const
{
    QString msg;
    msg += QString("Receive Token: %1\n").arg(m_google->token());
    return msg;
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

GDriveFileGet *GDriveService::fileGet(const QString &fileId, const QString &fields)
{
    return new GDriveFileGet(m_google,fileId,fields);
}

GDriveFileSearch *GDriveService::fileList(const QString &q, const QString &pageToken)
{
     return new GDriveFileSearch(m_google,q,pageToken);
}

GDriveFileDownloader *GDriveService::fileDownload(const QString &fileId, const QString &fields, QSharedPointer<QFile> file)
{
    return new GDriveFileDownloader(m_google,fileId,fields,file);
}
