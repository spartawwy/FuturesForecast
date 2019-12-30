#include "kline_wall.h"

#include <cassert>
#include <tuple>
 
#include <qdebug.h>
#include <qdatetime.h>

#include <TLib/core/tsystem_utility_functions.h>

#include "mainwindow.h"
#include "stkfo_common.h"
#include "futures_forecast_app.h"
#include "exchange_calendar.h"


using namespace TSystem;


void KLineWall::AppendData()
{ 
    int oldest_day = QDateTime::currentDateTime().toString("yyyyMMdd").toInt();
    if( !p_hisdata_container_->empty() )
        oldest_day = p_hisdata_container_->front()->stk_item.date;

    QDate qdate_obj(oldest_day/10000, (oldest_day%10000)/100, oldest_day%100);
    int start_date = qdate_obj.addDays( -1 * (4 * 30) ).toString("yyyyMMdd").toInt(); 
    switch(k_type_)
    {
    case TypePeriod::PERIOD_YEAR: start_date = qdate_obj.addDays( -1 * (5 * 12 * 30) ).toString("yyyyMMdd").toInt(); break;
    case TypePeriod::PERIOD_MON:
    case TypePeriod::PERIOD_WEEK: start_date = qdate_obj.addDays( -1 * (2 * 12 * 30) ).toString("yyyyMMdd").toInt(); break;
    case TypePeriod::PERIOD_DAY: start_date = qdate_obj.addDays( -1 * (1 * 12 * 30) ).toString("yyyyMMdd").toInt(); break;
    case TypePeriod::PERIOD_HOUR:start_date = qdate_obj.addDays( -1 * (6 * 30) ).toString("yyyyMMdd").toInt(); break;
    case TypePeriod::PERIOD_30M: start_date = qdate_obj.addDays( -1 * (3 * 30) ).toString("yyyyMMdd").toInt(); break;
    case TypePeriod::PERIOD_15M: start_date = qdate_obj.addDays( -1 * (1 * 30) ).toString("yyyyMMdd").toInt(); break;
    case TypePeriod::PERIOD_5M: start_date = qdate_obj.addDays( -1 * (1 * 5) ).toString("yyyyMMdd").toInt(); break;
    case TypePeriod::PERIOD_1M: start_date = qdate_obj.addDays( -1 * (1 * 3) ).toString("yyyyMMdd").toInt(); break;
    default: break;
    }
     
    app_->stock_data_man().AppendStockData(ToPeriodType(k_type_), nmarket_, stock_code_, start_date, oldest_day, is_index_);
}

void KLineWall::AppendPreData(int date)
{
    assert( date > 19800000 && date < 20500000 );

    int oldest_day = QDateTime::currentDateTime().toString("yyyyMMdd").toInt();
    if( !p_hisdata_container_->empty() )
        oldest_day = p_hisdata_container_->front()->stk_item.date;

    if( date > oldest_day )
        return; 
    app_->stock_data_man().AppendStockData(ToPeriodType(k_type_), nmarket_, stock_code_, date, oldest_day, is_index_);
}

void KLineWall::ResetTypePeriod(TypePeriod  type)
{ 
    if( k_type_ == type )
        return;
    ResetStock(stock_code_.c_str(), type, is_index_, nmarket_);
}

void KLineWall::UpdateKwallMinMaxPrice()
{
    std::tuple<double, double> price_tuple;
    std::tuple<int, int, int, int> date_times_tuple;
    if( GetContainerMaxMinPrice(ToPeriodType(k_type_), stock_code_, k_num_, price_tuple, date_times_tuple) )
    {
        float h_price = std::get<0>(price_tuple);
        float l_price = std::get<1>(price_tuple);
        double try_new_high = h_price * cst_k_mm_enlarge_times;
        if( h_price > 100.0 )
            try_new_high = h_price * 1.002;

        if( try_new_high < this->highestMaxPrice_ || try_new_high > this->highestMaxPrice_)
            SetHighestMaxPrice(try_new_high);

        double try_new_low = l_price * cst_k_mm_narrow_times;
        if( l_price > 100.0 )
            try_new_low = l_price * 0.998;
        if( try_new_low < this->lowestMinPrice_ || try_new_low > this->lowestMinPrice_)
            SetLowestMinPrice(try_new_low);
        highest_price_date_ = std::get<0>(date_times_tuple);
        highest_price_hhmm_ = std::get<1>(date_times_tuple);
        lowest_price_date_ = std::get<2>(date_times_tuple);
        lowest_price_hhmm_ = std::get<3>(date_times_tuple);
    }
}

PeriodType KLineWall::ToPeriodType(TypePeriod src)
{
    switch(src)
    {
    case TypePeriod::PERIOD_1M: return PeriodType::PERIOD_1M;
    case TypePeriod::PERIOD_5M: return PeriodType::PERIOD_5M;
    case TypePeriod::PERIOD_15M: return PeriodType::PERIOD_15M;
    case TypePeriod::PERIOD_30M: return PeriodType::PERIOD_30M;
    case TypePeriod::PERIOD_HOUR: return PeriodType::PERIOD_HOUR;
    case TypePeriod::PERIOD_DAY: return PeriodType::PERIOD_DAY;
    case TypePeriod::PERIOD_WEEK: return PeriodType::PERIOD_WEEK;
    case TypePeriod::PERIOD_MON: return PeriodType::PERIOD_MON;
    default: assert(false); 
    }
    return PeriodType::PERIOD_DAY;
}

double KLineWall::GetCurWinKLargetstVol()
{
    double largest_vol = 0.0;
    int k = k_num_;
    for( auto iter = p_hisdata_container_->rbegin() + k_rend_index_;
        iter != p_hisdata_container_->rend() && k > 0; 
        ++iter, --k)
        if( (*iter)->stk_item.vol > largest_vol ) 
            largest_vol = (*iter)->stk_item.vol;
    return largest_vol;
}

T_KlineDataItem * KLineWall::GetKLineDataItemByDate(int date, int hhmm)
{ 
    for( auto iter = p_hisdata_container_->rbegin();
        iter != p_hisdata_container_->rend(); 
        ++iter )
    {   
        if( iter->get()->stk_item.date == date && iter->get()->stk_item.hhmmss == hhmm )
            return iter->get(); 
    }
    return nullptr;
}

// dates : tuple<highest related date, highest related hhmm, lowest related date, lowest related hhmm>
bool KLineWall::GetContainerMaxMinPrice(PeriodType period_type, const std::string& code, int k_num, std::tuple<double, double>& ret, std::tuple<int, int, int, int> &date_times)
{
    T_HisDataItemContainer &container = app_->stock_data_man().GetHisDataContainer(period_type, code);
    if( container.empty() )
        return false;
    assert( container.size() > k_rend_index_ );

    unsigned int start_index = container.size() - k_rend_index_ > k_num ? (container.size() - k_rend_index_ - k_num) : 0; 
    if( start_index == container.size() )
        start_index = container.size() - 1;
    unsigned int end_index = container.size() - 1 > 0 ? container.size() - 1 - k_rend_index_ : 0;
    if( end_index < 0 )
        end_index = 0;
    if( end_index < start_index )
    {
        unsigned int temp_val = end_index;
        end_index = start_index;
        start_index = temp_val;
    }
    double highest_price = MIN_PRICE;
    double lowest_price = MAX_PRICE;
    int highest_price_date = 0;
    int highest_price_hhmm = 0;
    int lowest_price_date = 0;
    int lowest_price_hhmm = 0;
    for( unsigned int i = start_index; i <= end_index; ++ i )
    {
        if( container.at(i)->stk_item.high_price > highest_price )
        {
            highest_price = container.at(i)->stk_item.high_price; 
            highest_price_date = container.at(i)->stk_item.date;
            highest_price_hhmm = container.at(i)->stk_item.hhmmss;
        }
        if( container.at(i)->stk_item.low_price < lowest_price )
        {
            lowest_price = container.at(i)->stk_item.low_price;
            lowest_price_date = container.at(i)->stk_item.date;
            lowest_price_hhmm = container.at(i)->stk_item.hhmmss;
        }
    } 

    double tmp_price = forcast_man_.FindMaxForcastPrice(code, ToTypePeriod(period_type), container.at(start_index)->stk_item.date, container.at(end_index)->stk_item.date);
    if( tmp_price > highest_price )
        highest_price = tmp_price;
    tmp_price = forcast_man_.FindMinForcastPrice(code, ToTypePeriod(period_type), container.at(start_index)->stk_item.date, container.at(end_index)->stk_item.date);
    if( tmp_price < lowest_price )
        lowest_price = tmp_price;

    ret = std::make_tuple(highest_price, lowest_price);
    date_times = std::make_tuple(highest_price_date, highest_price_hhmm, lowest_price_date, lowest_price_hhmm);
    return true;
}

// ps : contain iter
int KLineWall::FindTopItem_TowardLeft(T_HisDataItemContainer &his_data, T_HisDataItemContainer::reverse_iterator iter, int k_index, T_KlinePosData *&left_pos_data)
{
    auto left_tgt_iter = iter;
    int cp_j = k_index;
    for( ; left_tgt_iter != his_data.rend() && cp_j > 0; 
        ++left_tgt_iter, --cp_j)
    {
        int type = (*left_tgt_iter)->type;
        auto left_frac_type = MaxFractalType(type);
        if( left_frac_type >= FractalType::TOP_AXIS_T_3 )
            break;
    }
    if( left_tgt_iter != his_data.rend() && cp_j > 0 )
    {
        left_pos_data = std::addressof(left_tgt_iter->get()->kline_posdata(wall_index_));
        return cp_j;
    }else
        return 0;
}

// ps : contain iter
int KLineWall::FindTopFakeItem_TowardLeft(T_HisDataItemContainer &his_data, T_HisDataItemContainer::reverse_iterator iter, int k_index, T_KlinePosData *&left_pos_data)
{
    auto left_tgt_iter = iter;
    int cp_j = k_index - 1;
    for( ; left_tgt_iter != his_data.rend() && cp_j > 0; 
        ++left_tgt_iter, --cp_j)
    { 
        if( (*left_tgt_iter)->type == (int)FractalType::TOP_FAKE )
            break;
    }
    if( left_tgt_iter != his_data.rend() && cp_j > 0 )
    {
        left_pos_data = std::addressof(left_tgt_iter->get()->kline_posdata(wall_index_));
        return cp_j;
    }else
        return 0;
}

// ps : not contain iter
int KLineWall::FindBtmItem_TowardLeft(T_HisDataItemContainer &his_data, T_HisDataItemContainer::reverse_iterator iter, int k_index, T_KlinePosData *&left_pos_data)
{
    auto left_tgt_iter = iter + 1;
    int cp_j = k_index - 1;
    for( ; left_tgt_iter != his_data.rend() && cp_j > 0; 
        ++left_tgt_iter, --cp_j) // find left btm_axis_iter 
    {
        auto left_frac_type = BtmestFractalType((*left_tgt_iter)->type);
        if( left_frac_type != FractalType::UNKNOW_FRACTAL )
            break;
    }

    if( left_tgt_iter != his_data.rend() && cp_j > 0 )
    {
        left_pos_data = std::addressof(left_tgt_iter->get()->kline_posdata(wall_index_));
        return cp_j;
    }else
        return 0;
}

// ps : contain iter
int KLineWall::FindBtmFakeItem_TowardLeft(T_HisDataItemContainer &his_data, T_HisDataItemContainer::reverse_iterator iter, int k_index, T_KlinePosData *&left_pos_data)
{
    auto left_tgt_iter = iter;
    int cp_j = k_index - 1;
    for( ; left_tgt_iter != his_data.rend() && cp_j > 0; 
        ++left_tgt_iter, --cp_j) // find left btm_axis_iter 
    {
        if( (*left_tgt_iter)->type == (int)FractalType::BTM_FAKE )
            break;
    }

    if( left_tgt_iter != his_data.rend() && cp_j > 0 )
    {
        left_pos_data = std::addressof(left_tgt_iter->get()->kline_posdata(wall_index_));
        return cp_j;
    }else
        return 0;
}

int KLineWall::Calculate_k_mm_h()
{
    int mm_h = this->height() - HeadHeight() - BottomHeight();
    for( unsigned int i = 0 ; i < zb_windows_.size(); ++i )
    {
        if( zb_windows_[i] )
            mm_h -= zb_windows_[i]->Height();
    }
    return mm_h;
}

void KLineWall::ClearForcastData()
{ 
    auto iter = forcast_man_.Find2pForcastVector(stock_code_, k_type_, true);
    if( iter )
        iter->clear();
    auto iter0 = forcast_man_.Find2pForcastVector(stock_code_, k_type_, false);
    if( iter0 )
        iter0->clear();
    auto iter_3pdown_vector = forcast_man_.Find3pForcastVector(stock_code_, k_type_, true);
    if( iter_3pdown_vector )
        iter_3pdown_vector->clear();
    auto iter_3pup_vector = forcast_man_.Find3pForcastVector(stock_code_, k_type_, false);
    if( iter_3pup_vector )
        iter_3pup_vector->clear();
}


// ret: <date, hhmm>
std::tuple<int, int> GetKDataTargetDateTime(ExchangeCalendar &exch_calender, TypePeriod type_period, QDate & date, QTime &time, int k_count)
{
    return GetKDataTargetDateTime(exch_calender, type_period, date.toString("yyyyMMdd").toInt(), time.hour() * 100 + time.minute(), k_count);
}

int CalculateSpanDays(TypePeriod type_period, int k_count)
{
    int span_day = 0;
    switch( type_period )
    {
    case TypePeriod::PERIOD_YEAR: 
        {
            if( k_count < 2 )
                span_day = -365;
            else
                span_day = -1 * k_count * 365;
        }break;
    case TypePeriod::PERIOD_MON:
        {
            if( k_count < 2 )
                span_day = -31;
            else
                span_day = -1 * (k_count * 30);
        }break;
    case TypePeriod::PERIOD_DAY:
        span_day = -1 * (k_count * 3 / 2);
        break;
    case TypePeriod::PERIOD_WEEK:
        span_day = -1 * (k_count * 30 / 20);
        break;
    case TypePeriod::PERIOD_HOUR:  // ndchk 
        {
            if( k_count * 30 / (20 * 4) < 1 )
                span_day = -1;
            else
                span_day = -1 * (k_count * 30 / (20 * 4));
            break;
        }
    case TypePeriod::PERIOD_30M:
        {
            if( k_count * 30 / (20 * 4 * 2) < 1 )
                span_day = -1;
            else
                span_day = -1 * (k_count * 30 / (20 * 4 * 2)); 
            break;
        }
    case TypePeriod::PERIOD_15M:
        {
            if( k_count * 30 / (20 * 4 * 2 * 2) < 1 )
                span_day = -1;
            else
                span_day = -1 * (k_count * 30 / (20 * 4 * 2 * 2));
            break;
        }
    case TypePeriod::PERIOD_5M:
        {
            if( k_count * 30 / (20 * 4 * 2 * 2 * 3) < 1 )
                span_day = -1;
            else
                span_day = -1 * (k_count * 30 / (20 * 4 * 2 * 2 * 3)); 
            break;
        }
    case TypePeriod::PERIOD_1M:
        {
            if( k_count * 30 / (20 * 4 * 2 * 2 * 3 * 5) < 1 )
                span_day = -1;
            else
                span_day = -1 * (k_count * 30 / (20 * 4 * 2 * 2 * 3 * 5)); 
            break;
        }
    default:assert(false);
    }
    return span_day;
}

 
std::tuple<int, int> GetKDataTargetDateTime(ExchangeCalendar &exch_calender, TypePeriod type_period, int end_date, int tmp_hhmm, int max_k_count)
{
    static auto get_hhmm = [](int hhmm_para, int *tp_array, int num)->int
    {
        assert(num > 0);
        for( int i = 0; i < num; ++i )
        {
            if( hhmm_para <= tp_array[i] ) 
                return tp_array[i];
        }
        return tp_array[num-1];
    };
    int pre_days = CalculateSpanDays(type_period, max_k_count);

    int start_date = exch_calender.PreTradeDate(end_date, abs(pre_days));
     
    int hhmm = 0;
    switch( type_period )
    {
    case TypePeriod::PERIOD_YEAR: 
    case TypePeriod::PERIOD_MON:
    case TypePeriod::PERIOD_DAY:
    case TypePeriod::PERIOD_WEEK:
        break;
    case TypePeriod::PERIOD_HOUR:  // ndchk 
        {
            //10:30 13:00 14:00 15:00
            int tp_array[] = {30, 1030, 1300, 1400, 1500};
            hhmm = get_hhmm(tmp_hhmm, tp_array, sizeof(tp_array)/sizeof(tp_array[0]));
            break;
        }
    case TypePeriod::PERIOD_30M:
        {
            int tp_array[] = {30, 100, 130, 200, 230, 930, 1000, 1030, 1100, 1130, 1330, 1400, 1430, 1500, 2130, 2200, 2230, 2300, 2330, 2359};
            hhmm = get_hhmm(tmp_hhmm, tp_array, sizeof(tp_array)/sizeof(tp_array[0]));
            break; 
        }
    case TypePeriod::PERIOD_15M:
        {
            int tp_array[] = { 945, 1000, 1015, 1030, 1045, 1100, 1115, 1130, 1315, 1330, 1345, 1400, 1415, 1430, 1445, 1500};
            hhmm = get_hhmm(tmp_hhmm, tp_array, sizeof(tp_array)/sizeof(tp_array[0]));
            break;
        }
    case TypePeriod::PERIOD_5M:
        {
            // ndedt
            int tp_array[] = {5, 10, 15,20,25,30,35,40,45,50,55,100,105,110,115,120,125,130,135,140,145,150,155
                ,200,205,210,215,220,225,230
                ,935,940,945,950,955,1000,1005,1010,1015,1035,1040,1045,1050,1055,1100,1105
                ,1110,1115,1120,1125,1130,1335,1340,1345,1350,1355,1400,1405
                ,1410,1415,1420,1425,1430,1435,1440,1445,1450,1455,1500
                , 2105, 2110,2115, 2120, 2125,2130,2135,2140,2145,2150,2155,2200
                , 2205, 2210,2215, 2220, 2225,2230,2235,2240,2245,2250,2255,2300
                , 2305, 2310,2315, 2320, 2325,2330,2335,2340,2345,2350,2355,2359};
            hhmm = get_hhmm(tmp_hhmm, tp_array, sizeof(tp_array)/sizeof(tp_array[0]));
            if( hhmm == 2359 )
                hhmm = 0;
            break;
        }
    case TypePeriod::PERIOD_1M:
        hhmm = tmp_hhmm;
        break;
    }
    /*QDate q_date(end_date/10000, (end_date%10000)/100, end_date%100);
    int start_date = q_date.addDays(span_day).toString("yyyyMMdd").toInt();*/
    return std::make_tuple(start_date, hhmm);
}

int GetKDataTargetStartTime(TypePeriod type_period, int para_hhmm)
{ 
    int hhmm = 0;  
    switch( type_period )
    {
    case TypePeriod::PERIOD_YEAR: 
    case TypePeriod::PERIOD_MON: 
    case TypePeriod::PERIOD_DAY:
    case TypePeriod::PERIOD_WEEK: 
        break;
    case TypePeriod::PERIOD_HOUR:   
        hhmm = 1030;
        break; 
    case TypePeriod::PERIOD_30M: 
        hhmm = 1000;
        break; 
    case TypePeriod::PERIOD_15M: 
        hhmm = 945;
        break; 
    case TypePeriod::PERIOD_5M: 
        if( para_hhmm < 900 )
            hhmm = 0;
        else
            hhmm = 905;
        break; 
    case TypePeriod::PERIOD_1M: 
        if( para_hhmm < 900 )
            hhmm = 0;
        else
            hhmm = 901;
        break; 
    }
    return hhmm;
}

// ps: from p_hisdata_container back to front
int FindKRendIndex(T_HisDataItemContainer *p_hisdata_container, int date_val, int hhmm)
{
    bool is_find = false;
    int j = 0;
    int near_span = 99999;
    int near_j = -1;
    for( auto iter = p_hisdata_container->rbegin();
        iter != p_hisdata_container->rend(); 
        ++iter, ++j )
    { 
        if( iter->get()->stk_item.date == date_val && iter->get()->stk_item.hhmmss == hhmm )
        {
            is_find = true;
            break;
        }else if( iter->get()->stk_item.date == date_val && iter->get()->stk_item.hhmmss < hhmm )
        {
            near_span = date_val - iter->get()->stk_item.date;
            near_j = j;
            break;
        }
        else if( iter->get()->stk_item.date < date_val )
        {
            near_span = date_val - iter->get()->stk_item.date;
            near_j = j;
            break;
        }

    }
    if( is_find )
        return j;
    else
        return near_j;
}