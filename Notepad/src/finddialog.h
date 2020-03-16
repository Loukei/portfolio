#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

namespace Ui {
class FindDialog;
}

class TextEditor;
class QComboBox;

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr,TextEditor *_editor = nullptr);
    ~FindDialog() override;
    inline bool isNullEditor();

private slots:
    void on_pushButton_Find_clicked();
    void on_pushButton_FindPrev_clicked();
    void on_pushButton_Replace_clicked();
    void on_pushButton_ReplaceAll_clicked();

    void on_pushButton_clicked();

protected:
    void closeEvent(QCloseEvent *e) override;

private:
    Ui::FindDialog *ui;
    TextEditor *editor; //! Search target

    void updateCombox(QComboBox *box);
    QString dispatchSearchFunc(bool isSearchBackward = false);   //! execute the correspond search function,update status label
    QString dispatchReplaceAllFunc();  //! execute TextEditor::replaceAll with given search settings
};

#endif // FINDDIALOG_H

//! Note： 利用editor::ExtraSelections來標記搜尋結果,因此每次開始搜尋/取代前,都要清理一次
