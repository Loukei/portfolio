#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = nullptr,
                          const QString &fileID = QString(),
                          const QString &filePath = QString());
    ~UpdateDialog() override;

    /* Required Parameter Getter */
    /// return FileID
    QString getFileID() const;
    /// return File Path
    QString getFilePath() const;
    /// return Upload Type in int form, native combox return
    int getUploadType() const;
    /// return Upload Type in String form
    QString getUploadTypeStr() const;

    /* Optional Parameter Getter & Setter */
    void setAddParents(const QString &addParents = "");
    QString getAddParents() const;
    void setEnforceSingleParent(bool enforceSingleParent = false);
    bool getEnforceSingleParent() const;
    void setKeepRevisionForever(bool keepRevisionForever = false);
    bool getKeepRevisionForever() const;
    void setOcrLanguage(const QString &ocrLanguage = "");
    QString getOcrLanguage();
    void setRemoveParents(const QString &removeParents = "");
    QString getRemoveParents() const;
    void setUseContentAsIndexableText(bool useContentAsIndexableText = false);
    bool getUseContentAsIndexableText() const;

private slots:
    void on_pushButton_FilePath_clicked();

private:
    Ui::UpdateDialog *ui;
    /// Convert Boolean value to String
    QString formBool(bool value);
    /// Convert String value to Boolean
    bool formString(const QString &value);
};

#endif // UPDATEDIALOG_H
