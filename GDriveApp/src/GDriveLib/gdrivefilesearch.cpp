#include "gdrivefilesearch.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>
#include <QJsonParseError>
#include <QJsonDocument>

GDrive::GDriveFileSearch::GDriveFileSearch(const QString &q,
                                           const QString &pageToken,
                                           QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask (parent)
{

    QUrl url = buildUrl(
                buildUrlArgs("","","",false,"",100,pageToken,q,"",false));
    request_Search(url);
}

GDrive::GDriveFileSearch::GDriveFileSearch(const QUrlQuery &args,
                                           QOAuth2AuthorizationCodeFlow *parent)
    :GDriveFileTask(parent)
{
    QUrl url = buildUrl(args);
    request_Search(url);
}

GDrive::GDriveFileSearch::~GDriveFileSearch()
{

}

GDrive::GDriveFileResourceList GDrive::GDriveFileSearch::getFileList() const
{
    QJsonParseError jsonErr;
    QJsonDocument doc = QJsonDocument::fromJson(m_replyData,&jsonErr);
    if(jsonErr.error != QJsonParseError::NoError){
        qWarning() << Q_FUNC_INFO << jsonErr.errorString();
        return GDriveFileResourceList();
    }
    return GDriveFileResourceList(doc);
}

QByteArray GDrive::GDriveFileSearch::getReplyString() const
{
    return m_replyData;
}

QUrlQuery GDrive::GDriveFileSearch::buildUrlArgs(const QString &corpora,
                                                 const QString &driveId,
                                                 const QString &fields,
                                                 const bool includeItemsFromAllDrives,
                                                 const QString &orderBy,
                                                 const int pageSize,
                                                 const QString &pageToken,
                                                 const QString &q,
                                                 const QString &spaces,
                                                 const bool supportsAllDrives)
{
    QUrlQuery args;
    if(!corpora.isEmpty()){
        args.addQueryItem("corpora",corpora);
    }
    if(!driveId.isEmpty()){
        args.addQueryItem("driveId",driveId);
    }
    if(!fields.isEmpty()){
        args.addQueryItem("fields",fields);
    }
    if(includeItemsFromAllDrives){
        args.addQueryItem("includeItemsFromAllDrives",
                           QVariant(includeItemsFromAllDrives).toString());
    }
    if(!orderBy.isEmpty()){
        args.addQueryItem("orderBy",orderBy);
    }
    if(pageSize > 0){
        args.addQueryItem("pageSize",QString::number(pageSize));
    }
    if(!pageToken.isEmpty()){
        args.addQueryItem("pageToken",pageToken);
    }
    if(!q.isEmpty()){
        args.addQueryItem("q",q);
    }
    if(!spaces.isEmpty()){
        args.addQueryItem("spaces",spaces);
    }
    if(supportsAllDrives){
        args.addQueryItem("supportsAllDrives",
                           QVariant(supportsAllDrives).toString());
    }
    return args;
}

void GDrive::GDriveFileSearch::request_Search(const QUrl &url)
{
    QNetworkRequest request(url);
    const QString tokenHeader = QString("Bearer %1").arg(mp_google->token());
    request.setRawHeader(QByteArray("Authorization"),tokenHeader.toLatin1());

    auto reply = mp_google->networkAccessManager()->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileSearch::onSearchReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileSearch::onSearchReplyFinished);
}

QUrl GDrive::GDriveFileSearch::buildUrl() const
{
    return QUrl("https://www.googleapis.com/drive/v3/files");
}

QUrl GDrive::GDriveFileSearch::buildUrl(QUrlQuery args) const
{
    QUrl url("https://www.googleapis.com/drive/v3/files");
    url.setQuery(args);
    return url;
}

void GDrive::GDriveFileSearch::onSearchReplyFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Search reply error: " + reply->errorString();
        return;
    }
    m_replyData = reply->readAll();

    taskSucceeded();
    reply->deleteLater();
}

void GDrive::GDriveFileSearch::onSearchReplyError(QNetworkReply::NetworkError)
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //! Simply show error message
    QString errStr = QString("[Error]Reply error code:%1 >> %2")
            .arg(httpStatus)
            .arg(reply->errorString());
    qWarning() << Q_FUNC_INFO << errStr;
    m_errStr += errStr;
    m_replyData = reply->readAll();

    taskFailed();
    reply->deleteLater();
}
