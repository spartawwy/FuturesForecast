/********************************************************************************
** Form generated from reading UI file 'mocktrade.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOCKTRADE_H
#define UI_MOCKTRADE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MockTradeForm
{
public:
    QPushButton *pbtnBuy;
    QPushButton *pbtnSell;
    QPushButton *pbtnCondition;
    QDoubleSpinBox *dbspbFeeOpen;
    QLabel *lab_assets;
    QLabel *label;
    QLabel *label_6;
    QLabel *label_3;
    QDoubleSpinBox *dbspbFeeOpen_2;
    QDoubleSpinBox *dbspbBegCapital;
    QLineEdit *le_cur_capital;
    QLabel *label_8;
    QLabel *label_9;
    QTableView *table_view_record;
    QLabel *lab_status;
    QPushButton *pbtnclose;
    QLineEdit *le_long_pos;

    void setupUi(QWidget *MockTradeForm)
    {
        if (MockTradeForm->objectName().isEmpty())
            MockTradeForm->setObjectName(QStringLiteral("MockTradeForm"));
        MockTradeForm->resize(594, 579);
        pbtnBuy = new QPushButton(MockTradeForm);
        pbtnBuy->setObjectName(QStringLiteral("pbtnBuy"));
        pbtnBuy->setGeometry(QRect(220, 120, 75, 23));
        QFont font;
        font.setPointSize(10);
        pbtnBuy->setFont(font);
        pbtnSell = new QPushButton(MockTradeForm);
        pbtnSell->setObjectName(QStringLiteral("pbtnSell"));
        pbtnSell->setGeometry(QRect(120, 120, 75, 23));
        pbtnSell->setFont(font);
        pbtnCondition = new QPushButton(MockTradeForm);
        pbtnCondition->setObjectName(QStringLiteral("pbtnCondition"));
        pbtnCondition->setGeometry(QRect(120, 160, 75, 23));
        pbtnCondition->setFont(font);
        dbspbFeeOpen = new QDoubleSpinBox(MockTradeForm);
        dbspbFeeOpen->setObjectName(QStringLiteral("dbspbFeeOpen"));
        dbspbFeeOpen->setGeometry(QRect(310, 30, 81, 31));
        dbspbFeeOpen->setDecimals(4);
        dbspbFeeOpen->setMaximum(500);
        dbspbFeeOpen->setSingleStep(100);
        dbspbFeeOpen->setValue(100);
        lab_assets = new QLabel(MockTradeForm);
        lab_assets->setObjectName(QStringLiteral("lab_assets"));
        lab_assets->setGeometry(QRect(310, 70, 101, 21));
        lab_assets->setFont(font);
        label = new QLabel(MockTradeForm);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 30, 61, 21));
        label->setFont(font);
        label_6 = new QLabel(MockTradeForm);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 70, 61, 21));
        label_6->setFont(font);
        label_3 = new QLabel(MockTradeForm);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(230, 30, 71, 21));
        label_3->setFont(font);
        dbspbFeeOpen_2 = new QDoubleSpinBox(MockTradeForm);
        dbspbFeeOpen_2->setObjectName(QStringLiteral("dbspbFeeOpen_2"));
        dbspbFeeOpen_2->setGeometry(QRect(500, 30, 81, 31));
        dbspbFeeOpen_2->setDecimals(4);
        dbspbFeeOpen_2->setMaximum(500);
        dbspbFeeOpen_2->setSingleStep(100);
        dbspbFeeOpen_2->setValue(300);
        dbspbBegCapital = new QDoubleSpinBox(MockTradeForm);
        dbspbBegCapital->setObjectName(QStringLiteral("dbspbBegCapital"));
        dbspbBegCapital->setGeometry(QRect(100, 30, 111, 31));
        dbspbBegCapital->setMaximum(12000);
        dbspbBegCapital->setValue(12000);
        le_cur_capital = new QLineEdit(MockTradeForm);
        le_cur_capital->setObjectName(QStringLiteral("le_cur_capital"));
        le_cur_capital->setGeometry(QRect(100, 70, 111, 31));
        label_8 = new QLabel(MockTradeForm);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(270, 70, 41, 21));
        label_8->setFont(font);
        label_9 = new QLabel(MockTradeForm);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(420, 30, 71, 21));
        label_9->setFont(font);
        table_view_record = new QTableView(MockTradeForm);
        table_view_record->setObjectName(QStringLiteral("table_view_record"));
        table_view_record->setGeometry(QRect(10, 190, 561, 341));
        lab_status = new QLabel(MockTradeForm);
        lab_status->setObjectName(QStringLiteral("lab_status"));
        lab_status->setGeometry(QRect(10, 540, 571, 21));
        lab_status->setFont(font);
        pbtnclose = new QPushButton(MockTradeForm);
        pbtnclose->setObjectName(QStringLiteral("pbtnclose"));
        pbtnclose->setGeometry(QRect(320, 120, 75, 23));
        pbtnclose->setFont(font);
        le_long_pos = new QLineEdit(MockTradeForm);
        le_long_pos->setObjectName(QStringLiteral("le_long_pos"));
        le_long_pos->setGeometry(QRect(30, 120, 71, 31));

        retranslateUi(MockTradeForm);

        QMetaObject::connectSlotsByName(MockTradeForm);
    } // setupUi

    void retranslateUi(QWidget *MockTradeForm)
    {
        MockTradeForm->setWindowTitle(QApplication::translate("MockTradeForm", "\346\250\241\346\213\237\344\272\244\346\230\223", 0));
        pbtnBuy->setText(QApplication::translate("MockTradeForm", "\344\271\260\345\205\245", 0));
        pbtnSell->setText(QApplication::translate("MockTradeForm", "\345\215\226\345\207\272", 0));
        pbtnCondition->setText(QApplication::translate("MockTradeForm", "\346\235\241\344\273\266\345\215\225", 0));
        lab_assets->setText(QApplication::translate("MockTradeForm", "12000", 0));
        label->setText(QApplication::translate("MockTradeForm", "\345\210\235\345\247\213\350\265\204\351\207\221:", 0));
        label_6->setText(QApplication::translate("MockTradeForm", "\345\217\257\347\224\250\350\265\204\351\207\221:", 0));
        label_3->setText(QApplication::translate("MockTradeForm", "\345\274\200\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
        label_8->setText(QApplication::translate("MockTradeForm", "\346\235\203\347\233\212:", 0));
        label_9->setText(QApplication::translate("MockTradeForm", "\345\271\263\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
        lab_status->setText(QApplication::translate("MockTradeForm", "status", 0));
        pbtnclose->setText(QApplication::translate("MockTradeForm", "\345\271\263\344\273\223", 0));
    } // retranslateUi

};

namespace Ui {
    class MockTradeForm: public Ui_MockTradeForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOCKTRADE_H
