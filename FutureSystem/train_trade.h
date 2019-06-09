#ifndef TRAIN_TRADE_DSFSDF_H_
#define TRAIN_TRADE_DSFSDF_H_

#include <vector>
#include <tuple>
#include <QString>

#include "stkfo_common.h"

static const double cst_per_tick = 0.1; // pow(0.1, DEFAULT_DECIMAL)
static const double cst_per_tick_capital = 100.00;
static const double cst_margin_capital = 6000.00;
static const double cst_default_ori_capital = 1000000.00;
static const double cst_default_fee_rate_percent = 0.025;
static const int cst_default_ori_stock_num = 0;

enum PositionType : unsigned char
{
    POS_LONG  = 0, // ×ö¶à
    POS_SHORT = 1, // ×ö¿Õ
};
enum RecordAction : unsigned char
{
    OPEN = 0,
    CLOSE,
    //UNFREEZE, // only related to stock, this action is in the front of current day's other action
};

struct AccountAtom
{ 
    double  avaliable;
    double  frozen;
    double  float_profit;
    AccountAtom() : avaliable(0.0), frozen(0.0), float_profit(0.0){}
};

class TradeRecordAtom
{ 
public:
    int date;
    int hhmm;
    RecordAction action;
    PositionType pos_type;
    int quantity;
    double price;
    double profit;
    double fee;

    explicit TradeRecordAtom() : date(0), action(RecordAction::OPEN), pos_type(PositionType::POS_LONG), quantity(0), price(0.0), profit(0.0), fee(0.0){ }
    TradeRecordAtom(const TradeRecordAtom &lh) : date(lh.date), action(lh.action), pos_type(lh.pos_type), quantity(lh.quantity), price(lh.price), profit(lh.profit), fee(lh.fee){ }
    TradeRecordAtom & operator = (const TradeRecordAtom &lh)
    {
        if( &lh == this ) 
            return *this;
        date = lh.date; action = lh.action; pos_type = lh.pos_type;
        quantity = lh.quantity; price = lh.price; profit = lh.profit; fee = lh.fee;
        return *this;
    }

    QString ToQStr();
};

class PositionAtom
{
public:
    PositionAtom() : price(0.0), stop_loss_price(MAGIC_STOP_PRICE), stop_profit_price(MAGIC_STOP_PRICE), qty(0){}
    PositionAtom(const PositionAtom& lh) : price(lh.price),stop_loss_price(lh.stop_loss_price),stop_profit_price(lh.stop_profit_price), qty(lh.qty){}
    PositionAtom& operator = (const PositionAtom& lh)
    { 
        if( this == &lh ) 
            return *this; 
        else 
        {
            price = lh.price; 
            stop_loss_price = lh.stop_loss_price;
            stop_profit_price = lh.stop_profit_price;
            qty = lh.qty;
        }
        return *this; 
    }


    double price;
    double stop_loss_price;   // if < 0.0 means not set
    double stop_profit_price; // if < 0.0 means not set
    unsigned int qty;
};

class PositionInfo
{  
public:
     
    unsigned int LongPos();
    double LongAveragePrice();
    unsigned int ShortPos();
    double ShortAveragePirce();
    double FloatProfit(double price);
    // ret <low, high>
    std::tuple<double, double> GetForceClosePrices(double capital);

    std::vector<TradeRecordAtom> DoIfStopProfitLongPos(int date, int hhmm, double h_price, double *p_profit);
    std::vector<TradeRecordAtom> DoIfStopProfitShortPos(int date, int hhmm, double l_price, double *p_profit);
    std::vector<TradeRecordAtom> DoIfStopLossLongPos(int date, int hhmm, double l_price, double *p_profit);
    std::vector<TradeRecordAtom> DoIfStopLossShortPos(int date, int hhmm, double h_price, double *p_profit);
#if 0
    std::vector<TradeRecordAtom> DoIfStopProfit(int date, int hhmm, double h_price, double l_price, double *p_profit);
    std::vector<TradeRecordAtom> DoIfStopLoss(int date, int hhmm, double h_price, double l_price, double *p_profit);
#endif
    // return trades
    std::vector<TradeRecordAtom> CloseLong(int date, int hhmm, double price, unsigned int qty, double *p_profit);
    // return trades
    std::vector<TradeRecordAtom> CloseShort(int date, int hhmm, double price, unsigned int qty, double *p_profit);

    void PushBack(bool is_long, const PositionAtom& item)
    {
        if( is_long )
            long_positions_.push_back(item);
        else
            short_positions_.push_back(item);
    }

    PositionAtom PopBack(bool is_long);

private:

    std::vector<PositionAtom>  long_positions_;
    std::vector<PositionAtom>  short_positions_;
    /*int short_pos;
    double short_price_ave;*/
};

std::string ToStr(RecordAction action);
std::string ToStr(PositionType pos_tp);

double CalculateFee(int quantity, double price, bool is_close);
int CalculateMaxQtyAllowOpen(double capital, double price);

#endif // TRAIN_TRADE_DSFSDF_H_