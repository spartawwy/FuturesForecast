#include "stock_data_man.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <memory>
#include <cassert>
//#include <algorithm>
#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>

#include <Windows.h>
#include <QtWidgets/QMessageBox>
#include <QDebug>

#ifdef USE_PYTHON_QUOTE
#include "py_data_man.h"
#endif

#include "tdx_exhq_wrapper.h"
#include "zhibiao.h"
#include "exchange_calendar.h"
#include "kline_wall.h"

#define RESERVE_CAPACITY_IN_T_VECTOR    1024*16
//#define RESERVE_SIZE_IN_T_VECTOR    1024*16
//#define FIRST_STARTPOS_IN_T_VECTOR  10000

static bool compare(const T_StockHisDataItem &lh, const T_StockHisDataItem &rh)
{
    return (lh.date < rh.date) || (lh.date == rh.date && lh.hhmmss < rh.hhmmss);
}
 
// from small to big
static bool dompare( std::shared_ptr<T_KlineDataItem> &lh, std::shared_ptr<T_KlineDataItem> &rh)
{
    return (lh->stk_item.date < rh->stk_item.date) || (lh->stk_item.date == rh->stk_item.date && lh->stk_item.hhmmss < rh->stk_item.hhmmss);
}

static bool bompare(const T_KlineDataItem &lh, const T_KlineDataItem &rh)
{
    return (lh.stk_item.date < rh.stk_item.date) || (lh.stk_item.date == rh.stk_item.date && lh.stk_item.hhmmss < rh.stk_item.hhmmss);
}

 

StockDataMan::StockDataMan(/*KLineWall *p_kwall, */ExchangeCalendar *p_exchange_calendar)
    : /*kwall_(p_kwall)
    ,*/ m5_stock_his_items_(1024)
    , m1_stock_his_items_(1024)
    , m15_stock_his_items_(1024)
    , m30_stock_his_items_(1024)
    , hour_stock_his_items_(1024)
    , day_stock_his_items_(1024)
    , week_stock_his_items_(1024)
    , mon_stock_his_items_(1024)
    , m1_stock_bi_items_(64)
    , m5_stock_bi_items_(64)
    , m15_stock_bi_items_(64)
    , m30_stock_bi_items_(64)
    , hour_stock_bi_items_(64)
    , day_stock_bi_items_(64)
    , week_stock_bi_items_(64)
    , mon_stock_bi_items_(64)
    , m1_stock_struct_datas_(64)
    , m5_stock_struct_datas_(64)
    , m15_stock_struct_datas_(64)
    , m30_stock_struct_datas_(64)
    , hour_stock_struct_datas_(64)
    , day_stock_struct_datas_(64)
    , week_stock_struct_datas_(64)
    , mon_stock_struct_datas_(64)
#ifdef USE_WINNER_API
    , WinnerHisHq_GetKData_(nullptr)
    , WinnerHisHq_Connect_(nullptr)
    , WinnerHisHq_DisConnect_(nullptr)
#endif
    , p_stk_hisdata_item_vector_(nullptr)
    , tdx_exhq_wrapper_(p_exchange_calendar)
    , p_exchange_calendar_(p_exchange_calendar)
{
    //LoadDataFromFile("./data/600030.dat");
    zhibiao_types_.push_back(ZhibiaoType::MOMENTUM); // momentum is in pos MOMENTUM_POS: 0
}

StockDataMan::~StockDataMan()
{
#ifdef USE_WINNER_API
    if( WinnerHisHq_DisConnect_ )
        WinnerHisHq_DisConnect_();
#endif
}

bool StockDataMan::Init()
{
    bool ret = true;
#ifdef USE_PYTHON_QUOTE
    py_data_man_ = std::make_shared<PyDataMan>();
    ret = py_data_man_->Initiate();
#endif

    return tdx_exhq_wrapper_.Init();

    return ret;
}


T_HisDataItemContainer* StockDataMan::FindStockData(PeriodType period_type, const std::string &stk_code, int start_date, int end_date, int cur_hhmm, bool /*is_index*/)
{
    assert( !stk_code.empty() ); 
    T_HisDataItemContainer & items_in_container = GetHisDataContainer(period_type, stk_code);
    int real_start_date = exchange_calendar()->CeilingTradeDate(start_date);
    int real_end_date = exchange_calendar()->FloorTradeDate(end_date);
    if( real_start_date == 0 || real_end_date == 0 )
        return nullptr;
    int start_hhmm = 0;
    switch(period_type)
    {
        case PeriodType::PERIOD_YEAR:
        case PeriodType::PERIOD_MON:
        case PeriodType::PERIOD_WEEK:
        case PeriodType::PERIOD_DAY:
            start_hhmm = 0; break;
        case PeriodType::PERIOD_HOUR:
            start_hhmm = 1030; break;
        case PeriodType::PERIOD_30M:
            start_hhmm = 1000; break;
        case PeriodType::PERIOD_15M:
            start_hhmm = 945; break;
        case PeriodType::PERIOD_5M:
            if( real_start_date == start_date )
                start_hhmm = 905; 
            else
                start_hhmm = 2105; 
            break;
        case PeriodType::PERIOD_1M:
            if( real_start_date == start_date )
                start_hhmm = 900; 
            else
                start_hhmm = 2100; 
            break;
    }
    if( real_start_date == real_end_date )
    {
        if( FindDataIndex(items_in_container, real_end_date, cur_hhmm) > -1 )
            return &items_in_container;
        else
            return nullptr;
    }else if( FindDataIndex(items_in_container, real_start_date, start_hhmm) > -1 && FindDataIndex(items_in_container, real_end_date, cur_hhmm) > -1 )
        return &items_in_container;
    return nullptr;
}


T_HisDataItemContainer* StockDataMan::FindStockData(PeriodType period_type, const std::string &stk_code, int start_date, int end_date, bool /*is_index*/)
{
    assert( !stk_code.empty() ); 
    T_HisDataItemContainer & items_in_container = GetHisDataContainer(period_type, stk_code);
    int real_start_date = exchange_calendar()->CeilingTradeDate(start_date);
    int real_end_date = exchange_calendar()->FloorTradeDate(end_date);
    if( real_start_date == 0 || real_end_date == 0 )
        return nullptr;
     
    if( IsDataIn(items_in_container, real_start_date) && IsDataIn(items_in_container, real_end_date) )
        return &items_in_container;
    return nullptr;
}

// date is save from older(smaller) date to newer(bigger). ps: data in container is series trade date time
T_HisDataItemContainer* StockDataMan::AppendStockData(PeriodType period_type, int nmarket, const std::string &stk_code, int start_date, int end_date, bool is_index)
{
    assert( !stk_code.empty() );
     
    T_HisDataItemContainer & items_in_container = GetHisDataContainer(period_type, stk_code);
   
    std::string code = stk_code;
    if( is_index )
        code = TransIndex2TusharedCode(stk_code);

    auto p_stk_hisdata_item_vector = new std::vector<T_StockHisDataItem>();
    bool ret = tdx_exhq_wrapper_.GetHisKBars(code, is_index, nmarket, ToTypePeriod(period_type), start_date, end_date, *p_stk_hisdata_item_vector);
    if( !ret || p_stk_hisdata_item_vector->empty() ) // fail
    {
        delete p_stk_hisdata_item_vector; 
        p_stk_hisdata_item_vector = nullptr;
        return std::addressof(items_in_container);
    }
    std::vector<T_StockHisDataItem> &p_data_items = *p_stk_hisdata_item_vector;  
    const int count = p_data_items.size();
    
    // save data to items_in_container (notice: only can insert to back or front ) ---------------------------
   
    if( !items_in_container.empty() ) // old data is exists
    {
#if 1
        if( p_data_items[p_data_items.size()-1].date < items_in_container.back()->stk_item.date 
            || p_data_items[0].date < items_in_container.back()->stk_item.date )
        { 
            for( int k = count; k > 0; --k )
            {  
                //auto ck_val = p_data_items[k-1].date;
                // push old to front
                if( p_data_items[k-1].date < items_in_container.front()->stk_item.date || 
                    (p_data_items[k-1].date == items_in_container.front()->stk_item.date &&
                    p_data_items[k-1].hhmmss < items_in_container.front()->stk_item.hhmmss) )
                {
                    auto k_item = std::make_shared<T_KlineDataItem>(p_data_items[k-1]); 
                    k_item->zhibiao_atoms.push_back(std::move(std::make_shared<MomentumZhibiao>()));
                    items_in_container.push_front(std::move(k_item));
                }
            }
        }else
        {
            for( int k = 0; k < count; ++k )
            {
                if( p_data_items[k].date > items_in_container.back()->stk_item.date ||
                    (p_data_items[k].date == items_in_container.back()->stk_item.date &&
                    p_data_items[k].hhmmss > items_in_container.back()->stk_item.hhmmss) )
                {
                    auto k_item = std::make_shared<T_KlineDataItem>(p_data_items[k]); 
                    k_item->zhibiao_atoms.push_back(std::move(std::make_shared<MomentumZhibiao>()));
                    items_in_container.push_back(std::move(k_item));
                }
            }
        }
         
    }else // first insert
    {
        for( int k = 0; k < count; ++k )
        {
            auto k_item = std::make_shared<T_KlineDataItem>(p_data_items[k]); 
            k_item->zhibiao_atoms.push_back(std::move(std::make_shared<MomentumZhibiao>()));
            items_in_container.push_back(std::move(k_item));
        }
    }
#endif
    
    // sort T_KlineDateItems by day from small to bigger
    //std::sort(items_in_container.begin(), items_in_container.end(), dompare);

    CaculateZhibiao(items_in_container);

#if defined(USE_WINNER_API) || defined(USE_TDXHQ) 
    delete p_stk_hisdata_item_vector;
    p_stk_hisdata_item_vector = nullptr;
#else
    stk_hisdata_release_(p_data_items);
#endif
    TraverseClearFractalType(items_in_container);

    TraverseSetUpwardFractal(items_in_container);

    TraverseSetDownwardFractal(items_in_container);
    
    TraverseAjustFractal(items_in_container);
    if( is_index )
        TraverseGetBi(period_type, stk_code, items_in_container);
    else 
        TraverseGetBi(period_type, code, items_in_container);
    if( is_index )
        TraverseGetStuctLines(period_type, stk_code, items_in_container);
    else
        TraverseGetStuctLines(period_type, code, items_in_container);
    if( is_index )
        TraversGetSections(period_type, stk_code, items_in_container);
    else
        TraversGetSections(period_type, code, items_in_container);

    TraverSetSignale(ToTypePeriod(period_type), items_in_container, false);

	return std::addressof(items_in_container);

}

// ret 0: unupdated; 1: updated last item; 2: appended an item
int StockDataMan::UpdateOrAppendLatestItemStockData(PeriodType period_type, int nmarket, const std::string &stk_code, bool is_index)
{
    assert( !stk_code.empty() );
    int ret = 0;
    T_HisDataItemContainer & items_in_container = GetHisDataContainer(period_type, stk_code);
    
    T_StockHisDataItem  item;
    memset(&item, 0, sizeof(item));
    tdx_exhq_wrapper_.GetLatestKBar(stk_code, is_index, nmarket, ToTypePeriod(period_type), item);
    if( items_in_container.back()->stk_item.date <= item.date )
    {
        if( items_in_container.back()->stk_item.date == item.date )
        {
            if( items_in_container.back()->stk_item.hhmmss == item.hhmmss )
            {
                memcpy( std::addressof(items_in_container.back()->stk_item), &item, sizeof(item) );
                ret = 1;
            }
            else if( items_in_container.back()->stk_item.hhmmss < item.hhmmss )
            {
                auto k_date_item = std::make_shared<T_KlineDataItem>(item);
                k_date_item->zhibiao_atoms.push_back(std::move(std::make_shared<MomentumZhibiao>()));
                items_in_container.push_back(std::move(k_date_item));
                
                //kwall_->IncreaseRendIndex();
                ret = 2;
            }
            else
            { 
                return 0;
            }

        }else 
        {
            auto k_date_item = std::make_shared<T_KlineDataItem>(item);
            k_date_item->zhibiao_atoms.push_back(std::move(std::make_shared<MomentumZhibiao>()));
            items_in_container.push_back(std::move(k_date_item));
             
            ret = 2;
        }
        CaculateZhibiao(items_in_container);
        return ret;
    }else
        return 0;
}

// ps: data has sorted
void StockDataMan::CaculateZhibiao(T_HisDataItemContainer &data_items_in_container)
{
    for (unsigned int i = 0; i < zhibiao_types_.size(); ++i )
    {
        switch(zhibiao_types_[i])
        {
        case ZhibiaoType::MOMENTUM:
            {
                MomentumZhibiao::Caculate(data_items_in_container);
                break;
            }
        default: break;
        }
    }
}

T_HisDataItemContainer & StockDataMan::GetHisDataContainer(PeriodType period_type, const std::string& code)
{
    T_CodeMapHisDataItemContainer *p_code_map_container = nullptr;
    switch(period_type)
    {
    case PeriodType::PERIOD_1M:   p_code_map_container = &m1_stock_his_items_; break;
    case PeriodType::PERIOD_5M:   p_code_map_container = &m5_stock_his_items_; break;
    case PeriodType::PERIOD_15M:  p_code_map_container = &m15_stock_his_items_; break;
    case PeriodType::PERIOD_30M:  p_code_map_container = &m30_stock_his_items_; break;
    case PeriodType::PERIOD_HOUR: p_code_map_container = &hour_stock_his_items_; break;
    case PeriodType::PERIOD_DAY:  p_code_map_container = &day_stock_his_items_; break; 
    case PeriodType::PERIOD_WEEK: p_code_map_container = &week_stock_his_items_; break;
    case PeriodType::PERIOD_MON:  p_code_map_container = &mon_stock_his_items_; break;
    default: assert(false);
    }
    auto container_iter = p_code_map_container->find(code);
    if( container_iter == p_code_map_container->end() )
        container_iter = p_code_map_container->insert(std::make_pair(code, T_HisDataItemContainer())).first;
    return container_iter->second;
}
 
T_BiContainer & StockDataMan::GetBiContainer(PeriodType period_type, const std::string& code)
{
    T_CodeMapBiContainer *p_code_map_container = nullptr;
    switch(period_type)
    {
    case PeriodType::PERIOD_1M:   p_code_map_container = &m1_stock_bi_items_; break;
    case PeriodType::PERIOD_5M:   p_code_map_container = &m5_stock_bi_items_; break;
    case PeriodType::PERIOD_15M:  p_code_map_container = &m15_stock_bi_items_; break;
    case PeriodType::PERIOD_30M:  p_code_map_container = &m30_stock_bi_items_; break;
    case PeriodType::PERIOD_HOUR: p_code_map_container = &hour_stock_bi_items_; break;
    case PeriodType::PERIOD_DAY:  p_code_map_container = &day_stock_bi_items_; break; 
    case PeriodType::PERIOD_WEEK: p_code_map_container = &week_stock_bi_items_; break;
    case PeriodType::PERIOD_MON:  p_code_map_container = &mon_stock_bi_items_; break;
    default: assert(false);
    }
    auto container_iter = p_code_map_container->find(code);
    if( container_iter == p_code_map_container->end() )
        container_iter = p_code_map_container->insert(std::make_pair(code, T_BiContainer())).first;
    return container_iter->second;
}

T_StructLineContainer &StockDataMan::GetStructLineContainer(PeriodType period_type, const std::string& code)
{
    T_CodeMapStructDataContainer *p_code_map_container = nullptr;
    switch(period_type)
    {
    case PeriodType::PERIOD_1M:   p_code_map_container = &m1_stock_struct_datas_; break;
    case PeriodType::PERIOD_5M:   p_code_map_container = &m5_stock_struct_datas_; break;
    case PeriodType::PERIOD_15M:  p_code_map_container = &m15_stock_struct_datas_; break;
    case PeriodType::PERIOD_30M:  p_code_map_container = &m30_stock_struct_datas_; break;
    case PeriodType::PERIOD_HOUR: p_code_map_container = &hour_stock_struct_datas_; break;
    case PeriodType::PERIOD_DAY:  p_code_map_container = &day_stock_struct_datas_; break; 
    case PeriodType::PERIOD_WEEK: p_code_map_container = &week_stock_struct_datas_; break;
    case PeriodType::PERIOD_MON:  p_code_map_container = &mon_stock_struct_datas_; break;
    default: assert(false);
    }
    auto container_iter = p_code_map_container->find(code);
    if( container_iter == p_code_map_container->end() )
        container_iter = p_code_map_container->insert(std::make_pair(code, T_StructData())).first;
    return container_iter->second.struct_line_container;
}


T_SectionContainer & StockDataMan::GetStructSectionContainer(PeriodType period_type, const std::string& code/*, int wall_index*/)
{
    T_CodeMapStructDataContainer *p_code_map_container = nullptr;
    switch(period_type)
    {
    case PeriodType::PERIOD_1M:   p_code_map_container = &m1_stock_struct_datas_; break;
    case PeriodType::PERIOD_5M:   p_code_map_container = &m5_stock_struct_datas_; break;
    case PeriodType::PERIOD_15M:  p_code_map_container = &m15_stock_struct_datas_; break;
    case PeriodType::PERIOD_30M:  p_code_map_container = &m30_stock_struct_datas_; break;
    case PeriodType::PERIOD_HOUR: p_code_map_container = &hour_stock_struct_datas_; break;
    case PeriodType::PERIOD_DAY:  p_code_map_container = &day_stock_struct_datas_; break; 
    case PeriodType::PERIOD_WEEK: p_code_map_container = &week_stock_struct_datas_; break;
    case PeriodType::PERIOD_MON:  p_code_map_container = &mon_stock_struct_datas_; break;
    default: assert(false);
    }
    auto container_iter = p_code_map_container->find(code);
    if( container_iter == p_code_map_container->end() )
        container_iter = p_code_map_container->insert(std::make_pair(code, T_StructData())).first;
    return container_iter->second.section_container;
}

// ok: ret <  MAX_PRICE
double StockDataMan::GetHisDataLowestMinPrice(PeriodType period_type, const std::string& code, int start_date, int end_date)
{ 
	double lowestMinPrice = MAX_PRICE; 
    assert(start_date <= end_date);
    auto index_tuple = GetDateIndexFromContainer(period_type, code, start_date, end_date);
    if( index_tuple == std::make_tuple(-1, -1) )
        return lowestMinPrice;
    T_HisDataItemContainer & container = GetHisDataContainer(period_type, code);
     
    for( int j = std::get<0>(index_tuple); j <= std::get<1>(index_tuple); ++j )
    {
        if( lowestMinPrice > container.at(j)->stk_item.low_price )
        {
            lowestMinPrice = container.at(j)->stk_item.low_price;
        }
    }
    return lowestMinPrice;
}

// ok : > 0.0 
double StockDataMan::GetHisDataHighestMaxPrice(PeriodType period_type, const std::string& code, int start_date, int end_date)
{
    double higestMaxPrice = MIN_PRICE; 
    assert(start_date <= end_date);
    auto index_tuple = GetDateIndexFromContainer(period_type, code, start_date, end_date);
    if( index_tuple == std::make_tuple(-1, -1) )
        return higestMaxPrice;

    T_HisDataItemContainer & container = GetHisDataContainer(period_type, code);

    for( int j = std::get<0>(index_tuple); j <= std::get<1>(index_tuple); ++j )
    {
        if( higestMaxPrice < container.at(j)->stk_item.high_price )
        {
            higestMaxPrice = container.at(j)->stk_item.high_price;
        }
    }
	return higestMaxPrice;
}

// <-1, -1>: fail
std::tuple<int, int> StockDataMan::GetDateIndexFromContainer(PeriodType period_type, const std::string& stock, int start_date, int end_date)
{
    assert(start_date <= end_date);
    //std::tuple<int, int> result = std::make_tuple(-1, -1);

    T_HisDataItemContainer & container = GetHisDataContainer(period_type, stock);
    int temp_start_date = container.at(0)->stk_item.date;
    if( end_date < container.at(0)->stk_item.date )
        return std::make_tuple(-1, -1);
    if( start_date > container.at(0)->stk_item.date )
        temp_start_date = start_date;

    int start_index = -1;
    int end_index = -1;
    for( unsigned int i = 0; i < container.size(); ++i )
    {
        if( temp_start_date == container.at(i)->stk_item.date ) 
        {
            start_index = i;
            break;
        }
    }

    for( unsigned int i = container.size(); i > 0; --i )
    {
        if( end_date == container.at(i-1)->stk_item.date ) 
        {
            end_index = i - 1;
            break;
        }
    }
    if( start_index == -1 )
        start_index = 0;
    if( end_index == -1 )
        end_index = container.size() - 1;
    return std::make_tuple(start_index, end_index);
}
 
/*   \/ 
    kline_data_items[0] is smallest time  */
void TraverseSetUpwardFractal(T_HisDataItemContainer &kline_data_items, int backward_size/*= 0 */)
{
    assert( backward_size <= kline_data_items.size() );
    if( kline_data_items.size() < 1 )
        return;
    // begin from left
    unsigned int index = 1;
    if( backward_size > 0 )
    {
        index = kline_data_items.size() - backward_size + 1;
    }
    while( index < kline_data_items.size() - 1 )
    {
        //debug -------
        int ck_date = kline_data_items[index]->stk_item.date;
        ck_date = ck_date;
        //--------
        if( kline_data_items[index-1]->stk_item.low_price < kline_data_items[index]->stk_item.low_price
            || kline_data_items[index+1]->stk_item.low_price < kline_data_items[index]->stk_item.low_price )
        {
            ++index;
            continue;
        }
        int n_fractal_ahead = 0;
        int n_fractal_follow = 0;
        // search fractal ahead  -----------
        unsigned int index_to_check = index;
        int k = index_to_check - 1;
        int ahead_k_num = k;
        for( ; k > 0; )
        {
            if( kline_data_items[k]->stk_item.low_price > kline_data_items[index_to_check]->stk_item.low_price )
            { 
                if( kline_data_items[k]->stk_item.high_price > kline_data_items[index_to_check]->stk_item.high_price )
                { 
                    ++n_fractal_ahead;
                    index_to_check = k;
                } 
                --k;
            }else 
            {
                if( kline_data_items[k]->stk_item.low_price < kline_data_items[index]->stk_item.low_price )
                    break;
                --k;
            }
        }
        ahead_k_num = ahead_k_num - k;
        if( n_fractal_ahead > 0 )
        {   // search fractal follow  --------  
            index_to_check = index;
            bool is_transfer_k = false;
            for( unsigned int k = index + 1; k < kline_data_items.size() && k < index + 1 + 3*ahead_k_num; )
            {
                if( kline_data_items[k]->stk_item.low_price > kline_data_items[index_to_check]->stk_item.low_price )
                {
                    if( !is_transfer_k && kline_data_items[k]->stk_item.low_price > kline_data_items[index_to_check]->stk_item.high_price )
                    {
                        // it's transfer k line 
                        is_transfer_k = true;
                    }else if( kline_data_items[k]->stk_item.high_price > kline_data_items[index_to_check]->stk_item.high_price )
                    {
                        //kline_data_items[k]->type |= UPWARD_FRACTAL;
                        ++n_fractal_follow;
                        index_to_check = k; 
                        is_transfer_k = false;
                    } /* {else
                       kline_data_items[k]->type |= INSUFFIC_FRACTAL; 
                       }*/
                    ++k;
                }else if( kline_data_items[k]->stk_item.low_price < kline_data_items[index_to_check]->stk_item.low_price )
                    break;
                else
                    ++k;
            }
        }

        if( n_fractal_ahead > 0 && n_fractal_follow > 0 )
        {
            qDebug() << __FUNCTION__ << " set btm " << kline_data_items[index]->stk_item.date << " : " << kline_data_items[index]->stk_item.hhmmss << "\n";
            kline_data_items[index]->type |= int(FractalType::BTM_AXIS_T_3);
            if( n_fractal_ahead > 1 && n_fractal_follow > 1 )
            {
                kline_data_items[index]->type |= int(FractalType::BTM_AXIS_T_5);
                if( n_fractal_ahead > 2 && n_fractal_follow > 2 )
                {
                    kline_data_items[index]->type |= int(FractalType::BTM_AXIS_T_7);
                    if( n_fractal_ahead > 3 && n_fractal_follow > 3 )
                    {
                        kline_data_items[index]->type |= int(FractalType::BTM_AXIS_T_9);
                        if( n_fractal_ahead > 4 && n_fractal_follow > 4 )
                            kline_data_items[index]->type |= int(FractalType::BTM_AXIS_T_11);
                    }
                }
            }
        }
        ++index;
    }//while
}

/*   /\ 
    kline_data_items[0] is smallest time  */
void TraverseSetDownwardFractal( T_HisDataItemContainer &kline_data_items, int backward_size/* = 0*/)
{
    assert( backward_size <= kline_data_items.size() );
    if( kline_data_items.size() < 1 )
        return;

    unsigned int index = 1;
    if( backward_size > 0 )
    {
        index = kline_data_items.size() - backward_size + 1;
    }
    while( index < kline_data_items.size() - 1 )
    { 
        //debug -------
        int ck_date = kline_data_items[index]->stk_item.date;
        ck_date = ck_date;
        //--------
        if( kline_data_items[index-1]->stk_item.high_price > kline_data_items[index]->stk_item.high_price
            || kline_data_items[index+1]->stk_item.high_price > kline_data_items[index]->stk_item.high_price )
        {
            ++index;
            continue;
        }
        
        int n_fractal_ahead = 0;
        int n_fractal_follow = 0;
        // search fractal ahead  -----------
         
        
        int index_to_check = index;
        int k = index_to_check - 1;
        int ahead_k_num = k;
        for( ; k > 0; )
        {
            if( kline_data_items[k]->stk_item.low_price < kline_data_items[index_to_check]->stk_item.low_price )
            {
                if( kline_data_items[k]->stk_item.high_price < kline_data_items[index_to_check]->stk_item.high_price )
                { 
                    ++n_fractal_ahead;
                    index_to_check = k;
                }else
                   ;// ahead out containter 
                 --k;
            }else 
            {
                if( kline_data_items[k]->stk_item.high_price > kline_data_items[index]->stk_item.high_price )
                    break;
                else // unenough line or inner container line
                    --k;
            } 
        }//for
        ahead_k_num = ahead_k_num - k;
         
        if( n_fractal_ahead > 0 )
        {   // search fractal follow  --------  
            index_to_check = index;
            bool is_transfer_k = false;
            for( unsigned int k = index + 1; k < kline_data_items.size() && k < index + 1 + 3*ahead_k_num; )
            {
                if( kline_data_items[k]->stk_item.high_price < kline_data_items[index_to_check]->stk_item.high_price )
                {
                    if( !is_transfer_k && kline_data_items[k]->stk_item.high_price < kline_data_items[index_to_check]->stk_item.low_price )
                    {
                        // it's transfer k line 
                        is_transfer_k = true;
                    }else if( kline_data_items[k]->stk_item.low_price < kline_data_items[index_to_check]->stk_item.low_price )
                    {
                        //kline_data_items[k]->type |= UPWARD_FRACTAL;
                        ++n_fractal_follow;
                        index_to_check = k; 
                        is_transfer_k = false;
                    } /* {else
                       kline_data_items[k]->type |= INSUFFIC_FRACTAL; 
                       }*/
                    ++k;
                     
                }else 
                {   // out container k
                    if( kline_data_items[k]->stk_item.low_price < kline_data_items[index_to_check]->stk_item.low_price + 0.0001 )
                        ++k;
                    else
                        break;
                }
            }
        }

        if( n_fractal_ahead > 0 && n_fractal_follow > 0 )
        {
            qDebug() << __FUNCTION__ << " set top " << kline_data_items[index]->stk_item.date << " : " << kline_data_items[index]->stk_item.hhmmss << "\n";

            kline_data_items[index]->type |= int(FractalType::TOP_AXIS_T_3);
            if( n_fractal_ahead > 1 && n_fractal_follow > 1 )
            {
                kline_data_items[index]->type |= int(FractalType::TOP_AXIS_T_5);
                if( n_fractal_ahead > 2 && n_fractal_follow > 2 )
                {
                    kline_data_items[index]->type |= int(FractalType::TOP_AXIS_T_7);
                    if( n_fractal_ahead > 3 && n_fractal_follow > 3 )
                    {
                        kline_data_items[index]->type |= int(FractalType::TOP_AXIS_T_9);
                        if( n_fractal_ahead > 4 && n_fractal_follow > 4 )
                        {
                            kline_data_items[index]->type |= int(FractalType::TOP_AXIS_T_11);
                        }
                    }
                }
            }
        }
        ++index;
    }//while
}

void TraverseClearFractalType(T_HisDataItemContainer &kline_data_items, int backward_size/* = 0*/)
{
    if( kline_data_items.size() < 1 )
        return;
    unsigned int index = kline_data_items.size();
    if( backward_size == 0 )
    {
        while( --index > 0 )
        {
            kline_data_items[index]->type = (int)FractalType::UNKNOW_FRACTAL;
        }
    }else
    {
        int num = backward_size;
        while( --index > 0 && num-- > 0 )
        {
            kline_data_items[index]->type = (int)FractalType::UNKNOW_FRACTAL;
        }
    }
}

void TraverseAjustFractal( std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int backward_size/* = 0*/)
{
    static auto find_left_btm_frac = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int index)->int
    {
        int i = index;
        while( --i >= 0 )
        {
            if( BtmestFractalType(kline_data_items[i]->type) != FractalType::UNKNOW_FRACTAL )
                return i;
        }
        return -1;
    };
    static auto find_left_top_frac = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int index)->int
    {
        int i = index;
        while( --i >= 0 )
        {
            if( MaxFractalType(kline_data_items[i]->type) >= FractalType::TOP_AXIS_T_3 )
                return i;
        }
        return -1;
    };
    // from index to j (not contain index)
    static auto find_left_btm = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int index, int j)->int
    {
        double tmp_price = MAX_PRICE;
        int i = index;
        int target_i = -1;
        while( --i > j && i > -1 )
        {
            if( kline_data_items[i]->stk_item.low_price < tmp_price )
            {
                tmp_price = kline_data_items[i]->stk_item.low_price;
                target_i = i;
            }
        }
        return target_i;
    };
    // from index to j (contain index and j)
    static auto find_left_top = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int index, int j)->int
    {
        double tmp_price = MIN_PRICE;
        int i = index;
        int target_i = -1;
        while( i-- >= j && i > -1 )
        {
            if( kline_data_items[i]->stk_item.high_price > tmp_price )
            {
                tmp_price = kline_data_items[i]->stk_item.high_price;
                target_i = i;
            }
        }
        return target_i;
    };

    static auto find_left_top_uncontain = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int index, int j)->int
    {
        double tmp_price = MIN_PRICE;
        int i = index;
        int target_i = -1;
        while( --i > j && i > -1 )
        {
            if( kline_data_items[i]->stk_item.high_price > tmp_price )
            {
                tmp_price = kline_data_items[i]->stk_item.high_price;
                target_i = i;
            }
        }
        return target_i;
    };
    // ps : uncontain index and nearby_index
    static auto get_min_max = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int index, int nearby_index)->std::tuple<double, int, double, int>
    { 
        double tmp_max_price = MIN_PRICE;
        double tmp_min_price = MAX_PRICE;
        int i = index;
        int target_min_index = i - 1;
        int target_max_index = i - 1;
        while( --i > nearby_index && i > -1 )
        {
            if( kline_data_items[i]->stk_item.low_price < tmp_min_price )
            {
                tmp_min_price = kline_data_items[i]->stk_item.low_price;
                target_min_index = i;
            }
            if( kline_data_items[i]->stk_item.high_price > tmp_max_price )
            {
                tmp_max_price = kline_data_items[i]->stk_item.high_price;
                target_max_index = i;
            }
        }
        return std::make_tuple(tmp_min_price, target_min_index, tmp_max_price, target_max_index);
    };

    
    if( kline_data_items.size() < 1 )
        return;
    assert(backward_size <= kline_data_items.size());
     
    unsigned int index = kline_data_items.size();

    int num = kline_data_items.size();
    if( backward_size > 0 )
        num = backward_size;

    while( --index > 0 && num-- > 0 )
    {
        int frac_date = kline_data_items[index]->stk_item.date; 
        int frac_hhmmss = kline_data_items[index]->stk_item.hhmmss; 
        frac_date = frac_date; // debug
        if( kline_data_items[index]->type == (int)FractalType::UNKNOW_FRACTAL )
            continue;
        if( IsBtmFractal(kline_data_items[index]->type) ) // btm frac
        {
            int btm_index = find_left_btm_frac(kline_data_items, index);
            int top_index = find_left_top_frac(kline_data_items, index);
            if( btm_index > top_index ) // btm is nearby
            {
                if( index - btm_index == 1 && !IsTopFractal(kline_data_items[index]->type) )
                {
                    if( kline_data_items[index]->stk_item.low_price < kline_data_items[btm_index]->stk_item.low_price )
                        ClearBtmFractal(*kline_data_items[btm_index]);
                    else
                        ClearBtmFractal(*kline_data_items[index]);
                    continue;
                }
                // find left k which pirce is higest
                auto min_max_tuple = get_min_max(kline_data_items, index, btm_index);
                double left_min_price = std::get<0>(min_max_tuple);
                int left_min_index = std::get<1>(min_max_tuple);
                double left_top_price = std::get<2>(min_max_tuple);
                int left_top_index = std::get<3>(min_max_tuple);
                //int left_top_index = find_left_top_uncontain(kline_data_items, index, btm_index);
                if( left_top_index > 0 
                    && left_top_price > kline_data_items[btm_index]->stk_item.high_price 
                    && left_top_price > kline_data_items[index]->stk_item.high_price)
                { 
                    kline_data_items[left_top_index]->type |= (int)FractalType::TOP_FAKE;
                    qDebug() << __LINE__ << " set top fake flag date " <<  kline_data_items[left_top_index]->stk_item.date << " " <<  kline_data_items[left_top_index]->stk_item.hhmmss;
                    if( left_min_price < kline_data_items[index]->stk_item.low_price 
                        && left_min_index >= left_top_index )
                    {
                        qDebug() << __LINE__ << "index " << index << " left_min_index " << left_min_index << " left_top_index " << left_top_index;
                        qDebug() << __LINE__ << " set btm fake flag date " <<  kline_data_items[left_min_index]->stk_item.date<< " " <<  kline_data_items[left_min_index]->stk_item.hhmmss;
                        kline_data_items[left_min_index]->type |= (int)FractalType::BTM_FAKE;
                        
                        //qDebug() << __LINE__ << " clear btm  date " <<  kline_data_items[index]->stk_item.date << " " <<  kline_data_items[index]->stk_item.hhmmss;
                        ClearBtmFractal(*kline_data_items[index]);
                    }
                }else if( !IsTopFractal(kline_data_items[index]->type) )
                {
                    if( kline_data_items[btm_index]->stk_item.low_price > kline_data_items[index]->stk_item.low_price )
                    {
                        // clear serial btm flag 
                        //qDebug() << __LINE__ << " clear btm  date " <<  kline_data_items[btm_index]->stk_item.date << " " <<  kline_data_items[btm_index]->stk_item.hhmmss;
                        ClearBtmFractal(*kline_data_items[btm_index]);
                        int tmp_top_index = find_left_top_frac(kline_data_items, btm_index);
                        int tmp_btm_index = find_left_btm_frac(kline_data_items, btm_index);
#if 1
                        if( tmp_btm_index > tmp_top_index )
                        { 
                            auto min_max_tuple_t = get_min_max(kline_data_items, btm_index, tmp_btm_index);
                            double top_price_t = std::get<2>(min_max_tuple_t);
                            int top_index_t = std::get<3>(min_max_tuple);
                            if( top_price_t > MIN_PRICE )
                            {
                                kline_data_items[top_index_t]->type |= (int)FractalType::TOP_FAKE;
                                continue;
                            }
                        }
#endif
                        while( tmp_btm_index > -1 && tmp_btm_index > tmp_top_index )
                        {
                            ClearBtmFractal(*kline_data_items[tmp_btm_index]);
                            int tmp_index = tmp_btm_index;
                            tmp_top_index = find_left_top_frac(kline_data_items, tmp_index);
                            tmp_btm_index = find_left_btm_frac(kline_data_items, tmp_index);
                        }
                    }else
                        ClearBtmFractal(*kline_data_items[index]);
                    
                }
                 
            }else if( top_index > -1 )// top_index is nearby
            {
                // find max min price between   
                auto obj_tuple = get_min_max(kline_data_items, index, top_index);
                const double min_price = std::get<0>(obj_tuple);
                const int min_p_index = std::get<1>(obj_tuple);
                const double max_price = std::get<2>(obj_tuple);
                const int max_p_index = std::get<3>(obj_tuple);
                if( max_price > kline_data_items[top_index]->stk_item.high_price )
                { 
                    //qDebug() << __LINE__ << " clear top flag date " <<  kline_data_items[top_index]->stk_item.date;
                    ClearTopFractal(*kline_data_items[top_index]);
                    qDebug() << __LINE__ << " set top fake flag " << kline_data_items[max_p_index]->stk_item.date << ":" << kline_data_items[max_p_index]->stk_item.hhmmss;
                    kline_data_items[max_p_index]->type |= (int)FractalType::TOP_FAKE;
                }
                else // ndchk
                if( btm_index > -1 && min_price < kline_data_items[index]->stk_item.low_price )
                {
                    //qDebug() << __LINE__ << " clear btm fake date " <<  kline_data_items[index]->stk_item.date;
                    ClearBtmFractal(*kline_data_items[index]);
                    qDebug() << __LINE__ << " set btm fake flag " <<  kline_data_items[min_p_index]->stk_item.date << ":" << kline_data_items[min_p_index]->stk_item.hhmmss;
                    kline_data_items[min_p_index]->type |= (int)FractalType::BTM_FAKE;
                } 
            }
        }

        if( IsTopFractal(kline_data_items[index]->type) ) // top frac
        {
            int frac_date = kline_data_items[index]->stk_item.date; 
            frac_date = frac_date; //debug
            int top_index = find_left_top_frac(kline_data_items, index);
            int btm_index = find_left_btm_frac(kline_data_items, index);
            if( top_index > btm_index ) // left top fram is nearby
            {
                if( index - top_index == 1 && !IsBtmFractal(kline_data_items[index]->type) )
                {
                    if( kline_data_items[index]->stk_item.high_price > kline_data_items[top_index]->stk_item.high_price )
                    {
                        //qDebug() << __LINE__ << " clear top fake date " <<  kline_data_items[top_index]->stk_item.date;
                        ClearTopFractal(*kline_data_items[top_index]);
                    }
                    else
                    {
                        //qDebug() << __LINE__ << " clear top fake date " <<  kline_data_items[index]->stk_item.date;
                        ClearTopFractal(*kline_data_items[index]);
                    }
                    continue;
                }
                // find left k which pirce is lowest
                int left_btm_index = find_left_btm(kline_data_items, index, top_index);
                if( left_btm_index > 0 
                    && kline_data_items[left_btm_index]->stk_item.low_price < kline_data_items[index]->stk_item.low_price /*+ 0.0001*/
                    && kline_data_items[left_btm_index]->stk_item.low_price < kline_data_items[top_index]->stk_item.low_price /*+ 0.0001*/)
                { 
                    kline_data_items[left_btm_index]->type |= (int)FractalType::BTM_FAKE;
                    qDebug() << __LINE__ << " set btm fake flag date " <<  kline_data_items[left_btm_index]->stk_item.date;
                }else
                {
                    if( kline_data_items[top_index]->stk_item.high_price < kline_data_items[index]->stk_item.high_price )
                    {
                        // clear serial top flag 
                        ClearTopFractal(*kline_data_items[top_index]);
                        int tmp_top_index = find_left_top_frac(kline_data_items, top_index);
                        int tmp_btm_index = find_left_btm_frac(kline_data_items, top_index);
#if 1
                        if( tmp_top_index > tmp_btm_index )
                        {
                            auto min_max_tuple_t = get_min_max(kline_data_items, top_index, tmp_top_index);
                            double btm_price_t = std::get<0>(min_max_tuple_t);
                            int btm_index_t = std::get<1>(min_max_tuple_t); 
                            if( btm_price_t < MAX_PRICE )
                            {
                                kline_data_items[btm_index_t]->type |= (int)FractalType::BTM_FAKE;
                                continue;
                            }
                        }
#endif
                        while( tmp_top_index > -1 && tmp_top_index > tmp_btm_index )
                        {
                            ClearTopFractal(*kline_data_items[tmp_top_index]);
                            int tmp_index = tmp_top_index;
                            tmp_top_index = find_left_top_frac(kline_data_items, tmp_index);
                            tmp_btm_index = find_left_btm_frac(kline_data_items, tmp_index);
                        }
                    }else
                        ClearTopFractal(*kline_data_items[index]);
                } 
                
            }else if( btm_index > -1 ) // btm_index is nearby
            {
                // find max min price between   
                auto obj_tuple = get_min_max(kline_data_items, index, btm_index);
                const double min_price = std::get<0>(obj_tuple);
                const int min_p_index = std::get<1>(obj_tuple);
                const double max_price = std::get<2>(obj_tuple);
                const int max_p_index = std::get<3>(obj_tuple);
                if( top_index > -1 && max_price > kline_data_items[index]->stk_item.high_price )
                {
                    ClearTopFractal(*kline_data_items[index]);
                    qDebug() << __LINE__ << " set top fake flag date " <<  kline_data_items[max_p_index]->stk_item.date;
                    kline_data_items[max_p_index]->type |= (int)FractalType::TOP_FAKE;
                }
                if( min_price < kline_data_items[btm_index]->stk_item.low_price )
                {
                    ClearBtmFractal(*kline_data_items[btm_index]);
                    qDebug() << __LINE__ << " set btm fake flag date " <<  kline_data_items[min_p_index]->stk_item.date;
                    kline_data_items[min_p_index]->type |= (int)FractalType::BTM_FAKE;
                } 
            }
        }
    }
}


// ps: called after TraverseAjustFractal
void StockDataMan::TraverseGetBi(PeriodType period_type, const std::string &code, std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items)
{
    static auto create_start_end_point =[](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int i, int index)->std::tuple<T_BiPoint, T_BiPoint>
    {
        T_BiPoint  start;
        start.date = kline_data_items[i]->stk_item.date;
        start.hhmm = kline_data_items[i]->stk_item.hhmmss;
        start.index = i;
        start.frac_type = FractalType(kline_data_items[i]->type);
        T_BiPoint  end;
        end.date = kline_data_items[index]->stk_item.date;
        end.hhmm = kline_data_items[index]->stk_item.hhmmss;
        end.index = index;
        end.frac_type = FractalType(kline_data_items[index]->type);
        end.date = kline_data_items[index]->stk_item.date;
        return std::make_tuple(start, end);
    };

    if( kline_data_items.size() < 1 )
        return;
    T_BiContainer &container = GetBiContainer(period_type, code); 
    container.clear();
    unsigned int index = kline_data_items.size();
    while( --index > 0 )
    {
        if( IsTopFractal(kline_data_items[index]->type) )
        {
            int ck_index_date = kline_data_items[index]->stk_item.date;
            int i = index;
            int max_count = 30;
            double low_price = MAX_PRICE;
            int btm_index = -1;
            int target_index = -1;
            while( --i >= 0 && max_count-- > 0 )
            {
                int ck_i_date = kline_data_items[i]->stk_item.date;
                if( kline_data_items[i]->stk_item.high_price > kline_data_items[index]->stk_item.high_price )
                    break;
                else if( IsBtmFractal(kline_data_items[i]->type) && kline_data_items[i]->stk_item.low_price < low_price)
                { 
                    btm_index = i;
                    low_price = kline_data_items[i]->stk_item.low_price;
                    if( index - i > 5 )
                        target_index = i;
                }else if( IsTopFractal(kline_data_items[i]->type) && btm_index > -1 && btm_index - i > 5 )
                    break;
            }
            if( target_index > -1 )
            {
                auto start_end = create_start_end_point(kline_data_items, target_index, index);
                container.push_front( std::make_shared<T_Bi>(BiType::UP, std::get<0>(start_end), std::get<1>(start_end)) );
                index = target_index + 1;
            }

        }else if( IsBtmFractal(kline_data_items[index]->type) )
        {
            int i = index;
            int max_count = 30;
            double high_price = MIN_PRICE;
            int high_index = -1;
            int target_index = -1;
            while( --i >= 0 && max_count-- > 0 )
            {
                if( kline_data_items[i]->stk_item.low_price < kline_data_items[index]->stk_item.low_price)
                    break;
                if( IsTopFractal(kline_data_items[i]->type) && kline_data_items[i]->stk_item.high_price > high_price)
                {
                    high_index = i;
                    high_price = kline_data_items[i]->stk_item.high_price;
                    if( index - i > 5 )
                        target_index = i;
                }else if( IsBtmFractal(kline_data_items[i]->type) && high_index > -1 && high_index - i > 5 )
                    break;
            }
            if( target_index > -1 )
            {
                auto start_end = create_start_end_point(kline_data_items, target_index, index);
                container.push_front( std::make_shared<T_Bi>(BiType::DOWN, std::get<0>(start_end), std::get<1>(start_end)) );
                index = target_index + 1;
            }
        }
    }
}

int find_next_btm_fractal(std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int from_index)
{
    int local_index = from_index;
    while( local_index >= 0 )
    {
        if( IsBtmFractal(kline_data_items[local_index]->type) )
            return local_index;
        --local_index;
    }
    return -1;
}

int find_next_top_fractal(std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int from_index)
{
    int local_index = from_index;
    while( local_index >= 0 )
    {
        if( IsTopFractal(kline_data_items[local_index]->type) )
            return local_index;
        --local_index;
    }
    return -1;
}

void find_down_towardleft_end(std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, bool is_src_k_same, double lowest_price, int start, int &end) 
{
    if( start == 0 )
        return;
    // start is top fractal. A
    int a_date = kline_data_items[start]->stk_item.date;
    double price_a = kline_data_items[start]->stk_item.high_price;
    // 1.find next btm fractal B 
    int btm_index_b = -1;
    if( !is_src_k_same && IsBtmFractal(kline_data_items[start]->type) ) // start is both top and btm fractal line 
        btm_index_b = start;
    else
        btm_index_b = find_next_btm_fractal(kline_data_items, start - 1);
    if( btm_index_b == -1 )
        return; 

    const double price_b = kline_data_items[btm_index_b]->stk_item.low_price;
    //assert( price_b < price_a );
    // find next top fractal C
    int top_index_c = -1;
    if( btm_index_b != start && IsTopFractal(kline_data_items[btm_index_b]->type) ) // start is both top and btm fractal line 
        top_index_c = btm_index_b;
    else
        top_index_c = find_next_top_fractal(kline_data_items, btm_index_b - 1);

    if( top_index_c == -1 )
    {
        end = btm_index_b;
        return;
    }

    //assert( IsTopFractal(kline_data_items[top_index_c]->type) ); 
    double price_c = kline_data_items[top_index_c]->stk_item.high_price;
    assert( !(price_c < price_b ) );  

    if( price_c < price_a )// judge if C is lower than A
    { 
        if( !(price_b > lowest_price) ) // price b <= lowest_price 
        {
            lowest_price = price_b;
            end = btm_index_b;
            find_down_towardleft_end(kline_data_items, btm_index_b == top_index_c, lowest_price, top_index_c, end);
        }
        else  // price_b > lowest_price
            return;
    }else // higher than A: set B as end point
    {
        if( !(price_b > lowest_price) ) // price_b <= lowest_price
        {
            end = btm_index_b;
        }
        return;
    }
} 


void find_up_towardleft_end(std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, bool is_src_k_same, double highest_price, int start, int &end) 
{
    if( start == 0 )
        return;
    // start is top fractal. A
    int a_date = kline_data_items[start]->stk_item.date;
    double price_a = kline_data_items[start]->stk_item.low_price;
    // 1.find next top fractal B 
    int top_index_b = -1;
    if( !is_src_k_same && IsTopFractal(kline_data_items[start]->type) ) // start is both top and btm fractal line 
        top_index_b = start;
    else
        top_index_b = find_next_top_fractal(kline_data_items, start - 1);
    if( top_index_b == -1 )
        return; 

    const double price_b = kline_data_items[top_index_b]->stk_item.high_price;

    // find next btm fractal C
    int btm_index_c = -1;
    if( top_index_b != start && IsBtmFractal(kline_data_items[top_index_b]->type) ) // start is both top and btm fractal line 
        btm_index_c = top_index_b;
    else
        btm_index_c = find_next_btm_fractal(kline_data_items, top_index_b - 1);

    if( btm_index_c == -1 )
    {
        end = top_index_b;
        return;
    }

    double price_c = kline_data_items[btm_index_c]->stk_item.low_price;
    assert( !(price_c > price_b ) );

    if( price_c > price_a )// judge if C is higher than A
    { 
        if( !(price_b < highest_price) ) // price b >= highest_price 
        {
            highest_price = price_b;
            end = top_index_b;
            find_up_towardleft_end(kline_data_items, top_index_b == btm_index_c, highest_price, btm_index_c, end);
        }
        else  // price_b < highest_price
            return;
    }else // C is lower than A: set B as end point
    {
        if( !(price_b < highest_price) ) // price_b >= highest_price
        {
            end = top_index_b;
        }
        return;
    }
} 

// ps : towards left 
void StockDataMan::TraverseGetStuctLines(PeriodType period_type, const std::string &code, std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items)
{
    auto find_next_up_struct_line_end_hight_p = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int index, double pre_btm_price)->double
    { 
        if( kline_data_items.size() < 1 )
            return -1 * MAX_PRICE;
        int temp_index = index;
        double temp_pre_btm_price = pre_btm_price;
        while( --temp_index > 0 )
        {
            if( IsTopFractal(kline_data_items[temp_index]->type) )
            {
                int temp_end = temp_index;
                double temp_b_price = MAX_PRICE;
                int temp_b_index = find_next_btm_fractal(kline_data_items, temp_index);
                if( temp_b_index != -1 )
                {
                    temp_b_price = kline_data_items[temp_b_index]->stk_item.low_price;
                    temp_end = temp_b_index;
                } 
                find_down_towardleft_end(kline_data_items, false, temp_b_price, temp_index, temp_end);
                if( temp_index != temp_end )
                {
                    if( kline_data_items[temp_end]->stk_item.low_price < temp_pre_btm_price )
                    {  
                        return kline_data_items[temp_index]->stk_item.high_price;
                    }
                } 
            }else if( IsBtmFractal(kline_data_items[temp_index]->type) )
                temp_pre_btm_price = MAX_PRICE;
        } // while( --temp_index > 0 )
        return -1 * MAX_PRICE;
    };

    auto find_next_down_struct_line_end_low_p = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, int index, double pre_top_price)->double
    { 
        if( kline_data_items.size() < 1 )
            return -1 * MAX_PRICE;
        int temp_index = index;
        double temp_pre_top_price = pre_top_price;
        while( --temp_index > 0 )
        {
            if( IsBtmFractal(kline_data_items[temp_index]->type) )
            {
                int temp_end = temp_index;
                double temp_b_price = MIN_PRICE;
                int temp_b_index = find_next_top_fractal(kline_data_items, temp_index);
                if( temp_b_index != -1 )
                {
                    temp_b_price = kline_data_items[temp_b_index]->stk_item.high_price;
                    temp_end = temp_b_index;
                } 
                find_up_towardleft_end(kline_data_items, false, temp_b_price, temp_index, temp_end);
                if( temp_index != temp_end )
                {
                    if( kline_data_items[temp_end]->stk_item.high_price > temp_pre_top_price )
                    {  
                        return kline_data_items[temp_index]->stk_item.low_price;
                    }
                } 
            }else if( IsTopFractal(kline_data_items[temp_index]->type) )
                temp_pre_top_price = MIN_PRICE;
        } // while( --temp_index > 0 )
        return -1 * MAX_PRICE;
    };
     
    if( kline_data_items.size() < 1 )
        return;
    T_StructLineContainer &container = GetStructLineContainer(period_type, code); 
    container.clear();
    unsigned int index = kline_data_items.size();
    double pre_btm_price = MAX_PRICE;
    double pre_top_price = MIN_PRICE;
    bool is_pre_add_up_line = false;
    bool is_pre_add_down_line = false;
    // towards left 
    bool is_index_both_btm_top = false;
    while( --index > 0 )
    { 
        int ck_index_date = kline_data_items[index]->stk_item.date;
        if( IsTopFractal(kline_data_items[index]->type) )
        { 
            pre_top_price = MIN_PRICE;

             int end = index; 
             double b_price = MAX_PRICE;
             int b_index = -1;
             if( is_index_both_btm_top )
                 b_index = find_next_btm_fractal(kline_data_items, index - 1);
             else
                 b_index = find_next_btm_fractal(kline_data_items, index);
             
             if( b_index != -1 )
             {
                 b_price = kline_data_items[b_index]->stk_item.low_price;
                 end = b_index;
             }
             
             find_down_towardleft_end(kline_data_items, is_index_both_btm_top, b_price, index, end);
             is_index_both_btm_top = false;
             if( index != end )
             {
                 bool is_to_add_line = false;
                 if( kline_data_items[end]->stk_item.low_price < pre_btm_price )
                 {
                     is_to_add_line = true;
                 }else
                 { 
                     double next_up_end_h_p = find_next_up_struct_line_end_hight_p(kline_data_items, end, kline_data_items[end]->stk_item.low_price);
                     if( next_up_end_h_p > 0.0 && next_up_end_h_p < kline_data_items[index]->stk_item.high_price )
                        is_to_add_line = true;
                 }
                 if( is_to_add_line )
                 {
                     pre_btm_price = kline_data_items[end]->stk_item.low_price;
                     
                     auto line = std::make_shared<T_StructLine>(LineType::UP, end, index);
                     container.push_back(std::move(line));
                     if( IsTopFractal(kline_data_items[end]->type) )
                         is_index_both_btm_top = true;
                     else
                         is_index_both_btm_top = false;
                 }

                 index = end + 1;
             } 
            
        }else if( IsBtmFractal(kline_data_items[index]->type) )
        {
             pre_btm_price = MAX_PRICE; 

             int end = index; 
             double b_price = MIN_PRICE;
             int b_index = -1;
             if( is_index_both_btm_top )
                 b_index = find_next_top_fractal(kline_data_items, index - 1);
             else
                 b_index = find_next_top_fractal(kline_data_items, index);
             if( b_index != -1 )
             {
                 b_price = kline_data_items[b_index]->stk_item.high_price;
                 end = b_index;
             } 
             find_up_towardleft_end(kline_data_items, is_index_both_btm_top, b_price, index, end);
             is_index_both_btm_top = false;
             if( index != end )
             {
                 bool is_to_add_down_line = false;
                 if( kline_data_items[end]->stk_item.high_price > pre_top_price )
                 {
                     is_to_add_down_line = true;
                 }else
                 { 
                     double next_down_end_l_p = find_next_down_struct_line_end_low_p(kline_data_items, end, kline_data_items[end]->stk_item.high_price);
                     if( next_down_end_l_p > 0.0 && next_down_end_l_p > kline_data_items[index]->stk_item.low_price )
                        is_to_add_down_line = true;
                 }
                 if( is_to_add_down_line )
                 {
                     pre_top_price = kline_data_items[end]->stk_item.high_price;
                     auto line = std::make_shared<T_StructLine>(LineType::DOWN, end, index);
                     container.push_back(std::move(line));
                     if( IsBtmFractal(kline_data_items[end]->type) )
                         is_index_both_btm_top = true;
                     else
                         is_index_both_btm_top = false;
                 }
                 index = end + 1;
             } 
        }

    } // while 
}

// data in kline_data_items is raw 
// ps:  TraverseGetStuctLines have to be called before 
void StockDataMan::TraversGetSections(PeriodType period_type, const std::string &code, std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items)
{
    // from right toward left, struct line 0 is '/'
    static auto if_fit_down_section_right_start = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, T_StructLineContainer &struct_line_container
        , int index)->bool
    { 
        if( struct_line_container[index]->type != LineType::UP )
            return false;
        int line_0_beg_index = struct_line_container[index]->beg_index;
        int line_0_end_index = struct_line_container[index]->end_index;
        const double line_0_beg_price = kline_data_items[line_0_beg_index]->stk_item.low_price;
        const double line_0_end_price = kline_data_items[line_0_end_index]->stk_item.high_price;
        if( kline_data_items[struct_line_container[index + 1]->beg_index]->stk_item.high_price > line_0_end_price ) 
            return false;
        //assert( struct_line_container[i + 1]->type == LineType::DOWN );
        //assert( struct_line_container[i + 2]->type == LineType::UP ); // line c
        if( kline_data_items[struct_line_container[index + 2]->beg_index]->stk_item.low_price > line_0_end_price ) 
            return false;
        if( kline_data_items[struct_line_container[index + 3]->beg_index]->stk_item.high_price < line_0_beg_price + 0.0001 )
            return false;
        return true;
    };

    // from right toward left, struct line 0 is '\'
    static auto if_fit_up_section_right_start = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, T_StructLineContainer &struct_line_container
        , int index)->bool
    {
        if( struct_line_container[index]->type != LineType::DOWN )
            return false;
        int line_0_beg_index = struct_line_container[index]->beg_index;
        int line_0_end_index = struct_line_container[index]->end_index;
        const double line_0_beg_price = kline_data_items[line_0_beg_index]->stk_item.high_price;
        const double line_0_end_price = kline_data_items[line_0_end_index]->stk_item.low_price;
        if( kline_data_items[struct_line_container[index + 1]->beg_index]->stk_item.low_price < line_0_end_price ) 
            return false;
         
        if( kline_data_items[struct_line_container[index + 3]->beg_index]->stk_item.low_price > line_0_beg_price )
            return false;
        return true;
    };

    static auto find_section_fit_hight_price = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, T_StructLineContainer &struct_line_container
        , bool is_section_down, const int i, const int j, std::set<int> &index_set) -> double
    {
        assert(j > i);
        int ret_index = 0;
        double min_price = MAX_PRICE;
        int right_start = 0;
        if( is_section_down )
            right_start = i + 1;
        else
            right_start = i;
        std::for_each(std::begin(index_set), std::end(index_set), [&](int val)
        {
            double total_dis = 0.0; 
            for( int temp_index = right_start; temp_index < j - 1; temp_index += 2 )
            {
                total_dis += fabs(kline_data_items[val]->stk_item.high_price 
                    - kline_data_items[struct_line_container[temp_index]->beg_index]->stk_item.high_price);
            }
            if( total_dis < min_price )
            {
                min_price = total_dis;
                ret_index = val;
            }
        });
        return kline_data_items[ret_index]->stk_item.high_price;
    };
    static auto find_section_fit_low_price = [](std::deque<std::shared_ptr<T_KlineDataItem> > &kline_data_items, T_StructLineContainer &struct_line_container
        , bool is_section_down, const int i, const int j, std::set<int> &index_set) -> double
    {
        assert(j > i);
        int ret_index = 0;
        double min_price = MAX_PRICE;
        int right_start = 0;
        if( is_section_down )
            right_start = i + 1;
        else
            right_start = i + 2;
        std::for_each(std::begin(index_set), std::end(index_set), [&](int val)
        {
            double total_dis = 0.0;
            for( int temp_index = right_start; temp_index < j - 1; temp_index += 2 )
            {
                total_dis += fabs(kline_data_items[val]->stk_item.low_price 
                    - kline_data_items[struct_line_container[temp_index]->end_index]->stk_item.low_price);
            }
            if( total_dis < min_price )
            {
                min_price = total_dis;
                ret_index = val;
            }
        });
        return kline_data_items[ret_index]->stk_item.low_price;
    };

    const int cst_least_line_num = 4; // have to >= 4
    if( kline_data_items.size() < cst_least_line_num )
        return;
    
    T_StructLineContainer &struct_line_container = GetStructLineContainer(period_type, code); 
    if( struct_line_container.empty() )
        return;

    T_SectionContainer &container = GetStructSectionContainer(period_type, code); 
    container.clear();
    // toward left 
    // todo:---------- 
    //  set line_count = 0;
    // 1: find up struct line a, and pre down struct line b
    // 2: judge if down struct line c begin point is lower than end point of a (endp_a)
    //    true: todo : 2.1: line_count++; find next down struct line d 
    //                 2.2: if begin point of d is higher than begin point of a (begp_a)
    //                      ture : goto 2.1
    //                      false: if line_count > 1 :
    //                                2.2.1: true : 2.2.1.1: set struct up line after line d as start line 0 of section 
    //                                              2.2.1.2. set end point of line 0 as top left of setion
    //                                              2.2.1.3  add section
    //                                       false: goto 1
    //   
    int i = 0;
    for( ; i < struct_line_container.size(); ++i ){ if( struct_line_container[i]->type == LineType::UP ) break; }
    if( i > struct_line_container.size() - cst_least_line_num )
        return;
    
    // ---j ----i--------
    while( i + 3 < struct_line_container.size() )
    {
        if( if_fit_down_section_right_start(kline_data_items, struct_line_container, i) )
        { 
            int j = i + 3;

            while( true )
            {
                ++j;
                if( j > struct_line_container.size() - 1 )
                {
                    ++i;
                    break;
                }
                //assert( struct_line_container[j]->type == LineType::UP );
                if( kline_data_items[struct_line_container[j]->beg_index]->stk_item.low_price 
                   > kline_data_items[struct_line_container[i + 1]->beg_index]->stk_item.high_price
                   || j == struct_line_container.size() - 1 )
                { 
                    T_Section  section;
                    section.top_left_index = (struct_line_container[j]->end_index + struct_line_container[j - 1]->end_index) / 2;
                    //section.top_left_price = kline_data_items[struct_line_container[i + 1]->beg_index]->stk_item.high_price;
                    std::set<int> top_index_set;
                    top_index_set.insert(struct_line_container[j - 2]->end_index);
                    top_index_set.insert(struct_line_container[i + 1]->beg_index);
                    section.top_left_price = find_section_fit_hight_price(kline_data_items, struct_line_container, true, i, j, top_index_set);

                    section.btm_right_index = (struct_line_container[i]->beg_index + struct_line_container[i]->end_index) / 2;
                    //section.btm_right_price = kline_data_items[struct_line_container[i]->beg_index]->stk_item.low_price;
                    std::set<int> btm_index_set;
                    btm_index_set.insert(struct_line_container[j - 1]->end_index);
                    btm_index_set.insert(struct_line_container[i + 1]->end_index);
                    section.btm_right_price = find_section_fit_low_price(kline_data_items, struct_line_container, true, i, j, btm_index_set);

                    container.push_back(std::move(section));

                    i = j;
                    break;
                }  

                ++j;
                //assert( struct_line_container[j]->type == LineType::DOWN );
                if( kline_data_items[struct_line_container[j]->beg_index]->stk_item.high_price 
                   < kline_data_items[struct_line_container[i + 1]->end_index]->stk_item.low_price 
                   || j == struct_line_container.size() - 1 )
                { 
                    T_Section  section;
                    section.top_left_index = (struct_line_container[j]->end_index + struct_line_container[j - 1]->end_index) / 2;
                    
                    std::set<int> top_index_set;
                    top_index_set.insert(struct_line_container[j - 1]->end_index);
                    top_index_set.insert(struct_line_container[i + 1]->beg_index);
                    section.top_left_price = find_section_fit_hight_price(kline_data_items, struct_line_container, true, i, j, top_index_set);

                    section.btm_right_index = (struct_line_container[i]->beg_index + struct_line_container[i]->end_index) / 2;
                    
                    std::set<int> btm_index_set;
                    btm_index_set.insert(struct_line_container[j - 2]->end_index);
                    btm_index_set.insert(struct_line_container[i + 1]->end_index);
                    section.btm_right_price = find_section_fit_low_price(kline_data_items, struct_line_container, true, i, j, btm_index_set);

                    container.push_back(std::move(section));

                    i = j - 1;
                    break;
                }
            }// while 

        }else if( if_fit_up_section_right_start(kline_data_items, struct_line_container, i) )
        {
            int j = i + 3;
            while( true )
            {
                ++j;
                if( j > struct_line_container.size() - 1 )
                {
                    ++i;
                    break;
                }
                // j is down line 
                if( kline_data_items[struct_line_container[j]->beg_index]->stk_item.high_price 
                    < kline_data_items[struct_line_container[i + 1]->beg_index]->stk_item.low_price
                   || j == struct_line_container.size() - 1 )
                { 
                    T_Section  section;
                    section.top_left_index = (struct_line_container[j]->end_index + struct_line_container[j - 1]->end_index) / 2;
                    
                    std::set<int> top_index_set;
                    top_index_set.insert(struct_line_container[j - 1]->end_index);
                    top_index_set.insert(struct_line_container[i]->beg_index);
                    section.top_left_price = find_section_fit_hight_price(kline_data_items, struct_line_container, false, i, j, top_index_set);
                    section.btm_right_index = (struct_line_container[i]->beg_index + struct_line_container[i]->end_index) / 2;
                   
                    std::set<int> btm_index_set;
                    btm_index_set.insert(struct_line_container[j - 2]->end_index);
                    btm_index_set.insert(struct_line_container[i + 1]->beg_index);
                    section.btm_right_price = find_section_fit_low_price(kline_data_items, struct_line_container, false, i, j, btm_index_set);

                    container.push_back(std::move(section));

                    i = j;
                    break;
                }

                ++j;
                // j is up line 
                if( kline_data_items[struct_line_container[j]->beg_index]->stk_item.low_price 
                   > kline_data_items[struct_line_container[i]->beg_index]->stk_item.high_price
                   || j == struct_line_container.size() - 1 )
                { 
                    T_Section  section;
                    section.top_left_index = (struct_line_container[j]->end_index + struct_line_container[j - 1]->end_index) / 2;
                   
                    std::set<int> top_index_set;
                    top_index_set.insert(struct_line_container[j - 2]->end_index);
                    top_index_set.insert(struct_line_container[i]->beg_index);
                    section.top_left_price = find_section_fit_hight_price(kline_data_items, struct_line_container, false, i, j, top_index_set);

                    section.btm_right_index = (struct_line_container[i]->beg_index + struct_line_container[i]->end_index) / 2;
                    
                    std::set<int> btm_index_set;
                    btm_index_set.insert(struct_line_container[j - 1]->end_index);
                    btm_index_set.insert(struct_line_container[i + 1]->beg_index);
                    section.btm_right_price = find_section_fit_low_price(kline_data_items, struct_line_container, false, i, j, btm_index_set);

                    container.push_back(std::move(section));

                    i = j - 1;
                    break;
                }
            }// while

        }else // not fit 
            ++i;
    } // while 
   
}

bool StockDataMan::GetInstrumentQuote(const std::string &code, int nmarket, T_Quote_Data &ret_quote_data)
{
    return tdx_exhq_wrapper_.GetInstrumentQuote(code, nmarket, ret_quote_data);
}

TypePeriod ToTypePeriod(PeriodType src)
{
    switch(src)
    {
    case PeriodType::PERIOD_1M: return TypePeriod::PERIOD_1M;
    case PeriodType::PERIOD_5M: return TypePeriod::PERIOD_5M;
    case PeriodType::PERIOD_15M: return TypePeriod::PERIOD_15M;
    case PeriodType::PERIOD_30M: return TypePeriod::PERIOD_30M;
    case PeriodType::PERIOD_HOUR: return TypePeriod::PERIOD_HOUR;
    case PeriodType::PERIOD_DAY: return TypePeriod::PERIOD_DAY;
    case PeriodType::PERIOD_WEEK: return TypePeriod::PERIOD_WEEK;
    case PeriodType::PERIOD_MON: return TypePeriod::PERIOD_MON;
    assert(false); 
    }
    return TypePeriod::PERIOD_DAY;
}

PeriodType ToPeriodType(TypePeriod src)
{
    switch(src)
    {
     case TypePeriod::PERIOD_1M:     return PeriodType::PERIOD_1M;  
     case TypePeriod::PERIOD_5M:     return PeriodType::PERIOD_5M; 
     case TypePeriod::PERIOD_15M:    return PeriodType::PERIOD_15M;
     case TypePeriod::PERIOD_30M:   return PeriodType::PERIOD_30M; 
     case TypePeriod::PERIOD_HOUR:  return PeriodType::PERIOD_HOUR;
     case TypePeriod::PERIOD_DAY:   return PeriodType::PERIOD_DAY; 
     case TypePeriod::PERIOD_WEEK:  return PeriodType::PERIOD_WEEK;
     case TypePeriod::PERIOD_MON:   return PeriodType::PERIOD_MON; 
     assert(false);
    }
    return PeriodType::PERIOD_DAY;
}

// < 0 : meaning no related data
int FindDataIndex(T_HisDataItemContainer &data_items_in_container, int date, int cur_hhmm)
{
    for( int i = data_items_in_container.size() - 1; i >= 0;  --i )
    {
        if( data_items_in_container.at(i)->stk_item.date == date && data_items_in_container.at(i)->stk_item.hhmmss == cur_hhmm)
            return i;
    }
    return -1;
}

bool IsDataIn(T_HisDataItemContainer &data_items_in_container, int date)
{
    auto iter = std::find_if(std::begin(data_items_in_container), std::end(data_items_in_container)
        , [date](T_HisDataItemContainer::reference entry){return entry->stk_item.date == date;} );

    return iter != data_items_in_container.end();
    
}

void TraverSetSignale(TypePeriod type_period, T_HisDataItemContainer &data_items_in_container, bool is_only_set_tail)
{ 
#if 1
    static auto proc_if_top_face = [](TypePeriod type_period, T_HisDataItemContainer &data_items_in_container, int index, bool is_only_set_tail)
    {
        const unsigned int max_inner_count = 5;
        // find left red k
        int target_front_index = -1;
        int k = index - 1;
        for( ; k >= 0 && k >= index - 1 - max_inner_count; --k ) // toward left
        { 
            if( data_items_in_container[k]->stk_item.low_price < data_items_in_container[index]->stk_item.low_price - EPSINON
                && data_items_in_container[k]->stk_item.high_price < data_items_in_container[index]->stk_item.high_price - EPSINON  )
            {
                if( data_items_in_container[k]->stk_item.close_price > data_items_in_container[k]->stk_item.open_price ) // red
                {
                    target_front_index = k;
                    break;
                }
            }else if( data_items_in_container[k]->stk_item.high_price > data_items_in_container[index]->stk_item.high_price - EPSINON )
                break;
        }
        if( target_front_index == -1 )
            return;
        // find right green k
        int target_follow_index = -1;
        k = index + 1;
        for( ; k < data_items_in_container.size() && k <= index + max_inner_count; ++k )
        {
            if( data_items_in_container[k]->stk_item.low_price < data_items_in_container[index]->stk_item.low_price - EPSINON
                && data_items_in_container[k]->stk_item.high_price < data_items_in_container[index]->stk_item.high_price - EPSINON  )
            {
                if( data_items_in_container[k]->stk_item.close_price < data_items_in_container[k]->stk_item.open_price ) // green
                {
                    target_follow_index = k;
                    break;
                }
            }else if( data_items_in_container[k]->stk_item.high_price > data_items_in_container[index]->stk_item.high_price - EPSINON )
                break;
        }
        if( target_follow_index == -1 )
            return;

        KGreenRedType left_gr_type = KGGetGreenRedType(data_items_in_container[target_front_index]->stk_item, type_period);
        KGreenRedType right_gr_type = KGGetGreenRedType(data_items_in_container[target_follow_index]->stk_item, type_period);

        data_items_in_container[index]->tag |= (int)TagType::SELL;
        if( is_only_set_tail )
            data_items_in_container[index]->type |= int(FractalType::TOP_AXIS_T_3);
 
    };
    static auto proc_if_down_face = [](TypePeriod type_period, T_HisDataItemContainer &data_items_in_container, int index, bool is_only_set_tail)
    {
        const unsigned int max_inner_count = 5;
        // find toward left 
        int target_front_index = -1;
        int k = index - 1;
        for( ; k >= 0 && k >= index - 1 - max_inner_count; --k )
        {
            if( data_items_in_container[k]->stk_item.low_price > data_items_in_container[index]->stk_item.low_price
                && data_items_in_container[k]->stk_item.high_price > data_items_in_container[index]->stk_item.high_price  )
            {
                if( data_items_in_container[k]->stk_item.close_price < data_items_in_container[k]->stk_item.open_price ) // GREEN
                {
                    target_front_index = k;
                    break;
                }
            }else if( data_items_in_container[k]->stk_item.low_price < data_items_in_container[index]->stk_item.low_price + EPSINON )
                break;
        }
        if( target_front_index == -1 )
            return;

        // find right red k
        int target_follow_index = -1;
        k = index + 1;
        for( ; k < data_items_in_container.size() && k <= index + max_inner_count; ++k )
        {
            if( data_items_in_container[k]->stk_item.low_price > data_items_in_container[index]->stk_item.low_price
                && data_items_in_container[k]->stk_item.high_price > data_items_in_container[index]->stk_item.high_price )
            {
                if( data_items_in_container[k]->stk_item.close_price > data_items_in_container[k]->stk_item.open_price ) // red
                {
                    target_follow_index = k;
                    break;
                }
            }else if( data_items_in_container[k]->stk_item.low_price < data_items_in_container[index]->stk_item.low_price + EPSINON )
                break;
        }
        if( target_follow_index == -1 )
            return;
        KGreenRedType left_gr_type = KGGetGreenRedType(data_items_in_container[target_front_index]->stk_item, type_period);
        KGreenRedType right_gr_type = KGGetGreenRedType(data_items_in_container[target_follow_index]->stk_item, type_period);
#if 0 
        if( data_items_in_container[target_follow_index]->stk_item.low_price > data_items_in_container[index]->stk_item.low_price 
            && data_items_in_container[target_follow_index]->stk_item.high_price > data_items_in_container[index]->stk_item.high_price 
            && data_items_in_container[target_front_index]->stk_item.low_price > data_items_in_container[index]->stk_item.low_price 
            && data_items_in_container[target_front_index]->stk_item.high_price > data_items_in_container[index]->stk_item.high_price )
        {

            if( left_gr_type >= KGreenRedType::SMALL_GREEN
                && right_gr_type >= KGreenRedType::SMALL_RED && right_gr_type < KGreenRedType::SMALL_GREEN )
            {
                data_items_in_container[index]->tag |= (int)TagType::BUY;
            }
            data_items_in_container[index]->type |= int(FractalType::BTM_AXIS_T_3);
        }
#else
        data_items_in_container[index]->tag |= (int)TagType::BUY;
        if( is_only_set_tail )
            data_items_in_container[index]->type |= int(FractalType::BTM_AXIS_T_3);
#endif
    };

#else
    static auto proc_if_face = [](TypePeriod type_period, T_HisDataItemContainer &data_items_in_container, int index)
    {
        if( data_items_in_container[index]->stk_item.close_price < data_items_in_container[index]->stk_item.open_price )
            data_items_in_container[index]->tag |= (int)TagType::SELL;
        else
            data_items_in_container[index]->tag |= (int)TagType::BUY;
        qDebug() << "proc_if_frace " << data_items_in_container[index]->stk_item.hhmmss << " set tag:" << data_items_in_container[index]->tag;
    };
#endif
    if( data_items_in_container.size() < 3 )
        return;
    int index = data_items_in_container.size() - 2;
    const unsigned int max_inner_count = 5;
    int count_when_only_set_tail = 50;
    while( index > 0 )
    {
        // clear sig
        if( (data_items_in_container[index]->tag & (int)TagType::BUY) == (int)TagType::BUY )
            data_items_in_container[index]->tag ^= (int)TagType::BUY;
        if( (data_items_in_container[index]->tag & (int)TagType::SELL) == (int)TagType::SELL )
            data_items_in_container[index]->tag ^= (int)TagType::SELL;
        // judge to set sig
        proc_if_top_face(type_period, data_items_in_container, index, is_only_set_tail);
        proc_if_down_face(type_period, data_items_in_container, index, is_only_set_tail);
        if( is_only_set_tail )
        {
            if( --count_when_only_set_tail == 0 )
                break;
        }
        --index;
    }
}