#ifndef GDRIVEFILEMULTIPARTCREATE_H
#define GDRIVEFILEMULTIPARTCREATE_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"

class QFile;
namespace GDrive {
class GDriveFileMultipartCreate : public GDriveFileTask
{
public:
    explicit GDriveFileMultipartCreate(QOAuth2AuthorizationCodeFlow *parent,
                                       const QString &filepath);
    ~GDriveFileMultipartCreate() override;
    GDriveFileResource getResource() const;
private:
    /// send Multipart upload request
    void request_UploadStart();
    /// retry upload
    void retry();
private slots:
    void on_UploadStart_ReplyFinished();
    void on_UploadStart_ReplyError(QNetworkReply::NetworkError);
private:
    /// An member of upload file,use to upload on Google Drive.
    QFile *m_file;
    /// Save network reply after upload finished
    QByteArray m_replyData;
};
}
#endif // GDRIVEFILEMULTIPARTCREATE_H
