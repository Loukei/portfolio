#ifndef TESTOAUTH_H
#define TESTOAUTH_H

#include <QObject>
#include "gdriveaboutresource.h"
#include "gdrivefileresource.h"
#include "gdrivefileresourcelist.h"
#include "gdriveerrorformat.h"
#include "gdriveabout.h"
#include "gdrivefiledownloader.h"
#include "gdrivefilesimplecreate.h"
#include "gdrivefilemultipartcreate.h"
#include "gdrivefileresumablecreate.h"
#include "gdrivefilesearch.h"
#include "gdrivefileget.h"

QT_BEGIN_NAMESPACE
class QOAuth2AuthorizationCodeFlow;
class QNetworkAccessManager;
class QFile;
QT_END_NAMESPACE

/*!
 * \namespace GDrive
 * \brief The GDrive namespace provide all the Google Drive Api library
 */
namespace GDrive {
/*!
 * \class GDriveService
 * \brief The main class to provide Google Drive serviece
 *
 * - 提供Google Drive Api的服務
 * - Google Drive Api採用資源導向操作，每個method都是對資源的操作
 * - 除了登入帳號外，其他method都使用factory pattern操作。
 * - 每執行一項操作都會回傳一個以`m_google`為parent的物件，我稱之為Task。
 * Task在建立時就會立刻執行，以建構子的參數處理對應的請求，並在完成時發出信號通知
 *
 * ## Note
 * QNetworkreply其中readyRead()信號繼承自QIODevice類，每當有新的數據可以讀取時，都會發射該信號；
 * 每當網絡請求的下載進度更新時都會發射downloadProgress()信號，它用來更新進度條；
 * 每當應答處理結束時，都會發射finished()信號，
 * 該信號與前面程序中QNetworkAccessManager類的finished()信號作用相同，只不過是發送者不同，參數也不同而已。
 *
 * ## Reference
 * - [Introduction to Google Drive API](https://developers.google.com/drive/api/v3/about-sdk)
 */
class GDriveService : public QObject
{
    Q_OBJECT
public:
    /// constructor of serviece
    explicit GDriveService(QObject *parent = nullptr);
    /// destructor
    virtual ~GDriveService() final;
    /// start Authorization
    void start();
    /// logout account (depercated)
    void logout();
    /// show OAuth 2.0 token
    QString receivedToken() const;
    /// send request to get About message, use deletelater to delete GDriveAbout
    GDrive::GDriveAbout* getAbout(GDriveAbout::AboutArgs args);
    /// simple upload create file
    GDriveFileSimpleCreate* fileSimpleCreate(const QString &filepath);
    /// multipart upload create file
    GDriveFileMultipartCreate* fileMultipartCreate(const QString &filepath);
    /// Resumable upload create file
    GDriveFileResumableCreate* fileResumableCreate(const QString &filepath);
    /// Gets a file's metadata by ID.
    GDriveFileGet* fileGet(const QString &fileId,const QString &fields);
    /// Search Files in drive
    GDriveFileSearch* fileList(const QString &q,
                               const QString &pageToken);
    /// Get File download by fileID
    GDriveFileDownloader* fileDownload(const QString &fileId,
                                 const QString &fields,
                                 QSharedPointer<QFile> file);

signals:
    /// This signal is emitted when the authorization flow finishes successfully.
    void granted();

private:
    /// Qt Oauth2 Authorization
    QOAuth2AuthorizationCodeFlow *m_google = nullptr;
    /// networkmanager
    QNetworkAccessManager *m_manager = nullptr;
};
}

#endif // TESTOAUTH_H
