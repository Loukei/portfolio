#include "gdriveargs.h"
#include <QUrlQuery>

using namespace GDrive;

/*==========================class UrlArgs======================================*/

bool GDrive::StringToBoolean(const QString &value)
{
    if((value == "True") | (value == "true")){
        return true;
    }else if ((value == "False") | (value == "false")) {
        return false;
    }else {
        return false;
    }
}

QString GDrive::BooleanToString(bool value)
{
    if(value)   return "true";
    else        return "false";
}

UrlArgs::UploadType UrlArgs::StringToUploadType(const QString &value)
{
    if(value == "media"){
        return UploadType::MEDIA;
    }else if (value == "multipart") {
        return UploadType::MULTIPART;
    }else if (value == "resumable") {
        return UploadType::RESUMABLE;
    }
    return UploadType::MEDIA;
}

QString UrlArgs::UploadTypeToString(UrlArgs::UploadType value)
{
    switch (value) {
    case UploadType::MEDIA:
        return "media";
    case UploadType::MULTIPART:
        return "multipart";
    case UploadType::RESUMABLE:
        return "resumable";
    }
}

/*==========================class FileUpdateArgs======================================*/

FileUpdateArgs::FileUpdateArgs(const QString &fileId,
                               UrlArgs::UploadType type,
                               const QString &addParents,
                               bool enforceSingleParent,
                               bool keepRevisionForever,
                               const QString &ocrLanguage,
                               const QString &removeParents,
                               bool useContentAsIndexableText)
    :UrlArgs ()
{
    m_fileId = fileId;
    m_uploadType = UploadTypeToString(type);
    m_addParents = addParents;
    m_enforceSingleParent = enforceSingleParent;
    m_keepRevisionForever = keepRevisionForever;
    m_ocrLanguage = ocrLanguage;
    m_removeParents = removeParents;
    m_useContentAsIndexableText = useContentAsIndexableText;
}

QString FileUpdateArgs::fileId() const
{
    return m_fileId;
}

QString FileUpdateArgs::uploadType() const
{
    return m_uploadType;
}

QString FileUpdateArgs::addParents() const
{
    return m_addParents;
}

void FileUpdateArgs::setAddParents(const QString &addParents)
{
    m_addParents = addParents;
}

bool FileUpdateArgs::enforceSingleParent() const
{
    return m_enforceSingleParent;
}

void FileUpdateArgs::setEnforceSingleParent(bool enforceSingleParent)
{
    m_enforceSingleParent = enforceSingleParent;
}

bool FileUpdateArgs::keepRevisionForever() const
{
    return m_keepRevisionForever;
}

void FileUpdateArgs::setKeepRevisionForever(bool keepRevisionForever)
{
    m_keepRevisionForever = keepRevisionForever;
}

QString FileUpdateArgs::ocrLanguage() const
{
    return m_ocrLanguage;
}

void FileUpdateArgs::setOcrLanguage(const QString &ocrLanguage)
{
    m_ocrLanguage = ocrLanguage;
}

QString FileUpdateArgs::removeParents() const
{
    return m_removeParents;
}

void FileUpdateArgs::setRemoveParents(const QString &removeParents)
{
    m_removeParents = removeParents;
}

bool FileUpdateArgs::useContentAsIndexableText() const
{
    return m_useContentAsIndexableText;
}

void FileUpdateArgs::setUseContentAsIndexableText(bool useContentAsIndexableText)
{
    m_useContentAsIndexableText = useContentAsIndexableText;
}

/*==========================class FileCreateArgs======================================*/

FileCreateArgs::FileCreateArgs(UploadType uploadType, bool enforceSingleParent, bool ignoreDefaultVisibility, bool keepRevisionForever, const QString &ocrLanguage, bool useContentAsIndexableText)
    :UrlArgs (),m_enforceSingleParent(enforceSingleParent),m_ignoreDefaultVisibility(ignoreDefaultVisibility),m_keepRevisionForever(keepRevisionForever),m_ocrLanguage(ocrLanguage),m_useContentAsIndexableText(useContentAsIndexableText)
{
    m_uploadType = UploadTypeToString(uploadType);
}

FileCreateArgs::~FileCreateArgs()
{

}

QString FileCreateArgs::uploadType() const
{
    return m_uploadType;
}

bool FileCreateArgs::useContentAsIndexableText() const
{
    return m_useContentAsIndexableText;
}

void FileCreateArgs::setUseContentAsIndexableText(bool useContentAsIndexableText)
{
    m_useContentAsIndexableText = useContentAsIndexableText;
}

QString FileCreateArgs::ocrLanguage() const
{
    return m_ocrLanguage;
}

void FileCreateArgs::setOcrLanguage(const QString &ocrLanguage)
{
    m_ocrLanguage = ocrLanguage;
}

bool FileCreateArgs::keepRevisionForever() const
{
    return m_keepRevisionForever;
}

void FileCreateArgs::setKeepRevisionForever(bool keepRevisionForever)
{
    m_keepRevisionForever = keepRevisionForever;
}

bool FileCreateArgs::ignoreDefaultVisibility() const
{
    return m_ignoreDefaultVisibility;
}

void FileCreateArgs::setIgnoreDefaultVisibility(bool ignoreDefaultVisibility)
{
    m_ignoreDefaultVisibility = ignoreDefaultVisibility;
}

bool FileCreateArgs::enforceSingleParent() const
{
    return m_enforceSingleParent;
}

void FileCreateArgs::setEnforceSingleParent(bool enforceSingleParent)
{
    m_enforceSingleParent = enforceSingleParent;
}

/*==========================class FileGetArgs======================================*/
FileGetArgs::FileGetArgs(const QString &fileId,
                         bool acknowledgeAbuse,
                         const QString &fields)
    :UrlArgs (),
      m_fileId(fileId),
      m_acknowledgeAbuse(acknowledgeAbuse),
      m_fields(fields)
{

}

QString FileGetArgs::fileId() const
{
    return m_fileId;
}

bool FileGetArgs::acknowledgeAbuse() const
{
    return m_acknowledgeAbuse;
}

void FileGetArgs::setAcknowledgeAbuse(bool acknowledgeAbuse)
{
    m_acknowledgeAbuse = acknowledgeAbuse;
}

QString FileGetArgs::fields() const
{
    return m_fields;
}

void FileGetArgs::setFields(const QString &fields)
{
    m_fields = fields;
}
