#include "gdrivefiles.h"
#include <QOAuth2AuthorizationCodeFlow>

using namespace GDrive;

GDriveFiles::GDriveFiles(QOAuth2AuthorizationCodeFlow *parent)
    : QObject(parent),m_google(parent)
{
}

GDriveFiles::~GDriveFiles()
{

}

GDriveUploader *GDriveFiles::create()
{
    return new GDriveSimpleUpload(m_google,"D:/TestData/test測試文件");
}

void GDriveFiles::update()
{

}

void GDriveFiles::remove()
{

}
