#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class SearchDialog;
}

class QJsonModel;
class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    ~SearchDialog();
signals:
    void query(const QString &q,
               const QString &spaces,
               const QString &fields,
               const QString &pageToken);
public slots:
    void onFileSearch_finished();

private slots:
    void on_pushButton_search_clicked();
private:
    Ui::SearchDialog *ui;
    /// saving data form search result
    QJsonModel *m_model;
};
/*
 * Credit:https://github.com/dridk/QJsonModel
*/
#endif // SEARCHDIALOG_H
