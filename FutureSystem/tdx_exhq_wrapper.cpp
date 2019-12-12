#include "tdx_exhq_wrapper.h"

#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <cassert>
#include <regex>

#include "tdxhq/ExHqApi.h"
#include <TLib/core/tsystem_local_logger.h>
#include <TLib/core/tsystem_utility_functions.h>

#include "exchange_calendar.h"

#include <qdebug.h>

#pragma comment(lib, "TradeX2-M.lib")

TdxExHqWrapper::TdxExHqWrapper(ExchangeCalendar  *exchange_calendar, TSystem::LocalLogger &local_logger)
    : conn_handle_(-1)
    , conn_handle_mutext_()
    , exchange_calendar_(exchange_calendar)
    , local_logger_(local_logger)
{

}

TdxExHqWrapper::~TdxExHqWrapper() 
{ 
    if( conn_handle_ > -1 ) 
        TdxExHq_Disconnect(conn_handle_); 
}

bool TdxExHqWrapper::Init()
{
   return ConnectServer();
}

bool TdxExHqWrapper::ConnectServer()
{ 
    std::lock_guard<std::mutex> locker(conn_handle_mutext_);
    conn_handle_ = _ConnectServer();
    return conn_handle_ > -1;
}

void TdxExHqWrapper::DisConnect()
{
    std::lock_guard<std::mutex> locker(conn_handle_mutext_);

    if( conn_handle_ > -1 )
        TdxExHq_Disconnect(conn_handle_);
    conn_handle_ = -1;
}

bool TdxExHqWrapper::ReconnectServer()
{ 
    std::lock_guard<std::mutex> locker(conn_handle_mutext_);

    if( conn_handle_ > -1 )
        TdxExHq_Disconnect(conn_handle_);

    conn_handle_ = _ConnectServer();
    return conn_handle_ > -1;
}

bool TdxExHqWrapper::IsConnhandleValid()
{
    std::lock_guard<std::mutex> locker(conn_handle_mutext_);
    return conn_handle_ > -1;
}


int TdxExHqWrapper::_ConnectServer()
{ 
    int handle = -1;
    const int cst_result_len = 1024 * 1024;
    const int cst_err_len = 1024; 

    char* m_szResult = new char[cst_result_len];
    char* m_szErrInfo = new char[cst_err_len];

    memset(m_szResult, 0, cst_result_len);
    memset(m_szErrInfo, 0, cst_err_len);

    //连接服务器
    char * servers[] = {
        "106.14.95.149"
        , "59.175.238.38"
        , "112.74.214.43"};
    for( int i = 0; i < sizeof(servers)/sizeof(servers[0]); ++i )
    {
        handle = TdxExHq_Connect(servers[i], 7727, m_szResult, m_szErrInfo);
        if( handle < 0 )
        {
            handle = TdxExHq_Connect(servers[i], 7727, m_szResult, m_szErrInfo);
            if( handle < 0 )
            {
                std::cout << m_szErrInfo << std::endl;
                continue;
            }else
            {
                std::cout << m_szResult << std::endl;
                return handle;
            }
        }else
        {
            std::cout << m_szResult << std::endl;
            return handle;
        }
    } 
    return handle;
}

// items date is from small to big
bool TdxExHqWrapper::GetHisKBars(const std::string &code, bool is_index, int nmarket, TypePeriod kbar_type, int start_date, int end_date, std::vector<T_StockHisDataItem> &items)
{  
    static auto get_indexs_one_day = [](TypePeriod type_period)->int
    { 
        switch( type_period )
        { 
        case TypePeriod::PERIOD_HOUR:  return 4;
        case TypePeriod::PERIOD_30M: return 8;
        case TypePeriod::PERIOD_15M: return 16; 
        case TypePeriod::PERIOD_5M: return 111;  
        case TypePeriod::PERIOD_1M: return 555;  
        default: return 1;
        }
    };

    std::vector<T_K_Data> resut;
    const int cst_result_len = 1024 * 1024;
    const int cst_err_len = 1024; 
       
    // 因为有个股停牌情况,所以 计算日期目标 不一定准确
    auto tuple_index_len = exchange_calendar_->GetStartIndexAndLen_backforward(kbar_type, start_date, end_date);
    
    //const short max_count = MAX_K_COUNT;
    short start = std::get<0>(tuple_index_len);  // back forward : <---
    const short count = std::get<1>(tuple_index_len);
 
    short local_count = MAX_K_COUNT;
    short total_get = 0;
    
    start += count % MAX_K_COUNT;
    if( count / MAX_K_COUNT > 0 )
        start += (count / MAX_K_COUNT - 1) * MAX_K_COUNT;
    for( int i = count / MAX_K_COUNT ; i > 0; --i )
    {
        local_count = MAX_K_COUNT;
        // tdx api get data from startindex to left(oldest date):     <---len---0 
        __GetHisKBars(code, is_index, nmarket, kbar_type, start, local_count, items);
        if( i > 1 )
            start -= local_count;
        total_get += local_count;
    } 
    if( count % MAX_K_COUNT > 0 )
    {
        start -= count % MAX_K_COUNT;
        local_count = count % MAX_K_COUNT;
        __GetHisKBars(code, is_index, nmarket, kbar_type, start, local_count, items);
        total_get += local_count;
    }
     
    local_logger_.LogLocal(TSystem::utility::FormatStr("GetHisKBars %d | %d %d | %d %d | %d %d", kbar_type, start_date, end_date, std::get<0>(tuple_index_len), count
        , items.back().date, items.back().hhmmss));

    return total_get > 0;
     
}

bool TdxExHqWrapper::GetLatestKBar(const std::string &code, bool is_index, int nmarket, TypePeriod kbar_type, T_StockHisDataItem &item)
{
    std::vector<T_StockHisDataItem>  items;
    short count = 1;
    if( __GetHisKBars(code, is_index, nmarket, kbar_type, 0, count, items) )
    {
        memcpy(&item, &items[0], sizeof(T_StockHisDataItem));
        return true;
    }else
        return false;
}

// items date is from small to big; // get data from start index to left(oldest date):     <---len---0 
bool TdxExHqWrapper::__GetHisKBars(const std::string &code, bool is_index, int nmarket, TypePeriod kbar_type, short start, short &count, std::vector<T_StockHisDataItem> &items)
{  
    bool result = false;
  
    // 获取指定品种的K线数据 
    bool (WINAPI* pFuncGetInstrumentBars)(
        int nConnID,
        char nCategory,
        char nMarket,
        const char* pszZqdm,
        int nStart,
        short* pnCount,
        char* pszResult,
        char* pszErrInfo);

    //bool1 = TdxExHq_GetInstrumentBars(nConn, 7, 30, "SC1907", 0, &Count, Result, ErrInfo);
     
    pFuncGetInstrumentBars = &TdxExHq_GetInstrumentBars;
     
    std::vector<T_K_Data> resut;
    const int cst_result_len = 1024 * 1024;
    const int cst_err_len = 1024; 

    char* m_szResult = new char[cst_result_len];
    char* m_szErrInfo = new char[cst_err_len];

    memset(m_szResult, 0, cst_result_len);
    memset(m_szErrInfo, 0, cst_err_len);
    
    //K线数据数据种类, 0->5分钟K线    1->15分钟K线    2->30分钟K线  3->1小时K线    4->日K线  5->周K线  6->月K线  7->1分钟K线  8->1分钟K线  9->日K线  10->季K线  11->年K线
    int ktype = 4;
    switch(kbar_type)
    {
    case TypePeriod::PERIOD_YEAR: ktype = 11; 
        break;
    case TypePeriod::PERIOD_MON:  ktype = 6; 
        break;
    case TypePeriod::PERIOD_WEEK: ktype = 5; 
        break;
    case TypePeriod::PERIOD_DAY:  ktype = 4; 
        break;
    case TypePeriod::PERIOD_HOUR: ktype = 3; 
        break;
    case TypePeriod::PERIOD_30M:  ktype = 2; 
        break;
    case TypePeriod::PERIOD_15M:  ktype = 1; 
        break;
    case TypePeriod::PERIOD_5M:   ktype = 0; 
        break;
    case TypePeriod::PERIOD_1M:   ktype = 7; 
        break;
    default:
        assert(false);
        ktype = 4; 
        break;
    }
do 
{ 
    //bool1 = TdxExHq_GetInstrumentBars(nConn, 4, 30, "SC1907", 0, &Count, Result, ErrInfo);
    if( !IsConnhandleValid() )
    {
        if( !IsConnhandleValid() ) // judge again
        {
           if( !ReconnectServer() )
               break;
        }
    }

    bool ret = false;

    do
    {
        std::lock_guard<std::mutex>  locker(conn_handle_mutext_);
        if( conn_handle_ < 0 ) 
            break;
        ret = pFuncGetInstrumentBars(conn_handle_, ktype, nmarket, const_cast<char*>(code.c_str()), start, &count, m_szResult, m_szErrInfo);
    }while(0);

    if( !ret )
    { 
        if( ReconnectServer() )
        {
            do
            {
                std::lock_guard<std::mutex>  locker(conn_handle_mutext_);
                if( conn_handle_ < 0 ) 
                    break;
                ret = pFuncGetInstrumentBars(conn_handle_, ktype, nmarket, const_cast<char*>(code.c_str()), start, &count, m_szResult, m_szErrInfo);
            }while(0);
            if( !ret )
                break;
        } else
            break;
    }
    if( strlen(m_szResult) < 1 )
    {
        std::cout << " result empty !" << std::endl;
        break;
    }
    qDebug() << m_szResult << "\n";

    const bool has_time = ( ktype < 4 || ktype == 7 || ktype == 8 ) ? true : false;
    std::string expresstion_str;
    if( has_time )
    {
        //2019-05-30 15:00  476.100037      476.500031      475.900024      476.100037      32384   2802    0.000000
        //时间                     开盘价         最高价           最低价          收盘价        持仓    成交    结算
        expresstion_str = "^(\\d{4})-(\\d{2})-(\\d{2})\\s+(\\d{2}):(\\d{2})\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+).*";
    }else
    {   //2019-05-30        476.100037      476.500031      475.900024      476.100037      32384   2802    0.000000
        expresstion_str = "^(\\d{8})\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+).*";
    }
    std::regex  regex_obj(expresstion_str);
    char *p = m_szResult;
    while( *p != '\0' && *p != '\n') ++p;

    ++p;

    char *p_line_beg = p;
    while( *p != '\0')
    {
        p_line_beg = p;
        while( *p != '\0' && *p != '\n')++p;
        *p = '\0';
        ++p;
        std::string src_str = p_line_beg;

        std::smatch  match_result;
        if( std::regex_match(src_str.cbegin(), src_str.cend(), match_result, regex_obj) )
        {
            int index = 1;
            T_StockHisDataItem  k_data;
            try
            {
                //std::cout << match_result[index] << " "; // date

                if( has_time )
                { 
                    int year = boost::lexical_cast<int>(match_result[index]);
                    ++index;
                    int mon = boost::lexical_cast<int>(match_result[index]);
                    ++index;
                    int day = boost::lexical_cast<int>(match_result[index]);
                    ++index;
                    k_data.date = year * 10000 + mon * 100 + day;
                    int hour = boost::lexical_cast<int>(match_result[index]);
                    ++index;  
                    int minute = boost::lexical_cast<int>(match_result[index]);
                    k_data.hhmmss = hour * 100 + minute;
#if 1
                    if( k_data.hhmmss > 2100 )
                    {
                        k_data.date = exchange_calendar_->PreTradeDate(k_data.date, 1);
                    }
#endif 
                }else
                {
                    k_data.date = boost::lexical_cast<int>(match_result[index]);
                    k_data.hhmmss = 0;
                }

                ++index;
                k_data.open_price = boost::lexical_cast<double>(match_result[index]);
                ++index;
                k_data.high_price = boost::lexical_cast<double>(match_result[index]);
                ++index;
                k_data.low_price = boost::lexical_cast<double>(match_result[index]);
                ++index;
                k_data.close_price = boost::lexical_cast<double>(match_result[index]);
                ++index;
                int hold = boost::lexical_cast<int>(match_result[index]);
                ++index;
                k_data.vol = boost::lexical_cast<double>(match_result[index]);
                ++index;
                k_data.capital = boost::lexical_cast<double>(match_result[index]); // 结算价
                items.push_back(std::move(k_data));

            }catch(boost::exception& e )
            {
                e = e; 
            }
        }else
            qDebug() << "match fail!\n";
    }
    //std::cout << m_szResult << std::endl;
    result = true; 

} while (0);

    delete []m_szResult;
    delete []m_szErrInfo;

    return result;
}

bool TdxExHqWrapper::GetInstrumentQuote(const std::string &code, int nmarket, T_Quote_Data &ret_quote_data)
{ 
    bool (WINAPI* pFuncGetInstrumentQuote)(
        int nConnID,
        char nMarket,
        const char* pszZqdm,
        char* pszResult,
        char* pszErrInfo);
    pFuncGetInstrumentQuote = &TdxExHq_GetInstrumentQuote;


    const int cst_result_len = 1024 * 1024;
    const int cst_err_len = 1024; 

    char m_szResult[1024] = {'\0'};
    char m_szErrInfo[1024] = {'\0'};
      
    memset(&ret_quote_data, 0, sizeof(ret_quote_data));
    bool result = false;
    do 
    {  
        if( !IsConnhandleValid() )
        {
            if( !IsConnhandleValid() ) // judge again
            {
                if( !ReconnectServer() )
                    break;
            }
        }
        
        bool ret = false;
        do
        {
            std::lock_guard<std::mutex>  locker(conn_handle_mutext_);
            if( conn_handle_ < 0 ) 
                break;
            ret = pFuncGetInstrumentQuote(conn_handle_, nmarket, const_cast<char*>(code.c_str()), m_szResult, m_szErrInfo);
        }while(0);
        if( !ret )
        { 
            if( ReconnectServer() )
            {
                do
                {
                    std::lock_guard<std::mutex>  locker(conn_handle_mutext_);
                    if( conn_handle_ < 0 ) 
                       break;
                    ret = pFuncGetInstrumentQuote(conn_handle_, nmarket, const_cast<char*>(code.c_str()), m_szResult, m_szErrInfo);
                }while(0);
                if( !ret )
                    break;
            } else
                break;
        }
        if( strlen(m_szResult) < 1 )
        {
            std::cout << " result empty !" << std::endl;
            break;
        }
        //qDebug() << m_szResult << "\n";

        std::string expresstion_str = "^(\\d+)\\s+(\\w+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+.*";
        std::regex  regex_obj(expresstion_str);
        char *p = m_szResult;
        while( *p != '\0' && *p != '\n') ++p;

        ++p;

        std::string src_str = p;

        std::smatch  match_result;
        if( std::regex_match(src_str.cbegin(), src_str.cend(), match_result, regex_obj) )
        {
            //cout <<  "cur_price:" << match_result[7] << "buy1:" << match_result[14] << " buy1_vol:" << match_result[19] << std::endl;
            ret_quote_data.cur_price = boost::lexical_cast<double>(match_result[7]);
            ret_quote_data.vol = boost::lexical_cast<double>(match_result[11]);
            ret_quote_data.buy_price = boost::lexical_cast<double>(match_result[14]);
            ret_quote_data.buy_vol = boost::lexical_cast<int>(match_result[19]);
            //cout << "sell1:" <<  match_result[24] << " sell1_vol:" << match_result[29] << std::endl;
            ret_quote_data.sell_price = boost::lexical_cast<double>(match_result[24]);
            ret_quote_data.sell_vol = boost::lexical_cast<int>(match_result[29]);
            result = true;
        }else
            result = false;

    }while(0);

    return result;
}