#ifndef STK_FORECAST_APP_SDF8533_H_
#define STK_FORECAST_APP_SDF8533_H_

#include <memory>

#include <QtWidgets/QApplication>
#include <QThread>

#include <TLib/tool/tsystem_server_client_appbase.h>
#include <TLib/core/tsystem_communication_common.h>
#include <TLib/core/tsystem_serialization.h>

#include "stock_data_man.h"

//class QTimer;
class ExchangeCalendar;
class DataBase;
class StockMan;
class MainWindow;
class KLineWall;
class FuturesForecastApp : public QApplication, public TSystem::ServerClientAppBase
{
    //Q_OBJECT

public:

    FuturesForecastApp(int argc, char* argv[]); 
    ~FuturesForecastApp();

    bool Init();
    void Stop();

    std::shared_ptr<ExchangeCalendar>&  exchange_calendar() { return exchange_calendar_;}
    std::shared_ptr<DataBase>& data_base() { return data_base_; }

    StockDataMan & stock_data_man() { return *stock_data_man_; }

    MainWindow * main_window() { return main_window_.get(); }

    void UpdateStockData();
     
    void UpdateStockQuote();

protected:

    virtual void HandleNodeHandShake(TSystem::communication::Connection* , const TSystem::Message& ) override {};
    virtual void HandleNodeDisconnect(std::shared_ptr<TSystem::communication::Connection>& 
        , const TSystem::TError& ) override {};

private:
    void UpdateStockData(int target_date, int cur_hhmm, const std::string &code, TypePeriod  type_period, int nmarket);

private:

    std::shared_ptr<MainWindow>  main_window_;

    std::shared_ptr<DataBase>  data_base_;
    std::shared_ptr<ExchangeCalendar>  exchange_calendar_;
    std::shared_ptr<StockMan>  stock_man_;

    std::shared_ptr<StockDataMan>  stock_data_man_;

    bool exit_flag_;
     
};

void Delay(__int64 mseconds);

#endif // STK_FORECAST_APP_SDF8533_H_