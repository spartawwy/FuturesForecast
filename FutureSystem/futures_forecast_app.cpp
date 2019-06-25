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
        QMessageBox::information(nullptr, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("���ݿ��ʼ��ʧ��!"));
        return false;
    }
    stock_man_ = std::make_shared<StockMan>();

    data_base_->LoadAllStockBaseInfo(stock_man_);
    exchange_calendar_ = std::make_shared<ExchangeCalendar>();
    data_base_->LoadTradeDate(exchange_calendar_.get());

    stock_data_man_ = std::make_shared<StockDataMan>(exchange_calendar_.get());
    if( !stock_data_man_->Init() )
    {
        QMessageBox::information(nullptr, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("stock_data_man������ʼ��ʧ��!"));
        return false;
    }
    if( !stock_man_->Initialize() )
    {
        QMessageBox::information(nullptr, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("stock_man������ʼ��ʧ��!"));
        return false;
    }

    this->task_pool().PostTask([this]()
    {
        int count = 0;
        while( !exit_flag_ )
        {
            Delay(1000);
            if( exit_flag_ )
                break;
            if( ++count % 5 == 0 )
                this->UpdateStockData();
            this->UpdateStockQuote();
            
        }
    });
    //---------------
    main_window_ = std::make_shared<MainWindow>(this);
    if( !main_window_->Initialize() )
        return false; 
    main_window_->show();
     
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
     
}

void FuturesForecastApp::UpdateStockQuote()
{
    main_window()->UpdateStockQuote();
}

void Delay(__int64 mseconds)
{ 
    std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));
}