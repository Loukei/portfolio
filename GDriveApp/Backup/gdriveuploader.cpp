#include "gdriveuploader.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QNetworkAccessManager>
//#include <QFile>
#include <QDebug>
#include <QMimeDatabase>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QRegularExpression>
#include <QTimer>
//#include <QNetworkConfiguration> // detect network interrupt
#include <QRandomGenerator>
#include <QUrlQuery>

using namespace GDrive;

GDriveUploader::GDriveUploader(QOAuth2AuthorizationCodeFlow *parent):QObject(parent){}

GDriveUploader::~GDriveUploader(){}

int GDriveUploader::getSleepTime(int collisions)
{
    const int MAX_RETRY = 10; //! Max retry time,collisions over this will stop
    const int SLOT_TIME = 1;
    if(collisions > MAX_RETRY){
        return -1;
    }
    //! sleep time = SLOT_TIME * Random{0...(2^i-1)}
    int p = int(powl(2,collisions)) -1;
    int r = QRandomGenerator::global()->bounded(p);
    return int(SLOT_TIME * r);
}

GDriveSimpleUpload::GDriveSimpleUpload(
        QOAuth2AuthorizationCodeFlow *parent,const QString &filepath)
    :GDriveUploader(parent),m_google(parent)
{
    qInfo() << "GDriveSimpleUpload::GDriveSimpleUpload()";
    //! check file exist
    if(!QFile::exists(filepath)){
        qWarning() << "file doesnt exist " << filepath;
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        emit received(false);
        return;
    }
    //! Initial file
    m_file = new QFile(filepath,this);
    //! Upload file
    request_UploadStart();
}

GDriveSimpleUpload::~GDriveSimpleUpload()
{

}

QString GDriveSimpleUpload::errorString()
{
    return m_errStr;
}

void GDriveSimpleUpload::retry()
{
    static int collisions = 0;
    int sleeptime = getSleepTime(collisions);
    qInfo() << QString("GDriveSimpleUpload::retry(): (collisions,sleeptime) = (%1,%2)")
               .arg(collisions).arg(sleeptime);
    if(sleeptime == -1){
        m_errStr += "[Error]Too many retrys, stops";
        emit received(false);
        return;
    }
    QTimer::singleShot(sleeptime,this,&GDriveSimpleUpload::request_UploadStart);
    collisions++;
}

void GDriveSimpleUpload::request_UploadStart()
{
    qInfo() << "GDriveSimpleUpload::request_UploadStart()";
    if(!m_file->open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]File %1: %2\n")
                .arg(m_file->fileName()).arg(m_file->errorString());
        emit received(false);
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
    query.addQueryItem("access_token",m_google->token());
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,mimeType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,fileSize.toLatin1());
    request.setRawHeader("Authorization:",
                         QByteArray("Bearer " + m_google->token().toLatin1()));
    //! send request & connect reply handle function
    auto reply = m_google->networkAccessManager()->post(request,m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveSimpleUpload::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveSimpleUpload::on_UploadStart_ReplyError);
}

void GDriveSimpleUpload::on_UploadStart_ReplyFinished()
{
    qInfo() << "GDriveSimpleUpload::on_UploadStart_ReplyFinished()";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Simple Upload reply error: " + reply->errorString();
        return;
    }
    qInfo() << "Info :" << reply->readAll();
    emit received(true);
    reply->deleteLater();
}

void GDriveSimpleUpload::on_UploadStart_ReplyError(QNetworkReply::NetworkError)
{
    qInfo() << "GDriveSimpleUpload::on_UploadStart_ReplyError";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        m_errStr += QString("[Error]Simple Upload reply error: %1\n")
                .arg(reply->errorString());
        emit received(false);
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " >> " << reply->errorString();
    reply->deleteLater();
}

GDriveMultipartUpload::GDriveMultipartUpload(
        QOAuth2AuthorizationCodeFlow *parent, const QString &filepath)
    :GDriveUploader(parent),m_google(parent)
{
    qInfo() << "GDriveMultipartUpload::GDriveMultipartUpload()";
    // check file exist
    if(!QFile::exists(filepath)){
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        emit received(false);
        return;
    }
    // Initial file
    m_file = new QFile(filepath,this);
    request_UploadStart();
}

GDriveMultipartUpload::~GDriveMultipartUpload()
{
}

QString GDriveMultipartUpload::errorString()
{
    return m_errStr;
}

void GDriveMultipartUpload::retry()
{
    static int collisions = 0; //! retry times
    int sleeptime = getSleepTime(collisions);
    qInfo() << QString("GDriveMultipartUpload::retry(): (collisions,sleeptime) = (%1,%2)")
               .arg(collisions).arg(sleeptime);
    if(sleeptime == -1){
        m_errStr += "[Error]Too many retrys, stops";
        emit received(false);
        return;
    }
    QTimer::singleShot(sleeptime,this,&GDriveMultipartUpload::request_UploadStart);
    collisions++;
}

void GDriveMultipartUpload::request_UploadStart()
{
    qInfo() << "GDriveMultipartUpload::request_UploadStart()";
    //! open file
    if(!m_file->open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]File %1: %2\n")
                .arg(m_file->fileName()).arg(m_file->errorString());
        emit received(false);
        return;
    }
    //! the url will use to create new upload
    QUrlQuery query;
    query.addQueryItem("uploadType","multipart");
    query.addQueryItem("access_token",m_google->token());
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);
    QNetworkRequest request(url);
    const QString tokenHeader = QString("Bearer %1").arg(m_google->token());
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

    auto reply = m_google->networkAccessManager()->post(request,multipart);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveMultipartUpload::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveMultipartUpload::on_UploadStart_ReplyError);
}

void GDriveMultipartUpload::on_UploadStart_ReplyFinished()
{
    qInfo() << "GDriveMultipartUpload::on_UploadStart_ReplyFinished()";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Multipart Upload reply error: " + reply->errorString();
        return;
    }
    qInfo() << "Info :" << reply->readAll();
    emit received(true);
    reply->deleteLater();
}

void GDriveMultipartUpload::on_UploadStart_ReplyError(QNetworkReply::NetworkError)
{
    qInfo() << "GDriveMultipartUpload::on_UploadStart_ReplyError()";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        m_errStr += QString("[Error]Multipart Upload reply error: %1\n")
                .arg(reply->errorString());
        emit received(false);
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}

GDriveResumableUpload::GDriveResumableUpload(
        QOAuth2AuthorizationCodeFlow *parent, const QString &filepath):GDriveUploader (parent)
  ,m_google(parent),m_file(filepath,this),m_sessionUri(QUrl()),m_maxRetry(3)
{
    //! Initialize file and make sure file exist
    qDebug() << "GDriveResumableUpload::GDriveResumableUpload()";
    if(!m_file.exists(filepath)){
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        emit received(false);
        return;
    }
    //! Start upload session
    request_InitialSession();
}

GDriveResumableUpload::~GDriveResumableUpload()
{}

QString GDriveResumableUpload::errorString()
{
    return m_errStr;
}

void GDriveResumableUpload::setStatus(GDriveResumableUpload::Status status)
{
    m_status = status;
    switch (status) {
    case InitialSession:    //Start or Restart
        if(!m_sessionUri.isEmpty()){
            m_sessionUri.clear();
        }
        request_InitialSession();
        break;
    case ResumeUpload:      //Resume
        request_AskUploadStatus();
        break;
    case UploadFinished:    //Upload success
        emit received(true);
        break;
    case UploadFailed:      //Upload failed
        emit received(false);
        break;
    }
}

void GDriveResumableUpload::request_InitialSession()
{
    /// reference:
    /// https://developers.google.com/drive/api/v3/manage-uploads
    /// #example_initiate_a_resumable_upload_session
    qDebug() << "GDriveResumableUpload::request_InitialSession()";
    //! collect file info
    const QString fileName = QFileInfo(m_file.fileName()).baseName();
    const QString fileMimeType = QMimeDatabase().mimeTypeForFile(m_file).name();
    const qint64 fileSize = m_file.size();
    //! Build request
    QUrlQuery query;
    query.addQueryItem("uploadType","resumable");
    query.addQueryItem("access_token",m_google->token());
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);

//    QUrl url(QString("https://www.googleapis.com/upload/drive/v3/files"
//                     "?uploadType=resumable&access_token=%1").arg(m_google->token()));
    QNetworkRequest request(url);
    const QByteArray body = QString("{\"name\":\"%1\"}").arg(fileName).toUtf8();
    request.setHeader(QNetworkRequest::ContentLengthHeader,body.size());
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json; charset=UTF-8");
    request.setRawHeader("Authorization",
                         QString("Bearer %1").arg(m_google->token()).toLatin1());
    request.setRawHeader("X-Upload-Content-Type",fileMimeType.toLatin1());
    request.setRawHeader("X-Upload-Content-Length",QByteArray::number(fileSize));
    //! Send request
    auto uriReply = m_google->networkAccessManager()->post(request,body);
    connect(uriReply,&QNetworkReply::finished,
            this,&GDriveResumableUpload::on_InitialSession_ReplyFinished);
    connect(uriReply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveResumableUpload::on_InitialSession_ReplyError);
}

void GDriveResumableUpload::request_UploadStart()
{
    /// reference: https://developers.google.com/drive/api/v3/manage-uploads
    /// #example_upload_the_file
    qDebug() << "GDriveResumableUpload::request_UploadStart()";
    if(!m_file.open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]File %1:%2")
                .arg(m_file.fileName()).arg(m_file.errorString());
        setStatus(Status::UploadFailed);
        return;
    }
    //! collect file info
    const QString fileMimeType = QMimeDatabase().mimeTypeForFile(m_file).name();
    //! build request
    QNetworkRequest request(m_sessionUri);
    request.setHeader(QNetworkRequest::ContentTypeHeader,fileMimeType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,m_file.size());
    //! connect reply and slot
    auto reply = m_google->networkAccessManager()->put(request,&m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveResumableUpload::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveResumableUpload::on_UploadStart_ReplyError);
}

void GDriveResumableUpload::request_AskUploadStatus()
{
    /// refenence:https://developers.google.com/drive/api/v3/manage-uploads
    /// #example_resume_an_interrupted_upload
    qDebug() << "GDriveResumableUpload::request_UploadStart()";
    //! collect file info
    const QByteArray rangeVal = "bytes */" + QByteArray::number(m_file.size());
    //! construct request
    QNetworkRequest request(m_sessionUri);
    request.setHeader(QNetworkRequest::ContentLengthHeader,0);
    request.setRawHeader("Content-Range",rangeVal);
    //! send request without request body
    auto reply = m_google->networkAccessManager()->put(request,QByteArray());
    connect(reply,&QNetworkReply::finished,
            this,&GDriveResumableUpload::on_AskUploadStatus_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveResumableUpload::on_AskUploadStatus_ReplyError);
}

void GDriveResumableUpload::request_UploadResume(const qint64 offset)
{
    /// reference:https://developers.google.com/drive/api/v3/manage-uploads
    /// #example_resume_an_interrupted_upload
    qDebug() << "GDriveResumableUpload::request_UploadStart()";
    if(!m_file.open(QIODevice::ReadOnly)){
        qWarning() << "file " << m_file.fileName() << ": " << m_file.errorString();
        setStatus(Status::UploadFailed);
        return;
    }
    //! collect file info
    qDebug() << "Receive offset:" << offset;
    const qint64 contentlenVal = m_file.size() - (offset+1);
    const QByteArray contentrangeVal =
            QString("bytes %1-%2/%3").arg(offset+1).arg(m_file.size()-1).arg(m_file.size())
                                        .toLatin1();
    QNetworkRequest request(m_sessionUri);
    request.setHeader(QNetworkRequest::ContentLengthHeader,contentlenVal);
    request.setRawHeader("Content-Range",contentrangeVal);
    //! send request
    auto reply = m_google->networkAccessManager()->put(request,&m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveResumableUpload::on_UploadResume_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveResumableUpload::on_UploadResume_ReplyError);
}

void GDriveResumableUpload::on_InitialSession_ReplyFinished()
{
    qDebug() << "GDriveResumableUpload::on_InitialSession_ReplyFinished()";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        m_errStr += QString("[Error]Resumable Upload Initial session error: %1\n")
                .arg(reply->errorString());
        return;
    }
    //! save URI
    m_sessionUri = reply->header(QNetworkRequest::LocationHeader).toUrl();
    qInfo() << "Session URI received";
    //! continue to request upload start
    request_UploadStart();
    reply->deleteLater();
}

void GDriveResumableUpload::on_InitialSession_ReplyError(QNetworkReply::NetworkError)
{
    qDebug() << "GDriveResumableUpload::on_InitialSession_ReplyError";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == (500|502|503|504|403)){   // retry request
        setStatus(Status::InitialSession);
    }else if(httpStatus == 404){               // restart upload
        setStatus(Status::InitialSession);
    }else {                                    // Bad End
        setStatus(Status::UploadFailed);
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}

void GDriveResumableUpload::on_UploadStart_ReplyFinished()
{
    qDebug() << "GDriveResumableUpload::on_UploadStart_ReplyFinished()";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Resumable Upload reply error: " << reply->errorString();
        return;
    }
    qInfo() << "Info :" << reply->readAll();
    setStatus(Status::UploadFinished);
    reply->deleteLater();
}

void GDriveResumableUpload::on_UploadStart_ReplyError(QNetworkReply::NetworkError)
{
    qDebug() << "GDriveResumableUpload::on_UploadStart_ReplyError";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == 503){                         // request resume upload
        setStatus(Status::ResumeUpload);
    }else if(httpStatus == (500|502|504|403)){     // request retry/resume upload
        setStatus(Status::ResumeUpload);
    }else if(httpStatus == 404){                   // restart upload
        setStatus(Status::InitialSession);
    }else {                                         // Bad End
        setStatus(Status::UploadFailed);
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}

void GDriveResumableUpload::on_AskUploadStatus_ReplyFinished()
{
    qDebug() << "GDriveResumableUpload::on_AskUploadStatus_ReplyFinished()";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Resumable Upload reply error: " << reply->errorString();
        return;
    }
    qInfo() << "Info :" << reply->readAll();
    setStatus(Status::UploadFinished);  //200 means upload has complete
    reply->deleteLater();
}

void GDriveResumableUpload::on_AskUploadStatus_ReplyError(QNetworkReply::NetworkError)
{
    qDebug() << "GDriveResumableUpload::on_AskUploadStatus_ReplyError";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    qWarning() << "[Error]" << reply->errorString();
    //! get Http status code
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //! dispatch to relate func
    if(httpStatus == 308){                             //continue upload
        auto pattern = QRegularExpression("Range: bytes=0-(?<offset>\\d+)");
        auto match = pattern.match(reply->rawHeader("Content-Range")); //"Range: bytes=0-XX"
        if(match.hasMatch()){
            const qint64 offset = match.captured("offset").toLongLong();
            request_UploadResume(offset);
        }else {
            qDebug() << "Match failed:" << reply->rawHeader("Content-Range");
        }
    }else if (httpStatus == (500|502|503|504|403)) {   //Resume
        setStatus(Status::ResumeUpload);
    }else if (httpStatus == 404) {                     //Restart
        setStatus(Status::InitialSession);
    }else {                                             //Badend
        setStatus(Status::UploadFailed);
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}

void GDriveResumableUpload::on_UploadResume_ReplyFinished()
{
    qDebug() << "GDriveResumableUpload::on_UploadResume_ReplyFinished()";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Resumable Upload reply error: " << reply->errorString();
        return;
    }
    qInfo() << "Info :" << reply->readAll();
    setStatus(Status::UploadFinished);
    reply->deleteLater();
}

void GDriveResumableUpload::on_UploadResume_ReplyError(QNetworkReply::NetworkError)
{
    qDebug() << "GDriveResumableUpload::on_UploadResume_ReplyError";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    //! get Http status code
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //! dispatch to relate func
    if(httpStatus == (500|502|503|504|403)){   // retry/resume request
        setStatus(Status::InitialSession);
    }else if(httpStatus == 404){               // restart upload
        setStatus(Status::InitialSession);
    }else {                                     // Bad End
        setStatus(Status::UploadFailed);
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}
