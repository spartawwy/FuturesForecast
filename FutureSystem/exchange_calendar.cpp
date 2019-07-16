#include "exchange_calendar.h"

#include <ctime>
#include <chrono>
#include <TLib/core/tsystem_time.h>
 
ExchangeCalendar::ExchangeCalendar() : min_trade_date_(0), max_trade_date_(0)
{ 
    trade_dates_ = std::make_shared<T_DateMapIsopen>(10*1024);
}

bool ExchangeCalendar::IsTradeDate(int date)
{
     assert(trade_dates_->size() > 0);

     T_DateMapIsopen &date_map_opend = *trade_dates_;
     auto iter = date_map_opend.find(date);
     return iter != date_map_opend.end() && iter->second;
}

bool ExchangeCalendar::IsTradeTime(int hhmm)
{ 
    //return (hhmm >= 900 &&  hhmm <= 1500) || IsMidNightTradeTime(hhmm);
    return (hhmm >= 900 &&  hhmm <= 1130) || (hhmm >= 1330 && hhmm <= 1500) || IsMidNightTradeTime(hhmm);
}

bool ExchangeCalendar::IsMidNightTradeTime(int hhmm)
{
    return hhmm >= 2100 || hhmm <= 230;
}


// ps: ceiling trade date may be bigger then param date. if fail return 0
int ExchangeCalendar::CeilingTradeDate(int date)
{
    assert(trade_dates_->size() > 0); 
    T_DateMapIsopen &date_map_opend = *trade_dates_;
    int a = 0;
    for( int i = 0; i < 30; ++i )
    {
        a = DateAddDays(date, i);
        if( a > max_trade_date_ )
            return 0;
        auto iter = date_map_opend.find(a);
        if( iter != date_map_opend.end() && iter->second )
           return a;
    }
    return 0;
}

// ps: ceiling trade date may be smaller then param date. if fail return 0
int ExchangeCalendar::FloorTradeDate(int date)
{
    assert(trade_dates_->size() > 0); 
    T_DateMapIsopen &date_map_opend = *trade_dates_;
    int a = 0;
    for( int i = 0; i < 30; ++i )
    {
        a = DateAddDays(date, -1 * i);
        if( a < min_trade_date_ )
            return 0;
        auto iter = date_map_opend.find(a);
        if( iter != date_map_opend.end() && iter->second )
            return a;
    }
    return 0;
}

// pre n trade date 
int ExchangeCalendar::PreTradeDate(int date, unsigned int n)
{   
    assert(trade_dates_->size() > 0);
    // auto date_time_point = TSystem::MakeTimePoint(date/10000, (date % 10000) / 100, date % 100);
    unsigned int count = 0;
    int i = 1;
    T_DateMapIsopen &date_map_opend = *trade_dates_;
    int a = 0;
    while( count < n )
    {
        a = DateAddDays(date, -1 * i);  
        if( a < min_trade_date_ )
            return 0;
        auto iter = date_map_opend.find(a);
        if( iter != date_map_opend.end() && iter->second )
            ++count;
        ++i;
    }
    return a;
}

int ExchangeCalendar::NextTradeDate(int date, unsigned int n)
{   
    assert(trade_dates_->size() > 0);
    //auto date_time_point = TSystem::MakeTimePoint(date/10000, (date % 10000) / 100, date % 100);
    unsigned int count = 0;
    int i = 1;
    T_DateMapIsopen &date_map_opend = *trade_dates_;
    int a = 0;
    while( count < n )
    {
        a = DateAddDays(date, i);  
        if( a > max_trade_date_ )
            return 0;
        auto iter = date_map_opend.find(a);
        if( iter != date_map_opend.end() && iter->second )
            ++count;
        ++i;
    }
    return a;
}

int Get5mLackKnum(int hhmm)
{
    int lack_k_num = 0;
    // calculate Cur tranding day's now k num
    if( hhmm <= 230 )
    { 
        int already_num = ((hhmm / 100) * 60 / 5) + ((hhmm % 100) + 5) / 5;
        lack_k_num = 111 - 66 + (30 - already_num);
    }else if( hhmm < 900 )
    {
        // such as 05:55
        lack_k_num = 111 - 66; 
    }else if( hhmm <= 1015 )
    {
        lack_k_num = 111 - 66 - (hhmm/100 - 9) * 12 - (hhmm%100 / 5 + 1);
    }else if( hhmm <= 1130 )
    {
        if( hhmm/100 < 11 ) // 1035-1059
            lack_k_num = 111 - 66 - 15 - ( (hhmm%100 - 30) / 5 + 1);
        else
            lack_k_num = 111 - 66 - 15 - 5 - ( hhmm%100 / 5 + 1);
    }else if( hhmm < 1330 )
    {
        lack_k_num = 111- 66 -27;
    }else if( hhmm < 1400 )
    {
        lack_k_num = 111- 66 - 27 - ((hhmm%100 - 30) / 5  + 1);
    }else if( hhmm < 1500 )
    {
        lack_k_num = 111- 66 - 27 - 5 - (hhmm%100 / 5 + 1);
    }else if ( hhmm < 2100 )
    {
        lack_k_num = 0;
    }else  // 2100-2359
    {
        int already_num = ((hhmm / 100 - 21) * 60 / 5) + ((hhmm % 100) + 5) / 5;
        lack_k_num = 111 - already_num;
    }
    return lack_k_num;
}


int Get1mLackKnum(int hhmm)
{
    int lack_k_num = 0;
    // calculate Cur tranding day's now k num
    if( hhmm <= 230 )
    { 
        int already_num = ((hhmm / 100) * 60) + (hhmm % 100);
        lack_k_num = (111 - 66)*5 + (30*5 - already_num);
    }else if( hhmm < 900 )
    {
        // such as 05:55
        lack_k_num = (111 - 66)*5; 
    }else if( hhmm <= 1015 )
    {
        lack_k_num = (111 - 66)*5 - (hhmm/100 - 9) * 60 - hhmm%100;
    }else if( hhmm <= 1130 ) // 1035-1130
    {
        if( hhmm/100 < 11 ) // 1035-1059
            lack_k_num = (111 - 66 - 15)*5 - (hhmm%100 - 30);
        else // 1100-1130
            lack_k_num = (111 - 66 - 15 - 5)*5 - hhmm%100;
    }else if( hhmm < 1330 )
    {
        lack_k_num = (111- 66 -27)*5;
    }else if( hhmm < 1400 )
    {
        lack_k_num = (111- 66 -27)*5 - (hhmm%100 - 30);
    }else if( hhmm < 1500 )
    {
        lack_k_num = (111- 66 - 27 - 5)*5 - hhmm%100;
    }else if ( hhmm < 2100 )
    {
        lack_k_num = 0;
    }else  // 2100-2359
    {
        int already_num = ((hhmm / 100 - 21) * 60) + (hhmm % 100);
        lack_k_num = 111*5 - already_num;
    }
    return lack_k_num;
}

// ps: end_date <= today .  inner use current time
T_TupleIndexLen ExchangeCalendar::GetStartIndexAndLen_backforward(TypePeriod type_period, int start_date, int end_date)
{ 
    assert(trade_dates_->size() > 0);
    assert(start_date <= end_date);

    time_t rawtime;
    struct tm timeinfo;
    time( &rawtime );
    localtime_s(&timeinfo, &rawtime); 
    const int hhmm = timeinfo.tm_hour * 100 + timeinfo.tm_min;

    int today = TSystem::Today();
    
   
    int lack_k_num = 0;
    if( IsTradeDate(today) )
    {
        if( type_period == TypePeriod::PERIOD_5M )
        {
            lack_k_num = Get5mLackKnum(hhmm);
        }else if( type_period == TypePeriod::PERIOD_1M )
        {
            lack_k_num = Get1mLackKnum(hhmm);
        }
    }
    // after 2100 procedure as next trade day
    if( hhmm >= 2100 )
    {
        today = DateAddDays(today, 1);
    }
    const int latest_trade_date = FloorTradeDate(today);

    int actual_start_date = CeilingTradeDate(start_date);

    int actual_end_date = end_date;
    if( actual_end_date >= today )
        actual_end_date = today;
    actual_end_date = FloorTradeDate(actual_end_date);

    int start_index = DateTradingSpan(actual_end_date, latest_trade_date);

    int span_len = DateTradingSpan(actual_start_date, actual_end_date) + 1;
     
    switch( type_period )
    {
    case TypePeriod::PERIOD_DAY: break;
    case TypePeriod::PERIOD_WEEK:
        if( start_index > 5 )
            start_index /= 5;
        if( span_len > 5 )
            span_len /= 5;
        break;
    case TypePeriod::PERIOD_HOUR:
        start_index *= 4;
        span_len *= 4;
        break;
    case TypePeriod::PERIOD_30M:
        start_index *= 8;
        span_len *= 8;
        break;
    case TypePeriod::PERIOD_15M:
        start_index *= 16;
        span_len *= 16;
        break;
    case TypePeriod::PERIOD_5M:
        start_index *= 111;
        span_len *= 111;
        break;
    case TypePeriod::PERIOD_1M:
        start_index *= 555;
        span_len *= 555;
        break;
    }
    if( start_index >= lack_k_num )
    {
        start_index -= lack_k_num;
    }
    if( span_len >= lack_k_num )
    {
        span_len -= lack_k_num;
    }
    return std::make_tuple(start_index, span_len);
}

// return span of trading dates between
// ps: start_date <= end_date
int  ExchangeCalendar::DateTradingSpan(int start_date, int end_date)
{
    assert(start_date <= end_date);

    if( start_date == end_date ) 
        return 0;
    int target_end_date = end_date;
    if( !IsTradeDate(target_end_date) )
        target_end_date = PreTradeDate(target_end_date, 1);

    int target_start_date = start_date;
    if( !IsTradeDate(target_start_date) )
        target_start_date = NextTradeDate(target_start_date, 1);
    if( target_end_date <= target_start_date ) 
        return 0;

    T_DateMapIsopen &date_map_opend = *trade_dates_;
    int tmp_date = 0;
    unsigned int span = 0;
    int i = 1;
    do{
        tmp_date = DateAddDays(target_end_date, -1 * i++);  
        auto iter = date_map_opend.find(tmp_date);
        if( iter != date_map_opend.end() && iter->second )
            ++span;
    }while( target_start_date < tmp_date );

    return span;
}

int ExchangeCalendar::TodayAddDays(int days)
{
    std::time_t day_t = 0;
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    if( days >= 0 )
        day_t = system_clock::to_time_t(now + std::chrono::hours(24*days));
    else
        day_t = system_clock::to_time_t(now - std::chrono::hours(24*abs(days)));
    tm tm_day_t;
    _localtime64_s(&tm_day_t, &day_t);
    return (tm_day_t.tm_year + 1900) * 10000 + (tm_day_t.tm_mon + 1) * 100 + tm_day_t.tm_mday;
}

int ExchangeCalendar::DateAddDays(int date, int days)
{
    auto date_time_point = TSystem::MakeTimePoint(date/10000, (date % 10000) / 100, date % 100);
    std::time_t day_t = 0;

    using namespace std::chrono;
    if( days >= 0 )
        day_t = system_clock::to_time_t(date_time_point + std::chrono::hours(24*days));
    else
        day_t = system_clock::to_time_t(date_time_point - std::chrono::hours(24*abs(days)));
    tm tm_day_t;
    _localtime64_s(&tm_day_t, &day_t);
    return (tm_day_t.tm_year + 1900) * 10000 + (tm_day_t.tm_mon + 1) * 100 + tm_day_t.tm_mday;
}