/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_7;
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_21;
    QPushButton *pushButton_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_20;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_6;
    QLabel *label_10;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout;
    QLabel *spectro;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_11;
    QVBoxLayout *verticalLayout_3;
    QLabel *colorbar_label1;
    QLabel *colorbar_label2;
    QLabel *colorbar_label3;
    QLabel *colorbar_label4;
    QLabel *colorbar_label5;
    QLabel *colorbar_label6;
    QLabel *colorbar_label7;
    QLabel *colorbar_label8;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1291, 1170);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setFocusPolicy(Qt::NoFocus);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout_7 = new QHBoxLayout(centralwidget);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(10);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(60, -1, 600, 0);
        label_21 = new QLabel(centralwidget);
        label_21->setObjectName(QStringLiteral("label_21"));
        QFont font;
        font.setPointSize(18);
        label_21->setFont(font);

        horizontalLayout_4->addWidget(label_21);

        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setEnabled(false);
        pushButton_2->setStyleSheet(QStringLiteral("background-color:red; border: none;"));

        horizontalLayout_4->addWidget(pushButton_2);

        horizontalLayout_4->setStretch(1, 2);

        verticalLayout_4->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(60, -1, 600, -1);
        label_20 = new QLabel(centralwidget);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setFont(font);

        horizontalLayout_5->addWidget(label_20);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setEnabled(false);
        pushButton->setStyleSheet(QStringLiteral("background-color:green; border: none;"));

        horizontalLayout_5->addWidget(pushButton);

        horizontalLayout_5->setStretch(1, 2);

        verticalLayout_4->addLayout(horizontalLayout_5);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(-1, 0, -1, 25);
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        QFont font1;
        font1.setPointSize(12);
        label_6->setFont(font1);
        label_6->setLayoutDirection(Qt::RightToLeft);
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);

        verticalLayout_2->addWidget(label_6);

        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setFont(font1);
        label_10->setLayoutDirection(Qt::RightToLeft);
        label_10->setAlignment(Qt::AlignBottom|Qt::AlignRight|Qt::AlignTrailing);

        verticalLayout_2->addWidget(label_10);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        verticalLayout_2->setStretch(0, 2);
        verticalLayout_2->setStretch(1, 1);

        horizontalLayout_2->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        spectro = new QLabel(centralwidget);
        spectro->setObjectName(QStringLiteral("spectro"));
        spectro->setPixmap(QPixmap(QString::fromUtf8("../../Downloads/waiting_spectro.png")));
        spectro->setScaledContents(true);

        verticalLayout->addWidget(spectro);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        QFont font2;
        font2.setPointSize(11);
        label->setFont(font2);

        horizontalLayout->addWidget(label);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font2);

        horizontalLayout->addWidget(label_2);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font2);

        horizontalLayout->addWidget(label_3);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font2);

        horizontalLayout->addWidget(label_4);

        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setFont(font2);

        horizontalLayout->addWidget(label_5);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        horizontalLayout_2->setStretch(0, 2);
        horizontalLayout_2->setStretch(1, 40);

        verticalLayout_4->addLayout(horizontalLayout_2);


        horizontalLayout_6->addLayout(verticalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_11 = new QLabel(centralwidget);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setPixmap(QPixmap(QString::fromUtf8("../../Downloads/colorbar.jpg")));
        label_11->setScaledContents(true);

        horizontalLayout_3->addWidget(label_11);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        colorbar_label1 = new QLabel(centralwidget);
        colorbar_label1->setObjectName(QStringLiteral("colorbar_label1"));
        QFont font3;
        font3.setPointSize(10);
        colorbar_label1->setFont(font3);

        verticalLayout_3->addWidget(colorbar_label1);

        colorbar_label2 = new QLabel(centralwidget);
        colorbar_label2->setObjectName(QStringLiteral("colorbar_label2"));
        colorbar_label2->setFont(font3);

        verticalLayout_3->addWidget(colorbar_label2);

        colorbar_label3 = new QLabel(centralwidget);
        colorbar_label3->setObjectName(QStringLiteral("colorbar_label3"));
        colorbar_label3->setFont(font3);

        verticalLayout_3->addWidget(colorbar_label3);

        colorbar_label4 = new QLabel(centralwidget);
        colorbar_label4->setObjectName(QStringLiteral("colorbar_label4"));
        colorbar_label4->setFont(font3);

        verticalLayout_3->addWidget(colorbar_label4);

        colorbar_label5 = new QLabel(centralwidget);
        colorbar_label5->setObjectName(QStringLiteral("colorbar_label5"));
        colorbar_label5->setFont(font3);

        verticalLayout_3->addWidget(colorbar_label5);

        colorbar_label6 = new QLabel(centralwidget);
        colorbar_label6->setObjectName(QStringLiteral("colorbar_label6"));
        colorbar_label6->setFont(font3);

        verticalLayout_3->addWidget(colorbar_label6);

        colorbar_label7 = new QLabel(centralwidget);
        colorbar_label7->setObjectName(QStringLiteral("colorbar_label7"));
        colorbar_label7->setFont(font3);

        verticalLayout_3->addWidget(colorbar_label7);

        colorbar_label8 = new QLabel(centralwidget);
        colorbar_label8->setObjectName(QStringLiteral("colorbar_label8"));
        colorbar_label8->setFont(font3);

        verticalLayout_3->addWidget(colorbar_label8);


        horizontalLayout_3->addLayout(verticalLayout_3);


        horizontalLayout_6->addLayout(horizontalLayout_3);


        horizontalLayout_7->addLayout(horizontalLayout_6);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1291, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        label_21->setText(QApplication::translate("MainWindow", "AM:", Q_NULLPTR));
        pushButton_2->setText(QString());
        label_20->setText(QApplication::translate("MainWindow", "FM:", Q_NULLPTR));
        pushButton->setText(QString());
        label_6->setText(QApplication::translate("MainWindow", "0", Q_NULLPTR));
        label_10->setText(QApplication::translate("MainWindow", "0", Q_NULLPTR));
        spectro->setText(QString());
        label->setText(QApplication::translate("MainWindow", "0", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "0.26", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "0.53", Q_NULLPTR));
        label_4->setText(QApplication::translate("MainWindow", "0.77", Q_NULLPTR));
        label_5->setText(QApplication::translate("MainWindow", "1.05(MHz)", Q_NULLPTR));
        label_11->setText(QString());
        colorbar_label1->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        colorbar_label2->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        colorbar_label3->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        colorbar_label4->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        colorbar_label5->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        colorbar_label6->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        colorbar_label7->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        colorbar_label8->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
