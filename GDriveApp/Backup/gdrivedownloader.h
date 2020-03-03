#ifndef GDRIVEDOWNLOADER_H
#define GDRIVEDOWNLOADER_H

#include <QObject>
#include <QNetworkReply> //for network slot

class QOAuth2AuthorizationCodeFlow;
class QFile;

namespace GDrive {
class GDriveDownloader : public QObject
{
    Q_OBJECT
public:
    explicit GDriveDownloader(
            QOAuth2AuthorizationCodeFlow *parent,const QString &fileId,
            QFile *file,const QString fields);
    virtual ~GDriveDownloader();
    /// return human readable string after receive(false) emit
    QString errorString();
signals:
    /// receive data
    void received(bool success);
protected:
    /// calculate exponential backoff sleep time
    int getSleepTime(int collisions);
private:
    /// An instance to manage Oauth request, and Token resource, no need to delete
    QOAuth2AuthorizationCodeFlow * const mp_google;
    /// Upload target file
    QFile * const mp_file;
    /// error string when receive(false) emit
    QString m_errStr;

    /// send download request
    void request_Download(const QString &fileId,const QString fields);
private slots:
    void on_Download_ReplyFinished();
    void on_Download_ReplyError(QNetworkReply::NetworkError);
};
}

#endif // GDRIVEDOWNLOADER_H
