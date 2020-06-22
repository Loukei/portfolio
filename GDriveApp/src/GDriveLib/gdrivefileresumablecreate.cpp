#include "gdrivefileresumablecreate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QUrlQuery>
#include <QRegularExpression>

GDrive::GDriveFileResumableCreate::GDriveFileResumableCreate(const QString &filepath,
                                                             QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent)
{
    //! check file exist
    if(!QFile::exists(filepath)){
        qWarning() << "file doesnt exist " << filepath;
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        taskFailed();
        return;
    }
    //! Setup Url
    m_iniUrl = buildUrl("resumable");
    //! Initial file
    m_file = new QFile(filepath,this);
    //! request session URI
    request_InitialSession();
}

GDrive::GDriveFileResumableCreate::GDriveFileResumableCreate(const QString &filepath,
                                                             const QUrlQuery &args,
                                                             QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent)
{
    //! check file exist
    if(!QFile::exists(filepath)){
        qWarning() << "file doesnt exist " << filepath;
        m_errStr += QString("[Error]File not exist: %1\n").arg(filepath);
        taskFailed();
        return;
    }
    //! Setup Url
    m_iniUrl = buildUrl("resumable",args);
    //! Initial file
    m_file = new QFile(filepath,this);
    //! request session URI
    request_InitialSession();
}

GDrive::GDriveFileResumableCreate::~GDriveFileResumableCreate()
{

}

QByteArray GDrive::GDriveFileResumableCreate::getReplyString() const
{
    return m_replyData;
}

QUrlQuery GDrive::GDriveFileResumableCreate::buildUrlArgs(const bool enforceSingleParent,
                                                          const bool ignoreDefaultVisibility,
                                                          const bool keepRevisionForever,
                                                          const QString &ocrLanguage,
                                                          const bool supportsAllDrives,
                                                          const bool useContentAsIndexableText)
{
    QUrlQuery args;
    args.addQueryItem("uploadType","resumable");
    if(enforceSingleParent){
        args.addQueryItem("enforceSingleParent",
                          QVariant(enforceSingleParent).toString());
    }
    if(ignoreDefaultVisibility){
        args.addQueryItem("ignoreDefaultVisibility",
                           QVariant().toString());
    }
    if(keepRevisionForever){
        args.addQueryItem("keepRevisionForever",
                           QVariant().toString());
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

void GDrive::GDriveFileResumableCreate::request_InitialSession()
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
    QNetworkRequest request(m_iniUrl);
    const QByteArray body = QString("{\"name\":\"%1\"}").arg(fileName).toUtf8();
    request.setHeader(QNetworkRequest::ContentLengthHeader,body.size());
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json; charset=UTF-8");
    request.setRawHeader("Authorization",
                         QString("Bearer %1").arg(mp_google->token()).toLatin1());
    request.setRawHeader("X-Upload-Content-Type",fileMimeType.toLatin1());
    request.setRawHeader("X-Upload-Content-Length",QByteArray::number(fileSize));
    //! Send request
    auto reply = mp_google->networkAccessManager()->post(request,body);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileResumableCreate::on_InitialSession_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileResumableCreate::on_InitialSession_ReplyError);
}

QUrl GDrive::GDriveFileResumableCreate::buildUrl(const QString &uploadType) const
{
    QUrlQuery query;
    //https://www.googleapis.com/upload/drive/v3/files?uploadType=resumable&access_token=%1
    query.addQueryItem("uploadType",uploadType);
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    url.setQuery(query);
    return url;
}

QUrl GDrive::GDriveFileResumableCreate::buildUrl(const QString &uploadType,QUrlQuery args) const
{
    auto url = QUrl("https://www.googleapis.com/upload/drive/v3/files");
    args.addQueryItem("uploadType",uploadType);
    url.setQuery(args);
    return url;
}

void GDrive::GDriveFileResumableCreate::request_UploadStart()
{
    /// reference: https://developers.google.com/drive/api/v3/manage-uploads
    /// #example_upload_the_file
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
            this,&GDriveFileResumableCreate::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileResumableCreate::on_UploadStart_ReplyError);
}

void GDrive::GDriveFileResumableCreate::request_AskUploadStatus()
{
    /// refenence:https://developers.google.com/drive/api/v3/manage-uploads
    /// #example_resume_an_interrupted_upload
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
            this,&GDriveFileResumableCreate::on_AskUploadStatus_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileResumableCreate::on_AskUploadStatus_ReplyError);
}

void GDrive::GDriveFileResumableCreate::request_UploadResume(const qint64 offset)
{
    /// reference:https://developers.google.com/drive/api/v3/manage-uploads
    /// #example_resume_an_interrupted_upload
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
            this,&GDriveFileResumableCreate::on_UploadResume_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileResumableCreate::on_UploadResume_ReplyError);
}

void GDrive::GDriveFileResumableCreate::on_InitialSession_ReplyFinished()
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

void GDrive::GDriveFileResumableCreate::on_InitialSession_ReplyError(QNetworkReply::NetworkError)
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

void GDrive::GDriveFileResumableCreate::on_UploadStart_ReplyFinished()
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

void GDrive::GDriveFileResumableCreate::on_UploadStart_ReplyError(QNetworkReply::NetworkError)
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

void GDrive::GDriveFileResumableCreate::on_AskUploadStatus_ReplyFinished()
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

void GDrive::GDriveFileResumableCreate::on_AskUploadStatus_ReplyError(QNetworkReply::NetworkError)
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
    }else {                                            //Badend
        taskFailed();
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " " << reply->errorString();
    reply->deleteLater();
}

void GDrive::GDriveFileResumableCreate::on_UploadResume_ReplyFinished()
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

void GDrive::GDriveFileResumableCreate::on_UploadResume_ReplyError(QNetworkReply::NetworkError)
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

void GDrive::GDriveFileResumableCreate::state_RestartUpload()
{
    m_sessionUri.clear();
    request_InitialSession();
}

void GDrive::GDriveFileResumableCreate::state_ResumeUpload()
{
    request_AskUploadStatus();
}
