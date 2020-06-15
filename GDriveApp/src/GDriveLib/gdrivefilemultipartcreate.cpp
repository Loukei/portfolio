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
    :GDriveFileTask (parent),m_replyData("")
{
    //! check file exist
    if(!QFile::exists(filepath)){
        qWarning() << "file doesnt exist " << filepath;
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        taskFailed();
        return;
    }
    //! Build url
    m_url = buildUrl("multipart");
    //! Initial file
    m_file = new QFile(filepath,this);
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileMultipartCreate::GDriveFileMultipartCreate(const QString &filepath,
                                                             const QUrlQuery &args,
                                                             QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent),m_replyData("")
{
    //! check file exist
    if(!QFile::exists(filepath)){
        qWarning() << "file doesnt exist " << filepath;
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        taskFailed();
        return;
    }
    //! Build url
    m_url = buildUrl("multipart",args);
    //! Initial file
    m_file = new QFile(filepath,this);
    //! Upload file
    request_UploadStart();
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

void GDrive::GDriveFileMultipartCreate::request_UploadStart()
{
    //! open file
    if(!m_file->open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]File %1: %2\n")
                .arg(m_file->fileName()).arg(m_file->errorString());
        taskFailed();
        return;
    }
    //! the url will use to create new upload
    QNetworkRequest request(m_url);
    const QString tokenHeader = QString("Bearer %1").arg(mp_google->token());
    request.setRawHeader(QByteArray("Authorization"),tokenHeader.toLatin1());
    //! Multipart request
    QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::RelatedType,this);
    multipart->setBoundary("foo_bar_baz");
    QHttpPart meta;
    meta.setHeader(QNetworkRequest::ContentTypeHeader,"application/json; charset=UTF-8");
    const QString matabody = QString("{\"name\":\"%1\"}").arg(QFileInfo(*m_file).baseName());
    meta.setBody(matabody.toUtf8());    //檔名包含中文
    QHttpPart media;
    media.setHeader(QNetworkRequest::ContentTypeHeader,
                    QMimeDatabase().mimeTypeForFile(*m_file).name());
    media.setBodyDevice(m_file);    //mediabody
    multipart->append(meta);
    multipart->append(media);

    auto reply = mp_google->networkAccessManager()->post(request,multipart);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileMultipartCreate::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileMultipartCreate::on_UploadStart_ReplyError);
}

QUrl GDrive::GDriveFileMultipartCreate::buildUrl(const QString &uploadType/*, const QString &access_token*/) const
{
    QUrlQuery query;
    query.addQueryItem("uploadType",uploadType);
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileMultipartCreate::buildUrl(const QString &uploadType/*,const QString &access_token*/,QUrlQuery args) const
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
        QTimer::singleShot(sleeptime,this
                           ,&GDriveFileMultipartCreate::request_UploadStart);
    }else {
        m_errStr += QStringLiteral("[Error]Too many retrys.");
        taskFailed();
    }
}

void GDrive::GDriveFileMultipartCreate::on_UploadStart_ReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Multipart Upload reply error: " + reply->errorString();
        return;
    }
    //! QByteArray support implicit sharing
    m_replyData = reply->readAll();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileMultipartCreate::on_UploadStart_ReplyError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        m_errStr += QString("[Error]Simple Upload reply error: %1\n")
                .arg(reply->errorString());
        taskFailed();
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " >> " << reply->errorString();
    reply->deleteLater();
}
