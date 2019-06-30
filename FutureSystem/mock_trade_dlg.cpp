#include "mock_trade_dlg.h"

#include <QString>
#include <qstandarditemmodel.h>

#include <TLib/core/tsystem_time.h>

#include "mainwindow.h"

#include "position_records_table_view.h"

MockTradeDlg::MockTradeDlg(MainWindow *main_win)
    : main_win_(main_win)
    , account_info_()
    , cur_price_(0.0)
    , force_close_low_(MAX_PRICE)
    , force_close_high_(MIN_PRICE)
{
    ui.setupUi(this);
    memset(&quote_data_, 0, sizeof(quote_data_));
     
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
    //model->setHorizontalHeaderItem(cst_column_data, new QStandardItem("data"));
    
    delete ui.table_view_record; 
    ui.table_view_record = new PositionRecordsTableView(this);
    ui.table_view_record->setObjectName(QStringLiteral("table_view_record"));
    ui.table_view_record->setGeometry(QRect(10, 190, 561, 341));

    ui.table_view_record->setModel(model);
     
    ui.table_view_record->setColumnWidth(cst_column_long_short, 30);
    ui.table_view_record->setColumnWidth(cst_column_qty, 40);

    ResetUi();
}

void MockTradeDlg::closeEvent(QCloseEvent * /*event*/)
{
    main_win_->is_mock_trade(false);
     
    auto model = static_cast<QStandardItemModel*>(ui.table_view_record->model());
    model->clear();

    memset(&quote_data_, 0, sizeof(quote_data_));
    ResetUi();
    //main_win_->tool_bar()->main_cycle_comb()->setEnabled(true);
}

void MockTradeDlg::ResetUi()
{
    account_info_.capital.avaliable = cst_default_ori_capital;

    ui.dbspbBegCapital->setValue(account_info_.capital.avaliable);
    ui.le_cur_capital->setText(QString::number(account_info_.capital.avaliable));
    ui.lab_assets->setText(QString::number(account_info_.capital.avaliable));
}

void MockTradeDlg::slotHandleQuote(double cur_price, double sell1, double buy1, int sell_vol, int buy_vol)
{
    if( !main_win_->is_mock_trade() )
        return;

    //{
        std::lock_guard<std::mutex> locker(quote_data_mutex_);

        quote_data_.cur_price = cur_price;
        quote_data_.sell_price = sell1;
        quote_data_.buy_price = buy1;
        quote_data_.sell_vol = sell_vol;
        quote_data_.buy_vol = buy_vol;
        ui.le_sell_price->setText(QString::number(sell1));
        ui.le_buy_price->setText(QString::number(buy1));

        ui.le_sell_vol->setText(QString::number(sell_vol));
        ui.le_buy_vol->setText(QString::number(buy_vol));
    //}
    
    if( Equal(cur_price_, cur_price) )
        return;

    cur_price_ = cur_price;
    auto model = static_cast<QStandardItemModel*>(ui.table_view_record->model());

    bool is_force_closed = JudgeDoForceClose(cur_price);
    if( is_force_closed )
    {
        model->clear();
    }else
    { 
        // todo: do if stop profit
        // todo: do if stop loss

        for( int i = 0; i < model->rowCount(); ++i )
        {
            auto item = model->item(i, cst_column_long_short);
            int trade_id = item->data().toInt();
            PositionAtom * atom = account_info_.position.FindPositionAtom(trade_id);
            assert(atom);
            double profit = atom->FloatProfit(cur_price);
            auto item_profit = model->item(i, cst_column_float_profit);
            item_profit->setText(QString::number(profit));
        } 
    }

    double avaliable_capital = account_info_.capital.avaliable + account_info_.position.FloatProfit(cur_price);

    ui.le_cur_capital->setText(QString::number(avaliable_capital));
    double asserts = cst_margin_capital * account_info_.position.TotalPosition() + avaliable_capital;
    ui.lab_assets->setText(QString::number(asserts));
}

void MockTradeDlg::slotOpenSell()
{
    _OpenBuySell(false);
}

void MockTradeDlg::slotOpenBuy()
{
   _OpenBuySell(true);
}

void MockTradeDlg::slotPositionClose()
{

}

void MockTradeDlg::slotBtnCondition()
{

}

void MockTradeDlg::_OpenBuySell(bool is_buy)
{
    if( ui.le_qty->text().isEmpty() || !IsNumber(ui.le_qty->text().trimmed().toLocal8Bit().data()) )
    {
        SetStatusBar(QString::fromLocal8Bit("数量非法!"));
        ui.le_qty->setFocus();
        return;
    }
    double quote_price = 0.0;
    {
        std::lock_guard<std::mutex> locker(quote_data_mutex_);
        quote_price = quote_data_.buy_price;
    }
    int qty = ui.le_qty->text().trimmed().toInt();

    int qty_can_open = CalculateMaxQtyAllowOpen(account_info_.capital.avaliable, quote_price);
    if( qty > qty_can_open )
    {
        SetStatusBar(QString::fromLocal8Bit("资金不足!"));
        return;
    }
    
    auto position_item = std::make_shared<PositionAtom>();
    position_item->trade_id = account_info_.position.GenerateTradeId();
    position_item->price = quote_price;
    position_item->qty = qty; 

    account_info_.position.PushBack(is_buy, position_item);
    account_info_.capital.avaliable -= cst_margin_capital * position_item->qty + CalculateFee(qty, quote_price, false);

    auto low_high = account_info_.position.GetForceClosePrices(account_info_.capital.avaliable + account_info_.capital.float_profit);
    force_close_low_ = std::get<0>(low_high);
    force_close_high_ = std::get<1>(low_high);

    //------------set ui-------------
    ui.le_cur_capital->setText(QString::number(account_info_.capital.avaliable));

    auto model = static_cast<QStandardItemModel *>(ui.table_view_record->model());
    model->insertRow(model->rowCount());
    int row_index = model->rowCount() - 1;
    auto item = new QStandardItem(is_buy ? QString::fromLocal8Bit("多") : QString::fromLocal8Bit("空"));

    QVariant qvar_data;
    qvar_data.setValue(position_item->trade_id);
    item->setData(qvar_data);
    item->setEditable(false);

    model->setItem(row_index, cst_column_long_short, item);
    
    item = new QStandardItem(QString::number(qty));
    //item->setEditable(false);
    model->setItem(row_index, cst_column_qty, item);
     
    item = new QStandardItem(QString::number(quote_price));
    item->setEditable(false);
    model->setItem(row_index, cst_column_ava_price, item);

    item = new QStandardItem("");
    item->setEditable(false);
    model->setItem(row_index, cst_column_float_profit, item);

    item = new QStandardItem("");
    item->setEditable(true);
    model->setItem(row_index, cst_column_stop_loss_price, item);

    item = new QStandardItem("");
    item->setEditable(true);
    model->setItem(row_index, cst_column_stop_profit_price, item);
     
}

void MockTradeDlg::SetStatusBar(const QString & val)
{
    ui.lab_status->setText(val);
}

void MockTradeDlg::UpDateStopProfitOrLossIfNecessary(int row_index, bool is_profit)
{
    auto model = static_cast<QStandardItemModel *>(ui.table_view_record->model());

    int target_col = is_profit ? cst_column_stop_profit_price : cst_column_stop_loss_price;

    int trade_id = model->item(row_index, cst_column_long_short)->data().toInt();
    double stop_price = model->item(row_index, target_col)->text().trimmed().toDouble();

    auto position_atom = account_info_.position.FindPositionAtom(trade_id);
    if( position_atom )
    {
        double *p_stop_price = is_profit ? &position_atom->stop_profit_price : &position_atom->stop_loss_price;
        if( !Equal(*p_stop_price, stop_price) )
        {
            *p_stop_price = stop_price;
        }
    }
}
 
double MockTradeDlg::GetStopProfit(int trade_id)
{
    auto position_atom = account_info_.position.FindPositionAtom(trade_id);
    if( position_atom )
        return position_atom->stop_profit_price;
    else
        return MAGIC_STOP_PRICE;
}

double MockTradeDlg::GetStopLoss(int trade_id)
{
    auto position_atom = account_info_.position.FindPositionAtom(trade_id);
    if( position_atom )
        return position_atom->stop_loss_price;
    else
        return MAGIC_STOP_PRICE;
}

bool MockTradeDlg::JudgeDoForceClose(double price)
{
    if( (force_close_low_ < MAX_PRICE && price < force_close_low_ + EPSINON)
        || (force_close_high_ > MIN_PRICE && price + EPSINON > force_close_high_) )
    {   // force close
        time_t rawtime;
        struct tm timeinfo;
        time( &rawtime );
        localtime_s(&timeinfo, &rawtime); 
        const int hhmm = timeinfo.tm_hour * 100 + timeinfo.tm_min;

        int today = TSystem::Today();

        double profit_close_long = 0.0;
        auto long_pos = account_info_.position.LongPos();
        if( long_pos > 0 )
        {
            double capital_ret = 0.0;
            std::vector<TradeRecordAtom> trades_close_long = account_info_.position.CloseLong(today, hhmm, force_close_low_, unsigned int(long_pos), capital_ret, &profit_close_long);
            trade_records_.insert(trade_records_.end(), trades_close_long.begin(), trades_close_long.end());
        }

        double profit_close_short = 0.0;
        auto short_pos = account_info_.position.ShortPos();
        if( short_pos > 0 )
        {
            double capital_ret = 0.0;
            std::vector<TradeRecordAtom> trades_close_short = account_info_.position.CloseShort(today, hhmm, force_close_low_, unsigned int(short_pos), capital_ret, &profit_close_short);
            trade_records_.insert(trade_records_.end(), trades_close_short.begin(), trades_close_short.end());
        }

        double this_price = 0.0;
        if( force_close_low_ < MAX_PRICE && price < force_close_low_ + EPSINON )
            this_price = force_close_low_;
        else
            this_price = force_close_high_;
        if( long_pos + short_pos > 0 )
            account_info_.capital.avaliable = (long_pos + short_pos) * cst_margin_capital
            - CalculateFee(long_pos + short_pos, this_price, true);
        assert(account_info_.capital.avaliable + account_info_.capital.float_profit > EPSINON);
        return true;
    }else 
        return false;
}