#include "gdrivefilesimplecreate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QTimer>

GDrive::GDriveFileSimpleCreate::GDriveFileSimpleCreate(const QString &filepath,
                                                       QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_file(filepath,this)
{
    m_url = buildUrl("media",mp_google->token());
}

GDrive::GDriveFileSimpleCreate::GDriveFileSimpleCreate(const QString &filepath,
                                                       const QUrlQuery &args,
                                                       QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_file(filepath,this)
{
    m_url = buildUrl("media",mp_google->token(),args);
}

GDrive::GDriveFileSimpleCreate::~GDriveFileSimpleCreate()
{
    m_currentReply->deleteLater();
}

QByteArray GDrive::GDriveFileSimpleCreate::getReplyString() const
{
    return m_replyData;
}

QUrlQuery GDrive::GDriveFileSimpleCreate::buildUrlArgs(const bool enforceSingleParent,
                                                       const bool ignoreDefaultVisibility,
                                                       const bool keepRevisionForever,
                                                       const QString &ocrLanguage,
                                                       const bool supportsAllDrives,
                                                       const bool useContentAsIndexableText)
{
    QUrlQuery args;
    if(enforceSingleParent){
        args.addQueryItem("enforceSingleParent",
                          QVariant(enforceSingleParent).toString());
    }
    if(ignoreDefaultVisibility){
        args.addQueryItem("ignoreDefaultVisibility",
                          QVariant(ignoreDefaultVisibility).toString());
    }
    if(keepRevisionForever){
        args.addQueryItem("keepRevisionForever",
                          QVariant(keepRevisionForever).toString());
    }
    if(!ocrLanguage.isEmpty()){
        args.addQueryItem("ocrLanguage",ocrLanguage);
    }
    if(supportsAllDrives){
        args.addQueryItem("supportsAllDrives",
                          QVariant(supportsAllDrives).toString());
    }
    if(useContentAsIndexableText){
        args.addQueryItem("useContentAsIndexableText",
                          QVariant(useContentAsIndexableText).toString());
    }
    return args;
}

bool GDrive::GDriveFileSimpleCreate::start()
{
    if(m_currentReply){
        m_errStr += QString("[Error]Already has reply.");
        taskFailed();
        return false;
    }
    if(!m_file.isOpen() && !m_file.open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]%1 open fail:%2").arg(m_file.fileName(),m_file.errorString());
        taskFailed();
        return false;
    }
    m_currentReply = requestUploadStart(buildRequest(m_url));
    return true;
}

void GDrive::GDriveFileSimpleCreate::abort()
{
    qInfo() << Q_FUNC_INFO;
    if(m_currentReply){
        m_currentReply->disconnect();
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
    m_file.close();
    m_errStr += QString("Abort upload.");
    taskFailed();
}

QNetworkReply *GDrive::GDriveFileSimpleCreate::requestUploadStart(const QNetworkRequest &request)
{
    // send request & connect reply handle function
    auto reply = mp_google->networkAccessManager()->post(request,&m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileSimpleCreate::onUploadStartReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileSimpleCreate::onUploadStartReplyError);
    connect(reply,&QNetworkReply::uploadProgress,
            this,&GDriveFileSimpleCreate::uploadProgress);
    return reply;
}

QNetworkRequest GDrive::GDriveFileSimpleCreate::buildRequest(const QUrl &url) const
{
    const QString mimeType = QMimeDatabase().mimeTypeForFile(m_file.fileName()).name();
    const QString fileSize = QString::number(m_file.size());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,mimeType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,fileSize.toLatin1());
    request.setRawHeader("Authorization:",QByteArray("Bearer " + mp_google->token().toLatin1()));
    return request;
}

QUrl GDrive::GDriveFileSimpleCreate::buildUrl(const QString &uploadType, const QString &access_token) const
{
    QUrlQuery query;
    query.addQueryItem("uploadType",uploadType);
    query.addQueryItem("access_token",access_token);
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileSimpleCreate::buildUrl(const QString &uploadType,const QString &access_token,QUrlQuery args) const
{
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    args.addQueryItem("uploadType",uploadType);
    args.addQueryItem("access_token",access_token);
    url.setQuery(args);
    return url;
}

void GDrive::GDriveFileSimpleCreate::retry()
{
    static int retryCount = 0;
    int sleeptime = getExpBackoffSleepTime(retryCount,1,10);
    if(sleeptime != -1){
        retryCount++;
        m_currentReply->deleteLater();
        QTimer::singleShot(sleeptime,this,&GDriveFileSimpleCreate::start);
    }else {
        m_errStr += QStringLiteral("[Error]Too many retrys.");
        taskFailed();
    }
}

void GDrive::GDriveFileSimpleCreate::onUploadStartReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Simple Upload reply error: " + reply->errorString();
        return;
    }
    /* Successful reply
     * QByteArray support implicit sharing */
    m_replyData = reply->readAll();
    m_file.close();
    reply->deleteLater();
    taskSucceeded();
}

void GDrive::GDriveFileSimpleCreate::onUploadStartReplyError(QNetworkReply::NetworkError code)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    auto replyErr = QString("Simple Update error code %1: %2\n").arg(QString(code),reply->errorString());
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        m_errStr += replyErr;
        m_replyData = reply->readAll();
        taskFailed();
    }
    reply->deleteLater();
}
