#ifndef GDRIVEFILEMULTIPARTUPDATE_H
#define GDRIVEFILEMULTIPARTUPDATE_H

#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

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
    explicit GDriveFileMultipartUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                       const QString &filepath,
                                       const QString &fileID);
    /// constructor with optional parameter
    explicit GDriveFileMultipartUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                       const QString &filepath,
                                       const QString &fileID,
                                       const QString &addParents,
                                       bool enforceSingleParent,
                                       bool keepRevisionForever,
                                       const QString &ocrLanguage,
                                       const QString &removeParents,
                                       bool useContentAsIndexableText);
    explicit GDriveFileMultipartUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                       const QString &filepath,
                                       const FileUpdateArgs &args);
    /// destructor
    ~GDriveFileMultipartUpdate() override;
    /// return GDriveFileResource as upload response
    GDriveFileResource getResource() const;
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

private:
    /// send Multipart upload request
    void request_UploadStart();
    /// return QUrl by fileID
    QUrl setupUrl(const QString &fileID,
                  const QString &addParents = "",
                  bool enforceSingleParent = false,
                  bool keepRevisionForever = false,
                  const QString &ocrLanguage = "",
                  const QString &removeParents = "",
                  bool useContentAsIndexableText = false);
    /// return QUrl by fileID and optional parameter
    QUrl setupUrl(const FileUpdateArgs &args);
    /// retry upload
    void retry();

private slots:
    /// Solt for `QNetworkReply::finished` in `request_UploadStart()`
    void on_UploadStart_ReplyFinished();
    /// Solt for `QNetworkReply::error` in `request_UploadStart()`
    void on_UploadStart_ReplyError(QNetworkReply::NetworkError);

private:
    /// An member of upload file,use to upload on Google Drive.
    QFile *m_file = nullptr;
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
    /// url for network request
    QUrl m_url;
};
}

#endif // GDRIVEFILEMULTIPARTUPDATE_H
