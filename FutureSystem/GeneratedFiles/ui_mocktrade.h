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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MockTradeForm
{
public:
    QPushButton *pbtnBuy;
    QPushButton *pbtnSell;
    QPushButton *pbtnCondition;
    QLabel *lab_assets;
    QLabel *label_6;
    QLineEdit *le_cur_capital;
    QLabel *label_8;
    QLabel *lab_status;
    QPushButton *pbtnClose;
    QLineEdit *le_qty;
    QLabel *label_2;
    QLabel *label_4;
    QLabel *le_sell_price;
    QLabel *le_buy_price;
    QLabel *le_sell_vol;
    QLabel *le_buy_vol;
    QTabWidget *tabPosRecord;
    QWidget *tab_position;
    QTableView *table_view_record;
    QWidget *tab_2;
    QPlainTextEdit *pe_trade_records;
    QGroupBox *groupBox;
    QLabel *label_3;
    QDoubleSpinBox *dbspbFeeOpen;
    QDoubleSpinBox *dbspbFeeOpen_2;
    QDoubleSpinBox *dbspbBegCapital;
    QLabel *label_9;
    QLabel *label;
    QPushButton *pbtnInit;
    QLabel *le_cur_price;
    QLabel *label_5;
    QLabel *le_cur_vol;

    void setupUi(QWidget *MockTradeForm)
    {
        if (MockTradeForm->objectName().isEmpty())
            MockTradeForm->setObjectName(QStringLiteral("MockTradeForm"));
        MockTradeForm->resize(637, 579);
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
        lab_assets = new QLabel(MockTradeForm);
        lab_assets->setObjectName(QStringLiteral("lab_assets"));
        lab_assets->setGeometry(QRect(310, 80, 101, 21));
        lab_assets->setFont(font);
        label_6 = new QLabel(MockTradeForm);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 80, 61, 21));
        label_6->setFont(font);
        le_cur_capital = new QLineEdit(MockTradeForm);
        le_cur_capital->setObjectName(QStringLiteral("le_cur_capital"));
        le_cur_capital->setGeometry(QRect(100, 80, 111, 31));
        label_8 = new QLabel(MockTradeForm);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(270, 80, 41, 21));
        label_8->setFont(font);
        lab_status = new QLabel(MockTradeForm);
        lab_status->setObjectName(QStringLiteral("lab_status"));
        lab_status->setGeometry(QRect(10, 540, 571, 21));
        lab_status->setFont(font);
        pbtnClose = new QPushButton(MockTradeForm);
        pbtnClose->setObjectName(QStringLiteral("pbtnClose"));
        pbtnClose->setGeometry(QRect(320, 120, 75, 23));
        pbtnClose->setFont(font);
        le_qty = new QLineEdit(MockTradeForm);
        le_qty->setObjectName(QStringLiteral("le_qty"));
        le_qty->setGeometry(QRect(30, 120, 71, 31));
        label_2 = new QLabel(MockTradeForm);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(420, 120, 21, 16));
        label_4 = new QLabel(MockTradeForm);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(420, 180, 21, 16));
        le_sell_price = new QLabel(MockTradeForm);
        le_sell_price->setObjectName(QStringLiteral("le_sell_price"));
        le_sell_price->setGeometry(QRect(460, 120, 46, 13));
        le_buy_price = new QLabel(MockTradeForm);
        le_buy_price->setObjectName(QStringLiteral("le_buy_price"));
        le_buy_price->setGeometry(QRect(460, 180, 46, 13));
        le_sell_vol = new QLabel(MockTradeForm);
        le_sell_vol->setObjectName(QStringLiteral("le_sell_vol"));
        le_sell_vol->setGeometry(QRect(520, 120, 46, 13));
        le_buy_vol = new QLabel(MockTradeForm);
        le_buy_vol->setObjectName(QStringLiteral("le_buy_vol"));
        le_buy_vol->setGeometry(QRect(520, 180, 46, 13));
        tabPosRecord = new QTabWidget(MockTradeForm);
        tabPosRecord->setObjectName(QStringLiteral("tabPosRecord"));
        tabPosRecord->setGeometry(QRect(0, 200, 631, 331));
        tab_position = new QWidget();
        tab_position->setObjectName(QStringLiteral("tab_position"));
        table_view_record = new QTableView(tab_position);
        table_view_record->setObjectName(QStringLiteral("table_view_record"));
        table_view_record->setGeometry(QRect(10, 10, 611, 291));
        tabPosRecord->addTab(tab_position, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        pe_trade_records = new QPlainTextEdit(tab_2);
        pe_trade_records->setObjectName(QStringLiteral("pe_trade_records"));
        pe_trade_records->setGeometry(QRect(0, 10, 621, 291));
        tabPosRecord->addTab(tab_2, QString());
        groupBox = new QGroupBox(MockTradeForm);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 0, 621, 71));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(200, 20, 71, 21));
        label_3->setFont(font);
        dbspbFeeOpen = new QDoubleSpinBox(groupBox);
        dbspbFeeOpen->setObjectName(QStringLiteral("dbspbFeeOpen"));
        dbspbFeeOpen->setGeometry(QRect(280, 20, 81, 31));
        dbspbFeeOpen->setDecimals(4);
        dbspbFeeOpen->setMaximum(500);
        dbspbFeeOpen->setSingleStep(100);
        dbspbFeeOpen->setValue(100);
        dbspbFeeOpen_2 = new QDoubleSpinBox(groupBox);
        dbspbFeeOpen_2->setObjectName(QStringLiteral("dbspbFeeOpen_2"));
        dbspbFeeOpen_2->setGeometry(QRect(450, 20, 81, 31));
        dbspbFeeOpen_2->setDecimals(4);
        dbspbFeeOpen_2->setMaximum(500);
        dbspbFeeOpen_2->setSingleStep(100);
        dbspbFeeOpen_2->setValue(300);
        dbspbBegCapital = new QDoubleSpinBox(groupBox);
        dbspbBegCapital->setObjectName(QStringLiteral("dbspbBegCapital"));
        dbspbBegCapital->setGeometry(QRect(80, 20, 111, 31));
        dbspbBegCapital->setMaximum(1e+07);
        dbspbBegCapital->setValue(12000);
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(370, 20, 71, 21));
        label_9->setFont(font);
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 61, 21));
        label->setFont(font);
        pbtnInit = new QPushButton(groupBox);
        pbtnInit->setObjectName(QStringLiteral("pbtnInit"));
        pbtnInit->setGeometry(QRect(540, 25, 71, 23));
        pbtnInit->setFont(font);
        le_cur_price = new QLabel(MockTradeForm);
        le_cur_price->setObjectName(QStringLiteral("le_cur_price"));
        le_cur_price->setGeometry(QRect(460, 150, 46, 13));
        label_5 = new QLabel(MockTradeForm);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(420, 150, 31, 20));
        le_cur_vol = new QLabel(MockTradeForm);
        le_cur_vol->setObjectName(QStringLiteral("le_cur_vol"));
        le_cur_vol->setGeometry(QRect(520, 150, 46, 13));

        retranslateUi(MockTradeForm);

        tabPosRecord->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MockTradeForm);
    } // setupUi

    void retranslateUi(QWidget *MockTradeForm)
    {
        MockTradeForm->setWindowTitle(QApplication::translate("MockTradeForm", "\346\250\241\346\213\237\344\272\244\346\230\223", 0));
        pbtnBuy->setText(QApplication::translate("MockTradeForm", "\344\271\260\345\205\245", 0));
        pbtnSell->setText(QApplication::translate("MockTradeForm", "\345\215\226\345\207\272", 0));
        pbtnCondition->setText(QApplication::translate("MockTradeForm", "\346\235\241\344\273\266\345\215\225", 0));
        lab_assets->setText(QApplication::translate("MockTradeForm", "12000", 0));
        label_6->setText(QApplication::translate("MockTradeForm", "\345\217\257\347\224\250\350\265\204\351\207\221:", 0));
        label_8->setText(QApplication::translate("MockTradeForm", "\346\235\203\347\233\212:", 0));
        lab_status->setText(QApplication::translate("MockTradeForm", "status", 0));
        pbtnClose->setText(QApplication::translate("MockTradeForm", "\345\271\263\344\273\223", 0));
        label_2->setText(QApplication::translate("MockTradeForm", "\345\215\2261:", 0));
        label_4->setText(QApplication::translate("MockTradeForm", "\344\271\2601:", 0));
        le_sell_price->setText(QApplication::translate("MockTradeForm", "\344\273\267\346\240\2741", 0));
        le_buy_price->setText(QApplication::translate("MockTradeForm", "\344\273\267\346\240\2742", 0));
        le_sell_vol->setText(QApplication::translate("MockTradeForm", "\351\207\2171", 0));
        le_buy_vol->setText(QApplication::translate("MockTradeForm", "\351\207\2172", 0));
        tabPosRecord->setTabText(tabPosRecord->indexOf(tab_position), QApplication::translate("MockTradeForm", "\344\273\223\344\275\215\350\256\260\345\275\225", 0));
        tabPosRecord->setTabText(tabPosRecord->indexOf(tab_2), QApplication::translate("MockTradeForm", "\346\210\220\344\272\244\350\256\260\345\275\225", 0));
        groupBox->setTitle(QApplication::translate("MockTradeForm", "\350\256\276\347\275\256", 0));
        label_3->setText(QApplication::translate("MockTradeForm", "\345\274\200\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
        label_9->setText(QApplication::translate("MockTradeForm", "\345\271\263\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
        label->setText(QApplication::translate("MockTradeForm", "\345\210\235\345\247\213\350\265\204\351\207\221:", 0));
        pbtnInit->setText(QApplication::translate("MockTradeForm", "\350\256\276\347\275\256", 0));
        le_cur_price->setText(QApplication::translate("MockTradeForm", "\344\273\267\346\240\2742", 0));
        label_5->setText(QApplication::translate("MockTradeForm", "\345\275\223\345\211\215:", 0));
        le_cur_vol->setText(QApplication::translate("MockTradeForm", "\351\207\217", 0));
    } // retranslateUi

};

namespace Ui {
    class MockTradeForm: public Ui_MockTradeForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOCKTRADE_H
