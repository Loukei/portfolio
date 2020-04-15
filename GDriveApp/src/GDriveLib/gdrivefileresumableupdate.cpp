#include "gdrivefileresumableupdate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QUrlQuery>
#include <QRegularExpression>
#include <QJsonParseError>
#include <QJsonDocument>

GDrive::GDriveFileResumableUpdate::GDriveFileResumableUpdate(QOAuth2AuthorizationCodeFlow *parent,
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
    //! Setup request url
    m_url = setupUrl(fileID);
    //! request session URI
    request_InitialSession();
}

GDrive::GDriveFileResumableUpdate::GDriveFileResumableUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                                             const QString &filepath,
                                                             const GDrive::FileUpdateArgs &args)
    :GDriveFileTask (parent)
{
    //! Initial file
    m_file = new QFile(filepath,this);
    //! Setup request url
    m_url = setupUrl(args);
    //! request session URI
    request_InitialSession();
}

GDrive::GDriveFileResumableUpdate::~GDriveFileResumableUpdate()
{

}

GDrive::GDriveFileResource GDrive::GDriveFileResumableUpdate::getResource() const
{
    QJsonParseError jsonErr;
    QJsonDocument doc = QJsonDocument::fromJson(m_replyData,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        qWarning() << Q_FUNC_INFO << jsonErr.errorString();
        return GDriveFileResource();
    }
    return GDriveFileResource(doc);
}

QByteArray GDrive::GDriveFileResumableUpdate::getReplyString() const
{
    return m_replyData;
}

void GDrive::GDriveFileResumableUpdate::request_InitialSession()
{
    /// reference:
    /// https://developers.google.com/drive/api/v3/manage-uploads
    /// #example_initiate_a_resumable_upload_session
    qDebug() << Q_FUNC_INFO;
    //! collect file info
    const QString fileName = QFileInfo(m_file->fileName()).baseName();
    const QString fileMimeType = QMimeDatabase().mimeTypeForFile(*m_file).name();
    const qint64 fileSize = m_file->size();
    //! Build request
    QNetworkRequest request(m_url);
    const QByteArray body = QString("{\"name\":\"%1\"}").arg(fileName).toUtf8();
    request.setHeader(QNetworkRequest::ContentLengthHeader,body.size());
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json; charset=UTF-8");
    request.setRawHeader("Authorization",
                         QString("Bearer %1").arg(mp_google->token()).toLatin1());
    request.setRawHeader("X-Upload-Content-Type",fileMimeType.toLatin1());
    request.setRawHeader("X-Upload-Content-Length",QByteArray::number(fileSize));
    //! Send request
//    auto reply = mp_google->networkAccessManager()->post(request,body);
    auto reply = mp_google->networkAccessManager()->sendCustomRequest(request,"PATCH",body);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileResumableUpdate::on_InitialSession_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileResumableUpdate::on_InitialSession_ReplyError);
}

void GDrive::GDriveFileResumableUpdate::request_UploadStart()
{
    qDebug() << Q_FUNC_INFO;
    if(!m_file->open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]File %1:%2")
                .arg(m_file->fileName()).arg(m_file->errorString());
        taskFailed();
        return;
    }
    //! collect file info
    const QString fileMimeType = QMimeDatabase().mimeTypeForFile(*m_file).name();
    //! build request
    QNetworkRequest request(m_sessionUri);
    request.setHeader(QNetworkRequest::ContentTypeHeader,fileMimeType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,m_file->size());
    //! connect reply and slot
    auto reply = mp_google->networkAccessManager()->put(request,m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileResumableUpdate::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileResumableUpdate::on_UploadStart_ReplyError);
}

void GDrive::GDriveFileResumableUpdate::request_AskUploadStatus()
{
    qDebug() << Q_FUNC_INFO;
    //! collect file info
    const QByteArray rangeVal = "bytes */" + QByteArray::number(m_file->size());
    //! construct request
    QNetworkRequest request(m_sessionUri);
    request.setHeader(QNetworkRequest::ContentLengthHeader,0);
    request.setRawHeader("Content-Range",rangeVal);
    //! send request without request body
    auto reply = mp_google->networkAccessManager()->put(request,QByteArray());
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileResumableUpdate::on_AskUploadStatus_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileResumableUpdate::on_AskUploadStatus_ReplyError);
}

void GDrive::GDriveFileResumableUpdate::request_UploadResume(const qint64 offset)
{
    qDebug() << Q_FUNC_INFO;
    if(!m_file->open(QIODevice::ReadOnly)){
        qWarning() << "file " << m_file->fileName() << ": " << m_file->errorString();
        taskFailed();
        return;
    }
    //! collect file info
    qDebug() << "Receive offset:" << offset;
    const qint64 contentlenVal = m_file->size() - (offset+1);
    const QByteArray contentrangeVal =
            QString("bytes %1-%2/%3").arg(offset+1).arg(m_file->size()-1).arg(m_file->size())
                                        .toLatin1();
    QNetworkRequest request(m_sessionUri);
    request.setHeader(QNetworkRequest::ContentLengthHeader,contentlenVal);
    request.setRawHeader("Content-Range",contentrangeVal);
    //! send request
    auto reply = mp_google->networkAccessManager()->put(request,m_file);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileResumableUpdate::on_UploadResume_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileResumableUpdate::on_UploadResume_ReplyError);
}

QUrl GDrive::GDriveFileResumableUpdate::setupUrl(const QString &fileID)
{
    //https://www.googleapis.com/upload/drive/v3/files?uploadType=resumable&access_token=%1
    QUrlQuery query;
    query.addQueryItem("uploadType","resumable");
    query.addQueryItem("access_token",mp_google->token());
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files/" + fileID);
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileResumableUpdate::setupUrl(const GDrive::FileUpdateArgs &args)
{
    //https://www.googleapis.com/upload/drive/v3/files?uploadType=resumable&access_token=%1
    QUrlQuery query;
    query.addQueryItem("uploadType","resumable");
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

void GDrive::GDriveFileResumableUpdate::on_InitialSession_ReplyFinished()
{
    qInfo() << Q_FUNC_INFO;
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

void GDrive::GDriveFileResumableUpdate::on_InitialSession_ReplyError(QNetworkReply::NetworkError)
{
    qDebug() << Q_FUNC_INFO;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == (500|502|503|504|403)){   // retry request
        request_InitialSession();
    }else if(httpStatus == 404){               // restart upload
        state_RestartUpload();
    }else {                                    // Bad End
        taskFailed();
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}

void GDrive::GDriveFileResumableUpdate::on_UploadStart_ReplyFinished()
{
    qInfo() << Q_FUNC_INFO;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Resumable Upload reply error: " << reply->errorString();
        return;
    }
    //! upload success: save file resource, set state, emit finished
    m_replyData = reply->readAll();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileResumableUpdate::on_UploadStart_ReplyError(QNetworkReply::NetworkError)
{
    qDebug() << Q_FUNC_INFO;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == 503){                         // request resume upload
        request_AskUploadStatus();
    }else if(httpStatus == (500|502|504|403)){     // request retry/resume upload
        state_ResumeUpload();
    }else if(httpStatus == 404){                   // restart upload
        state_RestartUpload();
    }else {                                        // Bad End
        taskFailed();
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}

void GDrive::GDriveFileResumableUpdate::on_AskUploadStatus_ReplyFinished()
{
    qDebug() << Q_FUNC_INFO;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Resumable Upload reply error: " << reply->errorString();
        return;
    }
    //200 means upload has complete
    qInfo() << "Info :" << reply->readAll();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileResumableUpdate::on_AskUploadStatus_ReplyError(QNetworkReply::NetworkError)
{
    qDebug() << Q_FUNC_INFO;
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
            qWarning() << "Match failed:" << reply->rawHeader("Content-Range");
            //STOP
            taskFailed();
        }
    }else if (httpStatus == (500|502|503|504|403)) {   //Resume
        state_ResumeUpload();
    }else if (httpStatus == 404) {                     //Restart
        state_RestartUpload();
    }else {                                             //Badend
        taskFailed();
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}

void GDrive::GDriveFileResumableUpdate::on_UploadResume_ReplyFinished()
{
    qDebug() << Q_FUNC_INFO;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Resumable Upload reply error: " << reply->errorString();
        return;
    }
    qInfo() << "Info :" << reply->readAll();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileResumableUpdate::on_UploadResume_ReplyError(QNetworkReply::NetworkError)
{
    qDebug() << Q_FUNC_INFO;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    //! get Http status code
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //! dispatch to relate func
    if(httpStatus == (500|502|503|504|403)){   // retry/resume request
        state_ResumeUpload();
    }else if(httpStatus == 404){               // restart upload
        state_RestartUpload();
    }else {                                     // Bad End
        taskFailed();
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}

void GDrive::GDriveFileResumableUpdate::state_RestartUpload()
{
    m_sessionUri.clear();
    request_InitialSession();
}

void GDrive::GDriveFileResumableUpdate::state_ResumeUpload()
{
    request_AskUploadStatus();
}

void GDrive::GDriveFileResumableUpdate::state_FailedUpload()
{
    m_isFailed = true;
    m_isComplete = true;
    emit finished();
}

void GDrive::GDriveFileResumableUpdate::state_CompleteUpload()
{
    m_isFailed = false;
    m_isComplete = true;
    emit finished();
}
