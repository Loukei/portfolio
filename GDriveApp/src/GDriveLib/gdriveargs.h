#ifndef GDRIVEARGS_H
#define GDRIVEARGS_H
#include <QString>

namespace GDrive {
/*!
 * \namespace Args
 * \brief The Args is use for define the argument of Google drive parameter
 */
namespace Args {

enum class UPLOADTYPE : char
{
    MEDIA = 0x01,
    MULTIPART = 0x02,
    RESUMABLE = 0x03
};

class FileCreateArgs{
public:
    FileCreateArgs();
    ~FileCreateArgs() = default;

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
//    QString m_uploadType = "media";
    bool m_enforceSingleParent = false;
    bool m_ignoreDefaultVisibility = false;
    bool m_keepRevisionForever = false;
    QString m_ocrLanguage = "";
    bool m_useContentAsIndexableText = false;
};

class FileGetArgs{
public:
    FileGetArgs(const QString &fileId = "");
    ~FileGetArgs() = default;

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

class FileUpdateArgs{
public:
    FileUpdateArgs(const QString &fileId);
    ~FileUpdateArgs() = default;

    QString m_fileId;
    QString m_addParents = "";
    bool m_enforceSingleParent = false;
    bool m_keepRevisionForever = false;
    QString m_ocrLanguage = "";
    QString m_removeParents = "";
    bool m_useContentAsIndexableText = false;
};
}


} // GDrive

#endif // GDRIVEARGS_H
