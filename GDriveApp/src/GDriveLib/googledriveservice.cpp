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
    /*
    void O2::unlink() {
    qDebug() << "O2::unlink";
    setLinked(false);
    setToken(QString());
    setRefreshToken(QString());
    setExpires(0);
    setExtraTokens(QVariantMap());
    Q_EMIT linkingSucceeded();
    }
    */
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

GDriveFileSimpleCreate *GDriveService::fileSimpleCreate(const QString &filepath)
{
    return new GDriveFileSimpleCreate(m_google,filepath);
}

GDriveFileMultipartCreate *GDriveService::fileMultipartCreate(const QString &filepath)
{
    return new GDriveFileMultipartCreate(m_google,filepath);
}

GDriveFileResumableCreate *GDriveService::fileResumableCreate(const QString &filepath)
{
    return new GDriveFileResumableCreate(m_google,filepath);
}

GDriveFileSimpleUpdate *GDriveService::fileSimpleUpdate(const QString &filepath, const QString &fileId, const QString &addParents, bool enforceSingleParent, bool keepRevisionForever, const QString &ocrLanguage, const QString &removeParents, bool useContentAsIndexableText)
{
    return new GDriveFileSimpleUpdate(m_google,filepath,fileId,addParents,enforceSingleParent,keepRevisionForever,ocrLanguage,removeParents,useContentAsIndexableText);
}

GDriveFileMultipartUpdate *GDriveService::fileMultipartUpdate(const QString &filepath, const QString &fileId, const QString &addParents, bool enforceSingleParent, bool keepRevisionForever, const QString &ocrLanguage, const QString &removeParents, bool useContentAsIndexableText)
{
    return new GDriveFileMultipartUpdate(m_google,filepath,fileId,addParents,enforceSingleParent,keepRevisionForever,ocrLanguage,removeParents,useContentAsIndexableText);
}

GDriveFileResumableUpdate *GDriveService::fileResumableUpdate(const QString &filepath, const QString &fileId, const QString &addParents, bool enforceSingleParent, bool keepRevisionForever, const QString &ocrLanguage, const QString &removeParents, bool useContentAsIndexableText)
{
    return new GDriveFileResumableUpdate(m_google,filepath,fileId,addParents,enforceSingleParent,keepRevisionForever,ocrLanguage,removeParents,useContentAsIndexableText);
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
