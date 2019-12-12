#ifndef TDX_EXHQ_WRAPPER_H_SDFDS34FFSDDS
#define TDX_EXHQ_WRAPPER_H_SDFDS34FFSDDS
 
#include <vector>
#include <mutex>

#include "stkfo_common.h"

namespace  TSystem
{
    class LocalLogger;
}
class ExchangeCalendar;
class TdxExHqWrapper
{
public:
    TdxExHqWrapper(ExchangeCalendar  *exchange_calendar, TSystem::LocalLogger &local_logger);
    ~TdxExHqWrapper();

    bool Init();

    bool GetHisKBars(const std::string &code, bool is_index, int nmarket, TypePeriod kbar_type, int start_date, int end_date, std::vector<T_StockHisDataItem> &items);

    bool GetLatestKBar(const std::string &code, bool is_index, int nmarket, TypePeriod kbar_type, T_StockHisDataItem &item);

    bool GetInstrumentQuote(const std::string &code, int nmarket, T_Quote_Data &ret_quote_data);

private:

    bool __GetHisKBars(const std::string &code, bool is_index, int nmarket, TypePeriod kbar_type, short start, short &count, std::vector<T_StockHisDataItem> &items);
     
    bool ConnectServer();
    void DisConnect();
    bool ReconnectServer();

    bool IsConnhandleValid();

    int _ConnectServer();

    int conn_handle_;
    std::mutex  conn_handle_mutext_;

    ExchangeCalendar  *exchange_calendar_;
    TSystem::LocalLogger &local_logger_;
};

#define  MAX_K_COUNT 500 //800 // 虽然(TdxHq_GetSecurityBars TdxHq_GetIndexBars)接口说明最大值 800, 但实际测试为 420
#endif