#ifndef ACCOUNTWIDGETACTS_H
#define ACCOUNTWIDGETACTS_H

#include <QMenu>
#include <QLabel>
#include <QWidgetAction>
#include <QAction>
#include <QPixmap>

#include <QPainter>  // To paint text Icon
#include "networkimgloader.h" // load image from url
class AccountWidgetActs{
public:
    AccountWidgetActs() = default;
    ~AccountWidgetActs()
    {
    }

#define IconSize QSize(64,64)
    const QPixmap icon_default = QPixmap(":/Icon/Icon/iconfinder_user_close_103746.png");
    QPixmap icon_current; //an icon to current user,QLabel doesn't handle storage
    QLabel *label_UserIcon;
    QLabel *label_UserName;
    QLabel *label_UserEmail;

    QWidgetAction *action_UserIcon;
    QWidgetAction *action_UserName;
    QWidgetAction *action_UserEmail;

    /// setup every QObject member to QMenu parent
    void setupUi(QMenu *parrent)
    {
        label_UserIcon = new QLabel(parrent);
        label_UserName = new QLabel(parrent);
        label_UserEmail = new QLabel(parrent);

        label_UserIcon->setPixmap(icon_default.scaled(IconSize));
        label_UserIcon->setAlignment(Qt::AlignCenter);
        label_UserName->setAlignment(Qt::AlignCenter);
        label_UserEmail->setAlignment(Qt::AlignCenter);

        action_UserIcon = new QWidgetAction(parrent);
        action_UserName = new QWidgetAction(parrent);
        action_UserEmail = new QWidgetAction(parrent);

        action_UserIcon->setDefaultWidget(label_UserIcon);
        action_UserName->setDefaultWidget(label_UserName);
        action_UserEmail->setDefaultWidget(label_UserEmail);

        action_UserName->setVisible(false);
        action_UserEmail->setVisible(false);
    }

    /// clear all labels content, set icon to default,then hide text label
    void resetUi()
    {
        label_UserIcon->setPixmap(icon_default.scaled(IconSize));
        label_UserName->clear();
        label_UserEmail->clear();
        action_UserName->setVisible(false);
        action_UserEmail->setVisible(false);
    }

    QList<QAction*> actions() const
    {
        return {action_UserIcon,action_UserName,action_UserEmail};
    }

    void updateUi(const QString &username,const QString &email,const QString &photolink,QNetworkAccessManager *manager)
    {
        label_UserName->setText(username);
        label_UserEmail->setText(email);
        /* If user doesnt have icon, use username to draw one */
        if(photolink.isEmpty()) { loadTextIcon(username); }
        else                    { loadNetworkIcon(photolink,manager);}
        /* Beaware,the order of setVisiable change the label size calculate*/
        this->action_UserName->setVisible(true);
        this->action_UserEmail->setVisible(true);
    }

    /// load pixmap from input data,then update label_UserIcon
    inline void loadUserIconFromData(const QByteArray &data)
    {
        icon_current.loadFromData(data);
        label_UserIcon->setPixmap(icon_current);
    }

    inline void setUserName(const QString &text)
    {
        label_UserName->setText(text);
    }

    inline QString userName() const
    {
        return label_UserName->text();
    }

    inline void setEmailAddress(const QString &text)
    {
        label_UserEmail->setText(text);
    }

    inline QString emailAddress() const
    {
        return label_UserEmail->text();
    }

    static QPixmap drawTextIcon(const QString &text)
    {
        QPixmap icon(IconSize);
//        icon.fill(QColor(255,255,255,0));
        icon.fill(Qt::darkGreen);
        QPainter painter(&icon);
        /*Background*/
//        painter.setBackgroundMode(Qt::TransparentMode);
        /*Draw Rect fram*/
//        painter.setPen(QPen(Qt::green,1));
//        painter.drawRect(0,0,64,64);
        /*Draw Text*/
        QFont font("Helvetica");
        font.setBold(true);
        painter.setFont(font);
        painter.setPen(Qt::white);
        painter.drawText(QRect(1,1,63,63),Qt::AlignCenter,text);
        return icon;
    }

    void loadNetworkIcon(const QUrl &url, QNetworkAccessManager *manager)
    {
        NetworkImgLoader *loader = new NetworkImgLoader(url,manager);
        /* Since "this" is not QObject,connect signal need another way*/
        QObject::connect(loader,&NetworkImgLoader::dataReceived,
                        label_UserIcon,[this,loader](){
            this->loadUserIconFromData(loader->data());
            loader->deleteLater();
        });
    }

    void loadTextIcon(const QString &text)
    {
        icon_current = drawTextIcon(text);
        label_UserIcon->setPixmap(icon_current);
    }
}; // AccountWidgetActs
#endif // ACCOUNTWIDGETACTS_H
