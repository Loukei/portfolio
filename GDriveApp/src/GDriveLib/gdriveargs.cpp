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
    return "media";
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

FileCreateArgs::FileCreateArgs(UploadType uploadType,
                               bool enforceSingleParent,
                               bool ignoreDefaultVisibility,
                               bool keepRevisionForever,
                               const QString &ocrLanguage,
                               bool useContentAsIndexableText)
    :UrlArgs (),
      m_enforceSingleParent(enforceSingleParent),
      m_ignoreDefaultVisibility(ignoreDefaultVisibility),
      m_keepRevisionForever(keepRevisionForever),
      m_ocrLanguage(ocrLanguage),
      m_useContentAsIndexableText(useContentAsIndexableText)
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

/*==========================class FileListArgs======================================*/
FileListArgs::FileListArgs(const QString &corpora,
                           const QString &driveId,
                           const QString &fields,
                           const QString &orderBy,
                           int pageSize,
                           const QString &pageToken,
                           const QString &q,
                           const QString &spaces)
    :m_corpora(corpora),
      m_driveId(driveId),
      m_fields(fields),
      m_orderBy(orderBy),
      m_pageSize(pageSize),
      m_pageToken(pageToken),
      m_q(q),
      m_spaces(spaces)
{

}

QString FileListArgs::corpora() const
{
    return m_corpora;
}

void FileListArgs::setCorpora(const QString &corpora)
{
    m_corpora = corpora;
}

QString FileListArgs::driveId() const
{
    return m_driveId;
}

void FileListArgs::setDriveId(const QString &driveId)
{
    m_driveId = driveId;
}

QString FileListArgs::fields() const
{
    return m_fields;
}

void FileListArgs::setFields(const QString &fields)
{
    m_fields = fields;
}

QString FileListArgs::orderBy() const
{
    return m_orderBy;
}

void FileListArgs::setOrderBy(const QString &orderBy)
{
    m_orderBy = orderBy;
}

int FileListArgs::pageSize() const
{
    return m_pageSize;
}

void FileListArgs::setPageSize(int pageSize)
{
    m_pageSize = pageSize;
}

QString FileListArgs::pageToken() const
{
    return m_pageToken;
}

void FileListArgs::setPageToken(const QString &pageToken)
{
    m_pageToken = pageToken;
}

QString FileListArgs::q() const
{
    return m_q;
}

void FileListArgs::setQ(const QString &q)
{
    m_q = q;
}

QString FileListArgs::spaces() const
{
    return m_spaces;
}

void FileListArgs::setSpaces(const QString &spaces)
{
    m_spaces = spaces;
}
