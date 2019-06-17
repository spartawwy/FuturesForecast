#ifndef TDX_EXHQ_WRAPPER_H_SDFDS34FFSDDS
#define TDX_EXHQ_WRAPPER_H_SDFDS34FFSDDS

//#include <memory>
#include <vector>
#include "stkfo_common.h"

class ExchangeCalendar;
class TdxExHqWrapper
{
public:
    TdxExHqWrapper(ExchangeCalendar  *exchange_calendar);
    ~TdxExHqWrapper();

    bool Init();
    bool ConnectServer();
    void DisConnect();

    bool GetHisKBars(const std::string &code, bool is_index, int nmarket, TypePeriod kbar_type, int start_date, int end_date, std::vector<T_StockHisDataItem> &items);

    bool GetLatestKBar(const std::string &code, bool is_index, int nmarket, TypePeriod kbar_type, T_StockHisDataItem &item);

private:
    bool __GetHisKBars(const std::string &code, bool is_index, int nmarket, TypePeriod kbar_type, short start, short &count, std::vector<T_StockHisDataItem> &items);

    int conn_handle_;
    ExchangeCalendar  *exchange_calendar_;
};

#define  MAX_K_COUNT 500 //800 // 虽然(TdxHq_GetSecurityBars TdxHq_GetIndexBars)接口说明最大值 800, 但实际测试为 420
#endif