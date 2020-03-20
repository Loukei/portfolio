#include "gdrivefilesearch.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>

GDrive::GDriveFileSearch::GDriveFileSearch(QOAuth2AuthorizationCodeFlow *parent,
                                           const QString &q,
                                           const QString &spaces,
                                           const QString &fields,
                                           const QString &pageToken)
    :GDriveFileTask (parent)
{
    request_Search(q,spaces,fields,pageToken);
}

GDrive::GDriveFileSearch::~GDriveFileSearch()
{

}

GDrive::GDriveFileList GDrive::GDriveFileSearch::getFileList() const
{
    return GDriveFileList(m_replyData);
}

QByteArray GDrive::GDriveFileSearch::getReplyString() const
{
    return m_replyData;
}

void GDrive::GDriveFileSearch::request_Search(const QString &q,
                                              const QString &spaces,
                                              const QString &fields,
                                              const QString &pageToken)
{
    QUrlQuery query;
//    query.addQueryItem("corpora","user");
    query.addQueryItem("q",q);
    if(!spaces.isEmpty()){
        query.addQueryItem("spaces",spaces);
    }
    if(!fields.isEmpty()){
        query.addQueryItem("fields",fields);
    }
    if(!pageToken.isEmpty()){
        query.addQueryItem("pageToken",pageToken);
    }
    query.addQueryItem("access_token",mp_google->token());
    QUrl url("https://www.googleapis.com/drive/v3/files");
    url.setQuery(query);

    QNetworkRequest request(url);
    const QString tokenHeader = QString("Bearer %1").arg(mp_google->token());
    request.setRawHeader(QByteArray("Authorization"),tokenHeader.toLatin1());

    auto reply = mp_google->networkAccessManager()->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&GDriveFileSearch::on_Search_ReplyFinished);
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,&GDriveFileSearch::on_Search_ReplyFinished);
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
    qWarning() << errStr;
    m_errStr += errStr;
    m_replyData = reply->readAll();

    m_isFailed = true;
    m_isComplete = true;
    emit finished();
    reply->deleteLater();
}
