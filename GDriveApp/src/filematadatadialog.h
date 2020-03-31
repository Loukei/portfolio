#ifndef FILEMATADATADIALOG_H
#define FILEMATADATADIALOG_H

#include <QDialog>

class QJsonModel;
namespace Ui {
class FileMataDataDialog;
}
/*!
 * \class FileMataDataDialog
 * \brief The FileMataDataDialog class is UI to get file matadata
 */
class FileMataDataDialog : public QDialog
{
    Q_OBJECT

public:
    /// constructor
    explicit FileMataDataDialog(QWidget *parent = nullptr,
                                const QString &fileID = QString(),
                                const QString &fields = QString());
    /// destructor
    ~FileMataDataDialog();
    /// return fileID in dialog
    QString getFileID() const;
    /// return Fields in dialog
    QString getFields() const;

public slots:
    /// slot when FileGet task complete, update m_model
    void onFileGet_finished();

signals:
    /// send this signal to get file matadata
    void query(const QString &fileID,
               const QString &fields);

private slots:
    /// emit signal `query`
    void on_pushButton_Get_clicked();

private:
    Ui::FileMataDataDialog *ui;
    QJsonModel *m_model;
};

#endif // FILEMATADATADIALOG_H
