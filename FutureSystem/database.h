#ifndef DATA_BASE_SDFS_H_
#define DATA_BASE_SDFS_H_
 
#include <boost/thread.hpp>  
#include <boost/thread/recursive_mutex.hpp>  
#include <boost/thread/mutex.hpp>

#include <string>
#include <atomic>
#include <unordered_map>

struct T_StockCodeName
{
    std::string code;
    std::string name;
    int type;  // 0 --normal stock ; 1--index 
    int nmarket;
    T_StockCodeName(const std::string &cd, const std::string &nm, int tp, int maket) : code(cd), name(nm), type(tp), nmarket(maket){}
    T_StockCodeName(const T_StockCodeName &lh) : code(lh.code), name(lh.name), type(lh.type), nmarket(lh.nmarket){}
};
namespace SQLite
{
    class SQLiteConnection;
}

namespace TSystem
{
    class TaskStrand;
}
class FuturesForecastApp;
class StockMan;
class DataBase
{
public:

    DataBase(FuturesForecastApp *app);
    ~DataBase();

    bool Initialize();

    void LoadAllStockBaseInfo(std::shared_ptr<StockMan> &stock_man);
    void LoadTradeDate(void *exchange_calendar);

    void GetStockCode(const std::string &code, std::vector<T_StockCodeName>& ret);

private:
     
    DataBase(DataBase&);
    DataBase& operator = (DataBase&);

    void Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn);

    //TSystem::LocalLogger *local_logger_;
    //WinnerApp *app_;
    std::shared_ptr<SQLite::SQLiteConnection>  db_conn_;

    std::shared_ptr<TSystem::TaskStrand>  strand_;

    typedef boost::shared_mutex            WRMutex;  
    typedef boost::unique_lock<WRMutex>    WriteLock;  
    typedef boost::shared_lock<WRMutex>    ReadLock;  
};
 


#endif