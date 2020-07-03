#ifndef GDRIVEABOUTRESOURCE_H
#define GDRIVEABOUTRESOURCE_H
#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QJsonDocument;
QT_END_NAMESPACE

namespace GDrive {
/*!
 * \class GDriveAboutResource
 * \brief Information about the user, the user's Drive, and system capabilities.
 *
 * - 儲存了使用者帳號以及drive的訊息
 * - `GDriveAboutResource`負責將原本的json字串轉成使用function的方式存取資料
 * - 內部使用QJsonObject保存資料，因為每次回傳的內容根據api指定的fields不同，有些內容不會回傳
 * - 編譯器有RVO的關係所以不實作拷貝&移動建構子
 * - `bool isEmpty()` 回傳本身是否空值，其於函數用來取得對應欄位的資料
 *
 * ## Reference 參考資料
 * - [About](https://developers.google.com/drive/api/v3/reference/about#resource)
 */
class GDriveAboutResource
{
public:
    /// construct by QBytearray data(deprecated)
    explicit GDriveAboutResource(const QByteArray &data);
    /// construct by QJsonDocument
    explicit GDriveAboutResource(const QJsonDocument &doc);
    /// Constructs an empty GDriveAboutResource object.
    explicit GDriveAboutResource();
    /// destructor
    ~GDriveAboutResource();

    /// Identifies what kind of resource this is. Value: the fixed string "drive#about".
    QString kind() const;
    /// The authenticated user.
    QJsonObject user() const;
    /// Identifies what kind of resource this is. Value: the fixed string "drive#user".
    QString user_kind() const;
    /// A plain text displayable name for this user.
    QString user_displayName() const;
    /// A link to the user's profile photo, if available.
    QString user_photoLink() const;
    /// Whether this user is the requesting user.
    bool user_me() const;
    /// The user's ID as visible in Permission resources.
    QString user_permissionId() const;
    /// The email address of the user.
    /// This may not be present in certain contexts if the user has not made their email address visible to the requester.
    QString user_emailAddress() const;
    /// The user's storage quota limits and usage. All fields are measured in bytes.
    QJsonObject storageQuota() const;
    /// The usage limit, if applicable. This will not be present if the user has unlimited storage.
    long storageQuota_limit() const;
    /// The total usage across all services.
    long storageQuota_usage() const;
    /// The usage by all files in Google Drive.
    long storageQuota_usageInDrive() const;
    /// The usage by trashed files in Google Drive.
    long storageQuota_usageInDriveTrash() const;
    /// A map of source MIME type to possible targets for all supported imports.
    QJsonObject importFormats() const;
    /// A map of source MIME type to possible targets for all supported exports.
    QJsonObject exportFormats() const;
    /// A map of maximum import sizes by MIME type, in bytes.
    QJsonObject maxImportSizes() const;
    /// The maximum upload size in bytes.
    long maxUploadSize() const;
    /// Whether the user has installed the requesting app.
    bool appInstalled() const;
    /// The currently supported folder colors as RGB hex strings.
    QJsonArray folderColorPalette() const;
    /// A list of themes that are supported for shared drives.
    QJsonArray driveThemes() const;
    /// The ID of the theme.
//    driveThemes[].id 	string
    /// A link to this theme's background image.
//    driveThemes[].backgroundImageLink 	string
    /// The color of this theme as an RGB hex string.
//    driveThemes[].colorRgb 	string
    /// Whether the user can create shared drives.
    bool canCreateDrives() const;
    /// Returns true if the object is empty
    bool isEmpty() const;

private:
    /// save data form constructer resource
    QJsonObject m_object = QJsonObject();
};
}

#endif // GDRIVEABOUTRESOURCE_H
