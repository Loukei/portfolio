#ifndef GDRIVEARGS_H
#define GDRIVEARGS_H
#include <QString>
#include <QUrl>

namespace GDrive {

/// Turn Boolean to "true" or "false"
bool StringToBoolean(const QString &value);
/// Turn String to Boolean
QString BooleanToString(bool value);

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

} // GDrive

#endif // GDRIVEARGS_H
