#include "gdrivefilesimpleupdate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QFile>
#include <QUrlQuery>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonParseError>

GDrive::GDriveFileSimpleUpdate::GDriveFileSimpleUpdate(const QString &filepath,
                                                       const QString &fileID, QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent)
{
    //! check file exist
    if(!QFile::exists(filepath)){
        qWarning() << "file doesnt exist " << filepath;
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        taskFailed();
        return;
    }
    //! Initial file
    m_file = new QFile(filepath,this);
    //! Setup Url
    m_url = buildUrl(fileID,"media",mp_google->token());
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileSimpleUpdate::GDriveFileSimpleUpdate(const QString &filepath,
                                                       const QString &fileID,
                                                       const QUrlQuery &args,
                                                       QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent)
{
    //! check file exist
    if(!QFile::exists(filepath)){
        qWarning() << "file doesnt exist " << filepath;
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        taskFailed();
        return;
    }
    //! Initial file
    m_file = new QFile(filepath,this);
    //! Setup Url
    m_url = buildUrl(fileID,"media",mp_google->token(),args);
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileSimpleUpdate::~GDriveFileSimpleUpdate()
{
    m_file->close();
}

GDrive::GDriveFileResource GDrive::GDriveFileSimpleUpdate::getResource() const
{
    QJsonParseError jsonErr;
    QJsonDocument doc = QJsonDocument::fromJson(m_replyData,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        qWarning() << Q_FUNC_INFO << jsonErr.errorString();
        return GDriveFileResource();
    }
    return GDriveFileResource(doc);
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
    //! Set optional parameters
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

void GDrive::GDriveFileSimpleUpdate::request_UploadStart()
{
    //! open file
    if(!m_file->open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]File %1: %2\n")
                .arg(m_file->fileName()).arg(m_file->errorString());
        taskFailed();
        return;
    }
    //! gather file message to build simple upload request
    QString mimeType = QMimeDatabase().mimeTypeForFile(m_file->fileName()).name();
    QString fileSize = QString::number(m_file->size());

    QNetworkRequest request(m_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,mimeType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,fileSize.toLatin1());
    request.setRawHeader("Authorization:",
                         QByteArray("Bearer " + mp_google->token().toLatin1()));
    //! send request & connect reply handle function
    auto reply = mp_google->networkAccessManager()->sendCustomRequest(request,"PATCH",m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileSimpleUpdate::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileSimpleUpdate::on_UploadStart_ReplyError);
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
        QTimer::singleShot(sleeptime,
                           this,&GDriveFileSimpleUpdate::request_UploadStart);
    }else {
        m_errStr += QStringLiteral("[Error]Too many retrys.");
        m_isFailed = true;
        m_isComplete = true;
        emit finished();
    }
}

void GDrive::GDriveFileSimpleUpdate::on_UploadStart_ReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
//        qWarning() << "[Error]Simple Update reply error: " + reply->errorString();
        return;
    }
    //! QByteArray support implicit sharing
    m_replyData = reply->readAll();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileSimpleUpdate::on_UploadStart_ReplyError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    auto replyErr = QString("[Error]Simple Update reply error %1: %2\n")
            .arg(httpStatus)
            .arg(reply->errorString());
    qWarning() << replyErr;
    m_errStr += replyErr;
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        taskFailed();
    }
    reply->deleteLater();
}
