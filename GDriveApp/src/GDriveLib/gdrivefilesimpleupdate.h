#ifndef GDRIVEFILESIMPLEUPDATE_H
#define GDRIVEFILESIMPLEUPDATE_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include <QFile>

namespace GDrive{
/*!
 * \class GDriveFileSimpleUpdate
 * \brief The GDriveFileSimpleUpdate class are provide `Files: update` method in Google Drive.
 *
 * - Updates a file's metadata and/or content.
 * - 更新檔案
 * - 與Create方法相似，但是差別在URL部份使用fileID
 * - 必須使用PATCH方法發送HTTP request，而非指南中所稱的PUT
 * - Access token 參數必定放在URL最後方
 *
 * ## Reference 參考資料
 * - [Files: update](https://developers.google.com/drive/api/v3/reference/files/update)
 * - [Perform a simple upload](https://developers.google.com/drive/api/v3/manage-uploads#simple)
 */
class GDriveFileSimpleUpdate : public GDriveFileTask
{
    Q_OBJECT
public:
    /// constructor
    explicit GDriveFileSimpleUpdate(const QString &filepath,
                                    const QString &fileID,
                                    QOAuth2AuthorizationCodeFlow *parent);
    explicit GDriveFileSimpleUpdate(const QString &filepath,
                                    const QString &fileID,
                                    const QUrlQuery &args,
                                    QOAuth2AuthorizationCodeFlow *parent);
    /// destructor
    ~GDriveFileSimpleUpdate() override;
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

    static QUrlQuery buildUrlArgs(const QString &addParents,
                                  const bool enforceSingleParent,
                                  const bool keepRevisionForever,
                                  const QString &ocrLanguage,
                                  const QString &removeParents,
                                  const bool supportsAllDrives,
                                  const bool useContentAsIndexableText);

    bool start();

public slots:
    void abort();

signals:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    /// send simpleupload request
    QNetworkReply* requestUploadStart(const QNetworkRequest &request);
    QNetworkRequest buildRequest(const QUrl &url) const;
    /// return QUrl by fileID
    QUrl buildUrl(const QString &fileID,const QString &uploadType,const QString &access_token) const;
    QUrl buildUrl(const QString &fileID,const QString &uploadType,const QString &access_token,QUrlQuery args) const;
    /// retry upload
    void retry();

private slots:
    void onUploadStartReplyFinished();
    void onUploadStartReplyError(QNetworkReply::NetworkError);

private:
    /// An member of upload file,use to upload on Google Drive.
    QFile m_file;
    /// The url for send request
    QUrl m_url;
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
};
}

#endif // GDRIVEFILESIMPLEUPDATE_H
