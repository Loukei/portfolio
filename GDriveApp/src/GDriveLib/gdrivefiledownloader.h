#ifndef GDRIVEFILEDOWNLOADER_H
#define GDRIVEFILEDOWNLOADER_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include <QSaveFile>

namespace GDrive {
/*!
 * \class GDriveFileDownloader
 * \brief Implement Google Drive `Files: get` method to download file,using fileID
 *
 * - 實作`Files: get`方法的下載檔案
 * - 必須在使用此class之前建立QFile指定存檔位置，使用QSharedPointer來操作QFile
 *
 * ## Reference 參考資料
 * - [Download files](https://developers.google.com/drive/api/v3/manage-downloads)
 */
class GDriveFileDownloader : public GDriveFileTask
{
    Q_OBJECT
public:
    /// Download file form fileID, write file when finished().
    /// GDriveFileDownloader doesnt take ownership of file
    explicit GDriveFileDownloader(const QString &fileId,
                                  const QString &fields,
                                  const QString &filepath,
                                  QOAuth2AuthorizationCodeFlow *parent);
    explicit GDriveFileDownloader(const QString &fileId,
                                  const QUrlQuery &args,
                                  const QString &filepath,
                                  QOAuth2AuthorizationCodeFlow *parent);
    /// destructor
    ~GDriveFileDownloader() override;
    /// start download
    bool start();
    /// return network reply
    QByteArray getReplyString() const;

    static QUrlQuery buildUrlArgs(bool acknowledgeAbuse = false,
                                  const QString &fields = QString(),
                                  bool supportsAllDrives = false);
public slots:
    void abort();

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QSaveFile m_file;
    /// save network reply usually file matadata
    QByteArray m_replyData = QByteArray();
    ///
    QUrl m_requestUrl;

private:
    bool checkAndOpenFile();
    /// Setup download url
    QUrl buildUrl(const QString &fileId,const QString &fields,const QString &key) const;
    QUrl buildUrl(const QString &fileId,const QString &alt,const QString &key,QUrlQuery args) const;
    /// setup request
    QNetworkRequest buildRequest(const QUrl &url) const;
    /// send download request
    QNetworkReply* requestDownload(const QNetworkRequest &request);
    /// Parse Error message form reply
    QString getErrorMessage(QNetworkReply *reply);

private slots:
    /// Solt for `QNetworkReply::finished` in `request_Download()`
    void onDownloadReplyFinished();
    /// Solt for `QNetworkReply::error` in `request_Download()`
    void onDownloadReplyError(QNetworkReply::NetworkError);
    /// Slot for ``
    void onDownloadReadyread();
};
}
#endif // GDRIVEFILEDOWNLOADER_H
