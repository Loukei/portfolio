#include "gdrivefilesimpleupdate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QTimer>

GDrive::GDriveFileSimpleUpdate::GDriveFileSimpleUpdate(const QString &filepath,
                                                       const QString &fileID, QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_file(filepath,this)
{
    m_url = buildUrl(fileID,"media",mp_google->token());
}

GDrive::GDriveFileSimpleUpdate::GDriveFileSimpleUpdate(const QString &filepath,
                                                       const QString &fileID,
                                                       const QUrlQuery &args,
                                                       QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_file(filepath,this)
{
    m_url = buildUrl(fileID,"media",mp_google->token(),args);
}

GDrive::GDriveFileSimpleUpdate::~GDriveFileSimpleUpdate()
{
    m_file.close();
}

QByteArray GDrive::GDriveFileSimpleUpdate::getReplyString() const
{
    return m_replyData;
}

QUrlQuery GDrive::GDriveFileSimpleUpdate::buildUrlArgs(const QString &addParents,
                                                       const bool enforceSingleParent,
                                                       const bool keepRevisionForever,
                                                       const QString &ocrLanguage,
                                                       const QString &removeParents,
                                                       const bool supportsAllDrives,
                                                       const bool useContentAsIndexableText)
{
    QUrlQuery query;
    // Set optional parameters
    if(!addParents.isEmpty()){
        query.addQueryItem("addParents",addParents);
    }
    if(enforceSingleParent){
        query.addQueryItem("enforceSingleParent",
                           QVariant(enforceSingleParent).toString());
    }
    if(keepRevisionForever){
        query.addQueryItem("keepRevisionForever",
                           QVariant().toString());
    }
    if(!ocrLanguage.isEmpty()){
        query.addQueryItem("ocrLanguage",ocrLanguage);
    }
    if(!removeParents.isEmpty()){
        query.addQueryItem("removeParents",ocrLanguage);
    }
    if(supportsAllDrives){
        query.addQueryItem("supportsAllDrives",
                           QVariant(supportsAllDrives).toString());
    }
    if(useContentAsIndexableText){
        query.addQueryItem("useContentAsIndexableText",
                           QVariant(useContentAsIndexableText).toString());
    }
    return query;
}

bool GDrive::GDriveFileSimpleUpdate::start()
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

void GDrive::GDriveFileSimpleUpdate::abort()
{
    qInfo() << Q_FUNC_INFO;
    if(this->isComplete()){
        return;
    }
    if(m_currentReply){
        m_currentReply->disconnect();
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
    m_file.close();
    m_errStr += QString("Abort upload.");
    taskFailed();
}

QNetworkReply *GDrive::GDriveFileSimpleUpdate::requestUploadStart(const QNetworkRequest &request)
{
    // send request & connect reply handle function
    auto reply = mp_google->networkAccessManager()->sendCustomRequest(request,"PATCH",&m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileSimpleUpdate::onUploadStartReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileSimpleUpdate::onUploadStartReplyError);
    connect(reply,&QNetworkReply::uploadProgress,
            this,&GDriveFileSimpleUpdate::uploadProgress);
    return reply;
}

QNetworkRequest GDrive::GDriveFileSimpleUpdate::buildRequest(const QUrl &url) const
{
    QString mimeType = QMimeDatabase().mimeTypeForFile(m_file.fileName()).name();
    QString fileSize = QString::number(m_file.size());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,mimeType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,fileSize.toLatin1());
    request.setRawHeader("Authorization:",QByteArray("Bearer " + mp_google->token().toLatin1()));
    return request;
}

QUrl GDrive::GDriveFileSimpleUpdate::buildUrl(const QString &fileID,
                                              const QString &uploadType,
                                              const QString &access_token) const
{
    QUrlQuery query;
    query.addQueryItem("uploadType",uploadType);
    query.addQueryItem("access_token",access_token);
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files/" + fileID);
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileSimpleUpdate::buildUrl(const QString &fileID,
                                              const QString &uploadType,
                                              const QString &access_token,
                                              QUrlQuery args) const
{
    args.addQueryItem("uploadType",uploadType);
    args.addQueryItem("access_token",access_token);
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files/" + fileID);
    url.setQuery(args);
    return url;
}

void GDrive::GDriveFileSimpleUpdate::retry()
{
    static int retryCount = 0;
    int sleeptime = GDriveFileTask::getExpBackoffSleepTime(retryCount,1,10);
    if(sleeptime != -1){
        retryCount++;
        QTimer::singleShot(sleeptime,this,&GDriveFileSimpleUpdate::start);
    }else {
        m_errStr += QStringLiteral("Too many retrys.");
        taskFailed();
    }
}

void GDrive::GDriveFileSimpleUpdate::onUploadStartReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        return;
    }
    // QByteArray support implicit sharing
    m_replyData = reply->readAll();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileSimpleUpdate::onUploadStartReplyError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        m_errStr = QString("Simple Update reply error %1: %2\n").arg(QString(httpStatus),reply->errorString());
        m_replyData = reply->readAll();
        taskFailed();
    }
    reply->deleteLater();
}
