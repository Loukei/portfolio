#ifndef GDRIVEFILEMULTIPARTUPDATE_H
#define GDRIVEFILEMULTIPARTUPDATE_H

#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include <QFile>

namespace GDrive {
/*!
 * \class GDriveFileMultipartUpdate
 * \brief The GDriveFileMultipartUpdate class are provide `Files: update` method in Google Drive.
 *
 * - Updates a file's metadata and/or content.
 * - 更新檔案
 * - 與Create方法相似，但是差別在URL部份使用fileID
 * - 必須使用PATCH方法發送HTTP request，而非指南中所稱的PUT
 * - Access token 必定放在最後方
 *
 * ## Reference 參考資料
 * - [Files: update](https://developers.google.com/drive/api/v3/reference/files/update)
 * - [Send a multipart upload request](https://developers.google.com/drive/api/v3/manage-uploads#multipart)
 */
class GDriveFileMultipartUpdate : public GDriveFileTask
{
    Q_OBJECT
public:
    /// constructor
    explicit GDriveFileMultipartUpdate(const QString &filepath,
                                       const QString &fileID,
                                       QOAuth2AuthorizationCodeFlow *parent);
    explicit GDriveFileMultipartUpdate(const QString &filepath,
                                       const QString &fileID,
                                       const QUrlQuery &args,
                                       QOAuth2AuthorizationCodeFlow *parent);
    /// destructor
    ~GDriveFileMultipartUpdate() override;
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
    /// send Multipart upload request
    QNetworkReply *requestUploadStart(const QNetworkRequest &request, QHttpMultiPart *multiPart);
    QNetworkRequest buildRequest(const QUrl &url) const;
    QHttpMultiPart* buildRequestBody(QFile *file);
    /// return QUrl by fileID
    QUrl buildUrl(const QString &fileID,const QString &uploadType/*,const QString &access_token*/) const;
    QUrl buildUrl(const QString &fileID,const QString &uploadType,QUrlQuery args) const;
    /// retry upload
    void retry();

private slots:
    /// Solt for `QNetworkReply::finished` in `request_UploadStart()`
    void onUploadStartReplyFinished();
    /// Solt for `QNetworkReply::error` in `request_UploadStart()`
    void onUploadStartReplyError(QNetworkReply::NetworkError);

private:
    /// An member of upload file,use to upload on Google Drive.
    QFile m_file;
    /// the request body part includeing upload file
    QHttpMultiPart *m_httpMultiPart;
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
    /// url for network request
    QUrl m_url;
};
}

#endif // GDRIVEFILEMULTIPARTUPDATE_H
