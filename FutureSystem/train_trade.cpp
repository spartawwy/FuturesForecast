#include "train_trade.h"

#include <cassert>
#include <cstdio>
#include <cmath>
#include <algorithm>

#include <QString>

#include <TLib/core/tsystem_time.h>

#include "stkfo_common.h"


QString TradeRecordAtom::ToQStr()
{
    char buf[1024] = {'\0'};
    sprintf_s(buf, "编号:%d 时间:%d-%04d %s %s 数量:%d 价格:%.2f (止赢价:%.2f 止损价:%.2f) 佣金税费:%.2f 盈亏:%.2f "
        , this->trade_id
        , this->date
        , this->hhmm
        , ToStr(this->action).c_str()
        , ToStr(this->pos_type).c_str()
        , this->quantity
        , this->price
        , this->price_stop_profit
        , this->price_stop_loss
        , this->fee
        , this->profit
        );
    return QString::fromLocal8Bit(buf);
}

//----------------------
double PositionAtom::FloatProfit(double cur_price)
{
    double dis_price = is_long ? cur_price - price : price - cur_price;
    double val = dis_price / cst_per_tick * cst_per_tick_capital * qty;
    return ProcDecimal(val, 0);
}

//----------------------
unsigned int PositionInfo::LongPos()
{
    unsigned int qty = 0;
    std::for_each( std::begin(long_positions_), std::end(long_positions_), [&qty](T_PositionAtoms::reference entry)
    {
        qty += entry->qty;
    });
    return qty;
}

double PositionInfo::LongAveragePrice()
{
    if( long_positions_.empty() )
        return 0.0;
    double ave_price = 0;
    double amount = 0.0;
    int qty = 0;
    for( unsigned int i = 0; i < long_positions_.size(); ++i ) 
    {
        amount += long_positions_.at(i)->price * long_positions_.at(i)->qty;
        qty += long_positions_.at(i)->qty;
    }
    ave_price = amount / (double)qty;
    return ProcDecimal(ave_price, DEFAULT_DECIMAL);
}


unsigned int PositionInfo::ShortPos()
{
    unsigned int qty = 0;
    std::for_each( std::begin(short_positions_), std::end(short_positions_), [&qty](T_PositionAtoms::reference entry)
    {
        qty += entry->qty;
    });
    return qty;
}

double PositionInfo::ShortAveragePirce()
{
    if( short_positions_.empty() )
        return 0.0;
    double ave_price = 0;
    double amount = 0.0;
    int qty = 0;
    for( unsigned int i = 0; i < short_positions_.size(); ++i ) 
    {
        amount += short_positions_.at(i)->price * short_positions_.at(i)->qty;
        qty += short_positions_.at(i)->qty;
    }
    ave_price = amount / (double)qty;
    return ProcDecimal(ave_price, DEFAULT_DECIMAL);
}

double PositionInfo::FloatProfit(double price)
{
    assert(price > 0.0);
    double profit = 0.0;
    std::for_each(std::begin(long_positions_), std::end(long_positions_), [&](T_PositionAtoms::reference entry)
    {
        profit += (price - entry->price) / cst_per_tick * cst_per_tick_capital * entry->qty;
    });
    std::for_each(std::begin(short_positions_), std::end(short_positions_), [&](T_PositionAtoms::reference entry)
    {
        profit += (entry->price - price) / cst_per_tick * cst_per_tick_capital * entry->qty;
    });
    return ProcDecimal(profit, 0);
}

// return <low, high> ;  capital is remain capital
std::tuple<double, double> PositionInfo::GetForceClosePrices(double capital)
{
    assert(capital > EPSINON);

    double low_price = MAX_PRICE;
    double high_price = MIN_PRICE;

    double long_ava_price = LongAveragePrice();
    if( long_ava_price > 50.0 ) // means exists long position
    {
        double start_price = long_ava_price;
        while( start_price > long_ava_price - 50.0 )
        {
            start_price -= cst_per_tick;
            double profit = FloatProfit(start_price);
            if( capital + profit < EPSINON )
            {
                low_price = start_price;
                break;
            }
        }
    }
    
    double short_ava_price = ShortAveragePirce();
    if( short_ava_price > 50.0 )// means exists short position
    {
        double start_price = short_ava_price;
        while( start_price < short_ava_price + 50.0 )
        {
            start_price += cst_per_tick;
            double profit = FloatProfit(start_price);
            if( capital + profit < EPSINON )
            {
                high_price = start_price;
                break;
            }
        }
    }
    return std::make_tuple(low_price, high_price);
}

// return profit of close action
std::vector<TradeRecordAtom> PositionInfo::DoIfStopProfitLongPos(int date, int hhmm, double h_price, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit)
{
    assert(date > 0);
    assert(h_price > EPSINON);

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    capital_ret = 0.0;
    //for( int i = long_positions_.size() - 1; !long_positions_.empty() && i >= 0 ; --i ) 
    for( auto iter = long_positions_.begin(); iter != long_positions_.end(); )
    {
        if( (*iter)->stop_profit_price > EPSINON && h_price > (*iter)->stop_profit_price - EPSINON ) // >= stop profit price
        {
            TradeRecordAtom  trade_item;
            trade_item.trade_id = GenerateTradeId();
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = RecordAction::CLOSE;
            trade_item.pos_type = PositionType::POS_LONG;
            trade_item.quantity = (*iter)->qty;
            if( p_cur_price )
                trade_item.price = *p_cur_price - cst_per_tick;
            else
                trade_item.price = (*iter)->stop_profit_price - cst_per_tick;
            trade_item.profit = (trade_item.price - (*iter)->price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            capital_ret += cst_margin_capital * (*iter)->qty;
            profit += trade_item.profit - trade_item.fee;
            ret_ids.push_back((*iter)->trade_id);
            iter = long_positions_.erase(iter);
        }else
            ++iter;
    }
    if( p_profit )
        *p_profit = ProcDecimal(profit, 0);
    return ret;
}

// return profit of close action
std::vector<TradeRecordAtom> PositionInfo::DoIfStopProfitShortPos(int date, int hhmm, double l_price, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit)
{
    assert(l_price > EPSINON);

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    capital_ret = 0.0;
    //for( int i = short_positions_.size() - 1; !short_positions_.empty() && i >= 0 ; --i ) 
    for( auto iter = short_positions_.begin(); iter != short_positions_.end(); )
    {
        if( (*iter)->stop_profit_price > EPSINON && l_price < (*iter)->stop_profit_price + EPSINON ) // <= profit price
        {
            TradeRecordAtom  trade_item;
            trade_item.trade_id = GenerateTradeId();
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = RecordAction::CLOSE;
            trade_item.pos_type = PositionType::POS_SHORT;
            trade_item.quantity = (*iter)->qty;
            if( p_cur_price )
                trade_item.price = *p_cur_price + cst_per_tick;
            else
                trade_item.price = (*iter)->stop_profit_price + cst_per_tick;
            trade_item.profit = ((*iter)->price - trade_item.price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            capital_ret += cst_margin_capital * (*iter)->qty;
            profit += trade_item.profit - trade_item.fee;
            ret_ids.push_back((*iter)->trade_id);

            iter = short_positions_.erase(iter);
        }else
            ++iter;
    }
    if( p_profit )
        *p_profit = ProcDecimal(profit, 0);
    return ret;
}

// return profit of close action
std::vector<TradeRecordAtom> PositionInfo::DoIfStopLossLongPos(int date, int hhmm, double l_price, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit)
{
    assert(l_price > EPSINON); 

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    capital_ret = 0.0;
    //for( int i = long_positions_.size() - 1; !long_positions_.empty() && i >= 0 ; --i ) 
    for( auto iter = long_positions_.begin(); iter != long_positions_.end(); )
    {
        if( l_price < (*iter)->stop_loss_price + EPSINON ) // <= stop loss price
        {
            TradeRecordAtom  trade_item;
            trade_item.trade_id = GenerateTradeId();
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = RecordAction::CLOSE;
            trade_item.pos_type = PositionType::POS_LONG;
            trade_item.quantity = (*iter)->qty;
            if( p_cur_price )
                trade_item.price = *p_cur_price - cst_per_tick;
            else
                trade_item.price = (*iter)->stop_loss_price - cst_per_tick;
            trade_item.profit = (trade_item.price - (*iter)->price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            capital_ret += cst_margin_capital * (*iter)->qty;
            profit += trade_item.profit - trade_item.fee;
            ret_ids.push_back((*iter)->trade_id);

            iter = long_positions_.erase(iter);
        }else
            ++iter;
    }
    if( p_profit )
        *p_profit = ProcDecimal(profit, 0);
    return ret;
}

// return profit(include fee) of close action 
std::vector<TradeRecordAtom> PositionInfo::DoIfStopLossShortPos(int date, int hhmm, double h_price, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit)
{
    assert(h_price > EPSINON);  

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    capital_ret = 0.0;
    //for( int i = short_positions_.size() - 1; !short_positions_.empty() && i >= 0 ; --i ) 
    for( auto iter = short_positions_.begin(); iter != short_positions_.end(); )
    {
        if( (*iter)->stop_loss_price > EPSINON && h_price + EPSINON > (*iter)->stop_loss_price )
        {
            TradeRecordAtom  trade_item;
            trade_item.trade_id = GenerateTradeId();
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = RecordAction::CLOSE;
            trade_item.pos_type = PositionType::POS_SHORT;
            trade_item.quantity = (*iter)->qty;
            if( p_cur_price )
                trade_item.price = *p_cur_price + cst_per_tick;
            else
                trade_item.price = (*iter)->stop_loss_price + cst_per_tick;
            trade_item.profit = ((*iter)->price - trade_item.price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item); 
            capital_ret += cst_margin_capital * (*iter)->qty;
            profit += trade_item.profit - trade_item.fee;
            ret_ids.push_back((*iter)->trade_id);

            iter = short_positions_.erase(iter);
        }else
            ++iter;
    }
    if( p_profit )
        *p_profit = ProcDecimal(profit, 0);
    return ret;
}

#if 0 
// ps : make sure h_price > l_price
std::vector<TradeRecordAtom> PositionInfo::DoIfStopProfit(int date, int hhmm, double h_price, double l_price, double *p_profit)
{
    assert(l_price > EPSINON);
    assert(h_price + EPSINON > l_price);

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    for( int i = long_positions_.size() - 1; i >= 0 ; --i ) 
    {
        if( h_price > long_positions_.at(i).stop_profit_price - EPSINON ) // >= stop profit price
        {
            TradeRecordAtom  trade_item;
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = RecordAction::CLOSE;
            trade_item.pos_type = PositionType::POS_LONG;
            trade_item.quantity = long_positions_.at(i).qty;
            trade_item.price = long_positions_.at(i).stop_profit_price - cst_per_tick;
            trade_item.profit = (trade_item.price - long_positions_.at(i).price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            profit += trade_item.profit - trade_item.fee;
        }
    }

    for( int i = short_positions_.size() - 1; i >= 0 ; --i ) 
    {
        if( l_price + EPSINON < short_positions_.at(i).stop_profit_price )
        {
            TradeRecordAtom  trade_item;
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = RecordAction::CLOSE;
            trade_item.pos_type = PositionType::POS_SHORT;
            trade_item.quantity = short_positions_.at(i).qty;
            trade_item.price = short_positions_.at(i).stop_profit_price + cst_per_tick;
            trade_item.profit = (short_positions_.at(i).price - trade_item.price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            profit += trade_item.profit - trade_item.fee;
        }
    }
    if( p_profit )
        *p_profit = profit;
    return ret;
}

std::vector<TradeRecordAtom> PositionInfo::DoIfStopLoss(int date, int hhmm, double h_price, double l_price, double *p_profit)
{
    assert(l_price > EPSINON);
    assert(h_price + EPSINON > l_price);

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    for( int i = long_positions_.size() - 1; i >= 0 ; --i ) 
    {
        if( l_price < long_positions_.at(i).stop_loss_price - EPSINON ) // <= stop loss price
        {
            TradeRecordAtom  trade_item;
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = RecordAction::CLOSE;
            trade_item.pos_type = PositionType::POS_LONG;
            trade_item.quantity = long_positions_.at(i).qty;
            trade_item.price = long_positions_.at(i).stop_loss_price - cst_per_tick;
            trade_item.profit = (trade_item.price - long_positions_.at(i).price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            profit += trade_item.profit - trade_item.fee;
        }
    }

    for( int i = short_positions_.size() - 1; i >= 0 ; --i ) 
    {
        if( h_price + EPSINON > short_positions_.at(i).stop_loss_price )
        {
            TradeRecordAtom  trade_item;
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = RecordAction::CLOSE;
            trade_item.pos_type = PositionType::POS_SHORT;
            trade_item.quantity = short_positions_.at(i).qty;
            trade_item.price = short_positions_.at(i).stop_loss_price + cst_per_tick;
            trade_item.profit = (short_positions_.at(i).price - trade_item.price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            profit += trade_item.profit - trade_item.fee;
        }
    }
    if( p_profit )
        *p_profit = profit;
    return ret;
}
#endif 

std::vector<TradeRecordAtom> PositionInfo::CloseLong(int date, int hhmm, double price, unsigned int qty, double &capital_ret, double *p_profit)
{ 
    std::vector<TradeRecordAtom> ret;
    assert( qty > 0 );

    /*if( long_positions_.empty() )
        return ret;*/
    
    unsigned int remain_tgt_qty = qty;
    assert( LongPos() >= remain_tgt_qty );

    double profit = 0.0;
    capital_ret = 0.0;
    //for( int i = long_positions_.size() - 1; i >= 0 ; --i ) 
    for( auto iter = long_positions_.begin(); iter != long_positions_.end(); )
    {
        double this_profit = 0.0;
        int this_qty = 0;
        if( (*iter)->qty >= remain_tgt_qty )
        {
            this_qty = remain_tgt_qty;
            this_profit = (price - (*iter)->price) / cst_per_tick * cst_per_tick_capital * remain_tgt_qty;
            capital_ret += cst_margin_capital * remain_tgt_qty;
            if( (*iter)->qty == remain_tgt_qty )
                iter = long_positions_.erase(iter);
            else
            {
                (*iter)->qty -= remain_tgt_qty;
                ++iter;
            }
            remain_tgt_qty = 0; 
        }else
        {
            this_qty = (*iter)->qty;
            this_profit = (price - (*iter)->price) / cst_per_tick * cst_per_tick_capital * this_qty;
            remain_tgt_qty -= this_qty;
            capital_ret += cst_margin_capital * this_qty;
            iter = long_positions_.erase(iter);
        }
        profit += this_profit;

        TradeRecordAtom item;
        item.trade_id = GenerateTradeId();
        item.date = date;
        item.hhmm = hhmm;
        item.action = RecordAction::CLOSE;
        item.pos_type = PositionType::POS_LONG;
        item.quantity = this_qty;
        item.price = price;
        item.profit = this_profit;
        item.fee = CalculateFee(item.quantity, price, true);
        ret.push_back(item);

        if( remain_tgt_qty == 0 )
            break;
    } // for 

    if( p_profit )
        *p_profit = profit;
    return ret;
}

std::vector<TradeRecordAtom> PositionInfo::CloseShort(int date, int hhmm, double price, unsigned int qty, double &capital_ret, double *p_profit)
{ 
    assert( qty > 0 );
    assert( short_positions_.size() > 0 );
    std::vector<TradeRecordAtom> ret;
    unsigned int remain_tgt_qty = qty;
    assert( ShortPos() >= remain_tgt_qty );

    double profit = 0.0;
    capital_ret = 0.0;
    //for( int i = short_positions_.size() - 1; i >= 0 ; --i ) 
    for( auto iter = short_positions_.begin(); iter != short_positions_.end(); )
    {
        double this_profit = 0.0;
        int this_qty = 0;
        if( (*iter)->qty >= remain_tgt_qty )
        {
            this_qty = remain_tgt_qty;
            this_profit = ((*iter)->price - price) / cst_per_tick * cst_per_tick_capital * remain_tgt_qty;
            capital_ret += cst_margin_capital * remain_tgt_qty;
            if( (*iter)->qty == remain_tgt_qty )
                iter = short_positions_.erase(iter);
            else
            {
                (*iter)->qty -= remain_tgt_qty;
                ++iter;
            }
            remain_tgt_qty = 0;
        }else
        {
            this_qty = (*iter)->qty;
            this_profit = ((*iter)->price - price) / cst_per_tick * cst_per_tick_capital * this_qty;
            capital_ret += cst_margin_capital * this_qty;
            remain_tgt_qty -= this_qty;
            iter = short_positions_.erase(iter);
        }
        profit += this_profit;

        TradeRecordAtom item;
        item.trade_id = GenerateTradeId();
        item.date = date;
        item.hhmm = hhmm;
        item.action = RecordAction::CLOSE;
        item.pos_type = PositionType::POS_SHORT;
        item.quantity = this_qty;
        item.price = price;
        item.profit = this_profit;
        item.fee = CalculateFee(item.quantity, price, true);
        ret.push_back(item);

        if( remain_tgt_qty == 0 )
            break;
    }// for
    if( p_profit )
        *p_profit = profit;
    return ret;
}

void PositionInfo::PushBack(bool is_long, std::shared_ptr<PositionAtom> &item)
{
    assert(item->trade_id >= 0);
    auto iter = position_holder_.insert(std::make_pair(item->trade_id, item)).first;
    if( is_long )
        long_positions_.push_back(iter->second.get());
    else
        short_positions_.push_back(iter->second.get());
}

PositionAtom * PositionInfo::PopBack(bool is_long)
{
    PositionAtom *ret = nullptr;
    if( is_long ) 
    {
        assert( !long_positions_.empty() );
        ret = long_positions_.at(long_positions_.size() - 1);
        long_positions_.pop_back();
    }else
    {
        assert( !short_positions_.empty() );
        ret = short_positions_.at(short_positions_.size() - 1);
        short_positions_.pop_back();
    }
    return ret;
}

PositionAtom * PositionInfo::FindPositionAtom(int id)
{
    auto iter = position_holder_.find(id);
    if( iter != position_holder_.end() )
        return iter->second.get();
    else
        return nullptr;
}


TradeRecordAtom  PositionInfo::ClosePositionAtom(int id, double price, double *capital_ret)
{
    //capital_ret = 0.0;
    PositionType  pos_type;
    bool is_find = false;
    auto iter = std::find_if(short_positions_.begin(), short_positions_.end(), [id](T_PositionAtoms::reference pos){ return pos->trade_id == id; });
    if( iter != short_positions_.end() )
    {
        pos_type = PositionType::POS_SHORT;
        is_find = true;
    }else
    {
        iter = std::find_if(long_positions_.begin(), long_positions_.end(), [id](T_PositionAtoms::reference pos){ return pos->trade_id == id; });
        if( iter != long_positions_.end() )
        {
            pos_type = PositionType::POS_LONG;
            is_find = true;
        }
    }

    if( is_find )
    {
        TradeRecordAtom trade_item;
        trade_item.trade_id = GenerateTradeId();
        trade_item.date = TSystem::Today();
        trade_item.hhmm = cur_hhmm();
        trade_item.action = RecordAction::CLOSE;
        trade_item.pos_type = pos_type;
        trade_item.price = price;
        trade_item.quantity = (*iter)->qty;  // close all position of this record 
        trade_item.profit =(*iter)->FloatProfit(trade_item.price);

        trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
         
        if( capital_ret )
            *capital_ret = cst_margin_capital * trade_item.quantity + trade_item.profit - trade_item.fee;

        if( pos_type == PositionType::POS_SHORT )
            short_positions_.erase(iter);
        else
            long_positions_.erase(iter);

        return trade_item;
    }else
        return TradeRecordAtom();

}


std::string ToStr(RecordAction action)
{
    switch(action)
    {
    case RecordAction::OPEN:  return "开"; break;
    case RecordAction::CLOSE: return "平"; break;
        //case RecordAction::UNFREEZE: return "解冻";break;
    default:assert(false);
    }
    return "";
}

std::string ToStr(PositionType pos_tp)
{
    switch(pos_tp)
    {
    case PositionType::POS_LONG:  return "多头"; break;
    case PositionType::POS_SHORT: return "空头"; break; 
    default:assert(false);
    }
    return "";
}


double CalculateFee(int quantity, double price, bool is_close)
{
    assert(quantity > 0.0);
    //assert(price > 0.0);
    return is_close ? quantity * 300.0 : quantity * 100.0;
}

int CalculateMaxQtyAllowOpen(double capital, double price)
{
    assert(capital > 0.0);
    assert(price > 0.0);
    int qty = capital / cst_margin_capital;
    if( qty < 1 )
        return 0;
    double fee = CalculateFee(qty, price, false);
    if( qty == 1 && qty * cst_margin_capital + fee + cst_per_tick_capital > capital )
        return 0;

    for( int i = 0; i < 100; ++i )
    {
        if( qty > 1 && qty * cst_margin_capital + fee + cst_per_tick_capital > capital ) // capital is not enough
        {
            qty -= 1;
            fee = CalculateFee(qty, price, false);
        }else
            break;
    }
    return qty < 1 ? 0 : qty;
}
