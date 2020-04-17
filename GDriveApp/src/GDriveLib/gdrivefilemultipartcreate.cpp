#include "gdrivefilemultipartcreate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QFile>
#include <QUrlQuery>
#include <QMimeDatabase>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QTimer>
#include <QJsonParseError>
#include <QJsonDocument>

GDrive::GDriveFileMultipartCreate::GDriveFileMultipartCreate(QOAuth2AuthorizationCodeFlow *parent,
                                                             const QString &filepath)
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
    m_url = setupUrl();
    //! Initial file
    m_file = new QFile(filepath,this);
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileMultipartCreate::GDriveFileMultipartCreate(QOAuth2AuthorizationCodeFlow *parent,
                                                             const QString &filepath,
                                                             const GDrive::FileCreateArgs &args)
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
    m_url = setupUrl(args);
    //! Initial file
    m_file = new QFile(filepath,this);
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileMultipartCreate::~GDriveFileMultipartCreate()
{

}

GDrive::GDriveFileResource GDrive::GDriveFileMultipartCreate::getResource() const
{
    QJsonParseError jsonErr;
    QJsonDocument doc = QJsonDocument::fromJson(m_replyData,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        qWarning() << Q_FUNC_INFO << jsonErr.errorString();
        return GDriveFileResource();
    }
    return GDriveFileResource(doc);
}

QByteArray GDrive::GDriveFileMultipartCreate::getReplyString() const
{
    return m_replyData;
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

QUrl GDrive::GDriveFileMultipartCreate::setupUrl()
{
    QUrlQuery query;
    query.addQueryItem("uploadType","multipart");
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileMultipartCreate::setupUrl(const GDrive::FileCreateArgs &args)
{
    QUrlQuery query;
    query.addQueryItem("uploadType","multipart");

    if(args.enforceSingleParent()){
        query.addQueryItem("enforceSingleParent",
                           BooleanToString(args.enforceSingleParent()));
    }
    if(args.ignoreDefaultVisibility()){
        query.addQueryItem("ignoreDefaultVisibility",
                           BooleanToString(args.ignoreDefaultVisibility()));
    }
    if(args.keepRevisionForever()){
        query.addQueryItem("keepRevisionForever",
                           BooleanToString(args.keepRevisionForever()));
    }
    if(!args.ocrLanguage().isEmpty()){
        query.addQueryItem("ocrLanguage",args.ocrLanguage());
    }
    if(args.useContentAsIndexableText()){
        query.addQueryItem("useContentAsIndexableText",
                           BooleanToString(args.useContentAsIndexableText()));
    }

    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);
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
