#ifndef GDRIVEFILERESUMABLECREATE_H
#define GDRIVEFILERESUMABLECREATE_H
#include "gdrivefiletask.h"
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
class QOAuth2AuthorizationCodeFlow;
class QFile;
QT_END_NAMESPACE

namespace GDrive {
/*!
 * \class GDriveFileResumableCreate
 * \brief Creates a new file with Resumable upload.
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

private:
    /// Start resumable upload session, first step to get session Uri
    void request_InitialSession();
    /// return QUrl for Initial request
    QUrl buildUrl(const QString &uploadType/*,const QString &access_token*/) const;
    QUrl buildUrl(const QString &uploadType/*,const QString &access_token*/,QUrlQuery args) const;
    /// After receive Session Uri, send upload request by Single request method
    void request_UploadStart();
    /// Upload interrupt, asking google server for upload status
    void request_AskUploadStatus();
    /// Resume upload when interrupt form offset position
    void request_UploadResume(const qint64 offset);

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

private:
    /// An member of upload file,use to upload on Google Drive.
    QFile *m_file = nullptr;
    /// The Url for request initial session
    QUrl m_iniUrl;
    /// save session uri
    QUrl m_sessionUri = QUrl();
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
};
}
#endif // GDRIVEFILERESUMABLECREATE_H
