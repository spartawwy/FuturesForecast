#include "futures_forecast_app.h"

#include <thread>

#include <qmessageBox>
#include <qdebug.h>

#include <Tlib/core/tsystem_core_common.h>

#include "exchange_calendar.h"
#include "database.h"
#include "stock_man.h"
 
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

    stock_data_man_ = std::make_shared<StockDataMan>(exchange_calendar_.get());
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
        int milli_second = 500;
        while( !exit_flag_ )
        {
            Delay(milli_second);
            if( exit_flag_ )
                break;
            if( ++count % 5 == 0 )
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
     main_window()->UpdateStockData();
     

     ////////////////////////////////////
     TypePeriod  type_periods_to_judge[] = {TypePeriod::PERIOD_1M, TypePeriod::PERIOD_5M, TypePeriod::PERIOD_15M, TypePeriod::PERIOD_30M, TypePeriod::PERIOD_HOUR
         , TypePeriod::PERIOD_DAY, TypePeriod::PERIOD_WEEK}; 

     if( !main_window()->MainKlineWall() )
         return;

     std::vector<TypePeriod> target_periods;

     // update other k period stock data
     for( int i = 0; i < sizeof(type_periods_to_judge)/sizeof(type_periods_to_judge[0]); ++i )
     {
         if( main_window()->MainKlineWall()->k_type() == type_periods_to_judge[i] )
             continue;
         if( main_window()->SubKlineWall() && main_window()->SubKlineWall()->k_type() == type_periods_to_judge[i] )
             continue; 

         T_HisDataItemContainer &container = stock_data_man_->GetHisDataContainer(type_periods_to_judge[i], main_window()->MainKlineWall()->stock_code());

         if( container.empty() ) // has no opened
             continue;

         target_periods.push_back(type_periods_to_judge[i]);

     }

     
}

void FuturesForecastApp::UpdateStockData(TypePeriod  type_period)
{
    bool is_need_updated = false;

    int cur_date = QDate::currentDate().year() * 10000 + QDate::currentDate().month() * 100 + QDate::currentDate().day();
    int cur_hhmm = QTime::currentTime().hour() * 100 + QTime::currentTime().minute();
    int target_date = 0;
 
    int pre_date = exchange_calendar()->PreTradeDate(cur_date, 1);
    bool is_trade_time = false;
    if( exchange_calendar()->IsTradeDate(cur_date) && exchange_calendar()->IsTradeTime(cur_hhmm) )
    {
        is_trade_time = true;
        target_date = cur_date;
    }else if( exchange_calendar()->IsTradeDate(pre_date) && app_->exchange_calendar()->IsMidNightTradeTime(cur_hhmm) )
    {
        is_trade_time = true;
        target_date = app_->exchange_calendar()->NextTradeDate(cur_date, 1);
    }
    if( !is_trade_time )
        return;

    //std::lock_guard<std::mutex> locker(painting_mutex_);
     
    int hhmm = GetKDataTargetStartTime(k_type_, cur_hhmm);
    
    T_HisDataItemContainer &container = app_->stock_data_man().GetHisDataContainer(ToPeriodType(k_type_), stock_code_);
    if( !container.empty() )
    {
        auto p_contain = app_->stock_data_man().FindStockData(ToPeriodType(k_type_), stock_code_, target_date, target_date, hhmm/*, bool is_index*/);
        if( p_contain ) // current time k data exists
        {
            if( draw_action_ != DrawAction::NO_ACTION || main_win_->is_train_mode() )
                return;
            int ret = app_->stock_data_man().UpdateOrAppendLatestItemStockData(ToPeriodType(k_type_), nmarket_, stock_code_, is_index_);
            if( ret == 1 )
                TraverSetSignale(k_type_, container, true);
            else if( ret == 2 )
                TraverSetSignale(k_type_, container, false);
            is_need_updated = ret > 0;
            
        }else
        {
             
            auto date_time = GetKDataTargetDateTime(*app_->exchange_calendar(), k_type_, target_date, cur_hhmm, WOKRPLACE_DEFUALT_K_NUM);
            auto p_cur_time_contain = app_->stock_data_man().AppendStockData(ToPeriodType(k_type_), nmarket_, stock_code_, std::get<0>(date_time), target_date, is_index_);

            if( draw_action_ != DrawAction::NO_ACTION || main_win_->is_train_mode() )
                return;
            if( p_cur_time_contain )
            {
                p_hisdata_container_ = p_cur_time_contain;
                is_need_updated = true;
            }
        }
       /* if( is_need_updated )
        {
            Emit_UpdateKwall(); 
        }*/
    }
}

void FuturesForecastApp::UpdateStockQuote()
{
    main_window()->UpdateStockQuote();
}

void Delay(__int64 mseconds)
{ 
    std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));
}