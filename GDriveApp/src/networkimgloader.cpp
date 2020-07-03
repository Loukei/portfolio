#include "networkimgloader.h"
#include <QNetworkReply>
#include <QPixmap>

NetworkImgLoader::NetworkImgLoader(const QUrl &url, QNetworkAccessManager *manager)
    :QObject (manager),m_manager(manager)
{
    requestResource(url);
}

QByteArray NetworkImgLoader::data() const
{
    return m_data;
}

void NetworkImgLoader::requestResource(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = m_manager->get(request);
    connect(reply,&QNetworkReply::finished,
            this,&NetworkImgLoader::onNetworkreplyFinished);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,[reply](){
        const QString info = QString("Network error: %1").arg(reply->errorString());
        qWarning("%s",qUtf8Printable(info));
    });
}

void NetworkImgLoader::onNetworkreplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error() == QNetworkReply::NetworkError::NoError){
        m_data = reply->readAll();
        emit dataReceived();
    }
    reply->deleteLater();
}
