#ifndef GDRIVEARGS_H
#define GDRIVEARGS_H
#include <QString>

namespace GDrive {

enum uploadType{

};

class FileCreateArg{
public:
    FileCreateArg();
    ~FileCreateArg() = default;

    bool getEnforceSingleParent() const;
    void setEnforceSingleParent(bool value);

    bool getIgnoreDefaultVisibility() const;
    void setIgnoreDefaultVisibility(bool value);

    bool getKeepRevisionForever() const;
    void setKeepRevisionForever(bool value);

    QString getOcrLanguage() const;
    void setOcrLanguage(const QString &value);

    bool getUseContentAsIndexableText() const;
    void setUseContentAsIndexableText(bool value);

protected:
    bool enforceSingleParent = false;
    bool ignoreDefaultVisibility = false;
    bool keepRevisionForever = false;
    QString ocrLanguage = "";
    bool useContentAsIndexableText = false;
};

class FileGetArg{
public:
    FileGetArg(const QString &fileId = "");
    ~FileGetArg() = default;

    QString getFileId() const;
    void setFileId(const QString &value);

    bool getAcknowledgeAbuse() const;
    void setAcknowledgeAbuse(bool value);

    QString getFields() const;
    void setFields(const QString &value);

protected:
    QString fileId;
    bool acknowledgeAbuse = false;
    QString fields = "";
};

class FileUpdateArg{
public:
    FileUpdateArg();
    ~FileUpdateArg() = default;

    QString getAddParents() const;
    void setAddParents(const QString &value);

    bool getEnforceSingleParent() const;
    void setEnforceSingleParent(bool value);

    bool getKeepRevisionForever() const;
    void setKeepRevisionForever(bool value);

    QString getOcrLanguage() const;
    void setOcrLanguage(const QString &value);

    QString getRemoveParents() const;
    void setRemoveParents(const QString &value);

    bool getUseContentAsIndexableText() const;
    void setUseContentAsIndexableText(bool value);

protected:
    QString addParents = "";
    bool enforceSingleParent = false;
    bool keepRevisionForever = false;
    QString ocrLanguage = "";
    QString removeParents = "";
    bool useContentAsIndexableText = false;
};

}

#endif // GDRIVEARGS_H
