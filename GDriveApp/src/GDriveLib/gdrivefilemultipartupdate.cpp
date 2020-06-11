#include "gdrivefilemultipartupdate.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QFile>
#include <QUrlQuery>
#include <QMimeDatabase>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QTimer>
#include <QJsonParseError>
#include <QJsonDocument>

GDrive::GDriveFileMultipartUpdate::GDriveFileMultipartUpdate(const QString &filepath,
                                                             const QString &fileID, QOAuth2AuthorizationCodeFlow *parent):
    GDriveFileTask (parent)
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
    /// Setup Url
    m_url = buildUrl(fileID,"multipart");
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileMultipartUpdate::GDriveFileMultipartUpdate(const QString &filepath, const QString &fileID,
                                                             const QUrlQuery &args, QOAuth2AuthorizationCodeFlow *parent)
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
    /// Setup Url
    m_url = buildUrl(fileID,"multipart",args);
    //! Upload file
    request_UploadStart();
}

GDrive::GDriveFileMultipartUpdate::~GDriveFileMultipartUpdate()
{

}

GDrive::GDriveFileResource GDrive::GDriveFileMultipartUpdate::getResource() const
{
    QJsonParseError jsonErr;
    QJsonDocument doc = QJsonDocument::fromJson(m_replyData,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        qWarning() << Q_FUNC_INFO << jsonErr.errorString();
        return GDriveFileResource();
    }
    return GDriveFileResource(doc);
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
    //! Set optional parameters
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

void GDrive::GDriveFileMultipartUpdate::request_UploadStart()
{
    //! open file
    if(!m_file->open(QIODevice::ReadOnly)){
        m_errStr += QString("[Error]File %1: %2\n")
                .arg(m_file->fileName()).arg(m_file->errorString());
        taskFailed();
        return;
    }

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

    auto reply = mp_google->networkAccessManager()->sendCustomRequest(request,"PATCH",multipart);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileMultipartUpdate::on_UploadStart_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileMultipartUpdate::on_UploadStart_ReplyError);
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
        QTimer::singleShot(sleeptime,this
                           ,&GDriveFileMultipartUpdate::request_UploadStart);
    }else {
        m_errStr += QStringLiteral("[Error]Too many retrys.");
        taskFailed();
    }
}

void GDrive::GDriveFileMultipartUpdate::on_UploadStart_ReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Multipart Update reply error: " + reply->errorString();
        return;
    }
    //! QByteArray support implicit sharing
    m_replyData = reply->readAll();
    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileMultipartUpdate::on_UploadStart_ReplyError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus == (500|502|503|504|403)){    //Retry upload
        retry();
    }else if (httpStatus == 404) {              //Restart upload
        retry();
    }else {                                     //Unslove error
        m_errStr += QString("[Error]Simple Update reply error: %1\n")
                .arg(reply->errorString());
        taskFailed();
    }
    qWarning() << "[Error]Reply error code:" << httpStatus << " >> " << reply->errorString();
    reply->deleteLater();
}
