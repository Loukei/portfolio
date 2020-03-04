#include "gdrivefilemultipartcreate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QFile>
#include <QUrlQuery>
#include <QMimeDatabase>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QTimer>

GDrive::GDriveFileMultipartCreate::GDriveFileMultipartCreate(QOAuth2AuthorizationCodeFlow *parent,
                                                             const QString &filepath)
    :GDriveFileTask (parent),m_replyData("")
{
    //! check file exist
    if(!QFile::exists(filepath)){
        qWarning() << "file doesnt exist " << filepath;
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        m_isFailed = false;
        m_isComplete = true;
        return;
    }
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
    return GDriveFileResource(m_replyData);
}

void GDrive::GDriveFileMultipartCreate::request_UploadStart()
{
    //! open file
    if(!m_file->open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]File %1: %2\n")
                .arg(m_file->fileName()).arg(m_file->errorString());
        m_isFailed = true;
        m_isComplete = false;
        return;
    }
    //! the url will use to create new upload
    QUrlQuery query;
    query.addQueryItem("uploadType","multipart");
    query.addQueryItem("access_token",mp_google->token());
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);

    QNetworkRequest request(url);
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
        m_isFailed = true;
        m_isComplete = true;
        emit finished();
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
    m_isFailed = false;
    m_isComplete = true;
    emit finished();
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
        m_isFailed = true;
        m_isComplete = true;
        emit finished();
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " >> " << reply->errorString();
    reply->deleteLater();
}
