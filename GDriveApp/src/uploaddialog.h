#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H

#include <QDialog>

namespace Ui {
class UploadDialog;
}

class UploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UploadDialog(QWidget *parent = nullptr,
                          const QString &caption = QString(),
                          const QString &filePath = QString());
    ~UploadDialog() override;

    QString getFilePath() const;

    int getUploadType() const;
    QString getUploadTypeStr() const;

    bool getEnforceSingleParent() const;
    void setEnforceSingleParent(bool enforceSingleParent);

    bool getIgnoreDefaultVisibility() const;
    void setIgnoreDefaultVisibility(bool ignoreDefaultVisibility);

    bool getKeepRevisionForever() const;
    void setKeepRevisionForever(bool keepRevisionForever);

    QString getOcrLanguage() const;
    void setOcrLanguage(const QString &ocrLanguage);

    bool getUseContentAsIndexableText() const;
    void setUseContentAsIndexableText(bool useContentAsIndexableText);

private slots:
    void on_pushButton_FilePath_clicked();

private:
    Ui::UploadDialog *ui;
};

#endif // UPLOADDIALOG_H
