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
    explicit UploadDialog(QWidget *parent = nullptr);
    ~UploadDialog() override;

    QString getFilePath() const;
    void setFilePath(const QString &filepath = QStringLiteral("C:/"));

    int getUploadType() const;

    bool getEnforceSingleParent() const;
    bool getIgnoreDefaultVisibility() const;
    bool getKeepRevisionForever() const;
    QString getOcrLanguage() const;
    bool getSupportsAllDrives() const;
    bool getUseContentAsIndexableText() const;

private slots:
    void on_pushButton_FilePath_clicked();

private:
    Ui::UploadDialog *ui;
};

#endif // UPLOADDIALOG_H
