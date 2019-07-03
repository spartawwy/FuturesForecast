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
    ret = QObject::connect(ui.pbtnInit, SIGNAL(clicked()), this, SLOT(slotBtnInit()));

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
    ui.table_view_record = new PositionRecordsTableView(ui.tab_position, this);
    ui.table_view_record->setObjectName(QStringLiteral("table_view_record")); 
    ui.table_view_record->setGeometry(QRect(10, 10, 571, 291));

    ui.table_view_record->setModel(model);
     
    ui.table_view_record->setColumnWidth(cst_column_long_short, 30);
    ui.table_view_record->setColumnWidth(cst_column_qty, 40);

    ResetUi();
}

void MockTradeDlg::closeEvent(QCloseEvent * /*event*/)
{
    main_win_->is_mock_trade(false);
     
    auto model = static_cast<QStandardItemModel*>(ui.table_view_record->model());
    if( model->rowCount() > 0 )
        model->removeRows(0, model->rowCount());

    memset(&quote_data_, 0, sizeof(quote_data_));
    ResetUi();
    //main_win_->tool_bar()->main_cycle_comb()->setEnabled(true);
}

bool MockTradeDlg::is_closed()
{
    return !main_win_->is_mock_trade();
}

double MockTradeDlg::GetTargetPrice(bool is_buy)
 {
    std::lock_guard<std::mutex> locker(quote_data_mutex_);
    if( is_buy )
        return quote_data_.sell_price;
    else
        return quote_data_.buy_price;
}

double MockTradeDlg::CurPrice()
{
     //std::lock_guard<std::mutex> locker(quote_data_mutex_);
     return quote_data_.cur_price;
}

bool MockTradeDlg::IsLegalStopPrice(int trade_id, double price, bool is_stop_profit)
{
    bool ret = false;

    auto iterm = account_info_.position.FindPositionAtom(trade_id);
    assert( iterm );
    if( iterm->is_long )
    {
        return is_stop_profit ? price > iterm->price : price < iterm->price;
    }else
    {
        return is_stop_profit ? price < iterm->price : price > iterm->price;
    }
}

void MockTradeDlg::ResetUi()
{
    std::lock_guard<std::mutex> locker(account_info_.position.mutex_);

    account_info_.capital.avaliable = cst_default_ori_capital;
    account_info_.position.Clear();
    ui.dbspbBegCapital->setValue(account_info_.capital.avaliable);
    ui.le_cur_capital->setText(QString::number(account_info_.capital.avaliable));
    ui.lab_assets->setText(QString::number(account_info_.capital.avaliable));
}

void MockTradeDlg::slotHandleQuote(double cur_price, double sell1, double buy1, int vol, int sell_vol, int buy_vol)
{
    if( !main_win_->is_mock_trade() )
        return;

    //{
        std::lock_guard<std::mutex> locker(quote_data_mutex_);

        quote_data_.cur_price = cur_price;
        quote_data_.vol = vol;
        quote_data_.sell_price = sell1;
        quote_data_.buy_price = buy1;
        quote_data_.sell_vol = sell_vol;
        quote_data_.buy_vol = buy_vol;
        ui.le_sell_price->setText(QString::number(sell1));
        ui.le_cur_price->setText(QString::number(cur_price));
        ui.le_buy_price->setText(QString::number(buy1));
        ui.le_cur_vol->setText(QString::number(vol));
        ui.le_sell_vol->setText(QString::number(sell_vol));
        ui.le_buy_vol->setText(QString::number(buy_vol));
    //}
    
    if( Equal(cur_price_, cur_price) )
        return;

    cur_price_ = cur_price;
    std::lock_guard<std::mutex>  pos_locker(account_info_.position.mutex_);

    auto model = static_cast<QStandardItemModel*>(ui.table_view_record->model());

    bool is_force_closed = JudgeDoForceClose(cur_price);
    if( is_force_closed )
    {
        model->clear();
        RefreshCapitalAssertsUi();
         
        PrintTradeRecords();
    }else
    { 
        //   do force stop profit or loss ---------- 
        int today = TSystem::Today();
        int hhmm = cur_hhmm();
        double profit = 0.0;
        bool has_trade = false;

        double profit_long_pos = 0.0;
        std::vector<int> stop_profit_long_ids;
        double capital_ret_stop_profit_long = 0.0, capital_ret_stop_loss_short = 0.0, capital_ret_stop_profit_short = 0.0, capital_ret_stop_loss_long = 0.0;
        std::vector<TradeRecordAtom> trades_stop_profit_long = account_info_.position.DoIfStopProfitLongPos(today, hhmm, cur_price_, capital_ret_stop_profit_long, stop_profit_long_ids, &cur_price_, &profit_long_pos);
        has_trade = (has_trade || !trades_stop_profit_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_long.begin(), trades_stop_profit_long.end());
        
        double loss_short_pos = 0.0;
        std::vector<int> stop_loss_short_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_short = account_info_.position.DoIfStopLossShortPos(today, hhmm, cur_price_, capital_ret_stop_loss_short, stop_loss_short_ids, &cur_price_, &loss_short_pos);
        has_trade = (has_trade || !trades_stop_loss_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_short.begin(), trades_stop_loss_short.end());

        double profit_short_pos = 0.0;
        std::vector<int> stop_profit_short_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_short = account_info_.position.DoIfStopProfitShortPos(today, hhmm, cur_price_, capital_ret_stop_profit_short, stop_profit_short_ids, &cur_price_, &profit_short_pos);
        has_trade = (has_trade || !trades_stop_profit_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_short.begin(), trades_stop_profit_short.end());
        
        double loss_long_pos = 0.0;
        std::vector<int> stop_loss_long_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_long = account_info_.position.DoIfStopLossLongPos(today, hhmm, cur_price_, capital_ret_stop_loss_long, stop_loss_long_ids, &cur_price_, &loss_long_pos);
        has_trade = (has_trade || !trades_stop_loss_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_long.begin(), trades_stop_loss_long.end());

        profit = profit_long_pos + loss_short_pos + profit_short_pos + loss_long_pos;
        std::unordered_map<int, bool> ret_ids;
        if( has_trade )
        {
            PrintTradeRecords();
            account_info_.capital.avaliable += profit + capital_ret_stop_profit_short + capital_ret_stop_loss_long + capital_ret_stop_profit_long + capital_ret_stop_loss_short;
            auto low_high = account_info_.position.GetForceClosePrices(account_info_.capital.avaliable + account_info_.capital.float_profit);
            force_close_low_ = std::get<0>(low_high);
            force_close_high_ = std::get<1>(low_high);
            std::for_each( stop_profit_long_ids.begin(), stop_profit_long_ids.end(), [&ret_ids](int id){ ret_ids.insert(std::make_pair(id, true)); } );
            std::for_each( stop_profit_short_ids.begin(), stop_profit_short_ids.end(), [&ret_ids](int id){ ret_ids.insert(std::make_pair(id, true)); } );
            std::for_each( stop_loss_short_ids.begin(), stop_loss_short_ids.end(), [&ret_ids](int id){ ret_ids.insert(std::make_pair(id, true)); } );
            std::for_each( stop_loss_long_ids.begin(), stop_loss_long_ids.end(), [&ret_ids](int id){ ret_ids.insert(std::make_pair(id, true)); } );
        } 
        account_info_.capital.float_profit = account_info_.position.FloatProfit(cur_price);

        RefreshCapitalAssertsUi();
       
        for( int i = 0; i < model->rowCount(); ++i )
        {
            auto item = model->item(i, cst_column_long_short);
            int trade_id = item->data().toInt();

            auto iter = ret_ids.find(trade_id);
            if( iter != ret_ids.end() )
            {
                model->removeRows(i, 1);
            }else
            {
                PositionAtom * atom = account_info_.position.FindPositionAtom(trade_id);
                assert(atom);
                double profit = atom->FloatProfit(cur_price);
                auto item_profit = model->item(i, cst_column_float_profit);
                item_profit->setText(QString::number(profit));
            }
        } // for
    }
}

void MockTradeDlg::slotBtnInit()
{

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
    int row_index = ui.table_view_record->currentIndex().row();
    if( row_index < 0 )
        return;
    /*if( ui.le_qty->text().isEmpty() || !IsNumber(ui.le_qty->text().trimmed().toLocal8Bit().data()) )
    {
        SetStatusBar(QString::fromLocal8Bit("数量非法!"));
        ui.le_qty->setFocus();
        return;
    }*/
    auto model = static_cast<QStandardItemModel*>(ui.table_view_record->model());
    int trade_id = model->item(row_index, cst_column_long_short)->data().toInt();
    assert(trade_id > 0);

    double capital_ret = 0.0;
    PositionAtom * atom = account_info_.position.FindPositionAtom(trade_id);
    assert(atom);
    double target_price = GetTargetPrice(atom->is_long);
    auto trade_item = account_info_.position.ClosePositionAtom(trade_id, target_price, &capital_ret);
    if( trade_item.trade_id > 0 )
    {
        trade_records_.push_back(trade_item);
        account_info_.capital.avaliable += capital_ret;

        model->removeRows(row_index, 1);

        RefreshCapitalAssertsUi();
        PrintTradeRecords();
    }
     
}

void MockTradeDlg::slotBtnCondition()
{

}

void MockTradeDlg::_OpenBuySell(bool is_buy)
{
    if( ui.le_qty->text().isEmpty() || !IsNumber(ui.le_qty->text().trimmed().toLocal8Bit().data()) )
    {
        SetStatusBar(QString::fromLocal8Bit("数量输入非法!"));
        ui.le_qty->setFocus();
        return;
    }
    double target_price = GetTargetPrice(is_buy);
    
    int qty = ui.le_qty->text().trimmed().toInt();
    if( qty < 0 || qty > 500 )
     {
        SetStatusBar(QString::fromLocal8Bit("数量必须大于0,小于等于500"));
        ui.le_qty->setFocus();
        return;
    }

    int trade_id = 0;

    {
    std::lock_guard<std::mutex>  locker(account_info_.position.mutex_);

    int qty_can_open = CalculateMaxQtyAllowOpen(account_info_.capital.avaliable/* + account_info_.capital.float_profit*/, target_price);
    if( qty > qty_can_open )
    {
        SetStatusBar(QString::fromLocal8Bit("资金不足!"));
        return;
    }
    
    trade_id = account_info_.position.GenerateTradeId();

    TradeRecordAtom  trade_item;
    trade_item.trade_id = trade_id;
    trade_item.date = TSystem::Today();
    trade_item.hhmm = cur_hhmm();
    trade_item.action = RecordAction::OPEN;
    trade_item.pos_type = is_buy ? PositionType::POS_LONG : PositionType::POS_SHORT;
    trade_item.quantity = qty;
    trade_item.price = target_price; 
    trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, false);
    trade_records_.push_back(trade_item);
    //position ---------
    auto position_item = std::make_shared<PositionAtom>();
    position_item->trade_id = trade_id;
    position_item->price = target_price;
    position_item->is_long = is_buy;
    position_item->qty = qty; 
     
    account_info_.position.PushBack(is_buy, position_item);
    // avaliable can be < 0 , when avaliable + float_profit > 0 
    account_info_.capital.avaliable -= cst_margin_capital * position_item->qty + CalculateFee(qty, target_price, false);
    
    auto low_high = account_info_.position.GetForceClosePrices(account_info_.capital.avaliable + account_info_.capital.float_profit);
    force_close_low_ = std::get<0>(low_high);
    force_close_high_ = std::get<1>(low_high);
    PrintTradeRecords();
    }
    //------------set ui-------------
    RefreshCapitalAssertsUi();

    auto model = static_cast<QStandardItemModel *>(ui.table_view_record->model());
    model->insertRow(model->rowCount());
    int row_index = model->rowCount() - 1;
    auto item = new QStandardItem(is_buy ? QString::fromLocal8Bit("多") : QString::fromLocal8Bit("空"));

    QVariant qvar_data;
    qvar_data.setValue(trade_id);
    item->setData(qvar_data);
    item->setEditable(false);

    model->setItem(row_index, cst_column_long_short, item);
    
    item = new QStandardItem(QString::number(qty));
    //item->setEditable(false);
    model->setItem(row_index, cst_column_qty, item);
     
    item = new QStandardItem(QString::number(target_price));
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
    SetStatusBar("");
    auto model = static_cast<QStandardItemModel *>(ui.table_view_record->model());

    int target_col = is_profit ? cst_column_stop_profit_price : cst_column_stop_loss_price;

    int trade_id = model->item(row_index, cst_column_long_short)->data().toInt();
    double stop_price = model->item(row_index, target_col)->text().trimmed().toDouble();
    stop_price = ProcDecimal(stop_price, 1);
    std::lock_guard<std::mutex>  locker(account_info_.position.mutex_);

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
        const int hhmm = cur_hhmm();
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
            account_info_.capital.avaliable += (long_pos + short_pos) * cst_margin_capital
            - CalculateFee(long_pos + short_pos, this_price, true);
        account_info_.capital.float_profit = 0.0;
        assert(account_info_.capital.avaliable > EPSINON);
        return true;
    }else 
        return false;
}

void MockTradeDlg::RefreshCapitalAssertsUi()
{
    ui.le_cur_capital->setText(QString::number(account_info_.capital.avaliable + account_info_.capital.float_profit));
    double asserts = cst_margin_capital * account_info_.position.TotalPosition() + account_info_.capital.avaliable + account_info_.capital.float_profit;
    ui.lab_assets->setText(QString::number(asserts));
}

void MockTradeDlg::PrintTradeRecords()
{
    ui.pe_trade_records->clear();

    QString records_str;
    for(unsigned int i = 0; i < trade_records_.size(); ++i )
    {
        records_str.append(trade_records_.at(i).ToQStr());
        records_str.append("\n");
    }
    ui.pe_trade_records->setPlainText(records_str);
    
}

void MockTradeDlg::AppendStrToRecordsUi(const QString &str)
{
    ui.pe_trade_records->setPlainText(str);
}

void MockTradeDlg::AppendTradesToRecordsUi(std::vector<TradeRecordAtom> &trades)
{
    QString records_str;
    for(unsigned int i = 0; i < trades.size(); ++i )
    {
        records_str.append(trades.at(i).ToQStr());
        records_str.append("\n");
    }
    ui.pe_trade_records->setPlainText(records_str);
}
