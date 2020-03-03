#ifndef TESTOAUTH_H
#define TESTOAUTH_H

#include <QObject>
#include "gdriveabout.h"
#include "gdriveaboutresource.h"
#include "gdrivefileresource.h"
#include "gdrivefiledownloader.h"
#include "gdrivefilesimplecreate.h"
#include "gdrivefilemultipartcreate.h"
#include "gdrivefileresumablecreate.h"

class QOAuth2AuthorizationCodeFlow;
class QOAuthHttpServerReplyHandler;
class QNetworkAccessManager;
class QFile;

namespace GDrive {
//class GDriveUploader;
class GDriveService : public QObject
{
    Q_OBJECT
public:
    explicit GDriveService(QObject *parent = nullptr);
    virtual ~GDriveService() final;

//    enum UploadType{
//        SimpleUpload,
//        MultipartUpload,
//        ResumableUpload
//    };

    /// start Authorization
    void start();
    /// logout
    void logout();
    /// show Object Info
    QString showInfo() const;
    /// send request to get About message, use deletelater to delete GDriveAbout
    GDrive::GDriveAbout* getAbout(GDriveAbout::AboutArgs args);
    /// create new file on drive
//    GDriveUploader* fileCreate(const QString &filepath,UploadType type);
    GDriveFileSimpleCreate* fileSimpleCreate(const QString &filepath);
    GDriveFileMultipartCreate* fileMultipartCreate(const QString &filepath);
    GDriveFileResumableCreate* fileResumableCreate(const QString &filepath);
    /// Gets a file's metadata by ID.
//    GDriveFileTask* fileGet(const QString &fileId,const QString &fields);
    /// Get File download by fileID
    GDriveFileDownloader* fileDownload(const QString &fileId,
                                 const QString &fields,
                                 QSharedPointer<QFile> file);
signals:
    void granted();

private:
    /// Qt Oauth2 Authorization
    QOAuth2AuthorizationCodeFlow *m_google;
    /// networkmanager
    QNetworkAccessManager *m_manager;
};
}

#endif // TESTOAUTH_H

/* Note:
 * QNetworkreply其中readyRead()信號繼承自QIODevice類，每當有新的數據可以讀取時，都會發射該信號；
 * 每當網絡請求的下載進度更新時都會發射downloadProgress()信號，它用來更新進度條；
 * 每當應答處理結束時，都會發射finished()信號，
 * 該信號與前面程序中QNetworkAccessManager類的finished()信號作用相同，只不過是發送者不同，參數也不同而已。
*/
