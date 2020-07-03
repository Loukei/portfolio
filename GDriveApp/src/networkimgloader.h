#ifndef NETWORKIMGLOADER_H
#define NETWORKIMGLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QLabel>

class NetworkImgLoader : public QObject
{
    Q_OBJECT
public:
    explicit NetworkImgLoader(const QUrl &url,QNetworkAccessManager *manager);

    QByteArray data() const;
signals:
    /// emit when network reply receive and success
    void dataReceived();
protected:
    void requestResource(const QUrl &url);

protected slots:
    void onNetworkreplyFinished();

private:
    QNetworkAccessManager *m_manager;
    QByteArray m_data;
};

#endif // NETWORKIMGLOADER_H
