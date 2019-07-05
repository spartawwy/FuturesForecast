#include "train_dlg.h"

#include <QtWidgets/QComboBox>
#include <QMessageBox>

#include "mainwindow.h"
#include "tool_bar.h"
#include "kline_wall.h"


//static const int cst_period_day_index = 4;
 
//////////////////////////////////////////
TrainDlg::TrainDlg(KLineWall *parent,  MainWindow *main_win)
    : QWidget(nullptr)
    , parent_(parent)
    , main_win_(main_win)
    , trade_dlg_(this, true)
    , is_started_(false)
    , account_info_()
    , force_close_low_(MAX_PRICE)
    , force_close_high_(MIN_PRICE)
{
    ui.setupUi(this);

    trade_dlg_.setWindowFlags(trade_dlg_.windowFlags() | Qt::WindowStaysOnTopHint/*Qt::Dialog*/ );
    bool ret = false;
    //ui.dateEditTrainBeg->mousePressEvent
    //bool ret = connect(ui.calendar, SIGNAL(clicked(const QDate &)), this, SLOT(OnCalendarClicked(const QDate &)));
    ret = connect(ui.pbtnStart, SIGNAL(clicked()), this, SLOT(OnStartTrain()));
    ret = connect(ui.pbtnStop, SIGNAL(clicked()), this, SLOT(OnStopTrain()));
    ret = connect(ui.pbtnNextK, SIGNAL(clicked()), this, SLOT(OnMoveToNextK()));
    ret = connect(ui.pbtnPreK, SIGNAL(clicked()), this, SLOT(OnMoveToPreK()));
    ret = connect(ui.pbtnBuy, SIGNAL(clicked()), this, SLOT(OnOpenOpenWin()));
    ret = connect(ui.pbtnSell, SIGNAL(clicked()), this, SLOT(OnOpenCloseWin()));

    ret = connect(trade_dlg_.ui.pbt_trade, SIGNAL(clicked()), this, SLOT(OnTrade()));
    ret = ret;

    //ui.le_date->text().clear();
    //ui.le_date->setReadOnly(true);
    OnStopTrain();
      
    //ui.dbspbFeeRate->setValue(cst_default_fee_rate_percent);
}

const T_StockHisDataItem & TrainDlg::CurHisStockDataItem()
{
    return parent_->CurTrainStockDataItem();
}

void TrainDlg::OnCalendarClicked(const QDate & date)
{ 
    //ui.le_date->setText(date.toString("yyyyMMdd"));
}

void TrainDlg::closeEvent(QCloseEvent * /*event*/)
{
    main_win_->is_train_mode(false);
    parent_->k_rend_index_for_train_ = 0;
    if( main_win_->SubKlineWall() )
        main_win_->SubKlineWall()->k_rend_index_for_train_ = 0;
    main_win_->tool_bar()->main_cycle_comb()->setEnabled(true);

    is_started_ = false;

    OnStopTrain();
}

//void TrainDlg::hideEvent(QHideEvent * event)
//{
//    main_win_->is_train_mode(false);
//    parent_->k_rend_index_for_train_ = 0;
//    main_win_->tool_bar()->main_cycle_comb()->setEnabled(true);
//}

void TrainDlg::OnStartTrain()
{
    ui.de_begin->setFocus();

    trade_records_.clear();

    account_info_.capital.avaliable = ui.dbspbBegCapital->value();
    account_info_.capital.frozen = 0.0;
    
    ui.le_cur_capital->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.frozen));
    ui.le_long_pos->setText(ToQString(0));
    ui.le_short_pos->setText(ToQString(0));
    ui.lab_assets->setText(ToQString(cst_default_ori_capital));

    ui.plain_te_record->clear();

    ui.dbspbBegCapital->setEnabled(false);
    //ui.dbspbFeeRate->setEnabled(false);
    //fee_rate_ = ui.dbspbFeeRate->value() / 100;

    ui.de_begin->setEnabled(false);
    ui.pbtnStart->setEnabled(false);
    ui.pbtnStop->setEnabled(true);

    ui.pbtnNextK->setEnabled(true);
    ui.pbtnPreK->setEnabled(true);

    ui.pbtnBuy->setEnabled(true);
    ui.pbtnSell->setEnabled(true);

    /*if( is_started_ )
    {   
         this->hide();
         auto ret = QMessageBox::information(nullptr, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�Ƿ����¿�ʼѵ��?"), QMessageBox::Yes, QMessageBox::No); 
         if( ret == QMessageBox::No )
         {
             this->showNormal();
             return;
         }
         this->showNormal();
    }*/

    int date = ui.de_begin->date().toString("yyyyMMdd").toInt();
    int time = ui.de_begin->time().toString("hhmm").toInt();
    //int date = ui.le_date->text().toInt();
    parent_->SetTrainStartDateTime(TypePeriod(main_win_->tool_bar_->main_cycle_comb()->currentData().toInt()), date, time);

    if( main_win_->SubKlineWall() )
    {
        main_win_->SubKlineWall()->SetTrainStartDateTime(TypePeriod(main_win_->tool_bar_->sub_cycle_comb()->currentData().toInt())
            , date, time);
        main_win_->SubKlineWall()->right_clicked_k_date(date);
        main_win_->SubKlineWall()->right_clicked_k_hhmm(time);

        main_win_->SubKlineWall()->slotOpenRelatedSubKwall(false);
    }
    
    is_started_ = true;
}

void TrainDlg::OnStopTrain()
{
    force_close_low_ = MAX_PRICE;
    force_close_high_ = MIN_PRICE;

    ui.de_begin->setEnabled(true);
    ui.pbtnStart->setEnabled(true);
    ui.pbtnStop->setEnabled(false);

    ui.pbtnNextK->setEnabled(false);
    ui.pbtnPreK->setEnabled(false);

    ui.pbtnBuy->setEnabled(false);
    ui.pbtnSell->setEnabled(false);

    //ui.dbspbFeeRate->setEnabled(true);
    ui.dbspbBegCapital->setEnabled(true);

    account_info_.capital.avaliable = cst_default_ori_capital;
    ui.dbspbBegCapital->setValue(account_info_.capital.avaliable);
    ui.le_cur_capital->setText(ToQString(account_info_.capital.avaliable));
    ui.lab_assets->setText(ToQString(account_info_.capital.avaliable));

    ui.le_long_pos->setText(ToQString(int(account_info_.position.LongPos())));
    ui.le_short_pos->setText(ToQString(int(account_info_.position.ShortPos())));
}

void TrainDlg::OnMoveToNextK()
{
    auto date_time = parent_->MoveRightEndToNextK();
    if( main_win_->SubKlineWall() )
        main_win_->SubKlineWall()->MoveRightEndToNextK(std::get<0>(date_time), std::get<1>(date_time));

    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
    if( stock_item.date == 0 )
    {
        SetStatusBar(QString::fromLocal8Bit("����Ϊ0, �����쳣!"));
        return;
    }
    //assert(trade_dlg_.date_ == stock_item.date);
    
    double profit = 0.0;
    double capital_ret_stop_profit_short = 0.0;
    double capital_ret_stop_loss_long = 0.0;
    double capital_ret_stop_profit_long = 0.0;
    double capital_ret_stop_loss_short = 0.0;
    
    bool has_trade = false;
    if( fabs(stock_item.close_price - stock_item.high_price) > fabs(stock_item.close_price - stock_item.low_price) ) 
    {
        // close price is nearby low price: first high price then low price
        double profit_long_pos = 0.0;
        std::vector<int> stop_profit_long_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_long = account_info_.position.DoIfStopProfitLongPos(stock_item.date, stock_item.hhmmss, stock_item.high_price, capital_ret_stop_profit_long, stop_profit_long_ids, nullptr, &profit_long_pos);
        has_trade = (has_trade || !trades_stop_profit_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_long.begin(), trades_stop_profit_long.end());
        double loss_short_pos = 0.0;
        std::vector<int> stop_loss_short_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_short = account_info_.position.DoIfStopLossShortPos(stock_item.date, stock_item.hhmmss, stock_item.high_price, capital_ret_stop_loss_short, stop_loss_short_ids, nullptr, &loss_short_pos);
        has_trade = (has_trade || !trades_stop_loss_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_short.begin(), trades_stop_loss_short.end());

        double profit_short_pos = 0.0;
        std::vector<int> stop_profit_short_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_short = account_info_.position.DoIfStopProfitShortPos(stock_item.date, stock_item.hhmmss, stock_item.low_price, capital_ret_stop_profit_short, stop_profit_short_ids, nullptr, &profit_short_pos);
        has_trade = (has_trade || !trades_stop_profit_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_short.begin(), trades_stop_profit_short.end());
        double loss_long_pos = 0.0;
        std::vector<int> stop_loss_long_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_long = account_info_.position.DoIfStopLossLongPos(stock_item.date, stock_item.hhmmss, stock_item.low_price, capital_ret_stop_loss_long, stop_loss_long_ids, nullptr, &loss_long_pos);
        has_trade = (has_trade || !trades_stop_loss_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_long.begin(), trades_stop_loss_long.end());

        profit = profit_long_pos + loss_short_pos + profit_short_pos + loss_long_pos;
    }else
    { // close price is nearby high price: first low price then high price
        double profit_short_pos = 0.0;
        std::vector<int> stop_profit_short_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_short = account_info_.position.DoIfStopProfitShortPos(stock_item.date, stock_item.hhmmss, stock_item.low_price, capital_ret_stop_profit_short, stop_profit_short_ids, nullptr, &profit_short_pos);
        has_trade = (has_trade || !trades_stop_profit_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_short.begin(), trades_stop_profit_short.end());
        double loss_long_pos = 0.0;
        std::vector<int> stop_loss_long_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_long = account_info_.position.DoIfStopLossLongPos(stock_item.date, stock_item.hhmmss, stock_item.low_price, capital_ret_stop_loss_long, stop_loss_long_ids, nullptr, &loss_long_pos);
        has_trade = (has_trade || !trades_stop_loss_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_long.begin(), trades_stop_loss_long.end());

        double profit_long_pos = 0.0;
        std::vector<int> stop_profit_long_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_long = account_info_.position.DoIfStopProfitLongPos(stock_item.date, stock_item.hhmmss, stock_item.high_price, capital_ret_stop_profit_long, stop_profit_long_ids, nullptr, &profit_long_pos);
        has_trade = (has_trade || !trades_stop_profit_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_long.begin(), trades_stop_profit_long.end());
        double loss_short_pos = 0.0;
        std::vector<int> stop_loss_short_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_short = account_info_.position.DoIfStopLossShortPos(stock_item.date, stock_item.hhmmss, stock_item.high_price, capital_ret_stop_loss_short, stop_loss_short_ids, nullptr, &loss_short_pos);
        has_trade = (has_trade || !trades_stop_loss_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_short.begin(), trades_stop_loss_short.end());

        profit = profit_long_pos + loss_short_pos + profit_short_pos + loss_long_pos;
    }
     
    if( has_trade )
    {
        account_info_.capital.float_profit = account_info_.position.FloatProfit(stock_item.open_price);
        account_info_.capital.avaliable += profit + capital_ret_stop_profit_short + capital_ret_stop_loss_long + capital_ret_stop_profit_long + capital_ret_stop_loss_short;
        auto low_high = account_info_.position.GetForceClosePrices(account_info_.capital.avaliable + account_info_.capital.float_profit);
        force_close_low_ = std::get<0>(low_high);
        force_close_high_ = std::get<1>(low_high);
    }

    if( (force_close_low_ < MAX_PRICE && stock_item.low_price < force_close_low_ + EPSINON)
        || (force_close_high_ > MIN_PRICE && stock_item.high_price + EPSINON > force_close_high_) )
    {  // force close
        double profit_close_long = 0.0;
        auto long_pos = account_info_.position.LongPos();
        if( long_pos > 0 )
        {
        double capital_ret = 0.0;
        std::vector<TradeRecordAtom> trades_close_long = account_info_.position.CloseLong(stock_item.date, stock_item.hhmmss, force_close_low_, unsigned int(long_pos), capital_ret, &profit_close_long);
        trade_records_.insert(trade_records_.end(), trades_close_long.begin(), trades_close_long.end());
        }

        double profit_close_short = 0.0;
        auto short_pos = account_info_.position.ShortPos();
        if( short_pos > 0 )
        {
        double capital_ret = 0.0;
        std::vector<TradeRecordAtom> trades_close_short = account_info_.position.CloseShort(stock_item.date, stock_item.hhmmss, force_close_low_, unsigned int(short_pos), capital_ret, &profit_close_short);
        trade_records_.insert(trade_records_.end(), trades_close_short.begin(), trades_close_short.end());
        }

        double this_price = 0.0;
        if( force_close_low_ < MAX_PRICE && stock_item.low_price < force_close_low_ + EPSINON )
            this_price = force_close_low_;
        else
            this_price = force_close_high_;
        if( long_pos + short_pos > 0 )
            account_info_.capital.avaliable = (long_pos + short_pos) * cst_margin_capital
                                          - CalculateFee(long_pos + short_pos, this_price, true);
        assert(account_info_.capital.avaliable + account_info_.capital.float_profit > EPSINON);
    }else
    {
        account_info_.capital.float_profit = account_info_.position.FloatProfit(stock_item.close_price);
        assert(account_info_.capital.avaliable + account_info_.capital.float_profit > EPSINON);
    }
     
    auto long_pos = account_info_.position.LongPos();
    auto short_pos = account_info_.position.ShortPos();

    ui.le_long_pos->setText(ToQString(int(long_pos)));
    ui.le_short_pos->setText(ToQString(int(short_pos)));
     
    ui.le_cur_capital->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.float_profit));
    ui.lab_assets->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.float_profit
                                    + cst_margin_capital * (long_pos + short_pos)));

    PrintTradeRecords();
}

void TrainDlg::OnMoveToPreK()
{
#if 0 
    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
    // roll back trade records of cur k line 's 
    while( !trade_records_.empty() && trade_records_.back().date == stock_item.date )
    {
        // do roll back ---------------
        TradeRecordAtom & trade_item = trade_records_.back();
        switch(trade_item.action) 
        {
        case RecordAction::SELL:
            {
                account_info_.capital.avaliable -= trade_item.price * trade_item.quantity - trade_item.fee;
                account_info_.stock.avaliable += trade_item.quantity;
            }break;
        case RecordAction::BUY:
            {
                account_info_.capital.avaliable += trade_item.price * trade_item.quantity + trade_item.fee;
                assert(account_info_.stock.frozen > trade_item.quantity - 0.0001);
                account_info_.stock.frozen -= trade_item.quantity;
            }break;
        case RecordAction::UNFREEZE:
            {
                assert(account_info_.stock.avaliable > trade_item.quantity - 0.0001);
                account_info_.stock.avaliable -= trade_item.quantity;
                account_info_.stock.frozen += trade_item.quantity;
            }break;
        default:assert(false); break;
        }
         
        trade_records_.pop_back();
    } // while 

    parent_->MoveRightEndToPreDayK();
    if( main_win_->SubKlineWall() )
        main_win_->SubKlineWall()->MoveRightEndToPreDayK();

    const T_StockHisDataItem & pre_stock_item = CurHisStockDataItem();
    ui.le_cur_stock_num->setText(ToQString(int(account_info_.stock.avaliable + account_info_.stock.frozen)));
    ui.le_cur_capital->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.frozen));
    
    double assets_today_end = pre_stock_item.close_price * (account_info_.stock.avaliable + account_info_.stock.frozen) 
        + account_info_.capital.avaliable + account_info_.capital.frozen;
    ui.lab_assets->setText(ToQString(assets_today_end));

    PrintTradeRecords();
#endif
}

void TrainDlg::OnOpenOpenWin()
{
    trade_dlg_.SetStatusBar("");
    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
    if( stock_item.date == 0 )
    {
        trade_dlg_.SetStatusBar(QString::fromLocal8Bit("��ǰ�����޶�Ӧ�ڻ�����!"));
        return;
    }
    this->hide();
    trade_dlg_.is_close(false);
    trade_dlg_.SetDate(stock_item.date);
    trade_dlg_.SetHhmm(stock_item.hhmmss);
    trade_dlg_.ui.radioBtn_open->setChecked(true);
    trade_dlg_.ui.radioBtn_close->setChecked(false);
    //const double allow_highest_price = (stock_item.open_price < stock_item.close_price ? stock_item.close_price : stock_item.open_price);

    trade_dlg_.ui.le_price->setText(ToQString(stock_item.close_price));
    trade_dlg_.ui.le_stop_profit->setText(ToQString(MAGIC_STOP_PRICE));
    trade_dlg_.ui.le_stop_loss->setText(ToQString(MAGIC_STOP_PRICE));

    int qty = CalculateMaxQtyAllowOpen(account_info_.capital.avaliable + account_info_.capital.float_profit, stock_item.close_price);
    trade_dlg_.ui.le_qty_ava->setText(ToQString(qty));
    trade_dlg_.ui.le_qty->setText(ToQString(qty));
    trade_dlg_.ui.le_capital_ava->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.float_profit));

    trade_dlg_.showNormal();
}

void TrainDlg::OnOpenCloseWin()
{
    trade_dlg_.SetStatusBar("");
    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
    if( stock_item.date == 0 )
    {
        trade_dlg_.SetStatusBar(QString::fromLocal8Bit("��ǰ�����޶�Ӧ�ڻ�����!"));
        return;
    }
    this->hide();
    trade_dlg_.is_close(true);
    trade_dlg_.SetDate(stock_item.date);
    trade_dlg_.SetHhmm(stock_item.hhmmss);
    trade_dlg_.ui.radioBtn_close->setChecked(true);
    trade_dlg_.ui.radioBtn_open->setChecked(false);

    //const double allow_highest_price = (stock_item.open_price < stock_item.close_price ? stock_item.close_price : stock_item.open_price);

    trade_dlg_.ui.le_price->setText(ToQString(stock_item.close_price));

    if( trade_dlg_.ui.radioBtn_long->isChecked() )
    {
        trade_dlg_.ui.le_qty->setText(ToQString((int)account_info_.position.LongPos()));
        trade_dlg_.ui.le_qty_ava->setText(ToQString((int)account_info_.position.LongPos()));
    }else
    {
        trade_dlg_.ui.le_qty->setText(ToQString((int)account_info_.position.ShortPos()));
        trade_dlg_.ui.le_qty_ava->setText(ToQString((int)account_info_.position.ShortPos()));
    }
    trade_dlg_.ui.le_capital_ava->setText(ToQString(account_info_.capital.avaliable));

    trade_dlg_.showNormal();
}

void TrainDlg::OnTrade()
{
    trade_dlg_.SetStatusBar("");
    if( trade_dlg_.date_ == 0 )
    {
        trade_dlg_.SetStatusBar(QString::fromLocal8Bit("��ǰ�����޶�Ӧ��Ʊ����!"));
        return;
    }
    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
    //assert(trade_dlg_.date_ == stock_item.date);

    double price = trade_dlg_.ui.le_price->text().toDouble();
    int quantity = (int)trade_dlg_.ui.le_qty->text().toDouble();
    trade_dlg_.ui.le_qty->setText(ToQString(quantity));

    double allow_lowest_price = (stock_item.open_price < stock_item.close_price ? stock_item.open_price : stock_item.close_price);
    allow_lowest_price = ProcDecimal(allow_lowest_price, DEFAULT_DECIMAL);
    double allow_highest_price = (stock_item.open_price < stock_item.close_price ? stock_item.close_price : stock_item.open_price);
    allow_highest_price = ProcDecimal(allow_highest_price, DEFAULT_DECIMAL);
    if( price < allow_lowest_price || price > allow_highest_price ) 
    {
        trade_dlg_.SetStatusBar(QString::fromLocal8Bit("���۷Ƿ�!"));
        trade_dlg_.ui.le_price->setFocus();
        return;
    }
    const bool is_pos_type_long = trade_dlg_.is_long();

    if( trade_dlg_.is_close_ )
    {
        if( quantity <= 0 )
        {
            trade_dlg_.SetStatusBar(QString::fromLocal8Bit("ƽ�������Ƿ�!"));
            trade_dlg_.ui.le_qty->setFocus();
            return;
        } 
        int avaliable_pos = 0;
        if( is_pos_type_long )
            avaliable_pos = (int)account_info_.position.LongPos();
        else
            avaliable_pos = (int)account_info_.position.ShortPos();

        if( quantity > avaliable_pos )
        {
            trade_dlg_.SetStatusBar(QString::fromLocal8Bit("��ƽ��������!"));
            trade_dlg_.ui.le_qty->setFocus();
            return;
        }
        
        double final_fill_price = 0.0;
        double profit = 0.0;
        double capital_ret = 0.0;
        if( is_pos_type_long )
        {
            final_fill_price = price - cst_per_tick;
            account_info_.position.CloseLong(stock_item.date, stock_item.hhmmss, final_fill_price, unsigned int(quantity), capital_ret, &profit);
        }else
        {
            final_fill_price = price + cst_per_tick;
            account_info_.position.CloseShort(stock_item.date, stock_item.hhmmss, final_fill_price, unsigned int(quantity), capital_ret, &profit);
        }
        double fee = CalculateFee(quantity, final_fill_price, true);
         
        account_info_.capital.avaliable += profit - fee + capital_ret; 

        TradeRecordAtom  trade_item;
        trade_item.trade_id = account_info_.position.GenerateTradeId();
        trade_item.date = trade_dlg_.date_;
        trade_item.hhmm = trade_dlg_.hhmm_;
        trade_item.action = RecordAction::CLOSE;
        trade_item.pos_type = is_pos_type_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
        trade_item.price = final_fill_price;
        trade_item.quantity = quantity;
        trade_item.fee = fee;
        trade_item.profit = profit; 
        trade_records_.push_back(trade_item);
    }else // open 
    { 
        if( quantity < 1 )
        {
            trade_dlg_.SetStatusBar(QString::fromLocal8Bit("���������Ƿ�!"));
            trade_dlg_.ui.le_qty->setFocus();
            return;
        }
        double capital_buy = cst_margin_capital * quantity;
        double final_fill_price = 0.0;
        if( is_pos_type_long )
        {
            final_fill_price = price + cst_per_tick;
        }else
        {
            final_fill_price = price - cst_per_tick;
        }
        double fee = CalculateFee(quantity, final_fill_price, false);
        if( capital_buy + fee > account_info_.capital.avaliable )
        {
            trade_dlg_.SetStatusBar(QString::fromLocal8Bit("�����ʽ���!"));
            return;
        }
        double stop_profit_price = MAGIC_STOP_PRICE;
        if( !trade_dlg_.ui.le_stop_profit->text().trimmed().isEmpty() )
            stop_profit_price = trade_dlg_.ui.le_stop_profit->text().trimmed().toDouble();
        double stop_loss_price = MAGIC_STOP_PRICE;
        if( !trade_dlg_.ui.le_stop_loss->text().trimmed().isEmpty() )
            stop_loss_price = trade_dlg_.ui.le_stop_loss->text().trimmed().toDouble();

        account_info_.capital.avaliable -= capital_buy + fee;

        auto pos_atom = std::make_shared<PositionAtom>();
        pos_atom->trade_id = account_info_.position.GenerateTradeId();
        pos_atom->price = final_fill_price;
        pos_atom->qty = (unsigned int)quantity;
        pos_atom->stop_profit_price = stop_profit_price;
        pos_atom->stop_loss_price = stop_loss_price;
        account_info_.position.PushBack(is_pos_type_long, pos_atom);
 
        TradeRecordAtom  trade_item;
        trade_item.trade_id = pos_atom->trade_id;
        trade_item.date = trade_dlg_.date_;
        trade_item.hhmm = trade_dlg_.hhmm_;
        trade_item.action = RecordAction::OPEN;
        trade_item.pos_type = is_pos_type_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
        trade_item.price = final_fill_price;
        trade_item.quantity = quantity;
        trade_item.profit = 0.0;
        trade_item.fee = fee;
        trade_item.price_stop_loss = stop_loss_price;
        trade_item.price_stop_profit = stop_profit_price;
        trade_records_.push_back(trade_item);

        trade_dlg_.ui.le_capital_ava->setText(ToQString(account_info_.capital.avaliable));
    }
    
    auto force_close_low_high = account_info_.position.GetForceClosePrices(account_info_.capital.avaliable + account_info_.capital.frozen);
    force_close_low_ = std::get<0>(force_close_low_high);
    force_close_high_ = std::get<1>(force_close_low_high);

    // ui --------
    ui.le_cur_capital->setText(ToQString(account_info_.capital.avaliable));
    ui.le_long_pos->setText(ToQString(int(account_info_.position.LongPos())));
    ui.le_short_pos->setText(ToQString(int(account_info_.position.ShortPos())));

    double assets_today_end = account_info_.capital.avaliable + account_info_.capital.frozen
        + account_info_.position.FloatProfit(price) + account_info_.position.TotalPosition() * cst_margin_capital;
    ui.lab_assets->setText(ToQString(assets_today_end));

    trade_dlg_.hide();
    PrintTradeRecords();
    this->showNormal();
}

void TrainDlg::PrintTradeRecords()
{
    ui.plain_te_record->clear();

    QString records_str;
    for(unsigned int i = 0; i < trade_records_.size(); ++i )
    {
        records_str.append(trade_records_.at(i).ToQStr());
        records_str.append("\n");
    }
    ui.plain_te_record->setPlainText(records_str);
    //ui.plain_te_record->setText()
}
