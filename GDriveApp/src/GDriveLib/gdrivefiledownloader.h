#ifndef GDRIVEFILEDOWNLOADER_H
#define GDRIVEFILEDOWNLOADER_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

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
                                  QSharedPointer<QFile> file,
                                  QOAuth2AuthorizationCodeFlow *parent);
    explicit GDriveFileDownloader(const QString &fileId,
                                  const QUrlQuery &args,
                                  QSharedPointer<QFile> file,
                                  QOAuth2AuthorizationCodeFlow *parent);
    /// destructor
    ~GDriveFileDownloader() override;
    /// return network reply
    QByteArray getReplyString() const;

    static QUrlQuery buildUrlArgs(bool acknowledgeAbuse = false,
                                  const QString &fields = QString(),
                                  bool supportsAllDrives = false);

private:
    /// pointer to Shared QFile ready to write,DELETE by owner
    QSharedPointer<QFile> mp_file;
    /// save network reply usually file matadata
    QByteArray m_replyData = QByteArray();

private:
    /// send download request
    void request_Download(const QUrl &url);
    /// Setup download url
    QUrl buildUrl(const QString &fileId,const QString &fields,const QString &key) const;
    QUrl buildUrl(const QString &fileId,const QString &alt,const QString &key,QUrlQuery args) const;
    /// Write data to mp_file;
    bool writeFile(QNetworkReply *reply);
    /// Parse Error message form reply
    QString getErrorMessage(QNetworkReply *reply);

private slots:
    /// Solt for `QNetworkReply::finished` in `request_Download()`
    void on_Download_ReplyFinished();
    /// Solt for `QNetworkReply::error` in `request_Download()`
    void on_Download_ReplyError(QNetworkReply::NetworkError);
};
}
#endif // GDRIVEFILEDOWNLOADER_H
