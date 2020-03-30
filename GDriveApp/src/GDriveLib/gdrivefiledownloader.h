#ifndef GDRIVEFILEDOWNLOADER_H
#define GDRIVEFILEDOWNLOADER_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"

class QFile;
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
    explicit GDriveFileDownloader(QOAuth2AuthorizationCodeFlow *parent,
                                  const QString &fileId,
                                  const QString &fields,
                                  QSharedPointer<QFile> file);
    /// destructor
    ~GDriveFileDownloader() override;
    /// return GDriveFileResource
//    GDriveFileResource getResource() const;
    /// return network reply
    QByteArray getReplyString() const;

private:
    /// pointer to Shared QFile ready to write,DELETE by owner
    QSharedPointer<QFile> mp_file;
    /// save network reply usually file matadata
    QByteArray m_replyData = QByteArray();

private:
    /// send download request
    void request_Download(const QString &fileId,const QString &fields);
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
