/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLogin_account;
    QAction *actionAbout;
    QAction *actionDownload_file;
    QAction *actionSimple_Upload;
    QAction *actionMultipart_Upload;
    QAction *actionResumable_Upload;
    QAction *action_Logout_Account;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QPlainTextEdit *plainTextEdit;
    QMenuBar *menuBar;
    QMenu *menuAcount;
    QMenu *menuFile;
    QMenu *menuUpload_file;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(630, 495);
        actionLogin_account = new QAction(MainWindow);
        actionLogin_account->setObjectName(QString::fromUtf8("actionLogin_account"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAbout->setEnabled(false);
        actionDownload_file = new QAction(MainWindow);
        actionDownload_file->setObjectName(QString::fromUtf8("actionDownload_file"));
        actionDownload_file->setEnabled(false);
        actionSimple_Upload = new QAction(MainWindow);
        actionSimple_Upload->setObjectName(QString::fromUtf8("actionSimple_Upload"));
        actionMultipart_Upload = new QAction(MainWindow);
        actionMultipart_Upload->setObjectName(QString::fromUtf8("actionMultipart_Upload"));
        actionResumable_Upload = new QAction(MainWindow);
        actionResumable_Upload->setObjectName(QString::fromUtf8("actionResumable_Upload"));
        action_Logout_Account = new QAction(MainWindow);
        action_Logout_Account->setObjectName(QString::fromUtf8("action_Logout_Account"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        plainTextEdit = new QPlainTextEdit(centralWidget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 630, 25));
        menuAcount = new QMenu(menuBar);
        menuAcount->setObjectName(QString::fromUtf8("menuAcount"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuFile->setEnabled(true);
        menuUpload_file = new QMenu(menuFile);
        menuUpload_file->setObjectName(QString::fromUtf8("menuUpload_file"));
        menuUpload_file->setEnabled(false);
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuAcount->menuAction());
        menuBar->addAction(menuFile->menuAction());
        menuAcount->addAction(actionLogin_account);
        menuAcount->addAction(action_Logout_Account);
        menuAcount->addAction(actionAbout);
        menuFile->addAction(menuUpload_file->menuAction());
        menuFile->addAction(actionDownload_file);
        menuUpload_file->addAction(actionSimple_Upload);
        menuUpload_file->addAction(actionMultipart_Upload);
        menuUpload_file->addAction(actionResumable_Upload);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionLogin_account->setText(QCoreApplication::translate("MainWindow", "Login account", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About...", nullptr));
        actionDownload_file->setText(QCoreApplication::translate("MainWindow", "Download file", nullptr));
        actionSimple_Upload->setText(QCoreApplication::translate("MainWindow", "Simple Upload", nullptr));
        actionMultipart_Upload->setText(QCoreApplication::translate("MainWindow", "Multipart Upload", nullptr));
        actionResumable_Upload->setText(QCoreApplication::translate("MainWindow", "Resumable Upload", nullptr));
        action_Logout_Account->setText(QCoreApplication::translate("MainWindow", "&Logout Account", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Account:", nullptr));
        menuAcount->setTitle(QCoreApplication::translate("MainWindow", "Account", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuUpload_file->setTitle(QCoreApplication::translate("MainWindow", "Upload file", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
