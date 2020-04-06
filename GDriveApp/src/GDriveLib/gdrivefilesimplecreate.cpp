#include "gdrivefilesimplecreate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QFile>
#include <QUrlQuery>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonParseError>

GDrive::GDriveFileSimpleCreate::GDriveFileSimpleCreate(QOAuth2AuthorizationCodeFlow *parent,
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
    //! Initial file
    m_file = new QFile(filepath,this);
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileSimpleCreate::~GDriveFileSimpleCreate()
{
    m_file->close();
}

GDrive::GDriveFileResource GDrive::GDriveFileSimpleCreate::getResource() const
{
    QJsonParseError jsonErr;
    QJsonDocument doc = QJsonDocument::fromJson(m_replyData,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        qWarning() << Q_FUNC_INFO << jsonErr.errorString();
        return GDriveFileResource();
    }
    return GDriveFileResource(doc);
}

QByteArray GDrive::GDriveFileSimpleCreate::getReplyString() const
{
    return m_replyData;
}

void GDrive::GDriveFileSimpleCreate::request_UploadStart()
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
//    QByteArray fileBody("\r\n\r\n");
//    fileBody.append(m_file->readAll());
//    m_file->close();
    QUrlQuery query;
    query.addQueryItem("uploadType","media");
    query.addQueryItem("access_token",mp_google->token());
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,mimeType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,fileSize.toLatin1());
    request.setRawHeader("Authorization:",
                         QByteArray("Bearer " + mp_google->token().toLatin1()));
    //! send request & connect reply handle function
    auto reply = mp_google->networkAccessManager()->post(request,m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileSimpleCreate::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileSimpleCreate::on_UploadStart_ReplyError);
}

void GDrive::GDriveFileSimpleCreate::retry()
{
    static int retryCount = 0;
    int sleeptime = getExpBackoffSleepTime(retryCount,1,10);
    if(sleeptime != -1){
        retryCount++;
        QTimer::singleShot(sleeptime,this
                           ,&GDriveFileSimpleCreate::request_UploadStart);
    }else {
        m_errStr += QStringLiteral("[Error]Too many retrys.");
        taskFailed();
    }
}

void GDrive::GDriveFileSimpleCreate::on_UploadStart_ReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Simple Upload reply error: " + reply->errorString();
        return;
    }
    //! QByteArray support implicit sharing
    m_replyData = reply->readAll();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileSimpleCreate::on_UploadStart_ReplyError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    auto replyErr = QString("[Error]Simple Update reply error %1: %2\n")
            .arg(httpStatus)
            .arg(reply->errorString());
    qWarning() << Q_FUNC_INFO << replyErr;
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        m_errStr += replyErr;
        taskFailed();
    }
    reply->deleteLater();
}
