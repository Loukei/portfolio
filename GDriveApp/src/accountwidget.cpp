#include "accountwidget.h"
#include "ui_accountwidget.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

AccountWidget::AccountWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountWidget)
{
    ui->setupUi(this);
    m_defaultUserIcon = QPixmap(":/Icon/Icon/iconfinder_user_close_103746.png");
    m_defaultUserIcon.scaled(QSize(64,64));
    ui->label_UserIcon->setPixmap(m_defaultUserIcon);
    ui->label_DisplayName->hide();
    ui->label_EmailAddress->hide();

    m_manager = new QNetworkAccessManager(this);
}

AccountWidget::~AccountWidget()
{
    delete ui;
}

void AccountWidget::setUserIcon(const QPixmap &icon)
{
    return ui->label_UserIcon->setPixmap(icon);
}

const QPixmap *AccountWidget::userIcon() const
{
    return ui->label_UserIcon->pixmap();
}

void AccountWidget::setDisplayName(const QString &displayname)
{
    QString oldname = ui->label_DisplayName->text();
    if(!displayname.isEmpty() && (oldname != displayname)){
        ui->label_DisplayName->show();
        return ui->label_DisplayName->setText(displayname);
    }
}

QString AccountWidget::displayName() const
{
    return ui->label_DisplayName->text();
}

void AccountWidget::setEmailAddress(const QString &emailaddress)
{
    QString oldaddress = ui->label_EmailAddress->text();
    if(!oldaddress.isEmpty() && (oldaddress != emailaddress)){
        ui->label_EmailAddress->show();
        return ui->label_EmailAddress->setText(emailaddress);
    }
}

QString AccountWidget::emailAddress() const
{
    return ui->label_EmailAddress->text();
}

void AccountWidget::loadNetworkImage(const QUrl &url)
{
    if(!url.isEmpty())
        requestUserIcon(url);
    else
        qDebug() << Q_FUNC_INFO << "Empty photo url";
}

void AccountWidget::resetUI()
{
    ui->label_UserIcon->setPixmap(m_defaultUserIcon);
    ui->label_DisplayName->clear();
    ui->label_DisplayName->hide();
    ui->label_EmailAddress->clear();
    ui->label_EmailAddress->hide();
}

QPixmap AccountWidget::DrawImage(const QString &text)
{
    /*
    QImage image(QSize(400,300),QImage::Format_RGB32);
    QPainter painter(&image);
    painter.setBrush(QBrush(Qt::green));
    painter.fillRect(QRectF(0,0,400,300),Qt::green);
    painter.fillRect(QRectF(100,100,200,100),Qt::white);
    painter.setPen(QPen(Qt::black));
    painter.drawText(QRect(100,100,200,100),"Text you want to draw...");
    image.save("testImage.png");
    // https://stackoverflow.com/questions/41904610/how-to-create-a-simple-image-qimage-with-text-and-colors-in-qt-and-save-it-as
    */
    return QPixmap();
}

void AccountWidget::requestUserIcon(const QUrl &url)
{
    QNetworkReply *reply = m_manager->get(QNetworkRequest(url));
    connect(reply,&QNetworkReply::finished,
            this,&AccountWidget::on_requestUserIcon_finished);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
         [=](QNetworkReply::NetworkError code){
        qDebug() << Q_FUNC_INFO << "Error code" << code;
    });
}

void AccountWidget::on_requestUserIcon_finished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error() == QNetworkReply::NetworkError::NoError){
        m_icon.loadFromData(reply->readAll());
        setUserIcon(m_icon);
    }
    reply->deleteLater();
}
