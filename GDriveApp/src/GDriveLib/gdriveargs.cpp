#include "gdriveargs.h"

GDrive::FileUpdateArg::FileUpdateArg()
{

}

QString GDrive::FileUpdateArg::getAddParents() const
{
    return addParents;
}

void GDrive::FileUpdateArg::setAddParents(const QString &value)
{
    addParents = value;
}

bool GDrive::FileUpdateArg::getUseContentAsIndexableText() const
{
    return useContentAsIndexableText;
}

void GDrive::FileUpdateArg::setUseContentAsIndexableText(bool value)
{
    useContentAsIndexableText = value;
}

QString GDrive::FileUpdateArg::getRemoveParents() const
{
    return removeParents;
}

void GDrive::FileUpdateArg::setRemoveParents(const QString &value)
{
    removeParents = value;
}

QString GDrive::FileUpdateArg::getOcrLanguage() const
{
    return ocrLanguage;
}

void GDrive::FileUpdateArg::setOcrLanguage(const QString &value)
{
    ocrLanguage = value;
}

bool GDrive::FileUpdateArg::getKeepRevisionForever() const
{
    return keepRevisionForever;
}

void GDrive::FileUpdateArg::setKeepRevisionForever(bool value)
{
    keepRevisionForever = value;
}

bool GDrive::FileUpdateArg::getEnforceSingleParent() const
{
    return enforceSingleParent;
}

void GDrive::FileUpdateArg::setEnforceSingleParent(bool value)
{
    enforceSingleParent = value;
}

GDrive::FileCreateArg::FileCreateArg()
{

}

bool GDrive::FileCreateArg::getEnforceSingleParent() const
{
    return enforceSingleParent;
}

void GDrive::FileCreateArg::setEnforceSingleParent(bool value)
{
    enforceSingleParent = value;
}

bool GDrive::FileCreateArg::getIgnoreDefaultVisibility() const
{
    return ignoreDefaultVisibility;
}

void GDrive::FileCreateArg::setIgnoreDefaultVisibility(bool value)
{
    ignoreDefaultVisibility = value;
}

bool GDrive::FileCreateArg::getKeepRevisionForever() const
{
    return keepRevisionForever;
}

void GDrive::FileCreateArg::setKeepRevisionForever(bool value)
{
    keepRevisionForever = value;
}

QString GDrive::FileCreateArg::getOcrLanguage() const
{
    return ocrLanguage;
}

void GDrive::FileCreateArg::setOcrLanguage(const QString &value)
{
    ocrLanguage = value;
}

bool GDrive::FileCreateArg::getUseContentAsIndexableText() const
{
    return useContentAsIndexableText;
}

void GDrive::FileCreateArg::setUseContentAsIndexableText(bool value)
{
    useContentAsIndexableText = value;
}

GDrive::FileGetArg::FileGetArg(const QString &fileId)
{
    this->fileId = fileId;
}

QString GDrive::FileGetArg::getFileId() const
{
    return fileId;
}

void GDrive::FileGetArg::setFileId(const QString &value)
{
    fileId = value;
}

bool GDrive::FileGetArg::getAcknowledgeAbuse() const
{
    return acknowledgeAbuse;
}

void GDrive::FileGetArg::setAcknowledgeAbuse(bool value)
{
    acknowledgeAbuse = value;
}

QString GDrive::FileGetArg::getFields() const
{
    return fields;
}

void GDrive::FileGetArg::setFields(const QString &value)
{
    fields = value;
}
