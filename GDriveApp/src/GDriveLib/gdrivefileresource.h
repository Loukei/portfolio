#ifndef GDRIVEFILERESOURCE_H
#define GDRIVEFILERESOURCE_H
#include <QJsonDocument>

namespace GDrive {
class GDriveFileResource
{
public:
    ///construct by QBytearray data
    explicit GDriveFileResource(const QByteArray &data);
    ~GDriveFileResource();
    /// Identifies what kind of resource this is. Value: the fixed string "drive#file".
    QString kind() const;
    QString fileID() const;
    QString name() const;
    QString mimeType() const;
    QString description() const;
    QStringList parents() const;

    long size() const;
    /// returns true if receive document in constructor is null.
    bool isNull() const;
    /// return current error string
    QString errorString() const;
private:
    /// save data form constructer resource
    QJsonDocument m_data;
    /// save parse error use human readable string
    QString m_errorString;
};
}

#endif // GDRIVEFILERESOURCE_H
