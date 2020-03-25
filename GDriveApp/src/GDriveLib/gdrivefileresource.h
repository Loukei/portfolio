#ifndef GDRIVEFILERESOURCE_H
#define GDRIVEFILERESOURCE_H
//#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

class QJsonDocument;
namespace GDrive {
/*!
 * \class GDriveFileResource
 * \brief The metadata for a file.
 * \reentrant
 * - `File`儲存檔案的元資料
 * - `GDriveFileResource`負責把原本由QNetworkReply的json字串轉成使用function的方式存取資料
 * - 內部使用QJsonObject保存資料，因為每次回傳的內容根據api指定的fields不同，有些內容不會回傳
 * - 編譯器有RVO的關係所以不實作拷貝&移動建構子
 *
 * ## Reference 參考資料
 * [Files]:https://developers.google.com/drive/api/v3/reference/files#resource
 */
class GDriveFileResource
{
public:
    /// construct by QJsonValue(for jsonArray)
    explicit GDriveFileResource(const QJsonValue &value);
    /// construct by QJsonDocument
    explicit GDriveFileResource(const QJsonDocument &doc);
    /// construct empty FileResource, for file::list return
    explicit GDriveFileResource();
    /// destructor
    ~GDriveFileResource();

    /// Identifies what kind of resource this is. Value: the fixed string "drive#file".
    QString kind() const;
    /// The ID of the file.
    QString id() const;
    /// The name of the file.
    /// This is not necessarily unique within a folder.
    /// Note that for immutable items such as the top level folders of shared drives, My Drive root folder, and Application Data folder the name is constant.
    QString name() const;
    /// The MIME type of the file.
    /// Google Drive will attempt to automatically detect an appropriate value from uploaded content if no value is provided.
    /// The value cannot be changed unless a new revision is uploaded.
    /// If a file is created with a Google Doc MIME type, the uploaded content will be imported if possible.
    /// The supported import formats are published in the About resource.
    QString mimeType() const;
    /// A short description of the file.
    QString description() const;
    /// Whether the user has starred the file.
//    bool starred() const;
    /// Whether the file has been trashed, either explicitly or from a trashed parent folder.
    /// Only the owner may trash a file, and other users cannot see files in the owner's trash.
//    bool trashed() const;
    /// Whether the file has been explicitly trashed, as opposed to recursively trashed from a parent folder.
//    bool explicitlyTrashed() const;
    /// The IDs of the parent folders which contain the file.
    /// If not specified as part of a create request, the file will be placed directly in the user's My Drive folder.
    /// If not specified as part of a copy request, the file will inherit any discoverable parents of the source file.
    /// Update requests must use the addParents and removeParents parameters to modify the parents list.
//    QJsonArray parents() const;
    /// A collection of arbitrary key-value pairs which are visible to all apps.
    /// Entries with null values are cleared in update and copy requests.
//    QJsonObject properties() const;
    /// A collection of arbitrary key-value pairs which are private to the requesting app.
    /// Entries with null values are cleared in update and copy requests.
//    QJsonObject appProperties() const;
    /// The list of spaces which contain the file.
    /// The currently supported values are 'drive', 'appDataFolder' and 'photos'.
//    QJsonArray spaces() const;
    /// A monotonically increasing version number for the file.
    /// This reflects every change made to the file on the server, even those not visible to the user.
    long version() const;
    /// A link for downloading the content of the file in a browser.
    /// This is only available for files with binary content in Google Drive.
//    QString webContentLink() const;
    /// A link for opening the file in a relevant Google editor or viewer in a browser.
//    QString webViewLink() const;
    /// A static, unauthenticated link to the file's icon.
//    QString iconLink() const;
    /// A short-lived link to the file's thumbnail, if available.
    /// Typically lasts on the order of hours.
    /// Only populated when the requesting app can access the file's content.
//    QString thumbnailLink() const;
    /// Whether the file has been viewed by this user.
//    bool viewedByMe() const;
    /// The last time the file was viewed by the user (RFC 3339 date-time).
//    QDateTime viewedByMeTime() const;
    /// The time at which the file was created (RFC 3339 date-time).
//    QDateTime createdTime() const;
    /// The last time the file was modified by anyone (RFC 3339 date-time).
    /// Note that setting modifiedTime will also update modifiedByMeTime for the user.
//    QDateTime modifiedTime() const;
    /// The last time the file was modified by the user (RFC 3339 date-time).
//    QDateTime modifiedByMeTime() const;
    /// The time at which the file was shared with the user, if applicable (RFC 3339 date-time).
//    QDateTime sharedWithMeTime() const;
    /// The user who shared the file with the requesting user, if applicable.
//    QJsonObject sharingUser() const;
    /// Identifies what kind of resource this is. Value: the fixed string "drive#user".
//    QString sharingUser_kind() const;
    /// A plain text displayable name for this user.
//    QString sharingUser_displayName() const;
    /// A link to the user's profile photo, if available.
//    QString sharingUser_photoLink() const;
    /// Whether this user is the requesting user.
//    bool sharingUser_me() const;
    /// The user's ID as visible in Permission resources.
//    QString sharingUser_permissionId() const;
    /// The email address of the user.
    /// This may not be present in certain contexts if the user has not made their email address visible to the requester.
//    QString sharingUser_emailAddress() const;
    /// The owners of the file. Currently, only certain legacy files may have more than one owner.
    /// Not populated for items in shared drives.
//    QJsonArray owners() const;
    /// The last user to modify the file.
//    QJsonObject lastModifyingUser() const;
    /// Identifies what kind of resource this is. Value: the fixed string "drive#user".
//    QString lastModifyingUser_kind() const;
    /// A plain text displayable name for this user.
//    QString lastModifyingUser_displayName() const;
    /// A link to the user's profile photo, if available.
//    QString lastModifyingUser_photoLink() const;
    /// Whether this user is the requesting user.
//    bool lastModifyingUser_me() const;
    /// The user's ID as visible in Permission resources.
//    QString lastModifyingUser_permissionId() const;
    /// The email address of the user.
    /// This may not be present in certain contexts if the user has not made their email address visible to the requester.
//    QString lastModifyingUser_emailAddress() const;
    /// Whether the file has been shared. Not populated for items in shared drives.
//    bool shared() const;
    /// Whether the user owns the file. Not populated for items in shared drives.
//    bool ownedByMe() const;
    /// Whether users with only writer permission can modify the file's permissions.
    /// Not populated for items in shared drives.
//    bool writersCanShare() const;
    /// The full list of permissions for the file.
    /// This is only available if the requesting user can share the file.
    /// Not populated for items in shared drives.
//    QJsonArray permissions() const;
    /// The color for a folder as an RGB hex string.
    /// The supported colors are published in the folderColorPalette field of the About resource.
    /// If an unsupported color is specified, the closest color in the palette will be used instead.
//    QString folderColorRgb() const;
    /// The original filename of the uploaded content if available, or else the original value of the name field.
    /// This is only available for files with binary content in Google Drive.
//    QString originalFilename() const;
    /// The full file extension extracted from the name field.
    /// May contain multiple concatenated extensions, such as "tar.gz".
    /// This is only available for files with binary content in Google Drive.
    /// This is automatically updated when the name field changes, however it is not cleared if the new name does not contain a valid extension.
//    QString fullFileExtension() const;
    /// The final component of fullFileExtension.
    /// This is only available for files with binary content in Google Drive.
//    QString fileExtension() const;
    /// The MD5 checksum for the content of the file.
    /// This is only applicable to files with binary content in Google Drive.
//    QString md5Checksum() const;
    /// The size of the file's content in bytes.
    /// This is only applicable to files with binary content in Google Drive.
    long size() const;
    /// The number of storage quota bytes used by the file.
    /// This includes the head revision as well as previous revisions with keepForever enabled.
//    long quotaBytesUsed() const;
    /// The ID of the file's head revision.
    /// This is currently only available for files with binary content in Google Drive.
//    QString headRevisionId() const;
    /// Additional information about the content of the file.
    /// These fields are never populated in responses.
//    QJsonObject contentHints() const;
    /// A thumbnail for the file.
    /// This will only be used if Google Drive cannot generate a standard thumbnail.
//    QJsonObject contentHints_thumbnail() const;
    /// The thumbnail data encoded with URL-safe Base64 (RFC 4648 section 5).
//    QByteArray contentHints_thumbnail_image() const;
    /// The MIME type of the thumbnail.
//    QString contentHints_thumbnail_mimeType() const;
    /// Text to be indexed for the file to improve fullText queries.
    /// This is limited to 128KB in length and may contain HTML elements.
//    QString contentHints_indexableText() const;
    /// Additional metadata about image media, if available.
//    QJsonObject imageMediaMetadata() const;
    /// The width of the image in pixels.
//    int imageMediaMetadata_width() const;
    /// The height of the image in pixels.
//    int imageMediaMetadata_height() const;
    /// The number of clockwise 90° rotations applied from the image's original orientation.
//    int imageMediaMetadata_rotation() const;
    /// Geographic location information stored in the image.
//    QJsonObject imageMediaMetadata_location() const;
    /// The latitude stored in the image.
//    double imageMediaMetadata_location_latitude() const;
    /// The longitude stored in the image.
//    double imageMediaMetadata_location_longitude() const;
    /// The altitude stored in the image.
//    double imageMediaMetadata_location_altitude() const;
    /// The date and time the photo was taken (EXIF DateTime).
//    QString imageMediaMetadata_time() const;
    /// The make of the camera used to create the photo.
//    QString imageMediaMetadata_cameraMake() const;
    /// The model of the camera used to create the photo.
//    QString imageMediaMetadata_cameraModel() const;
    /// The length of the exposure, in seconds.
//    float imageMediaMetadata_exposureTime() const;
    /// The aperture used to create the photo (f-number).
//    float imageMediaMetadata_aperture() const;
    /// Whether a flash was used to create the photo.
//    bool imageMediaMetadata_flashUsed() const;
    /// The focal length used to create the photo, in millimeters.
//    float imageMediaMetadata_focalLength() const;
    /// The ISO speed used to create the photo.
//    int imageMediaMetadata_isoSpeed() const;
    /// The metering mode used to create the photo.
//    QString imageMediaMetadata_meteringMode() const;
    /// The type of sensor used to create the photo.
//    QString imageMediaMetadata_sensor() const;
    /// The exposure mode used to create the photo.
//    QString imageMediaMetadata_exposureMode() const;
    /// The color space of the photo.
//    QString imageMediaMetadata_colorSpace() const;
    /// The white balance mode used to create the photo.
//    QString imageMediaMetadata_whiteBalance() const;
    /// The exposure bias of the photo (APEX value).
//    float imageMediaMetadata_exposureBias() const;
    /// The smallest f-number of the lens at the focal length used to create the photo (APEX value).
//    float imageMediaMetadata_maxApertureValue() const;
    /// The distance to the subject of the photo, in meters.
//    int imageMediaMetadata_subjectDistance() const;
    /// The lens used to create the photo.
//    QString imageMediaMetadata_lens() const;
    /// Additional metadata about video media.
    /// This may not be available immediately upon upload.
//    QJsonObject videoMediaMetadata() const;
    /// The width of the video in pixels.
//    int videoMediaMetadata_width() const;
    /// The height of the video in pixels.
//    int videoMediaMetadata_height() const;
    /// The duration of the video in milliseconds.
//    long videoMediaMetadata_durationMillis() const;
    /// Capabilities the current user has on this file.
    /// Each capability corresponds to a fine-grained action that a user may take.
//    QJsonObject capabilities() const;
    /// Whether the current user can edit this file.
    /// Other factors may limit the type of changes a user can make to a file.
    /// For example, see canChangeCopyRequiresWriterPermission or canModifyContent.
//    bool capabilities_canEdit() const;
    /// Whether the current user can comment on this file.
//    bool capabilities_canComment() const;
    /// Whether the current user can modify the sharing settings for this file.
//    bool capabilities_canShare() const;
    /// Whether the current user can copy this file.
    /// For an item in a shared drive, whether the current user can copy non-folder descendants of this item, or this item itself if it is not a folder.
//    bool capabilities_canCopy() const;
    /// Whether the current user can read the revisions resource of this file.
    /// For a shared drive item, whether revisions of non-folder descendants of this item, or this item itself if it is not a folder, can be read.
//    bool capabilities_canReadRevisions() const;
    /// Whether the file was created or opened by the requesting app.
//    bool isAppAuthorized() const;
    /// Whether this file has a thumbnail.
    /// This does not indicate whether the requesting app has access to the thumbnail.
    /// To check access, look for the presence of the thumbnailLink field.
//    bool hasThumbnail() const;
    /// The thumbnail version for use in thumbnail cache invalidation.
//    long thumbnailVersion() const;
    /// Whether the file has been modified by this user.
//    bool modifiedByMe() const;
    /// If the file has been explicitly trashed, the user who trashed it.
    /// Only populated for items in shared drives.
//    QJsonObject trashingUser() const;
    /// Identifies what kind of resource this is. Value: the fixed string "drive#user".
//    QString trashingUser_kind() const;
    /// A plain text displayable name for this user.
//    QString trashingUser_displayName() const;
    /// A link to the user's profile photo, if available.
//    QString trashingUser_photoLink() const;
    /// Whether this user is the requesting user.
//    bool trashingUser_me() const;
    /// The user's ID as visible in Permission resources.
//    QString trashingUser_permissionId() const;
    /// The email address of the user.
    /// This may not be present in certain contexts if the user has not made their email address visible to the requester.
//    QString trashingUser_emailAddress() const;
    /// The time that the item was trashed (RFC 3339 date-time).
    /// Only populated for items in shared drives.
//    QDateTime trashedTime() const;
    /// Whether the current user can add children to this folder.
    /// This is always false when the item is not a folder.
//    bool capabilities_canAddChildren() const;
    /// Whether the current user can delete this file.
//    bool capabilities_canDelete() const;
    /// Whether the current user can download this file.
//    bool capabilities_canDownload() const;
    /// Whether the current user can list the children of this folder.
    /// This is always false when the item is not a folder.
//    bool capabilities_canListChildren() const;
    /// Whether the current user can remove children from this folder.
    /// This is always false when the item is not a folder.
    /// For a folder in a shared drive, use canDeleteChildren or canTrashChildren instead.
//    bool capabilities_canRemoveChildren() const;
    /// Whether the current user can rename this file.
//    bool capabilities_canRename() const;
    /// Whether the current user can move this file to trash.
//    bool capabilities_canTrash() const;
    /// Whether the current user can restore this file from trash.
//    bool capabilities_canUntrash() const;
    /// Whether there are permissions directly on this file.
    /// This field is only populated for items in shared drives.
//    bool hasAugmentedPermissions() const;
    /// List of permission IDs for users with access to this file.
//    QJsonArray permissionIds() const;
    /// Whether the current user can change the copyRequiresWriterPermission restriction of this file.
//    bool capabilities_canChangeCopyRequiresWriterPermission() const;
    /// Whether the options to copy, print, or download this file, should be disabled for readers and commenters.
//    bool copyRequiresWriterPermission() const;
    /// Whether the current user can delete children of this folder.
    /// This is false when the item is not a folder. Only populated for items in shared drives.
//    bool capabilities_canDeleteChildren() const;
    /// Whether the current user can trash children of this folder.
    /// This is false when the item is not a folder.
    /// Only populated for items in shared drives.
//    bool capabilities_canTrashChildren() const;
    /// Links for exporting Google Docs to specific formats.
//    QJsonObject exportLinks() const;
    /// ID of the shared drive the file resides in. Only populated for items in shared drives.
//    QString driveId() const;
    /// Whether the current user can move children of this folder outside of the shared drive.
    /// This is false when the item is not a folder.
    /// Only populated for items in shared drives.
//    bool capabilities_canMoveChildrenOutOfDrive() const;
    /// Whether the current user can move children of this folder within the shared drive.
    /// This is false when the item is not a folder.
    /// Only populated for items in shared drives.
//    bool capabilities_canMoveChildrenWithinDrive() const;
    /// Whether the current user can move this item outside of this drive by changing its parent.
    /// Note that a request to change the parent of the item may still fail depending on the new parent that is being added.
//    bool capabilities_canMoveItemOutOfDrive() const;
    /// Whether the current user can move this item within this shared drive.
    /// Note that a request to change the parent of the item may still fail depending on the new parent that is being added.
    /// Only populated for items in shared drives.
//    bool capabilities_canMoveItemWithinDrive() const;
    /// Whether the current user can read the shared drive to which this file belongs.
    /// Only populated for items in shared drives.
//    bool capabilities_canReadDrive() const;
    /// Whether the current user can modify the content of this file.
//    bool capabilities_canModifyContent() const;

    /// return true if object is empty
    bool isEmpty() const;
private:
    /// save data form constructer resource
    QJsonObject m_object;
};
}

#endif // GDRIVEFILERESOURCE_H
