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

void GDriveFiles::create()
{

}

void GDriveFiles::update()
{

}

void GDriveFiles::remove()
{

}
