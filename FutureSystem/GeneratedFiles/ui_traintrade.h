/********************************************************************************
** Form generated from reading UI file 'traintrade.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRAINTRADE_H
#define UI_TRAINTRADE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrainTradeForm
{
public:
    QLabel *lab_price;
    QLineEdit *le_price;
    QLineEdit *le_qty;
    QLabel *lab_qty;
    QLabel *lab_capital_ava;
    QLineEdit *le_capital_ava;
    QLabel *label_reason;
    QTextEdit *textEdit_reason;
    QPushButton *pbtn_close;
    QPushButton *pbt_trade;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbt_all;
    QPushButton *pbtn_qty_half;
    QPushButton *pbtn_qty_one_third;
    QPushButton *pbtn_qty_one_fifth;
    QLabel *lab_status;
    QLabel *lab_qty_ava_2;
    QGroupBox *groupBox;
    QRadioButton *radioBtn_long;
    QRadioButton *radioBtn_short;
    QGroupBox *groupBox_2;
    QRadioButton *radioBtn_open;
    QRadioButton *radioBtn_close;
    QLabel *lab_qty_ava_3;
    QLabel *lab_qty_2;
    QLineEdit *le_qty_ava;
    QGroupBox *groupBox_3;
    QLabel *lab_price_2;
    QLineEdit *le_stop_profit;
    QLabel *lab_price_3;
    QLineEdit *le_stop_loss;

    void setupUi(QWidget *TrainTradeForm)
    {
        if (TrainTradeForm->objectName().isEmpty())
            TrainTradeForm->setObjectName(QStringLiteral("TrainTradeForm"));
        TrainTradeForm->setEnabled(true);
        TrainTradeForm->resize(484, 475);
        lab_price = new QLabel(TrainTradeForm);
        lab_price->setObjectName(QStringLiteral("lab_price"));
        lab_price->setGeometry(QRect(80, 10, 51, 31));
        le_price = new QLineEdit(TrainTradeForm);
        le_price->setObjectName(QStringLiteral("le_price"));
        le_price->setGeometry(QRect(140, 20, 113, 20));
        le_qty = new QLineEdit(TrainTradeForm);
        le_qty->setObjectName(QStringLiteral("le_qty"));
        le_qty->setGeometry(QRect(140, 130, 113, 20));
        lab_qty = new QLabel(TrainTradeForm);
        lab_qty->setObjectName(QStringLiteral("lab_qty"));
        lab_qty->setGeometry(QRect(80, 120, 51, 31));
        lab_capital_ava = new QLabel(TrainTradeForm);
        lab_capital_ava->setObjectName(QStringLiteral("lab_capital_ava"));
        lab_capital_ava->setGeometry(QRect(60, 180, 51, 31));
        le_capital_ava = new QLineEdit(TrainTradeForm);
        le_capital_ava->setObjectName(QStringLiteral("le_capital_ava"));
        le_capital_ava->setGeometry(QRect(140, 190, 113, 20));
        label_reason = new QLabel(TrainTradeForm);
        label_reason->setObjectName(QStringLiteral("label_reason"));
        label_reason->setGeometry(QRect(50, 310, 51, 31));
        textEdit_reason = new QTextEdit(TrainTradeForm);
        textEdit_reason->setObjectName(QStringLiteral("textEdit_reason"));
        textEdit_reason->setGeometry(QRect(110, 310, 321, 101));
        pbtn_close = new QPushButton(TrainTradeForm);
        pbtn_close->setObjectName(QStringLiteral("pbtn_close"));
        pbtn_close->setGeometry(QRect(270, 420, 75, 23));
        pbt_trade = new QPushButton(TrainTradeForm);
        pbt_trade->setObjectName(QStringLiteral("pbt_trade"));
        pbt_trade->setGeometry(QRect(189, 420, 75, 23));
        layoutWidget = new QWidget(TrainTradeForm);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 270, 401, 25));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pbt_all = new QPushButton(layoutWidget);
        pbt_all->setObjectName(QStringLiteral("pbt_all"));

        horizontalLayout->addWidget(pbt_all);

        pbtn_qty_half = new QPushButton(layoutWidget);
        pbtn_qty_half->setObjectName(QStringLiteral("pbtn_qty_half"));

        horizontalLayout->addWidget(pbtn_qty_half);

        pbtn_qty_one_third = new QPushButton(layoutWidget);
        pbtn_qty_one_third->setObjectName(QStringLiteral("pbtn_qty_one_third"));

        horizontalLayout->addWidget(pbtn_qty_one_third);

        pbtn_qty_one_fifth = new QPushButton(layoutWidget);
        pbtn_qty_one_fifth->setObjectName(QStringLiteral("pbtn_qty_one_fifth"));

        horizontalLayout->addWidget(pbtn_qty_one_fifth);

        lab_status = new QLabel(TrainTradeForm);
        lab_status->setObjectName(QStringLiteral("lab_status"));
        lab_status->setGeometry(QRect(0, 450, 481, 20));
        QFont font;
        font.setPointSize(10);
        lab_status->setFont(font);
        lab_qty_ava_2 = new QLabel(TrainTradeForm);
        lab_qty_ava_2->setObjectName(QStringLiteral("lab_qty_ava_2"));
        lab_qty_ava_2->setGeometry(QRect(80, 50, 31, 31));
        groupBox = new QGroupBox(TrainTradeForm);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(140, 50, 111, 35));
        radioBtn_long = new QRadioButton(groupBox);
        radioBtn_long->setObjectName(QStringLiteral("radioBtn_long"));
        radioBtn_long->setGeometry(QRect(10, 10, 41, 16));
        radioBtn_long->setChecked(true);
        radioBtn_short = new QRadioButton(groupBox);
        radioBtn_short->setObjectName(QStringLiteral("radioBtn_short"));
        radioBtn_short->setGeometry(QRect(60, 10, 41, 16));
        groupBox_2 = new QGroupBox(TrainTradeForm);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setEnabled(true);
        groupBox_2->setGeometry(QRect(140, 90, 111, 35));
        radioBtn_open = new QRadioButton(groupBox_2);
        radioBtn_open->setObjectName(QStringLiteral("radioBtn_open"));
        radioBtn_open->setEnabled(false);
        radioBtn_open->setGeometry(QRect(10, 10, 41, 16));
        radioBtn_open->setChecked(true);
        radioBtn_close = new QRadioButton(groupBox_2);
        radioBtn_close->setObjectName(QStringLiteral("radioBtn_close"));
        radioBtn_close->setEnabled(false);
        radioBtn_close->setGeometry(QRect(60, 10, 41, 16));
        lab_qty_ava_3 = new QLabel(TrainTradeForm);
        lab_qty_ava_3->setObjectName(QStringLiteral("lab_qty_ava_3"));
        lab_qty_ava_3->setGeometry(QRect(80, 90, 31, 31));
        lab_qty_2 = new QLabel(TrainTradeForm);
        lab_qty_2->setObjectName(QStringLiteral("lab_qty_2"));
        lab_qty_2->setGeometry(QRect(60, 150, 51, 31));
        le_qty_ava = new QLineEdit(TrainTradeForm);
        le_qty_ava->setObjectName(QStringLiteral("le_qty_ava"));
        le_qty_ava->setEnabled(false);
        le_qty_ava->setGeometry(QRect(140, 160, 113, 20));
        groupBox_3 = new QGroupBox(TrainTradeForm);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(280, 10, 161, 71));
        lab_price_2 = new QLabel(groupBox_3);
        lab_price_2->setObjectName(QStringLiteral("lab_price_2"));
        lab_price_2->setGeometry(QRect(10, 0, 31, 31));
        le_stop_profit = new QLineEdit(groupBox_3);
        le_stop_profit->setObjectName(QStringLiteral("le_stop_profit"));
        le_stop_profit->setGeometry(QRect(40, 10, 113, 20));
        lab_price_3 = new QLabel(groupBox_3);
        lab_price_3->setObjectName(QStringLiteral("lab_price_3"));
        lab_price_3->setGeometry(QRect(10, 30, 31, 31));
        le_stop_loss = new QLineEdit(groupBox_3);
        le_stop_loss->setObjectName(QStringLiteral("le_stop_loss"));
        le_stop_loss->setGeometry(QRect(40, 40, 113, 20));
        le_stop_profit->raise();
        lab_price_2->raise();
        lab_price_2->raise();
        le_stop_profit->raise();
        lab_price_3->raise();
        le_stop_loss->raise();
        lab_price->raise();
        le_price->raise();
        le_qty->raise();
        lab_qty->raise();
        lab_capital_ava->raise();
        le_capital_ava->raise();
        label_reason->raise();
        textEdit_reason->raise();
        pbtn_close->raise();
        pbt_trade->raise();
        layoutWidget->raise();
        lab_status->raise();
        lab_qty_ava_2->raise();
        groupBox->raise();
        groupBox_2->raise();
        lab_qty_ava_3->raise();
        lab_qty_2->raise();
        le_qty_ava->raise();
        groupBox_3->raise();

        retranslateUi(TrainTradeForm);

        QMetaObject::connectSlotsByName(TrainTradeForm);
    } // setupUi

    void retranslateUi(QWidget *TrainTradeForm)
    {
        TrainTradeForm->setWindowTitle(QApplication::translate("TrainTradeForm", "Form", 0));
        lab_price->setText(QApplication::translate("TrainTradeForm", "\344\273\267\346\240\274:", 0));
        lab_qty->setText(QApplication::translate("TrainTradeForm", "\346\225\260\351\207\217:", 0));
        lab_capital_ava->setText(QApplication::translate("TrainTradeForm", "\345\217\257\347\224\250\350\265\204\351\207\221:", 0));
        label_reason->setText(QApplication::translate("TrainTradeForm", "\344\272\244\346\230\223\347\220\206\347\224\261:", 0));
        pbtn_close->setText(QApplication::translate("TrainTradeForm", "\345\205\263\351\227\255", 0));
        pbt_trade->setText(QApplication::translate("TrainTradeForm", "\344\271\260\345\205\245", 0));
        pbt_all->setText(QApplication::translate("TrainTradeForm", "\345\205\250\351\207\217", 0));
        pbtn_qty_half->setText(QApplication::translate("TrainTradeForm", "1/2", 0));
        pbtn_qty_one_third->setText(QApplication::translate("TrainTradeForm", "1/3", 0));
        pbtn_qty_one_fifth->setText(QApplication::translate("TrainTradeForm", "1/5", 0));
        lab_status->setText(QApplication::translate("TrainTradeForm", "status", 0));
        lab_qty_ava_2->setText(QApplication::translate("TrainTradeForm", "\345\244\232\347\251\272:", 0));
        groupBox->setTitle(QString());
        radioBtn_long->setText(QApplication::translate("TrainTradeForm", "\345\244\232", 0));
        radioBtn_short->setText(QApplication::translate("TrainTradeForm", "\347\251\272", 0));
        groupBox_2->setTitle(QString());
        radioBtn_open->setText(QApplication::translate("TrainTradeForm", "\345\274\200", 0));
        radioBtn_close->setText(QApplication::translate("TrainTradeForm", "\345\271\263", 0));
        lab_qty_ava_3->setText(QApplication::translate("TrainTradeForm", "\345\274\200\345\271\263:", 0));
        lab_qty_2->setText(QApplication::translate("TrainTradeForm", "\345\217\257\347\224\250\346\225\260\351\207\217:", 0));
        groupBox_3->setTitle(QString());
        lab_price_2->setText(QApplication::translate("TrainTradeForm", "\346\255\242\350\265\242:", 0));
        lab_price_3->setText(QApplication::translate("TrainTradeForm", "\346\255\242\346\215\237:", 0));
    } // retranslateUi

};

namespace Ui {
    class TrainTradeForm: public Ui_TrainTradeForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRAINTRADE_H
