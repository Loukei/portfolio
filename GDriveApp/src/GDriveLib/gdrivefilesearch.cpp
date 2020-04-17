#include "gdrivefilesearch.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>
#include <QJsonParseError>
#include <QJsonDocument>

GDrive::GDriveFileSearch::GDriveFileSearch(QOAuth2AuthorizationCodeFlow *parent,
                                           const QString &q,
                                           const QString &pageToken)
    :GDriveFileTask (parent)
{
    GDrive::FileListArgs args = {"","","","",100,pageToken,q,""};
    request_Search(args);
}

GDrive::GDriveFileSearch::GDriveFileSearch(QOAuth2AuthorizationCodeFlow *parent,
                                           const GDrive::FileListArgs &args)
    :GDriveFileTask (parent)
{
    request_Search(args);
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

void GDrive::GDriveFileSearch::request_Search(const GDrive::FileListArgs &args)
{
    QUrl url = setupUrl(args);
    QNetworkRequest request(url);
    const QString tokenHeader = QString("Bearer %1").arg(mp_google->token());
    request.setRawHeader(QByteArray("Authorization"),tokenHeader.toLatin1());

    auto reply = mp_google->networkAccessManager()->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileSearch::on_Search_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileSearch::on_Search_ReplyFinished);
}

QUrl GDrive::GDriveFileSearch::setupUrl(const GDrive::FileListArgs &args)
{
    QUrlQuery query;
    if(!args.corpora().isEmpty()){
        query.addQueryItem("corpora",args.corpora());
    }
    if(!args.driveId().isEmpty()){
        query.addQueryItem("driveId",args.driveId());
    }
    if(!args.fields().isEmpty()){
        query.addQueryItem("fields",args.fields());
    }
    if(!args.orderBy().isEmpty()){
        query.addQueryItem("orderBy",args.orderBy());
    }
    if(args.pageSize() > 0){
        query.addQueryItem("pageSize",QString::number(args.pageSize()));
    }
    if(!args.pageToken().isEmpty()){
        query.addQueryItem("pageToken",args.pageToken());
    }
    if(!args.q().isEmpty()){
        query.addQueryItem("q",args.q());
    }
    if(!args.spaces().isEmpty()){
        query.addQueryItem("spaces",args.spaces());
    }

    QUrl url("https://www.googleapis.com/drive/v3/files");
    url.setQuery(query);

    return url;
}

void GDrive::GDriveFileSearch::on_Search_ReplyFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error()!=QNetworkReply::NoError){
        qWarning() << "[Error]Search reply error: " + reply->errorString();
        return;
    }
    m_replyData = reply->readAll();

    m_isFailed = false;
    m_isComplete = true;
    emit finished();
    reply->deleteLater();
}

void GDrive::GDriveFileSearch::on_Search_ReplyError(QNetworkReply::NetworkError)
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

    m_isFailed = true;
    m_isComplete = true;
    emit finished();
    reply->deleteLater();
}
