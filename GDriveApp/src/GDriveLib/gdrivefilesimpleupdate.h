#ifndef GDRIVEFILESIMPLEUPDATE_H
#define GDRIVEFILESIMPLEUPDATE_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>
#include "gdrivefileresource.h"

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

namespace GDrive{
/*!
 * \class GDriveFileSimpleUpdate
 * \brief The GDriveFileSimpleUpdate class
 */
class GDriveFileSimpleUpdate : public GDriveFileTask
{
    Q_OBJECT
public:
    /// constructor
    explicit GDriveFileSimpleUpdate(QOAuth2AuthorizationCodeFlow *parent,
                                    const QString &filepath,
                                    const QString &fileID);
    /// destructor
    ~GDriveFileSimpleUpdate() override;
    /// return File resource
    GDriveFileResource getResource() const;
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

private:
    /// send simpleupload request
    void request_UploadStart(const QString &fileID);
    /// retry upload
//    void retry();

private slots:
    void on_UploadStart_ReplyFinished();
    void on_UploadStart_ReplyError(QNetworkReply::NetworkError);

private:
    /// An member of upload file,use to upload on Google Drive.
    QFile *m_file = nullptr;
//    /// save file ID form constructor
//    QString m_fileID = QString();
    /// Save network reply after upload finished
    QByteArray m_replyData = QByteArray();
};
}

#endif // GDRIVEFILESIMPLEUPDATE_H
