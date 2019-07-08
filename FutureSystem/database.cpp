#include "database.h"

#include <boost/lexical_cast.hpp>
#include <cctype>
#include <memory>

#include <SQLite/sqlite_connection.h>
#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_sqlite_functions.h>
#include <TLib/core/tsystem_utility_functions.h>
#include <Tlib/core/tsystem_time.h>

#include "exchange_calendar.h"
#include "futures_forecast_app.h"
#include "stock_man.h"

using namespace  TSystem;
DataBase::DataBase(FuturesForecastApp *app)
    : db_conn_(nullptr)
    , strand_(std::make_shared<TSystem::TaskStrand>(app->task_pool()))
{

}

DataBase::~DataBase()
{

}

bool DataBase::Initialize()
{
    Open(db_conn_);

    return db_conn_ != nullptr;
}

void DataBase::Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn/*, const std::string db_file*/)
{
    db_conn = std::make_shared<SQLite::SQLiteConnection>();

    std::string db_file = "./exchbase.kd";

    if( db_conn->Open(db_file.c_str(), SQLite::SQLiteConnection::OpenMode::READ_WRITE) != SQLite::SQLiteCode::OK )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
        , "DBMoudle::Open"
        , "can't open database: " + db_file);

}

void DataBase::LoadAllStockBaseInfo(std::shared_ptr<StockMan> &stock_man)
{
    assert(db_conn_);
    assert(stock_man->code_stock_baseinfo_item_map_.empty());
    assert(stock_man->pinyin_stock_baseinfo_item_map_.empty());

    if( !utility::ExistTable("stock", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
        , "DataBase::LoadAllStockBaseInfo"
        , "can't find table stock: ");

    std::string sql = utility::FormatStr("SELECT code, type, name, pinyin, timeToMarket, industry, area, remark FROM stock ORDER BY code");
    db_conn_->ExecuteSQL(sql.c_str(), [this, &stock_man](int /*num_cols*/, char** vals, char** /*names*/)->int
    {
        auto item = std::make_shared<T_StockBaseInfoItem>();
        try
        {
            item->code = *vals;
            item->type = boost::lexical_cast<int>(*(vals + 1));
            if( *(vals + 2) )
                item->name = *(vals + 2);
            if( *(vals + 3) )
                item->pinyin = *(vals + 3);
            item->time_to_market = boost::lexical_cast<int>(*(vals + 4));
            if( *(vals + 5) )
                item->industry = *(vals + 5);
            if( *(vals + 6) )
                item->area = *(vals + 6);
            if( *(vals + 7) )
                item->remark = *(vals + 7);
        }catch(boost::exception & )
        { 
            return 0;
        }

        stock_man->pinyin_stock_baseinfo_item_map_.insert( std::make_pair(item->pinyin, item) );
        stock_man->code_stock_baseinfo_item_map_.insert( std::make_pair(item->code, std::move(item)) );
        return 0;
    });
}


void DataBase::LoadTradeDate(void *exchange_calendar)
{
    assert(db_conn_);
    if( !db_conn_ )
        Open(db_conn_);

    if( !utility::ExistTable("ExchangeDate", *db_conn_) )
        throw "DBMoudle::LoadTradeDate can't find table ExchangeDate"; 

    int end_date = ExchangeCalendar::TodayAddDays(7); 
    std::string sql = utility::FormatStr("SELECT date, is_tradeday FROM ExchangeDate WHERE date <= %d ORDER BY date DESC", end_date);
    int num = 0;
    ((ExchangeCalendar*)exchange_calendar)->max_trade_date_ = 0;
    db_conn_->ExecuteSQL(sql.c_str(), [&num, &exchange_calendar, this](int /*num_cols*/, char** vals, char** /*names*/)->int
    { 
        try
        { 
            ++num;
            int date =  boost::lexical_cast<int>(*(vals)); 
            bool is_trade_date = boost::lexical_cast<bool>(*(vals + 1)); 
            ((ExchangeCalendar*)exchange_calendar)->trade_dates_->insert(std::make_pair(date, is_trade_date));

            if( ((ExchangeCalendar*)exchange_calendar)->max_trade_date_ == 0 )
                ((ExchangeCalendar*)exchange_calendar)->max_trade_date_ = date;

            ((ExchangeCalendar*)exchange_calendar)->min_trade_date_ = date;
        }catch(boost::exception& )
        {
            return 0;
        } 
        return 0;
    }); 
}


void DataBase::GetStockCode(const std::string &code, std::vector<T_StockCodeName>& ret)
{
    ret.clear();

    if( !db_conn_ )
    {
        Open(db_conn_);
    }

    std::string sql;
    if( IsStrAlpha(code) )
    {
        sql = utility::FormatStr("SELECT code, name, type, nmarket FROM stock WHERE pinyin like '%s%%' ORDER BY code LIMIT 5", code.c_str());
    }else if( IsStrNum(code) )
    {
        sql = utility::FormatStr("SELECT code, name, type, nmarket FROM stock WHERE code like '%s%%' ORDER BY code LIMIT 5", code.c_str());
    }else
    {
        sql = utility::FormatStr("SELECT code, name, type, nmarket FROM stock WHERE name like '%s%%' ORDER BY code LIMIT 5", code.c_str());
    }

    if( !utility::ExistTable("stock", *db_conn_) )
        return;

    db_conn_->ExecuteSQL(sql.c_str(),[&ret, this](int /*num_cols*/, char** vals, char** /*names*/)->int
    { /*
      T_StockCodeName code_name;
      code_name.code = *vals;
      code_name.name = *(vals + 1);*/
        std::string name = *(vals + 1);
        int type = boost::lexical_cast<int>(*(vals + 2));
        int nmarket = boost::lexical_cast<int>(*(vals + 3));
        utf8ToGbk(name);
        ret.emplace_back(*vals, std::move(name), type, nmarket);
        return 0;
    });
    return;
}
