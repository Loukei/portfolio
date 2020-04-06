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
    //! Upload file
    request_UploadStart(fileID);
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

void GDrive::GDriveFileSimpleUpdate::request_UploadStart(const QString &fileID)
{
    //! open file
    if(!m_file->open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]File %1: %2\n")
                .arg(m_file->fileName()).arg(m_file->errorString());
        m_isFailed = true;
        m_isComplete = true;
        emit finished();
        return;
    }
    //! gather file message to build simple upload request
    QString mimeType = QMimeDatabase().mimeTypeForFile(m_file->fileName()).name();
    QString fileSize = QString::number(m_file->size());
    QUrlQuery query;
    query.addQueryItem("uploadType","media");
    query.addQueryItem("access_token",mp_google->token());
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files/" + fileID);
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,mimeType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,fileSize.toLatin1());
    request.setRawHeader("Authorization:",
                         QByteArray("Bearer " + mp_google->token().toLatin1()));
    //! send request & connect reply handle function
    auto reply = mp_google->networkAccessManager()->post(request,m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileSimpleUpdate::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileSimpleUpdate::on_UploadStart_ReplyError);
}

//void GDrive::GDriveFileSimpleUpdate::retry()
//{
//    static int retryCount = 0;
//    int sleeptime = GDriveFileTask::getExpBackoffSleepTime(retryCount,1,10);
//    if(sleeptime != -1){
//        retryCount++;
//        QTimer::singleShot(sleeptime,
//                           this,
//                           SLOT(GDriveFileSimpleUpdate::request_UploadStart()));
//    }else {
//        m_errStr += QStringLiteral("[Error]Too many retrys.");
//        m_isFailed = true;
//        m_isComplete = true;
//        emit finished();
//    }
//}

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
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    auto replyErr = QString("[Error]Simple Update reply error %1: %2\n")
            .arg(httpStatus)
            .arg(reply->errorString());
    qWarning() << replyErr;
//    if(httpStatus == (500|502|503|504|403)){    //Retry upload
//        retry();
//    }else if (httpStatus == 404) {              //Restart upload
//        retry();
//    }else {                                     //Unslove error
//        m_errStr += QString("[Error]Simple Update reply error: %1\n")
//                .arg(reply->errorString());
//        m_isFailed = true;
//        m_isComplete = true;
//        emit finished();
//    }
    m_errStr += replyErr;
    m_isFailed = true;
    m_isComplete = true;
    emit finished();
    reply->deleteLater();
}
