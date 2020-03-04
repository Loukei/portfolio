#ifndef GDRIVEUPLOADER_H
#define GDRIVEUPLOADER_H

#include <QObject>
#include <QNetworkReply> //for network slot
#include <QFile>
#include <functional>

class QOAuth2AuthorizationCodeFlow;
class QFile;

namespace GDrive {
/// A Base class to handle new file upload.
/// use deleteLater after emit received(true)
class GDriveUploader : public QObject
{
    Q_OBJECT
public:
    explicit GDriveUploader(QOAuth2AuthorizationCodeFlow *parent);
    virtual ~GDriveUploader();
    /// return human readable string after receive(false) emit
    virtual QString errorString() = 0;
signals:
    /// receive data
    void received(bool success);
protected:
    /// calculate exponential backoff sleep time
    int getSleepTime(int collisions);
};

class GDriveSimpleUpload : public GDriveUploader
{
    Q_OBJECT
public:
    explicit GDriveSimpleUpload(QOAuth2AuthorizationCodeFlow *parent,const QString &filepath);
    ~GDriveSimpleUpload() final;
    /// return human readable string after receive(false) emit
    QString errorString() override;
private:
    /// retry upload by using exponential backoff startegy
    void retry();
    /// send simpleupload request
    void request_UploadStart();
private slots:
    void on_UploadStart_ReplyFinished();
    void on_UploadStart_ReplyError(QNetworkReply::NetworkError);
private:
    /// An instance to manage Oauth request, and Token resource, no need to delete
    const QOAuth2AuthorizationCodeFlow* m_google;
    /// Upload target file
    QFile *m_file;
    /// error string when receive(false) emit
    QString m_errStr;
};

class GDriveMultipartUpload : public GDriveUploader
{
    Q_OBJECT
public:
    explicit GDriveMultipartUpload(QOAuth2AuthorizationCodeFlow *parent,const QString &filepath);
    ~GDriveMultipartUpload() final;
    /// return human readable string after receive(false) emit
    QString errorString() override;
private:
    /// retry upload
    void retry();
    /// send request
    void request_UploadStart();
private slots:
    void on_UploadStart_ReplyFinished();
    void on_UploadStart_ReplyError(QNetworkReply::NetworkError);
private:
    /// An instance to manage Oauth request, and Token resource, no need to delete
    const QOAuth2AuthorizationCodeFlow* m_google;
    ///
    QFile *m_file;
    ///
    QString m_errStr;
};

class GDriveResumableUpload : public GDriveUploader
{
    Q_OBJECT
public:
    explicit GDriveResumableUpload(QOAuth2AuthorizationCodeFlow *parent,const QString &filepath);
    ~GDriveResumableUpload() final;
    /// return human readable string after receive(false) emit
    QString errorString() override;
private:
    enum Status{
        InitialSession = 0, //Start or Restart
        ResumeUpload = 1,   //Resume upload
        UploadFinished = 2, //Upload success
        UploadFailed = 3    //Upload failed
    };
    ///
    void setStatus(GDriveResumableUpload::Status status);

    /// Start resumable upload session, first step to get session Uri
    void request_InitialSession();
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
    /// An instance to manage Oauth request, and Token resource, no need to delete
    const QOAuth2AuthorizationCodeFlow* m_google;
    /// Upload file path
    QFile m_file;
    ///
    QString m_errStr;
    /// save session uri
    QUrl m_sessionUri;
    /// Upload status
    Status m_status;
    /// Max retry upload count
    const int m_maxRetry;
};
/*! Resumable upload flow
 *
*/
}

#endif // GDRIVEUPLOADER_H
