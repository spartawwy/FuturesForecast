#include "tdx_exhq_wrapper.h"

#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <cassert>
#include <regex>

#include "tdxhq/ExHqApi.h"
#include "exchange_calendar.h"

#include <qdebug.h>

#pragma comment(lib, "TradeX2-M.lib")

TdxExHqWrapper::TdxExHqWrapper(ExchangeCalendar  *exchange_calendar)
    : conn_handle_(0)
    , exchange_calendar_(exchange_calendar)
{

}

TdxExHqWrapper::~TdxExHqWrapper() 
{ 
    if( conn_handle_ ) 
        TdxExHq_Disconnect(conn_handle_); 
}

bool TdxExHqWrapper::Init()
{
   return ConnectServer();
}

bool TdxExHqWrapper::ConnectServer()
{
    //开始获取行情数据

    const int cst_result_len = 1024 * 1024;
    const int cst_err_len = 1024; 

    char* m_szResult = new char[cst_result_len];
    char* m_szErrInfo = new char[cst_err_len];

    memset(m_szResult, 0, cst_result_len);
    memset(m_szErrInfo, 0, cst_err_len);
     
    //连接服务器
    char * servers[] = {
        "59.175.238.38"};
    for( int i = 0; i < sizeof(servers)/sizeof(servers[0]); ++i )
    {
        conn_handle_ = TdxExHq_Connect(servers[i], 7727, m_szResult, m_szErrInfo);
        if( conn_handle_ < 0 )
        {
            std::cout << m_szErrInfo << std::endl;
            continue;
        }else
        {
            std::cout << m_szResult << std::endl;
            return true;
        }
    } 
    return false;
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
        case TypePeriod::PERIOD_5M: return 16*3;  
        default: return 1;
        }
    };

    std::vector<T_K_Data> resut;
    const int cst_result_len = 1024 * 1024;
    const int cst_err_len = 1024; 
       
    // 因为有个股停牌情况,所以 计算日期目标 不一定准确
    auto tuple_index_len = exchange_calendar_->GetStartIndexAndLen_backforward(kbar_type, start_date, end_date);
    
    //const short max_count = MAX_K_COUNT;
    short start = std::get<0>(tuple_index_len);  // back forward
    const short count = std::get<1>(tuple_index_len);
#if 1
    short local_count = MAX_K_COUNT;
    short total_get = 0;
#if 1
    
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
     
#endif 
    return total_get > 0;

#else 
    
#endif
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
    bool result = true;
  
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

    int market_type = MARKET_TYPE_SH;
    if( is_index )
    {
        if( code.at(0) == '3' )
            market_type = MARKET_TYPE_SZ;
        else
            market_type = MARKET_TYPE_SH;
    }else if( code.at(0) == '6' )
        market_type = MARKET_TYPE_SH;
    else 
        market_type = MARKET_TYPE_SZ;

   
    //获取股票K线数据 
    //数据种类, 0->5分钟K线    1->15分钟K线    2->30分钟K线  3->1小时K线    4->日K线  5->周K线  6->月K线  7->1分钟K线  8->1分钟K线  9->日K线  10->季K线  11->年K线
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
    default:
        assert(false);
        ktype = 4; 
        break;
    }
do 
{ 
    //bool1 = TdxExHq_GetInstrumentBars(nConn, 4, 30, "SC1907", 0, &Count, Result, ErrInfo);

    bool bool1 = pFuncGetInstrumentBars(conn_handle_, ktype, nmarket, const_cast<char*>(code.c_str()), start, &count, m_szResult, m_szErrInfo);
    if( !bool1 )
    { 
        if( ConnectServer() )
        {
            bool1 = pFuncGetInstrumentBars(conn_handle_, ktype, market_type, const_cast<char*>(code.c_str()), start, &count, m_szResult, m_szErrInfo);
            if( !bool1 )
            {
                result = false;
                break;
            }
        } else
        {
            result = false;
            break;
        }
    }
    if( strlen(m_szResult) < 1 )
    {
        std::cout << " result empty !" << std::endl;
        result = false;
        break;
    }
    qDebug() << m_szResult << "\n";

    const bool has_time = ( ktype < 4 || ktype == 7 || ktype == 8 ) ? true : false;
    std::string expresstion_str;
    if( has_time )
        //expresstion_str = "^(\\d{4}-\\d{2}-\\d{2})\\s(\\d{2}:\\d{2})\\t(\\d+\\.\\d+)\\t(\\d+\\.\\d+)\\t(\\d+\\.\\d+)\\t(\\d+\\.\\d+)\\t(\\d+)\\t(\\d+\\.\\d+)$";
        expresstion_str = "^(\\d{4})-(\\d{2})-(\\d{2})\\s+(\\d{2}):(\\d{2})\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+).*";
    else
        expresstion_str = "^(\\d{8})\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+).*";
        //expresstion_str = "^(\\d{8})\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+).*";
     
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