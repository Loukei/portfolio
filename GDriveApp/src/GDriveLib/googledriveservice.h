#ifndef TESTOAUTH_H
#define TESTOAUTH_H

#include <QObject>
//#include <QOAuth2AuthorizationCodeFlow>
#include "googleauthorizationcodeflow.h"
#include "gdriveaboutresource.h"
#include "gdrivefileresource.h"
#include "gdrivefileresourcelist.h"
#include "gdriveerrorformat.h"
#include "gdriveabout.h"
#include "gdrivefiledownloader.h"
#include "gdrivefilesimplecreate.h"
#include "gdrivefilemultipartcreate.h"
#include "gdrivefileresumablecreate.h"
#include "gdrivefilesimpleupdate.h"
#include "gdrivefilemultipartupdate.h"
#include "gdrivefileresumableupdate.h"
#include "gdrivefilesearch.h"
#include "gdrivefileget.h"

QT_BEGIN_NAMESPACE
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

    /// return the expiration time of the current access token.
    QDateTime expirationAt() const;
    /// logout account (depercated)
    void logout();
    /// return m_google token
    QString token() const;
    /// Returns the current authentication status.
    QAbstractOAuth::Status status() const;
    /// Gets the current refresh token.
    QString refreshToken() const;
    /// Sets the new refresh token refreshToken to be used.
    void setRefreshToken(const QString &refreshToken);

    /// send request to get About message, use deletelater to delete GDriveAbout
    GDrive::GDriveAbout* getAbout(GDriveAbout::AboutArgs args);
    /// simple upload create file
    GDriveFileSimpleCreate* fileSimpleCreate(const QString &filepath,
                                             const FileCreateArgs &args);
    /// multipart upload create file
    GDriveFileMultipartCreate* fileMultipartCreate(const QString &filepath,
                                                   const FileCreateArgs &args);
    /// resumable upload create file
    GDriveFileResumableCreate* fileResumableCreate(const QString &filepath,
                                                   const FileCreateArgs &args);
    /// simple upload update file
    GDriveFileSimpleUpdate* fileSimpleUpdate(const QString &filepath,
                                             const FileUpdateArgs &args);
    /// multipart upload update file
    GDriveFileMultipartUpdate* fileMultipartUpdate(const QString &filepath,
                                                   const FileUpdateArgs &args);
    /// resumable upload update file
    GDriveFileResumableUpdate* fileResumableUpdate(const QString &filepath,
                                                   const FileUpdateArgs &args);
    /// Gets a file's metadata by ID.
    GDriveFileGet* fileGet(const GDrive::FileGetArgs &args);
    /// Search Files in drive
    GDriveFileSearch* fileList(const GDrive::FileListArgs &args);
    /// Get File download by fileID
    GDriveFileDownloader* fileDownload(const QString &fileId,
                                       const QString &fields,
                                       QSharedPointer<QFile> file);
public slots:
    /// Starts the authentication flow as described in The OAuth 2.0 Authorization Framework
    void grant();
    /// Call this function to refresh the token.
    /// Access tokens are not permanent.
    /// After a time specified along with the access token when it was obtained, the access token will become invalid.
    void refreshAccessToken();

signals:
    /// This signal is emitted when the authorization flow finishes successfully.
    void granted();
    /// emit when current authentication status changed
    void statusChanged(QAbstractOAuth::Status status);
    /// emit when expiration time changed
    void expirationAtChanged(const QDateTime &expiration);
    /// Signal emitted when the server responds to the request with an error:
    /// error is the name of the error;
    /// errorDescription describes the error and uri is an optional URI containing more information about the error.
    void error(const QString &error, const QString &errorDescription, const QUrl &uri);

private:
    /// Qt Oauth2 Authorization
//    QOAuth2AuthorizationCodeFlow *m_google = nullptr;
    GoogleAuthorizationCodeFlow *m_google = nullptr;
    /// networkmanager
    QNetworkAccessManager *m_manager = nullptr;
};
}

#endif // TESTOAUTH_H
