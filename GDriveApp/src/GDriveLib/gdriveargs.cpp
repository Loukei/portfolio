#include "gdriveargs.h"

GDrive::Args::FileCreateArgs::FileCreateArgs()
{

}

bool GDrive::Args::FileCreateArgs::useContentAsIndexableText() const
{
    return m_useContentAsIndexableText;
}

void GDrive::Args::FileCreateArgs::setUseContentAsIndexableText(bool useContentAsIndexableText)
{
    m_useContentAsIndexableText = useContentAsIndexableText;
}

QString GDrive::Args::FileCreateArgs::ocrLanguage() const
{
    return m_ocrLanguage;
}

void GDrive::Args::FileCreateArgs::setOcrLanguage(const QString &ocrLanguage)
{
    m_ocrLanguage = ocrLanguage;
}

bool GDrive::Args::FileCreateArgs::keepRevisionForever() const
{
    return m_keepRevisionForever;
}

void GDrive::Args::FileCreateArgs::setKeepRevisionForever(bool keepRevisionForever)
{
    m_keepRevisionForever = keepRevisionForever;
}

bool GDrive::Args::FileCreateArgs::ignoreDefaultVisibility() const
{
    return m_ignoreDefaultVisibility;
}

void GDrive::Args::FileCreateArgs::setIgnoreDefaultVisibility(bool ignoreDefaultVisibility)
{
    m_ignoreDefaultVisibility = ignoreDefaultVisibility;
}

bool GDrive::Args::FileCreateArgs::enforceSingleParent() const
{
    return m_enforceSingleParent;
}

void GDrive::Args::FileCreateArgs::setEnforceSingleParent(bool enforceSingleParent)
{
    m_enforceSingleParent = enforceSingleParent;
}

GDrive::Args::FileUpdateArgs::FileUpdateArgs(const QString &fileId)
{

}
