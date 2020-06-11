#ifndef GDRIVEFILEMULTIPARTCREATE_H
#define GDRIVEFILEMULTIPARTCREATE_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"


QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

namespace GDrive {
/*!
 * \class GDriveFileMultipartCreate
 * \brief Creates a new file with Multipart upload.
 *
 * - 使用Martipart方法上傳檔案
 * - 此方法建議在5mb以下檔案使用，與Simple upload不同的是可以自訂metadata
 * - 父類為`GDriveFileTask`，為通用的File類操作
 * - 使用`GDriveFileTask::finished`信號表示上傳完成(或失敗)
 * - 使用`GDriveFileMultipartCreate::getResource()`或`GDriveFileMultipartCreate::getResource()`
 * 接收上傳完成後的network reply資料
 *
 * ## Reference
 * - [Files: create](https://developers.google.com/drive/api/v3/reference/files/create)
 * - [Perform a multipart upload](https://developers.google.com/drive/api/v3/manage-uploads#multipart)
 */
class GDriveFileMultipartCreate : public GDriveFileTask
{
public:
    /// constructor
    explicit GDriveFileMultipartCreate(const QString &filepath,
                                       QOAuth2AuthorizationCodeFlow *parent);
    explicit GDriveFileMultipartCreate(const QString &filepath,
                                       const QUrlQuery &args,
                                       QOAuth2AuthorizationCodeFlow *parent);
    /// destructor
    ~GDriveFileMultipartCreate() override;
    /// return GDriveFileResource as upload response
    GDriveFileResource getResource() const;
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

    static QUrlQuery buildUrlArgs(const bool enforceSingleParent = false,
                                  const bool ignoreDefaultVisibility = false,
                                  const bool keepRevisionForever = false,
                                  const QString &ocrLanguage = QString(),
                                  const bool supportsAllDrives = false,
                                  const bool useContentAsIndexableText = false);

private:
    /// send Multipart upload request
    void request_UploadStart();
    QUrl buildUrl(const QString &uploadType/*,const QString &access_token*/) const;
    QUrl buildUrl(const QString &uploadType/*,const QString &access_token*/,QUrlQuery args) const;
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
    ///
    QUrl m_url;
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
};
}
#endif // GDRIVEFILEMULTIPARTCREATE_H
