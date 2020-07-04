#include "gdrivefilemultipartupdate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>
#include <QMimeDatabase>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QTimer>

GDrive::GDriveFileMultipartUpdate::GDriveFileMultipartUpdate(const QString &filepath,
                                                             const QString &fileID,
                                                             QOAuth2AuthorizationCodeFlow *parent):
    GDriveFileTask (parent),m_file(filepath,this),m_httpMultiPart(nullptr)
{
    m_url = buildUrl(fileID,"multipart");
}

GDrive::GDriveFileMultipartUpdate::GDriveFileMultipartUpdate(const QString &filepath,
                                                             const QString &fileID,
                                                             const QUrlQuery &args,
                                                             QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_file(filepath,this),m_httpMultiPart(nullptr)
{
    m_url = buildUrl(fileID,"multipart",args);
}

GDrive::GDriveFileMultipartUpdate::~GDriveFileMultipartUpdate()
{
    if(m_currentReply){
        m_currentReply->deleteLater();
    }
}

QByteArray GDrive::GDriveFileMultipartUpdate::getReplyString() const
{
    return m_replyData;
}

QUrlQuery GDrive::GDriveFileMultipartUpdate::buildUrlArgs(const QString &addParents,
                                                          const bool enforceSingleParent,
                                                          const bool keepRevisionForever,
                                                          const QString &ocrLanguage,
                                                          const QString &removeParents,
                                                          const bool supportsAllDrives,
                                                          const bool useContentAsIndexableText)
{
    QUrlQuery args;
    // Set optional parameters
    if(!addParents.isEmpty()){
        args.addQueryItem("addParents",addParents);
    }
    if(enforceSingleParent){
        args.addQueryItem("enforceSingleParent",
                          QVariant(enforceSingleParent).toString());
    }
    if(keepRevisionForever){
        args.addQueryItem("keepRevisionForever",
                          QVariant(keepRevisionForever).toString());
    }
    if(!ocrLanguage.isEmpty()){
        args.addQueryItem("ocrLanguage",ocrLanguage);
    }
    if(!removeParents.isEmpty()){
        args.addQueryItem("removeParents",removeParents);
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

bool GDrive::GDriveFileMultipartUpdate::start()
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
    /* create httpmultipart if file has correctly open, httpmultipart will only create once */
    if(!m_httpMultiPart){
        m_httpMultiPart = buildRequestBody(&m_file);
    }
    m_currentReply = requestUploadStart(buildRequest(m_url),m_httpMultiPart);
    return true;
}

void GDrive::GDriveFileMultipartUpdate::abort()
{
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

QNetworkReply *GDrive::GDriveFileMultipartUpdate::requestUploadStart(const QNetworkRequest &request,QHttpMultiPart *multiPart)
{
    auto reply = mp_google->networkAccessManager()->sendCustomRequest(request,"PATCH",multiPart);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileMultipartUpdate::onUploadStartReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileMultipartUpdate::onUploadStartReplyError);
    connect(reply,&QNetworkReply::uploadProgress,
            this,&GDriveFileMultipartUpdate::uploadProgress);
    return reply;
}

QNetworkRequest GDrive::GDriveFileMultipartUpdate::buildRequest(const QUrl &url) const
{
    QNetworkRequest request(url);
    const QString tokenHeader = QString("Bearer %1").arg(mp_google->token());
    request.setRawHeader(QByteArray("Authorization"),tokenHeader.toLatin1());
    return request;
}

QHttpMultiPart *GDrive::GDriveFileMultipartUpdate::buildRequestBody(QFile *file)
{
    // Multipart request
    QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::RelatedType,this);
    multipart->setBoundary("foo_bar_baz");
    QHttpPart meta;
    meta.setHeader(QNetworkRequest::ContentTypeHeader,"application/json; charset=UTF-8");
    const QString matabody = QString("{\"name\":\"%1\"}").arg(QFileInfo(m_file).baseName());
    meta.setBody(matabody.toUtf8());    //檔名包含中文
    QHttpPart media;
    media.setHeader(QNetworkRequest::ContentTypeHeader,QMimeDatabase().mimeTypeForFile(*file).name());
    media.setBodyDevice(&m_file);    //mediabody
    multipart->append(meta);
    multipart->append(media);
    return multipart;
}

QUrl GDrive::GDriveFileMultipartUpdate::buildUrl(const QString &fileID,
                                                 const QString &uploadType) const
{
    QUrlQuery query;
    query.addQueryItem("uploadType",uploadType);
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files/" + fileID);
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileMultipartUpdate::buildUrl(const QString &fileID,
                                                 const QString &uploadType,
                                                 QUrlQuery args) const
{
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files/" + fileID);
    args.addQueryItem("uploadType",uploadType);
    url.setQuery(args);
    return url;
}

void GDrive::GDriveFileMultipartUpdate::retry()
{
    static int retryCount = 0;
    int sleeptime = getExpBackoffSleepTime(retryCount,1,10);
    if(sleeptime != -1){
        retryCount++;
        QTimer::singleShot(sleeptime,this,&GDriveFileMultipartUpdate::start);
    }else {
        m_errStr += QStringLiteral("[Error]Too many retrys.");
        taskFailed();
    }
}

void GDrive::GDriveFileMultipartUpdate::onUploadStartReplyFinished()
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

void GDrive::GDriveFileMultipartUpdate::onUploadStartReplyError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qWarning() << "[Error]Reply error code:" << httpStatus << " >> " << reply->errorString();
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        m_errStr += QString("[Error]Simple Update reply error: %1\n").arg(reply->errorString());
        m_replyData = reply->readAll();
        taskFailed();
    }
    reply->deleteLater();
}
