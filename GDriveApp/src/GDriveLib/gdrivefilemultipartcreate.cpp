#include "gdrivefilemultipartcreate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QFile>
#include <QUrlQuery>
#include <QMimeDatabase>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QTimer>

GDrive::GDriveFileMultipartCreate::GDriveFileMultipartCreate(const QString &filepath,
                                                             QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_file(filepath,this),m_httpMultiPart(nullptr)
{
    m_url = buildUrl("multipart");
}

GDrive::GDriveFileMultipartCreate::GDriveFileMultipartCreate(const QString &filepath,
                                                             const QUrlQuery &args,
                                                             QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_file(filepath,this),m_httpMultiPart(nullptr)
{
    m_url = buildUrl("multipart",args);
}

GDrive::GDriveFileMultipartCreate::~GDriveFileMultipartCreate()
{

}

QByteArray GDrive::GDriveFileMultipartCreate::getReplyString() const
{
    return m_replyData;
}

QUrlQuery GDrive::GDriveFileMultipartCreate::buildUrlArgs(const bool enforceSingleParent,
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

bool GDrive::GDriveFileMultipartCreate::start()
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

void GDrive::GDriveFileMultipartCreate::abort()
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

QNetworkReply *GDrive::GDriveFileMultipartCreate::requestUploadStart(const QNetworkRequest &request,QHttpMultiPart *multiPart)
{
    auto reply = mp_google->networkAccessManager()->post(request,multiPart);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileMultipartCreate::onUploadStartReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileMultipartCreate::onUploadStartReplyError);
    connect(reply,&QNetworkReply::uploadProgress,
            this,&GDriveFileMultipartCreate::uploadProgress);
    return reply;
}

QNetworkRequest GDrive::GDriveFileMultipartCreate::buildRequest(const QUrl &url) const
{
    QNetworkRequest request(url);
    const QString tokenHeader = QString("Bearer %1").arg(mp_google->token());
    request.setRawHeader(QByteArray("Authorization"),tokenHeader.toLatin1());
    return request;
}

QHttpMultiPart* GDrive::GDriveFileMultipartCreate::buildRequestBody(QFile *file)
{
    QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::RelatedType,this);
    multipart->setBoundary("foo_bar_baz");
    QHttpPart meta;
    meta.setHeader(QNetworkRequest::ContentTypeHeader,"application/json; charset=UTF-8");
    const QString matabody = QString("{\"name\":\"%1\"}").arg(QFileInfo(*file).baseName());
    meta.setBody(matabody.toUtf8());    //檔名包含中文
    QHttpPart media;
    media.setHeader(QNetworkRequest::ContentTypeHeader,QMimeDatabase().mimeTypeForFile(*file).name());
    media.setBodyDevice(file);    //mediabody
    multipart->append(meta);
    multipart->append(media);
    return multipart;
}

QUrl GDrive::GDriveFileMultipartCreate::buildUrl(const QString &uploadType/*, const QString &access_token*/) const
{
    QUrlQuery query;
    query.addQueryItem("uploadType",uploadType);
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileMultipartCreate::buildUrl(const QString &uploadType,QUrlQuery args) const
{
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    args.addQueryItem("uploadType",uploadType);
    url.setQuery(args);
    return url;
}

void GDrive::GDriveFileMultipartCreate::retry()
{
    static int retryCount = 0;
    int sleeptime = getExpBackoffSleepTime(retryCount,1,10);
    if(sleeptime != -1){
        retryCount++;
        m_currentReply->deleteLater();
        QTimer::singleShot(sleeptime,this,&GDriveFileMultipartCreate::start);
    }else {
        m_errStr += QStringLiteral("[Error]Too many retrys.");
        taskFailed();
    }
}

void GDrive::GDriveFileMultipartCreate::onUploadStartReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        return;
    }
    // QByteArray support implicit sharing
    m_replyData = reply->readAll();
    m_file.close();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileMultipartCreate::onUploadStartReplyError(QNetworkReply::NetworkError code)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        m_errStr = QString("[Error]Simple Update reply error code %1: %2\n").arg(QString(code),reply->errorString());
        m_replyData = reply->readAll();
        taskFailed();
    }
    reply->deleteLater();
}
