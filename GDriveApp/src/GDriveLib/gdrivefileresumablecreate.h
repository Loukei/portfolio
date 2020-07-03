#ifndef GDRIVEFILERESUMABLECREATE_H
#define GDRIVEFILERESUMABLECREATE_H
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include <QFile>

namespace GDrive {
/*!
 * \class GDriveFileResumableCreate
 * \brief Creates a new file with Resumable upload.
 * \details
 *
 * ## Reference
 * - [Files: create](https://developers.google.com/drive/api/v3/reference/files/create)
 * - [Perform a resumable upload](https://developers.google.com/drive/api/v3/manage-uploads#resumable)
 */
class GDriveFileResumableCreate : public GDriveFileTask
{
    Q_OBJECT
public:
    explicit GDriveFileResumableCreate(const QString &filepath,
                                       QOAuth2AuthorizationCodeFlow *parent);
    explicit GDriveFileResumableCreate(const QString &filepath,
                                       const QUrlQuery &args,
                                       QOAuth2AuthorizationCodeFlow *parent);
    ~GDriveFileResumableCreate() override;
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

    static QUrlQuery buildUrlArgs(const bool enforceSingleParent = false,
                                  const bool ignoreDefaultVisibility = false,
                                  const bool keepRevisionForever = false,
                                  const QString &ocrLanguage = QString(),
                                  const bool supportsAllDrives = false,
                                  const bool useContentAsIndexableText = false);

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
    QUrl buildUrl(const QString &uploadType) const;
    QUrl buildUrl(const QString &uploadType,QUrlQuery args) const;

    QNetworkReply* requaetInitialSession(const QUrl &url);
    QNetworkReply* requestStartUpload(const QUrl &sessionUrl);
    QNetworkReply* requestUploadProgress(const QUrl &sessionUrl);
    /// offset means the value form reply header "Range: bytes=0-XX"
    QNetworkReply* requestResumeUpload(const QUrl &sessionUrl,const qint64 offset);

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
    void onInitialSession_ReplyError(QNetworkReply::NetworkError code);

    void onStartUpload_ReplyFinished();
    void onStartUpload_ReplyError(QNetworkReply::NetworkError code);

    void onUploadProgress_ReplyFinished();
    void onUploadProgress_ReplyError(QNetworkReply::NetworkError code);

    void onResumeUpload_ReplyFinished();
    void onResumeUpload_ReplyError(QNetworkReply::NetworkError code);
    inline void onResumeUpload_uploadProgress(qint64 bytesSent, qint64 bytesTotal);
}; // GDriveFileResumableCreate
}
#endif // GDRIVEFILERESUMABLECREATE_H
