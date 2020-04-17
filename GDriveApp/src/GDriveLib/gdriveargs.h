#ifndef GDRIVEARGS_H
#define GDRIVEARGS_H
#include <QString>
#include <QUrl>

namespace GDrive {

/// Turn Boolean to "true" or "false"
bool StringToBoolean(const QString &value);
/// Turn String to Boolean
QString BooleanToString(bool value);

/*!
 * \brief The UrlArgs class provide base method for Url method
 */
class UrlArgs{
public:
    explicit UrlArgs() = default;
    virtual ~UrlArgs() = default;

    enum class UploadType : char
    {
        MEDIA = 0x01,
        MULTIPART = 0x02,
        RESUMABLE = 0x03
    };

    static UploadType StringToUploadType(const QString &value);
    static QString UploadTypeToString(UploadType value);
};

/*!
 * \class FileCreateArgs
 * \brief The FileCreateArgs class provide `Files: create` method parameter
 *
 * ## Reference
 * - [Files: create](https://developers.google.com/drive/api/v3/reference/files/create)
 */
class FileCreateArgs : public UrlArgs
{
public:
    FileCreateArgs(UploadType uploadType = UploadType::MEDIA,
                   bool enforceSingleParent = false,
                   bool ignoreDefaultVisibility = false,
                   bool keepRevisionForever = false,
                   const QString &ocrLanguage = "",
                   bool useContentAsIndexableText = false);
    ~FileCreateArgs() override;

    QString uploadType() const;

    bool enforceSingleParent() const;
    void setEnforceSingleParent(bool enforceSingleParent);

    bool ignoreDefaultVisibility() const;
    void setIgnoreDefaultVisibility(bool ignoreDefaultVisibility);

    bool keepRevisionForever() const;
    void setKeepRevisionForever(bool keepRevisionForever);

    QString ocrLanguage() const;
    void setOcrLanguage(const QString &ocrLanguage);

    bool useContentAsIndexableText() const;
    void setUseContentAsIndexableText(bool useContentAsIndexableText);

protected:
    QString m_uploadType = "media";
    bool m_enforceSingleParent = false;
    bool m_ignoreDefaultVisibility = false;
    bool m_keepRevisionForever = false;
    QString m_ocrLanguage = "";
    bool m_useContentAsIndexableText = false;
};

/*!
 * \class FileGetArgs
 * \brief The FileGetArgs class provide `Files: get` method parameter
 *
 * - FileGetArgs 提供取得檔案內容(class GDriveFileDownloader)與查詢檔案元資料(class GDriveFileGet)的參數
 * - acknowledgeAbuse參數只有在下載檔案內容才有用
 *
 * ## Reference
 * - [Files: get](https://developers.google.com/drive/api/v3/reference/files/get)
 */
class FileGetArgs : public UrlArgs
{
public:
    FileGetArgs(const QString &fileId,
                bool acknowledgeAbuse = false,
                const QString &fields = "");
    ~FileGetArgs() override = default;

    QString fileId() const;

    bool acknowledgeAbuse() const;
    void setAcknowledgeAbuse(bool acknowledgeAbuse);

    QString fields() const;
    void setFields(const QString &fields);

protected:
    QString m_fileId;
    bool m_acknowledgeAbuse = false;
    QString m_fields = "";
};

/*!
 * \class FileUpdateArgs
 * \brief The FileUpdateArgs class provide `Files: update` method parameter
 *
 * ## Reference
 * - [Files: update](https://developers.google.com/drive/api/v3/reference/files/update)
 */
class FileUpdateArgs : public UrlArgs
{
public:
    FileUpdateArgs(const QString &fileId,
                   UploadType type = UploadType::MEDIA,
                   const QString &addParents = "",
                   bool enforceSingleParent = false,
                   bool keepRevisionForever = false,
                   const QString &ocrLanguage = "",
                   const QString &removeParents = "",
                   bool useContentAsIndexableText = false);
    ~FileUpdateArgs() override = default;

    QString fileId() const;

    QString uploadType() const;

    QString addParents() const;
    void setAddParents(const QString &addParents);

    bool enforceSingleParent() const;
    void setEnforceSingleParent(bool enforceSingleParent);

    bool keepRevisionForever() const;
    void setKeepRevisionForever(bool keepRevisionForever);

    QString ocrLanguage() const;
    void setOcrLanguage(const QString &ocrLanguage);

    QString removeParents() const;
    void setRemoveParents(const QString &removeParents);

    bool useContentAsIndexableText() const;
    void setUseContentAsIndexableText(bool useContentAsIndexableText);

private:
    QString m_fileId;
    QString m_uploadType = "media";
    QString m_addParents = "";
    bool m_enforceSingleParent = false;
    bool m_keepRevisionForever = false;
    QString m_ocrLanguage = "";
    QString m_removeParents = "";
    bool m_useContentAsIndexableText = false;
};

/*!
 * \class FileListArgs
 * \brief The FileListArgs class provide `Files: list` method parameter
 *
 * ## Reference
 * -[Files: list](https://developers.google.com/drive/api/v3/reference/files/list)
 */
class FileListArgs
{
public:
    FileListArgs(const QString &corpora = "",
                 const QString &driveId = "",
                 const QString &fields = "",
                 const QString &orderBy = "",
                 int pageSize = 100,
                 const QString &pageToken = "",
                 const QString &q = "",
                 const QString &spaces = "");
    ~FileListArgs() = default;

    QString corpora() const;
    void setCorpora(const QString &corpora);

    QString driveId() const;
    void setDriveId(const QString &driveId);

    QString fields() const;
    void setFields(const QString &fields);

    QString orderBy() const;
    void setOrderBy(const QString &orderBy);

    int pageSize() const;
    void setPageSize(int pageSize);

    QString pageToken() const;
    void setPageToken(const QString &pageToken);

    QString q() const;
    void setQ(const QString &q);

    QString spaces() const;
    void setSpaces(const QString &spaces);

private:
    QString m_corpora;
    QString m_driveId;
    QString m_fields;
    QString m_orderBy;
    int m_pageSize = 100;
    QString m_pageToken;
    QString m_q;
    QString m_spaces;
};

} // GDrive

#endif // GDRIVEARGS_H
