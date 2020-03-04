/********************************************************************************
** Form generated from reading UI file 'uidownloaddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UIDOWNLOADDIALOG_H
#define UI_UIDOWNLOADDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UIDownloadDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label_FileID;
    QLineEdit *lineEdit_FileID;
    QLabel *label_DownloadPath;
    QLineEdit *lineEdit_DownloadPath;
    QPushButton *pushButton_DownloadPath;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *UIDownloadDialog)
    {
        if (UIDownloadDialog->objectName().isEmpty())
            UIDownloadDialog->setObjectName(QString::fromUtf8("UIDownloadDialog"));
        UIDownloadDialog->resize(640, 200);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(UIDownloadDialog->sizePolicy().hasHeightForWidth());
        UIDownloadDialog->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(UIDownloadDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_FileID = new QLabel(UIDownloadDialog);
        label_FileID->setObjectName(QString::fromUtf8("label_FileID"));

        gridLayout->addWidget(label_FileID, 0, 0, 1, 1);

        lineEdit_FileID = new QLineEdit(UIDownloadDialog);
        lineEdit_FileID->setObjectName(QString::fromUtf8("lineEdit_FileID"));

        gridLayout->addWidget(lineEdit_FileID, 0, 1, 1, 1);

        label_DownloadPath = new QLabel(UIDownloadDialog);
        label_DownloadPath->setObjectName(QString::fromUtf8("label_DownloadPath"));

        gridLayout->addWidget(label_DownloadPath, 1, 0, 1, 1);

        lineEdit_DownloadPath = new QLineEdit(UIDownloadDialog);
        lineEdit_DownloadPath->setObjectName(QString::fromUtf8("lineEdit_DownloadPath"));

        gridLayout->addWidget(lineEdit_DownloadPath, 1, 1, 1, 1);

        pushButton_DownloadPath = new QPushButton(UIDownloadDialog);
        pushButton_DownloadPath->setObjectName(QString::fromUtf8("pushButton_DownloadPath"));

        gridLayout->addWidget(pushButton_DownloadPath, 1, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(UIDownloadDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(UIDownloadDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), UIDownloadDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), UIDownloadDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(UIDownloadDialog);
    } // setupUi

    void retranslateUi(QDialog *UIDownloadDialog)
    {
        UIDownloadDialog->setWindowTitle(QCoreApplication::translate("UIDownloadDialog", "Dialog", nullptr));
#if QT_CONFIG(tooltip)
        UIDownloadDialog->setToolTip(QCoreApplication::translate("UIDownloadDialog", "<html><head/><body><p>Input Google Drive fileID ,then select your download file to download google drive file.</p><p>To get fileID,open your borwser and find the file,Right-click the file name and select <span style=\" font-weight:600;\">Get shareable link</span>. The last part of the link is the file ID.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
        UIDownloadDialog->setWhatsThis(QCoreApplication::translate("UIDownloadDialog", "<html><head/><body><p>Input Google Drive <span style=\" font-weight:600;\">fileID</span> ,then select your <span style=\" font-weight:600;\">download file</span> to download google drive file.</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        label_FileID->setText(QCoreApplication::translate("UIDownloadDialog", "Google Drive FileID", nullptr));
#if QT_CONFIG(whatsthis)
        lineEdit_FileID->setWhatsThis(QCoreApplication::translate("UIDownloadDialog", "<html><head/><body><p>To get fileID,open your borwser and login google drive ,select your file Right-click the file name and select <span style=\" font-weight:600;\">Get shareable link</span>. The last part of the link is the file ID.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        label_DownloadPath->setText(QCoreApplication::translate("UIDownloadDialog", "Download Path", nullptr));
#if QT_CONFIG(whatsthis)
        lineEdit_DownloadPath->setWhatsThis(QCoreApplication::translate("UIDownloadDialog", "<html><head/><body><p>The file name and folder path</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        pushButton_DownloadPath->setText(QCoreApplication::translate("UIDownloadDialog", "Browse", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UIDownloadDialog: public Ui_UIDownloadDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UIDOWNLOADDIALOG_H
