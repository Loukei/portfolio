#ifndef GDRIVEFILESIMPLECREATE_H
#define GDRIVEFILESIMPLECREATE_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"

class QFile;
namespace GDrive {
class GDriveFileSimpleCreate : public GDriveFileTask
{
    Q_OBJECT
public:
    explicit GDriveFileSimpleCreate(QOAuth2AuthorizationCodeFlow *parent,
                                    const QString &filepath);
    ~GDriveFileSimpleCreate() override;
    GDriveFileResource getResource() const;
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
    QFile *m_file;
    /// Save network reply after upload finished
    QByteArray m_replyData;
};
/*
 * @class GDriveFileSimpleCreate gdrivefilesimplecreate.h "gdrivefilesimplecreate.h"
 * @brief Simple upload class with Files:create method
 *
 * Detail:
 * GDriveFileSimpleCreate is a functional class using for upload new file to users Drive
 * (Simple upload).
 *
 * GDriveFileSimpleCreate is created by GDriveService, when upload finished,signal finished()
 * emit,use isFailed() method to describe upload failed or success.
 *
 * To Create GDriveFileSimpleCreate instance:
 * use param QOAuth2AuthorizationCodeFlow* as parent,also parent is using to send upload request.
 * use param QString &filepath to identify existing file form users disk
 * use param GDriveFileResource* as return google drive File Resource
 *
 * Reference:
 * 1.https://developers.google.com/drive/api/v3/reference/files/create
 * 2.https://developers.google.com/drive/api/v3/manage-uploads#simple
*/
}
#endif // GDRIVEFILESIMPLECREATE_H
