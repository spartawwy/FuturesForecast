#include "futures_forecast_app.h"

#include <thread>

#include <qmessageBox>
#include <QDateTime>
#include <qdebug.h>

#include <Tlib/core/tsystem_core_common.h>

#include "exchange_calendar.h"
#include "database.h"
#include "stock_man.h"
#include "stock_data_man.h"

#include "mainwindow.h"

FuturesForecastApp::FuturesForecastApp(int argc, char* argv[])
    : QApplication(argc, argv)
    , ServerClientAppBase("client", "futures_forecast", "1.0")
    , data_base_(nullptr)
    , stock_man_(nullptr)
    , stock_data_man_(nullptr)
    , main_window_(nullptr)
    , exit_flag_(false)
{
}

FuturesForecastApp::~FuturesForecastApp()
{
}

bool FuturesForecastApp::Init()
{
    option_dir_type(AppBase::DirType::STAND_ALONE_APP);
    option_validate_app(false);

    std::string cur_dir(".//");
    work_dir(cur_dir);
    local_logger_.SetDir(cur_dir);
    //---------------

    data_base_ = std::make_shared<DataBase>(this);
    if( !data_base_->Initialize() )
    {
        QMessageBox::information(nullptr, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("数据库初始化失败!"));
        return false;
    }
    stock_man_ = std::make_shared<StockMan>();

    data_base_->LoadAllStockBaseInfo(stock_man_);
    exchange_calendar_ = std::make_shared<ExchangeCalendar>();
    data_base_->LoadTradeDate(exchange_calendar_.get());

    stock_data_man_ = std::make_shared<StockDataMan>(exchange_calendar_.get(), local_logger());
    if( !stock_data_man_->Init() )
    {
        QMessageBox::information(nullptr, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("stock_data_man构件初始化失败!"));
        return false;
    }
    if( !stock_man_->Initialize() )
    {
        QMessageBox::information(nullptr, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("stock_man构件初始化失败!"));
        return false;
    }
      
    main_window_ = std::make_shared<MainWindow>(this);
    if( !main_window_->Initialize() )
        return false; 
    main_window_->show();
     
    //---------------
    this->task_pool().PostTask([this]()
    {
        int count = 0;
        int milli_second = 300;  //500
        while( !exit_flag_ )
        {
            Delay(milli_second);
            if( exit_flag_ )
                break;
            //if( ++count % 5 == 0 )
            if( ++count % 4 == 0 ) // 
                this->UpdateStockData();
            this->UpdateStockQuote();

        }
    });
    return true;
}

void FuturesForecastApp::Stop()
{
    exit_flag_ = true;
    Shutdown();
    this->quit();
}

void FuturesForecastApp::UpdateStockData()
{ 
    int cur_date = QDate::currentDate().year() * 10000 + QDate::currentDate().month() * 100 + QDate::currentDate().day();
    int cur_hhmm = QTime::currentTime().hour() * 100 + QTime::currentTime().minute();
    int target_date = 0;

    int pre_date = exchange_calendar()->PreTradeDate(cur_date, 1);
    bool is_trade_time = false;
    if( exchange_calendar()->IsTradeDate(cur_date) && exchange_calendar()->IsTradeTime(cur_hhmm) )
    {
        is_trade_time = true;
        target_date = cur_date;
    }else if( exchange_calendar()->IsTradeDate(pre_date) && exchange_calendar()->IsMidNightTradeTime(cur_hhmm) )
    {
        is_trade_time = true;
        target_date = exchange_calendar()->NextTradeDate(cur_date, 1);
    }
    if( !is_trade_time )
        return;

     main_window()->UpdateStockData(target_date, cur_hhmm);// update main and sub window stock data
     
     ////////////////////////////////////
     TypePeriod  type_periods_to_judge[] = {TypePeriod::PERIOD_1M, TypePeriod::PERIOD_5M, TypePeriod::PERIOD_15M, TypePeriod::PERIOD_30M, TypePeriod::PERIOD_HOUR
         , TypePeriod::PERIOD_DAY, TypePeriod::PERIOD_WEEK}; 

     if( !main_window()->MainKlineWall() )
         return;

     std::vector<TypePeriod> target_periods;

     // update other k period stock data which has been opened----------------
                       
     for( unsigned int i = 0; i < sizeof(type_periods_to_judge)/sizeof(type_periods_to_judge[0]); ++i )
     {
         if( main_window()->MainKlineWall()->k_type() == type_periods_to_judge[i] )
             continue;
         if( main_window()->SubKlineWall() && main_window()->SubKlineWall()->k_type() == type_periods_to_judge[i] )
             continue; 

         T_HisDataItemContainer &container = stock_data_man_->GetHisDataContainer(type_periods_to_judge[i], main_window()->MainKlineWall()->stock_code());
         if( container.empty() ) // has not opened
             continue;

         target_periods.push_back(type_periods_to_judge[i]);
     }

     std::string code = main_window()->MainKlineWall()->stock_code();
     int nmarket = main_window()->MainKlineWall()->nmarket();
     
     for( unsigned int j = 0; j < target_periods.size(); ++j )
     {
         UpdateStockData(target_date, cur_hhmm, code, target_periods[j], nmarket);
     }
}

void FuturesForecastApp::UpdateStockData(int target_date, int cur_hhmm, const std::string &code, TypePeriod type_period, int nmarket)
{
    //bool is_need_updated = false;
    //int hhmm = GetKDataTargetStartTime(type_period, cur_hhmm);
    T_HisDataItemContainer &container = stock_data_man().GetHisDataContainer(ToPeriodType(type_period), code);
    if( !container.empty() )
    {
        int r_index = FindKRendIndexInHighPeriodContain(type_period, container, *exchange_calendar_, target_date, cur_hhmm);
        
        //auto p_contain = stock_data_man().FindStockData(ToPeriodType(type_period), code, target_date, target_date, hhmm/*, bool is_index*/);
        //if( p_contain ) // current time k data exists
        if( r_index > -1 )
        { 
            local_logger().LogLocal(TSystem::utility::FormatStr("UpdateStockData k ex %d %04d tp:%d", target_date, cur_hhmm, type_period));
            int ret = stock_data_man().UpdateOrAppendLatestItemStockData(ToPeriodType(type_period), nmarket, code, false);
            if( ret == 1 )
                TraverSetSignale(type_period, container, true);
            else if( ret == 2 )
            {
                TraverSetSignale(type_period, container, false);
                stock_data_man().TraverseGetStuctLines(ToPeriodType(type_period), code, container);
            }
            
        }else
        {
            int pre_k_num = container.size();
            auto date_time = GetKDataTargetDateTime(*exchange_calendar(), type_period, target_date, cur_hhmm, WOKRPLACE_DEFUALT_K_NUM);
            auto p_cur_time_contain = stock_data_man().AppendStockData(ToPeriodType(type_period), nmarket, code, std::get<0>(date_time), target_date, false);
            local_logger().LogLocal(TSystem::utility::FormatStr("UpdateStockData k app <%d %d %d> tp:%d %d %d", std::get<0>(date_time), std::get<1>(date_time), target_date, type_period, pre_k_num, p_cur_time_contain->size()));
        }
    }
}

void FuturesForecastApp::UpdateStockQuote()
{
    main_window()->UpdateStockQuote();
}



int GetOverDayPoint(TypePeriod tp_period, int hhmm)
{
    int over_day_point = 0;
    switch(tp_period)
    {
    case TypePeriod::PERIOD_1M: over_day_point = 2359; break;
    case TypePeriod::PERIOD_5M: over_day_point = 2355; break;
    case TypePeriod::PERIOD_15M: over_day_point = 2345; break;
    case TypePeriod::PERIOD_30M: over_day_point = 2330; break;
    case TypePeriod::PERIOD_HOUR: over_day_point = 2300; break;
   
    case TypePeriod::PERIOD_DAY: 
    case TypePeriod::PERIOD_WEEK: 
    case TypePeriod::PERIOD_MON: 
        over_day_point = hhmm + 1; break;
    default: break;
    }
    return over_day_point;
}

int find_over_day_index(T_HisDataItemContainer &hisdata_container, int target_day)
{ 
    int j = 0; 
    auto iter = hisdata_container.rbegin(); 
    for( auto iter = hisdata_container.rbegin();
        iter != hisdata_container.rend(); 
        ++iter, ++j )
    { 
        if( target_day == iter->get()->stk_item.date && iter->get()->stk_item.hhmmss == 0 )
            return j;
    }
    return -1;
}

int find_over_day_index2right(T_HisDataItemContainer &hisdata_container, int target_day)
{
    int i = 0; 
    for( auto iter = hisdata_container.begin();
        iter != hisdata_container.end(); 
        ++iter, ++i )
    { 
        if( target_day == iter->get()->stk_item.date && iter->get()->stk_item.hhmmss == 0 )
            return i;
    }
    return -1;
}

// ps: from p_hisdata_container back to front
int FindKRendIndexInHighPeriodContain(TypePeriod tp_period, T_HisDataItemContainer &p_hisdata_container, ExchangeCalendar &calender, int date_val, int hhmm)
{ 
    int over_day_point = GetOverDayPoint(tp_period, hhmm);
    bool is_find = false;
    int j = 0;
    int near_span = 99999;
    int near_j = -1;
    auto iter = p_hisdata_container.rbegin();
     
    for( ;iter != p_hisdata_container.rend(); ++iter, ++j ) // data has been sort by left small(date and hhmm) to right big(date and hhmm)
    { 
        if( iter->get()->stk_item.date < date_val )
        {
            near_span = date_val - iter->get()->stk_item.date;
            near_j = j - 1;
            break;
        }
        if( iter->get()->stk_item.date != date_val )
            continue;

        auto my_pre_iter = iter;
        bool pre_item_exist = (iter + 1) != p_hisdata_container.rend();
        bool next_item_exist = iter != p_hisdata_container.rbegin();
        if( pre_item_exist )
        {
            my_pre_iter = iter + 1; 
        }else if( hhmm <= iter->get()->stk_item.hhmmss ) // first k 
        {
            is_find = true;
            break;
        }
        if( iter->get()->stk_item.hhmmss == hhmm )
        {
            is_find = true;
            break;
        }else if( hhmm > over_day_point )
        {
            int target_day = calender.NextTradeDate(date_val, 1);
            int targ_index = find_over_day_index(p_hisdata_container, target_day);
            return targ_index;

        }else if( pre_item_exist 
            &&   ( 
            (my_pre_iter->get()->stk_item.date == date_val && (my_pre_iter->get()->stk_item.hhmmss < hhmm && hhmm < iter->get()->stk_item.hhmmss) )
            || my_pre_iter->get()->stk_item.date < date_val 
                  ) // hhmm in current k's duration
            ) 
        {
            int k_date = iter->get()->stk_item.date;
            int k_hhmm = iter->get()->stk_item.hhmmss;
            int pre_k_date = my_pre_iter->get()->stk_item.date;
            int pre_k_hhmm = my_pre_iter->get()->stk_item.hhmmss;
            is_find = true;
            break;
        }

    } // for
    if( is_find )
        return j;
    else
        return -1;
}


int GetKTagDateTime(TypePeriod type_period, int para_hhmm)
{
    static auto get_hhmm = [](int hhmm_para, int *tp_array, int num)->int
    {
        int ret_hhmm = 0;
        assert(num > 0);
        for( int i = 0; i < num; ++i )
        {
            if( hhmm_para <= tp_array[i] ) 
            {
                if( tp_array[i] == 2359 )
                    ret_hhmm = 0;
                else
                    ret_hhmm = tp_array[i];
                return ret_hhmm;
            }
        }
        if( tp_array[num-1] == 2359 )
            ret_hhmm = 0;
        else
            ret_hhmm = tp_array[num-1];
        return ret_hhmm;
    };

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
            int tp_array[] = {100, 200, 930, 1045, 1345, 1445, 1500, 2200, 2300, 2359};
            hhmm = get_hhmm(para_hhmm, tp_array, sizeof(tp_array)/sizeof(tp_array[0]));
            break;
        }
    case TypePeriod::PERIOD_30M:
        {
            int tp_array[] = {30, 100, 130, 200, 230, 930, 1045, 1345, 1445, 1500
                , 2130, 2200, 2230, 2300, 2330, 2359};
            hhmm = get_hhmm(para_hhmm, tp_array, sizeof(tp_array)/sizeof(tp_array[0]));
            break; 
        }
    case TypePeriod::PERIOD_15M:
        {
            int tp_array[] = { 15, 30, 45, 100, 115, 130, 145, 200, 215, 230
                , 915, 930, 945, 1000, 1015, 1045, 1100, 1115, 1130, 1345, 1400, 1415, 1430, 1445, 1500
                , 2115, 2130, 2145, 2200, 2215, 2230, 2245, 2300, 2315, 2330, 2345, 2359 };
            hhmm = get_hhmm(para_hhmm, tp_array, sizeof(tp_array)/sizeof(tp_array[0]));
            break;
        }
    case TypePeriod::PERIOD_5M:
        {
            int tp_array[] = {5, 10, 15,20,25,30,35,40,45,50,55,100,105,110,115,120,125,130,135,140,145,150,155
                ,200,205,210,215,220,225,230,   905, 910, 915, 920, 925,930
                ,935,940,945,950,955,1000,1005,1010,1015,1035,1040,1045,1050,1055,1100,1105
                ,1110,1115,1120,1125,1130,1335,1340,1345,1350,1355,1400,1405
                ,1410,1415,1420,1425,1430,1435,1440,1445,1450,1455,1500
                , 2105, 2110,2115, 2120, 2125,2130,2135,2140,2145,2150,2155,2200
                , 2205, 2210,2215, 2220, 2225,2230,2235,2240,2245,2250,2255,2300
                , 2305, 2310,2315, 2320, 2325,2330,2335,2340,2345,2350,2355,2359};
            hhmm = get_hhmm(para_hhmm, tp_array, sizeof(tp_array)/sizeof(tp_array[0]));
            break;
        }
    case TypePeriod::PERIOD_1M:
        hhmm = para_hhmm;
        break;
    } 
    return hhmm;
}


int GetKDataNatureStartTime(TypePeriod type_period)
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
    case TypePeriod::PERIOD_30M: 
        hhmm = 930;// checked from tdx
        break; 
    case TypePeriod::PERIOD_15M: 
        hhmm = 915;
        break; 
    case TypePeriod::PERIOD_5M:  
        hhmm = 905;
        break; 
    case TypePeriod::PERIOD_1M:  
        hhmm = 901;
        break; 
    }  
    return hhmm;
}

int GetKDataTargetStartTime(TypePeriod type_period, int para_hhmm)
{ 
    int hhmm = 0;  
    if( para_hhmm < 900 )
        hhmm = 0;
    else
    {
        switch( type_period )
        {
        case TypePeriod::PERIOD_YEAR: 
        case TypePeriod::PERIOD_MON: 
        case TypePeriod::PERIOD_DAY:
        case TypePeriod::PERIOD_WEEK: 
            break;
        case TypePeriod::PERIOD_HOUR:    
        case TypePeriod::PERIOD_30M: 
            hhmm = 930;// checked from tdx
            break; 
        case TypePeriod::PERIOD_15M: 
            hhmm = 915;
            break; 
        case TypePeriod::PERIOD_5M:  
            hhmm = 905;
            break; 
        case TypePeriod::PERIOD_1M:  
            hhmm = 901;
            break; 
        }
    }

    return hhmm;
}

void Delay(__int64 mseconds)
{ 
    std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));
}