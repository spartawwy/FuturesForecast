#include "futures_forecast_app.h"

#include <thread>

#include <qmessageBox>
#include <QDateTime>
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

     main_window()->UpdateStockData(target_date, cur_hhmm);
     
     ////////////////////////////////////
     TypePeriod  type_periods_to_judge[] = {TypePeriod::PERIOD_1M, TypePeriod::PERIOD_5M, TypePeriod::PERIOD_15M, TypePeriod::PERIOD_30M, TypePeriod::PERIOD_HOUR
         , TypePeriod::PERIOD_DAY, TypePeriod::PERIOD_WEEK}; 

     if( !main_window()->MainKlineWall() )
         return;

     std::vector<TypePeriod> target_periods;

     // update other k period stock data which has been opened----------------

     for( int i = 0; i < sizeof(type_periods_to_judge)/sizeof(type_periods_to_judge[0]); ++i )
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
     
     for( int j = 0; j < target_periods.size(); ++j )
     {
         UpdateStockData(target_date, cur_hhmm, code, target_periods[j], nmarket);
     }
}

void FuturesForecastApp::UpdateStockData(int target_date, int cur_hhmm, const std::string &code, TypePeriod type_period, int nmarket)
{
    //bool is_need_updated = false;
    int hhmm = GetKDataTargetStartTime(type_period, cur_hhmm);
    
    T_HisDataItemContainer &container = stock_data_man().GetHisDataContainer(ToPeriodType(type_period), code);
    if( !container.empty() )
    {
        auto p_contain = stock_data_man().FindStockData(ToPeriodType(type_period), code, target_date, target_date, hhmm/*, bool is_index*/);
        if( p_contain ) // current time k data exists
        { 
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
            auto date_time = GetKDataTargetDateTime(*exchange_calendar(), type_period, target_date, cur_hhmm, WOKRPLACE_DEFUALT_K_NUM);
            auto p_cur_time_contain = stock_data_man().AppendStockData(ToPeriodType(type_period), nmarket, code, std::get<0>(date_time), target_date, false);
        }
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