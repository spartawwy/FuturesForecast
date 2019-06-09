/********************************************************************************
** Form generated from reading UI file 'traindlg.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRAINDLG_H
#define UI_TRAINDLG_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrainDlgForm
{
public:
    QLabel *label;
    QLabel *label_2;
    QDoubleSpinBox *dbspbBegCapital;
    QPushButton *pbtnBuy;
    QPushButton *pbtnSell;
    QPushButton *pbtnNextK;
    QPushButton *pbtnPreK;
    QLabel *label_3;
    QDoubleSpinBox *dbspbFeeOpen;
    QTableView *table_view_record;
    QLabel *label_5;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbtnStart;
    QPushButton *pbtnStop;
    QLineEdit *le_cur_capital;
    QLabel *label_6;
    QLabel *label_7;
    QLineEdit *le_long_pos;
    QLabel *label_8;
    QLabel *lab_assets;
    QPlainTextEdit *plain_te_record;
    QLabel *label_9;
    QDoubleSpinBox *dbspbFeeOpen_2;
    QLabel *lab_status;
    QDateTimeEdit *de_begin;
    QLabel *label_10;
    QLineEdit *le_short_pos;

    void setupUi(QWidget *TrainDlgForm)
    {
        if (TrainDlgForm->objectName().isEmpty())
            TrainDlgForm->setObjectName(QStringLiteral("TrainDlgForm"));
        TrainDlgForm->resize(626, 608);
        label = new QLabel(TrainDlgForm);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(40, 170, 61, 21));
        QFont font;
        font.setPointSize(10);
        label->setFont(font);
        label_2 = new QLabel(TrainDlgForm);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 20, 81, 21));
        label_2->setFont(font);
        dbspbBegCapital = new QDoubleSpinBox(TrainDlgForm);
        dbspbBegCapital->setObjectName(QStringLiteral("dbspbBegCapital"));
        dbspbBegCapital->setGeometry(QRect(110, 170, 111, 31));
        dbspbBegCapital->setMaximum(12000);
        dbspbBegCapital->setValue(12000);
        pbtnBuy = new QPushButton(TrainDlgForm);
        pbtnBuy->setObjectName(QStringLiteral("pbtnBuy"));
        pbtnBuy->setGeometry(QRect(40, 140, 75, 23));
        pbtnBuy->setFont(font);
        pbtnSell = new QPushButton(TrainDlgForm);
        pbtnSell->setObjectName(QStringLiteral("pbtnSell"));
        pbtnSell->setGeometry(QRect(150, 140, 75, 23));
        pbtnSell->setFont(font);
        pbtnNextK = new QPushButton(TrainDlgForm);
        pbtnNextK->setObjectName(QStringLiteral("pbtnNextK"));
        pbtnNextK->setGeometry(QRect(150, 100, 75, 23));
        pbtnNextK->setFont(font);
        pbtnPreK = new QPushButton(TrainDlgForm);
        pbtnPreK->setObjectName(QStringLiteral("pbtnPreK"));
        pbtnPreK->setGeometry(QRect(40, 100, 75, 23));
        pbtnPreK->setFont(font);
        label_3 = new QLabel(TrainDlgForm);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(240, 170, 71, 21));
        label_3->setFont(font);
        dbspbFeeOpen = new QDoubleSpinBox(TrainDlgForm);
        dbspbFeeOpen->setObjectName(QStringLiteral("dbspbFeeOpen"));
        dbspbFeeOpen->setGeometry(QRect(320, 170, 81, 31));
        dbspbFeeOpen->setDecimals(4);
        dbspbFeeOpen->setMaximum(500);
        dbspbFeeOpen->setSingleStep(100);
        dbspbFeeOpen->setValue(100);
        table_view_record = new QTableView(TrainDlgForm);
        table_view_record->setObjectName(QStringLiteral("table_view_record"));
        table_view_record->setGeometry(QRect(10, 560, 601, 20));
        label_5 = new QLabel(TrainDlgForm);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 290, 61, 21));
        label_5->setFont(font);
        layoutWidget = new QWidget(TrainDlgForm);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(50, 60, 158, 25));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pbtnStart = new QPushButton(layoutWidget);
        pbtnStart->setObjectName(QStringLiteral("pbtnStart"));
        pbtnStart->setFont(font);

        horizontalLayout->addWidget(pbtnStart);

        pbtnStop = new QPushButton(layoutWidget);
        pbtnStop->setObjectName(QStringLiteral("pbtnStop"));
        pbtnStop->setFont(font);

        horizontalLayout->addWidget(pbtnStop);

        le_cur_capital = new QLineEdit(TrainDlgForm);
        le_cur_capital->setObjectName(QStringLiteral("le_cur_capital"));
        le_cur_capital->setGeometry(QRect(110, 210, 111, 31));
        label_6 = new QLabel(TrainDlgForm);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(40, 210, 61, 21));
        label_6->setFont(font);
        label_7 = new QLabel(TrainDlgForm);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(40, 250, 61, 21));
        label_7->setFont(font);
        le_long_pos = new QLineEdit(TrainDlgForm);
        le_long_pos->setObjectName(QStringLiteral("le_long_pos"));
        le_long_pos->setGeometry(QRect(110, 250, 111, 31));
        label_8 = new QLabel(TrainDlgForm);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(280, 210, 41, 21));
        label_8->setFont(font);
        lab_assets = new QLabel(TrainDlgForm);
        lab_assets->setObjectName(QStringLiteral("lab_assets"));
        lab_assets->setGeometry(QRect(320, 210, 101, 21));
        lab_assets->setFont(font);
        plain_te_record = new QPlainTextEdit(TrainDlgForm);
        plain_te_record->setObjectName(QStringLiteral("plain_te_record"));
        plain_te_record->setGeometry(QRect(10, 310, 601, 251));
        label_9 = new QLabel(TrainDlgForm);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(430, 170, 71, 21));
        label_9->setFont(font);
        dbspbFeeOpen_2 = new QDoubleSpinBox(TrainDlgForm);
        dbspbFeeOpen_2->setObjectName(QStringLiteral("dbspbFeeOpen_2"));
        dbspbFeeOpen_2->setGeometry(QRect(510, 170, 81, 31));
        dbspbFeeOpen_2->setDecimals(4);
        dbspbFeeOpen_2->setMaximum(500);
        dbspbFeeOpen_2->setSingleStep(100);
        dbspbFeeOpen_2->setValue(300);
        lab_status = new QLabel(TrainDlgForm);
        lab_status->setObjectName(QStringLiteral("lab_status"));
        lab_status->setGeometry(QRect(10, 580, 601, 20));
        lab_status->setFont(font);
        de_begin = new QDateTimeEdit(TrainDlgForm);
        de_begin->setObjectName(QStringLiteral("de_begin"));
        de_begin->setGeometry(QRect(110, 20, 161, 31));
        QFont font1;
        font1.setPointSize(12);
        de_begin->setFont(font1);
        de_begin->setDate(QDate(2019, 6, 1));
        de_begin->setTime(QTime(21, 0, 0));
        label_10 = new QLabel(TrainDlgForm);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(250, 250, 61, 21));
        label_10->setFont(font);
        le_short_pos = new QLineEdit(TrainDlgForm);
        le_short_pos->setObjectName(QStringLiteral("le_short_pos"));
        le_short_pos->setGeometry(QRect(320, 250, 111, 31));

        retranslateUi(TrainDlgForm);

        QMetaObject::connectSlotsByName(TrainDlgForm);
    } // setupUi

    void retranslateUi(QWidget *TrainDlgForm)
    {
        TrainDlgForm->setWindowTitle(QApplication::translate("TrainDlgForm", "\350\256\255\347\273\203\346\250\241\345\274\217", 0));
        label->setText(QApplication::translate("TrainDlgForm", "\345\210\235\345\247\213\350\265\204\351\207\221:", 0));
        label_2->setText(QApplication::translate("TrainDlgForm", "\350\256\255\347\273\203\345\274\200\345\247\213\346\227\266\351\227\264:", 0));
        pbtnBuy->setText(QApplication::translate("TrainDlgForm", "\345\274\200", 0));
        pbtnSell->setText(QApplication::translate("TrainDlgForm", "\345\271\263", 0));
        pbtnNextK->setText(QApplication::translate("TrainDlgForm", "\344\270\213\344\270\200\346\240\271=>", 0));
        pbtnPreK->setText(QApplication::translate("TrainDlgForm", "<=\344\270\212\344\270\200\346\240\271", 0));
        label_3->setText(QApplication::translate("TrainDlgForm", "\345\274\200\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
        label_5->setText(QApplication::translate("TrainDlgForm", "\346\210\220\344\272\244\350\256\260\345\275\225", 0));
        pbtnStart->setText(QApplication::translate("TrainDlgForm", "\345\274\200\345\247\213", 0));
        pbtnStop->setText(QApplication::translate("TrainDlgForm", "\347\273\223\346\235\237", 0));
        label_6->setText(QApplication::translate("TrainDlgForm", "\345\217\257\347\224\250\350\265\204\351\207\221:", 0));
        label_7->setText(QApplication::translate("TrainDlgForm", "\345\244\232\345\244\264\345\244\264\345\257\270:", 0));
        label_8->setText(QApplication::translate("TrainDlgForm", "\346\235\203\347\233\212:", 0));
        lab_assets->setText(QApplication::translate("TrainDlgForm", "12000", 0));
        label_9->setText(QApplication::translate("TrainDlgForm", "\345\271\263\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
        lab_status->setText(QApplication::translate("TrainDlgForm", "status", 0));
        label_10->setText(QApplication::translate("TrainDlgForm", "\347\251\272\345\244\264\345\244\264\345\257\270:", 0));
        le_short_pos->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TrainDlgForm: public Ui_TrainDlgForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRAINDLG_H
