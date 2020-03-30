#ifndef GDRIVEFILESIMPLECREATE_H
#define GDRIVEFILESIMPLECREATE_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"

class QFile;
namespace GDrive {
/*!
 * \class GDriveFileSimpleCreate
 * \brief Creates a new file with Simple upload.
 *
 * - `GDriveFileSimpleCreate` 是實現google Drive api的`Files: create`方法中的simple upload
 * - `GDriveFileSimpleCreate` 繼承自`GDriveFileTask`
 * - `GDriveFileSimpleCreate`，是一個功能性的類別:藉由建構式的參數呼叫`QOAuth2AuthorizationCodeFlow`
 * 完成上傳檔案的任務，在內部處理好網路請求與解析回傳內容
 * - 不要在任務完成前刪除作為parent的`QOAuth2AuthorizationCodeFlow`物件，否則會導致失敗
 * - 完成任務後，會發出`GDriveFileTask::finished()`信號
 * - 使用`GDriveFileSimpleCreate::getResource()`或`GDriveFileSimpleCreate::getReplyString()`
 * 取出回傳的內容
 * - 在連接`GDriveFileTask::finished()`的槽函式中使用`QObject::deleteLater()`刪除物件
 *
 * ## Reference
 * - [Files: create](https://developers.google.com/drive/api/v3/reference/files/create)
 * - [Perform a simple upload](https://developers.google.com/drive/api/v3/manage-uploads#simple)
 */
class GDriveFileSimpleCreate : public GDriveFileTask
{
    Q_OBJECT
public:
    /// constructor
    explicit GDriveFileSimpleCreate(QOAuth2AuthorizationCodeFlow *parent,
                                    const QString &filepath);
    /// destructor
    ~GDriveFileSimpleCreate() override;
    /// return File resource
    GDriveFileResource getResource() const;
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

private:
    /// send simpleupload request
    void request_UploadStart();
    /// retry upload
    void retry();

private slots:
    void on_UploadStart_ReplyFinished();
    void on_UploadStart_ReplyError(QNetworkReply::NetworkError);

private:
    /// An member of upload file,use to upload on Google Drive.
    QFile *m_file = nullptr;
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
};
}
#endif // GDRIVEFILESIMPLECREATE_H
