#ifndef STOCK_DATA_MAN_H
#define STOCK_DATA_MAN_H

//#include <list>
//#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
 
#include "stkfo_common.h"

#include "stockdayinfo.h"

#include "stk_quoter_api.h"
 
#include "winner_hq_api.h"
  
#include "tdx_exhq_wrapper.h"
 
#include "kline_wall.h"

TypePeriod ToTypePeriod(PeriodType src);
PeriodType ToPeriodType(TypePeriod src);

namespace  TSystem
{
    class LocalLogger;
}
class KLineWall;
class ExchangeCalendar;

class StockDataMan
{
public:

    StockDataMan(/*KLineWall *p_kwall, */ExchangeCalendar *p_exchange_calendar, TSystem::LocalLogger &local_logger);
    ~StockDataMan();
    bool Init();

    //std::vector<std::shared_ptr<T_KlineDataItem> > &day_kline_data_container() { return day_kline_data_container_; }
    ExchangeCalendar * exchange_calendar() { return p_exchange_calendar_;}

public:

    //从fileName指定的磁盘路径中将数据一行一行读取出来，每一行初始化一个StockDayInfo对象
    //void LoadDataFromFile(std::string &fileName);

    T_HisDataItemContainer* FindStockData(PeriodType period_type, const std::string &stk_code, int start_date, int end_date, int cur_hhmm, bool is_index=false);
    T_HisDataItemContainer* FindStockData(PeriodType period_type, const std::string &stk_code, int start_date, int end_date, bool is_index=false);
    T_HisDataItemContainer* AppendStockData(PeriodType period_type, int nmarket, const std::string &stk_code, int start_date, int end_date, bool is_index=false);
	     
    int UpdateOrAppendLatestItemStockData(PeriodType period_type, int nmarket, const std::string &stk_code, bool is_index=false);
    void TraverseGetBi(PeriodType period_type, const std::string &code, std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items);

    void TraverseGetStuctLines(PeriodType period_type, const std::string &code, std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items);
    void TraversGetSections(PeriodType period_type, const std::string &code, std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items);
     
    bool GetInstrumentQuote(const std::string &code, int nmarket, T_Quote_Data &ret_quote_data);

public:
      
	double GetHisDataLowestMinPrice(PeriodType period_type, const std::string& code, int start_date, int end_date);
	double GetHisDataHighestMaxPrice(PeriodType period_type, const std::string& code, int start_date, int end_date);

public: 

    T_HisDataItemContainer &GetHisDataContainer(TypePeriod type_period, const std::string& code)
    {
        return GetHisDataContainer(ToPeriodType(type_period), code);
    }

    T_HisDataItemContainer &GetHisDataContainer(PeriodType period_type, const std::string& code);
    T_BiContainer &GetBiContainer(PeriodType period_type, const std::string& code);
    T_StructLineContainer &GetStructLineContainer(PeriodType period_type, const std::string& code);
    T_SectionContainer &GetStructSectionContainer(PeriodType period_type, const std::string& code/*, int wall_index*/);

    // (stock , data)  date is from small to big
    T_CodeMapHisDataItemContainer m1_stock_his_items_;
    T_CodeMapHisDataItemContainer m5_stock_his_items_;
    T_CodeMapHisDataItemContainer m15_stock_his_items_;
    T_CodeMapHisDataItemContainer m30_stock_his_items_;
    T_CodeMapHisDataItemContainer hour_stock_his_items_;
    T_CodeMapHisDataItemContainer day_stock_his_items_;
    T_CodeMapHisDataItemContainer week_stock_his_items_;
    T_CodeMapHisDataItemContainer mon_stock_his_items_;

    T_CodeMapBiContainer m1_stock_bi_items_;
    T_CodeMapBiContainer m5_stock_bi_items_;
    T_CodeMapBiContainer m15_stock_bi_items_;
    T_CodeMapBiContainer m30_stock_bi_items_;
    T_CodeMapBiContainer hour_stock_bi_items_;
    T_CodeMapBiContainer day_stock_bi_items_;
    T_CodeMapBiContainer week_stock_bi_items_;
    T_CodeMapBiContainer mon_stock_bi_items_;

    T_CodeMapStructDataContainer m1_stock_struct_datas_;
    T_CodeMapStructDataContainer m5_stock_struct_datas_;
    T_CodeMapStructDataContainer m15_stock_struct_datas_;
    T_CodeMapStructDataContainer m30_stock_struct_datas_;
    T_CodeMapStructDataContainer hour_stock_struct_datas_;
    T_CodeMapStructDataContainer day_stock_struct_datas_;
    T_CodeMapStructDataContainer week_stock_struct_datas_;
    T_CodeMapStructDataContainer mon_stock_struct_datas_;

#ifndef USE_STK_QUOTER
    std::vector<T_StockHisDataItem> *p_stk_hisdata_item_vector_;
    bool is_fetched_stk_hisdata_;
#endif 
private:

    std::tuple<int, int> GetDateIndexFromContainer(PeriodType period_type, const std::string& stock, int start_date, int end_date);
    void CaculateZhibiao(T_HisDataItemContainer &data_items_in_container);

private:

    KLineWall *kwall_;
#if 1
    TdxExHqWrapper  tdx_exhq_wrapper_;
#endif
     
   std::vector<ZhibiaoType> zhibiao_types_;
   ExchangeCalendar *p_exchange_calendar_;
   TSystem::LocalLogger &local_logger_;
};

// < 0 : meaning no related data
int FindDataIndex(T_HisDataItemContainer &data_items_in_container, int date, int cur_hhmm);
bool IsDataIn(T_HisDataItemContainer &data_items_in_container, int date);

// 下分形遍历
void TraverseSetUpwardFractal(T_HisDataItemContainer &kline_data_items, int backward_size = 0);
// 上分形遍历
void TraverseSetDownwardFractal(T_HisDataItemContainer &kline_data_items, int backward_size = 0);

void TraverseClearFractalType(T_HisDataItemContainer &kline_data_items, int backward_size = 0);

void TraverseAjustFractal(T_HisDataItemContainer &kline_data_items, int backward_size = 0);

void TraverSetSignale(TypePeriod type_period, T_HisDataItemContainer &data_items_in_container, bool is_only_set_tail);

#endif // STOCK_DATA_MAN_H
