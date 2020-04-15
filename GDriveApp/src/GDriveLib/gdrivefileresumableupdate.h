#ifndef GDRIVEFILERESUMABLEUPDATE_H
#define GDRIVEFILERESUMABLEUPDATE_H

#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"

QT_BEGIN_NAMESPACE
class QOAuth2AuthorizationCodeFlow;
class QFile;
QT_END_NAMESPACE

namespace GDrive {
/*!
 * \class GDriveFileResumableUpdate
 * \brief The GDriveFileResumableUpdate class are provide `Files: update` method in Google Drive.
 *
 * - Updates a file's metadata and/or content.
 * - 更新檔案
 * - 與Create方法相似，但是差別在URL部份使用fileID
 * - 必須使用PATCH方法發送HTTP request，而非指南中所稱的PUT
 * - Access token 必定放在最後方
 *
 * ## Reference 參考資料
 * - [Files: update](https://developers.google.com/drive/api/v3/reference/files/update)
 * - [Perform a resumable upload](https://developers.google.com/drive/api/v3/manage-uploads#resumable)
 */
class GDriveFileResumableUpdate : public GDriveFileTask
{
    Q_OBJECT
public:
    /// constructor
    explicit GDriveFileResumableUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                       const QString& filepath,
                                       const QString& fileID);
    ///
    explicit GDriveFileResumableUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                       const QString& filepath,
                                       const FileUpdateArgs &args);
    /// destructor
    ~GDriveFileResumableUpdate() override;
    /// return File resource
    GDriveFileResource getResource() const;
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

private:
    /// Start resumable upload session, first step to get session Uri
    void request_InitialSession();
    /// After receive Session Uri, send upload request by Single request method
    void request_UploadStart();
    /// Upload interrupt, asking google server for upload status
    void request_AskUploadStatus();
    /// Resume upload when interrupt form offset position
    void request_UploadResume(const qint64 offset);
    /// return QUrl by fileID
    QUrl setupUrl(const QString &fileID);
    /// return QUrl by args
    QUrl setupUrl(const FileUpdateArgs &args);

private slots:
    /// process request_InitialSession reply finished
    void on_InitialSession_ReplyFinished();
    /// process request_InitialSession reply error
    void on_InitialSession_ReplyError(QNetworkReply::NetworkError);
    /// process request_UploadStart reply finished
    void on_UploadStart_ReplyFinished();
    /// process request_UploadStart reply error
    void on_UploadStart_ReplyError(QNetworkReply::NetworkError);
    /// process request_AskUploadStatus reply finished
    void on_AskUploadStatus_ReplyFinished();
    /// process request_AskUploadStatus reply error
    void on_AskUploadStatus_ReplyError(QNetworkReply::NetworkError);
    /// process request_UploadResume reply finished
    void on_UploadResume_ReplyFinished();
    /// process request_UploadResume reply error
    void on_UploadResume_ReplyError(QNetworkReply::NetworkError);

private:
    /// State to Restart Upload
    void state_RestartUpload();
    /// State to Resume Upload
    void state_ResumeUpload();
    /// State to emit finished() signal and return Fail upload
    void state_FailedUpload();
    /// State to emit finished() signal and return Success upload
    void state_CompleteUpload();

private:
    /// An member of upload file,use to upload on Google Drive.
    QFile *m_file = nullptr;
    /// Url for initial request
    QUrl m_url;
    /// save session uri
    QUrl m_sessionUri = QUrl();
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
};
}

#endif // GDRIVEFILERESUMABLEUPDATE_H
