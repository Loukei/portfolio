#ifndef GDRIVEFILERESUMABLEUPDATE_H
#define GDRIVEFILERESUMABLEUPDATE_H

#include "gdrivefiletask.h"
#include <QNetworkReply>
#include <QFile>

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
    explicit GDriveFileResumableUpdate(const QString& filepath,
                                       const QString& fileID,
                                       QOAuth2AuthorizationCodeFlow *parent);
    ///
    explicit GDriveFileResumableUpdate(const QString& filepath,
                                       const QString& fileID,
                                       const QUrlQuery &args,
                                       QOAuth2AuthorizationCodeFlow *parent);
    /// destructor
    ~GDriveFileResumableUpdate() override;
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

    static QUrlQuery buildUrlArgs(const QString &addParents,
                                  const bool enforceSingleParent,
                                  const bool keepRevisionForever,
                                  const QString &ocrLanguage,
                                  const QString &removeParents,
                                  const bool supportsAllDrives,
                                  const bool useContentAsIndexableText);

    enum UploadStatus{
        Initial = 0,                    /* Initial status, user doesn't call start() */
        RequestInitialSessionURI = 1,   /* step1: request google for Initial Session URI and send file matadata */
        RequestStartUpload = 2,         /* step2: Start upload file content with single chunk */
        RequestUploadprogress = 3,      /* step3: If upload has paused or step2 upload error,asking server for upload progress */
        RequestResumeUpload = 4,        /* step4: after setp3 receive upload progress, resume upload */
        Paused = 5,                     /* Task has been paused by user */
        Success = 6,                    /* Task has been Successfull finished */
        Error = 7                       /* Task has been unrecoverable error */
    };

    bool start();

public slots:
    void abort();
    void pause();
    void resume();

signals:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    /// save input parameters
    QUrl m_initialUrl;
    /// An member of upload file,use to upload on Google Drive.
    QFile m_file;
    /// save session uri
    QUrl m_sessionUri = QUrl();
    /// record upload progress has sent to server
    qint64 m_offset;
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
    /// Record the upload status
    UploadStatus m_status;

private:
    /// Start resumable upload session, first step to get session Uri
    QNetworkReply* requestInitialSession(const QUrl &url);
    /// After receive Session Uri, send upload request by Single request method
    QNetworkReply* requestStartUpload(const QUrl &sessionUrl);
    /// Upload interrupt, asking google server for upload status
    QNetworkReply* requestUploadProgress(const QUrl &sessionUrl);
    /// Resume upload when interrupt form offset position
    QNetworkReply* requestResumeUpload(const QUrl &sessionUrl,const qint64 offset);
    /// return QUrl by fileID
    QUrl buildUrl(const QString &fileID,const QString &uploadType) const;
    QUrl buildUrl(const QString &fileID,const QString &uploadType,QUrlQuery args) const;

    /// get upload status form reply, if match failed, return -1
    qint64 getContentRangeMax(const QNetworkReply* reply) const;
    /// restart whole upload task, this usually happen when server return 404 error.
    /// [Doc](https://developers.google.com/drive/api/v3/manage-uploads#errors)
    void restart();

    void nextStep(UploadStatus status);

    inline void taskFailed();
    inline void taskSucceeded();

private slots:
    void onInitialSession_ReplyFinished();
    void onInitialSession_ReplyError(QNetworkReply::NetworkError);

    void onStartUpload_ReplyFinished();
    void onStartUpload_ReplyError(QNetworkReply::NetworkError);

    void onUploadProgress_ReplyFinished();
    void onUploadProgress_ReplyError(QNetworkReply::NetworkError);

    void onResumeUpload_ReplyFinished();
    void onResumeUpload_ReplyError(QNetworkReply::NetworkError);
    inline void onResumeUpload_uploadProgress(qint64 bytesSent, qint64 bytesTotal);
}; // class GDriveFileResumableUpdate

} // namespace GDrive
#endif // GDRIVEFILERESUMABLEUPDATE_H
