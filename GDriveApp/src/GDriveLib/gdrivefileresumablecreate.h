#ifndef GDRIVEFILERESUMABLECREATE_H
#define GDRIVEFILERESUMABLECREATE_H
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"

class QOAuth2AuthorizationCodeFlow;
class QFile;
namespace GDrive {
/*!
 * \class GDriveFileResumableCreate
 * \brief Creates a new file with Resumable upload.
 *
 * ## Reference
 * [Files: create]:https://developers.google.com/drive/api/v3/reference/files/create
 * [Perform a resumable upload]:https://developers.google.com/drive/api/v3/manage-uploads#resumable
 */
class GDriveFileResumableCreate : public GDriveFileTask
{
    Q_OBJECT
public:
    explicit GDriveFileResumableCreate(QOAuth2AuthorizationCodeFlow *parent,
                                       const QString& filepath);
    ~GDriveFileResumableCreate() override;
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

    enum State{
        NONE = 0,
        RESTARTUPLOAD = 5,
        FAIL = 6,
        COMPLETE = 7
    };
//    void setState(State s);
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
    /// An member of upload file,use to upload on Google Drive.
    QFile *m_file = nullptr;
    /// save session uri
    QUrl m_sessionUri = QUrl();
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();

    void state_RestartUpload();
    void state_ResumeUpload();
    void state_FailedUpload();
    void state_CompleteUpload();
};
}
#endif // GDRIVEFILERESUMABLECREATE_H
