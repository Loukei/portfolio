#include "gdrivefilesimpleupdate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QFile>
#include <QUrlQuery>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonParseError>

GDrive::GDriveFileSimpleUpdate::GDriveFileSimpleUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                                       const QString &filepath,
                                                       const QString &fileID)
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
    m_url = setupUrl(fileID);
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileSimpleUpdate::GDriveFileSimpleUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                                       const QString &filepath,
                                                       const QString &fileID,
                                                       const QString &addParents,
                                                       bool enforceSingleParent,
                                                       bool keepRevisionForever,
                                                       const QString &ocrLanguage,
                                                       const QString &removeParents,
                                                       bool useContentAsIndexableText)
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
    m_url = setupUrl(fileID,
                     addParents,
                     enforceSingleParent,
                     keepRevisionForever,
                     ocrLanguage,
                     removeParents,
                     useContentAsIndexableText);
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileSimpleUpdate::GDriveFileSimpleUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                                       const QString &filepath,
                                                       const GDrive::FileUpdateArgs &args)
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
    m_url = setupUrl(args);
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
//    auto reply = mp_google->networkAccessManager()->post(request,m_file);
    auto reply = mp_google->networkAccessManager()->sendCustomRequest(request,"PATCH",m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileSimpleUpdate::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileSimpleUpdate::on_UploadStart_ReplyError);
}

QUrl GDrive::GDriveFileSimpleUpdate::setupUrl(const QString &fileID,
                                              const QString &addParents,
                                              bool enforceSingleParent,
                                              bool keepRevisionForever,
                                              const QString &ocrLanguage,
                                              const QString &removeParents,
                                              bool useContentAsIndexableText)
{
    QUrlQuery query;
    query.addQueryItem("uploadType","media");
    //! Set optional parameters
    if(!addParents.isEmpty()){
        query.addQueryItem("addParents",addParents);
    }
    if(enforceSingleParent){
        query.addQueryItem("enforceSingleParent",
                           GDrive::BooleanToString(enforceSingleParent));
    }
    if(keepRevisionForever){
        query.addQueryItem("keepRevisionForever",
                           GDrive::BooleanToString(keepRevisionForever));
    }
    if(!ocrLanguage.isEmpty()){
        query.addQueryItem("ocrLanguage",ocrLanguage);
    }
    if(!removeParents.isEmpty()){
        query.addQueryItem("removeParents",removeParents);
    }
    if(useContentAsIndexableText){
        query.addQueryItem("useContentAsIndexableText",
                           GDrive::BooleanToString(useContentAsIndexableText));
    }
    query.addQueryItem("access_token",mp_google->token());

    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files/" + fileID);
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileSimpleUpdate::setupUrl(const GDrive::FileUpdateArgs &args)
{
    QUrlQuery query;
    query.addQueryItem("uploadType","media");
    //! Set optional parameters
    if(!args.addParents().isEmpty()){
        query.addQueryItem("addParents",args.addParents());
    }
    if(args.enforceSingleParent()){
        query.addQueryItem("enforceSingleParent",
                           GDrive::BooleanToString(args.enforceSingleParent()));
    }
    if(args.keepRevisionForever()){
        query.addQueryItem("keepRevisionForever",
                           GDrive::BooleanToString(args.keepRevisionForever()));
    }
    if(!args.ocrLanguage().isEmpty()){
        query.addQueryItem("ocrLanguage",args.ocrLanguage());
    }
    if(!args.removeParents().isEmpty()){
        query.addQueryItem("removeParents",args.removeParents());
    }
    if(args.useContentAsIndexableText()){
        query.addQueryItem("useContentAsIndexableText",
                           GDrive::BooleanToString(args.useContentAsIndexableText()));
    }
    query.addQueryItem("access_token",mp_google->token());

    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files/" + args.fileId());
    url.setQuery(query);
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
