#ifndef GDRIVEFILESIMPLEUPDATE_H
#define GDRIVEFILESIMPLEUPDATE_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

namespace GDrive{
/*!
 * \class GDriveFileSimpleUpdate
 * \brief The GDriveFileSimpleUpdate class are provide `Files: update` method in Google Drive.
 *
 * - Updates a file's metadata and/or content.
 * - 更新檔案
 * - 與Create方法相似，但是差別在URL部份使用fileID
 * - 必須使用PATCH方法發送HTTP request，而非指南中所稱的PUT
 * - Access token 必定放在最後方
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
    explicit GDriveFileSimpleUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                    const QString &filepath,
                                    const QString &fileID);
    explicit GDriveFileSimpleUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                    const QString &filepath,
                                    const FileUpdateArgs &args);
    /// destructor
    ~GDriveFileSimpleUpdate() override;
    /// return File resource
    GDriveFileResource getResource() const;
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

private:
    /// send simpleupload request
    void request_UploadStart();
    /// return QUrl by fileID
    QUrl setupUrl(const QString &fileID);
    /// return QUrl by fileID and optional parameter
    QUrl setupUrl(const FileUpdateArgs &args);
    /// retry upload
    void retry();

private slots:
    void on_UploadStart_ReplyFinished();
    void on_UploadStart_ReplyError(QNetworkReply::NetworkError);

private:
    /// An member of upload file,use to upload on Google Drive.
    QFile *m_file = nullptr;
    /// The url for send request
    QUrl m_url;
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
};
}

#endif // GDRIVEFILESIMPLEUPDATE_H
