#include "mock_trade_dlg.h"

#include <qstring.h>
#include <qstandarditemmodel.h>

static int cst_column_long_short = 0;
static int cst_column_qty = 1;
static int cst_column_ava_price = 2;
static int cst_column_float_profit = 3;
static int cst_column_stop_loss_price = 4;
static int cst_column_stop_profit_price = 5;
static int cst_column_data = 6;

MockTradeDlg::MockTradeDlg()
    : account_info_()
{
    ui.setupUi(this);
    memset(&quote_data_, 0, sizeof(quote_data_));

    account_info_.capital.avaliable = cst_default_ori_capital;

    ui.dbspbBegCapital->setValue(account_info_.capital.avaliable);
    ui.le_cur_capital->setText(QString::number(account_info_.capital.avaliable));
    ui.lab_assets->setText(QString::number(account_info_.capital.avaliable));

    bool ret = false;
    ret = QObject::connect(ui.pbtnSell, SIGNAL(clicked()), this, SLOT(slotOpenSell()));
    ret = QObject::connect(ui.pbtnBuy, SIGNAL(clicked()), this, SLOT(slotOpenBuy()));
    ret = QObject::connect(ui.pbtnClose, SIGNAL(clicked()), this, SLOT(slotPositionClose()));
    ret = QObject::connect(ui.pbtnCondition, SIGNAL(clicked()), this, SLOT(slotBtnCondition()));
     
    //------------------------- 
    QStandardItemModel * model = new QStandardItemModel(0, 6, this);
    

    model->setHorizontalHeaderItem(cst_column_long_short, new QStandardItem(QString::fromLocal8Bit("头寸"))); // 空 多
    model->setHorizontalHeaderItem(cst_column_qty, new QStandardItem(QString::fromLocal8Bit("数量")));
    model->setHorizontalHeaderItem(cst_column_ava_price, new QStandardItem(QString::fromLocal8Bit("成交均价")));
    model->setHorizontalHeaderItem(cst_column_float_profit, new QStandardItem(QString::fromLocal8Bit("浮盈")));
    model->setHorizontalHeaderItem(cst_column_stop_loss_price, new QStandardItem(QString::fromLocal8Bit("止损")));
    model->setHorizontalHeaderItem(cst_column_stop_profit_price, new QStandardItem(QString::fromLocal8Bit("止盈")));
    //model->setHorizontalHeaderItem(cst_column_data, new QStandardItem( QVaralData()));

    ui.table_view_record->setModel(model);
     
    ui.table_view_record->setColumnWidth(cst_column_long_short, 30);
    ui.table_view_record->setColumnWidth(cst_column_qty, 40);
}

void MockTradeDlg::slotHandleQuote(double sell1, double buy1, int sell_vol, int buy_vol)
{
    std::lock_guard<std::mutex> locker(quote_data_mutex_);

    quote_data_.sell_price = sell1;
    quote_data_.buy_price = buy1;
    quote_data_.sell_vol = sell_vol;
    quote_data_.buy_vol = buy_vol;

    ui.le_sell_price->setText(QString::number(sell1));
    ui.le_buy_price->setText(QString::number(buy1));

    ui.le_sell_vol->setText(QString::number(sell_vol));
    ui.le_buy_vol->setText(QString::number(buy_vol));
     
}

void MockTradeDlg::slotOpenSell()
{
    if( ui.le_qty->text().trimmed().empty() || !IsNumber(ui.le_qty->text().trimmed()) )
    {
        return;
    }
    int qty = ui.le_qty->text().trimmed().toInt();
    double quote_price = 0.0;
    {
    std::lock_guard<std::mutex> locker(quote_data_mutex_);
    quote_price = quote_data_.buy_price;
    }
    PositionAtom  position_item;
    position_item.price = quote_price;
    position_item.qty = qty; 

    account_info_.position.PushBack(false, position_item);

    auto model = static_cast<QStandardItemModel *>(ui.table_view_record->model());
    model->insertRow(model->rowCount());
    int row_index = model->rowCount() - 1;
    auto item = new QStandardItem(QString::fromLocal8Bit("空"));
    model->setItem(row_index, cst_column_long_short, item);

    item = new QStandardItem(QString::number(qty));
    model->setItem(row_index, cst_column_qty, item);
     
    item = new QStandardItem(QString::number(quote_price));
    model->setItem(row_index, cst_column_ava_price, item);

}

void MockTradeDlg::slotOpenBuy()
{
    int qty = ui.le_qty->text().trimmed().toInt();
    double quote_price = 0.0;
    {
    std::lock_guard<std::mutex> locker(quote_data_mutex_);
    quote_price = quote_data_.sell_price;
    }

    auto model = static_cast<QStandardItemModel *>(ui.table_view_record->model());
    model->insertRow(model->rowCount());
    int row_index = model->rowCount() - 1;
    auto item = new QStandardItem(QString::fromLocal8Bit("多"));
    model->setItem(row_index, cst_column_long_short, item);

    item = new QStandardItem(QString::number(qty));
    model->setItem(row_index, cst_column_qty, item);
     
    item = new QStandardItem(QString::number(quote_price));
    model->setItem(row_index, cst_column_ava_price, item);
}

void MockTradeDlg::slotPositionClose()
{

}

void MockTradeDlg::slotBtnCondition()
{

}