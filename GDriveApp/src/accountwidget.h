#ifndef ACCOUNTWIDGET_H
#define ACCOUNTWIDGET_H

#include <QWidget>

namespace Ui {
class AccountWidget;
}

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
QT_END_NAMESPACE

class AccountWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountWidget(QWidget *parent = nullptr);
    ~AccountWidget();

    void setUserIcon(const QPixmap &icon);
    const QPixmap* userIcon() const;

    void setDisplayName(const QString &displayname);
    QString displayName() const;

    void setEmailAddress(const QString &emailaddress);
    QString emailAddress() const;

    void loadNetworkImage(const QUrl &url);

    /// reset UI to Unauthorizated
    void resetUI(); 
    /// Draw image with given string(for UserIcon)
    static QPixmap DrawImage(const QString &text);

protected:
    /// reimplment QWidget::resizeEvent(), resize usericon
//    void resizeEvent(QResizeEvent *event) override;
    /// send network request to get url image and save to m_icon
    void requestUserIcon(const QUrl &url);

protected slots:
    /// read network reply and set m_icon to ui->label_UserIcon
    void on_requestUserIcon_finished();

private:
    Ui::AccountWidget *ui;
    const QPixmap m_defaultUserIcon;

    QNetworkAccessManager *m_manager;
    QPixmap m_icon;
};

#endif // ACCOUNTWIDGET_H
