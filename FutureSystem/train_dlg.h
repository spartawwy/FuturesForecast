#ifndef TRAIN_DLG_SDFS23343543_H_
#define TRAIN_DLG_SDFS23343543_H_

//#include <unordered_map>
#include <cassert>
#include <vector>
#include <QtWidgets/QWidget>

#include "ui_traindlg.h"

#include "stkfo_common.h"
#include "train_trade.h"

#include "train_trade_dlg.h"

struct AccountInfo
{
    AccountAtom  capital; 
    PositionInfo  position;
};

class KLineWall;
class MainWindow;
class TrainDlg : public QWidget
{
    Q_OBJECT

public:

    TrainDlg(KLineWall *parent, MainWindow *main_win);
     
    AccountInfo & account_info() { return account_info_; }

    const T_StockHisDataItem & CurHisStockDataItem();

public slots:

    void OnCalendarClicked(const QDate &);
    void OnStartTrain();
    void OnStopTrain();

    void OnMoveToNextK();
    void OnMoveToPreK();

    void OnOpenOpenWin();
    void OnOpenCloseWin();

    void OnTrade();

    
protected:

    virtual void closeEvent(QCloseEvent *) override;
    //virtual void hideEvent(QHideEvent * event) override;
private:

    void PrintTradeRecords();
    void SetStatusBar(const QString & val)
    {
        ui.lab_status->setText(val);
    }

private:

    Ui::TrainDlgForm ui;
     
    TrainTradeDlg  trade_dlg_;

    KLineWall *parent_;
    MainWindow *main_win_;
     
    bool is_started_;
    
    AccountInfo  account_info_;
    double force_close_low_;
    double force_close_high_;

    double fee_rate_;
     
    std::vector<TradeRecordAtom>  trade_records_;
};

#endif // TRAIN_DLG_SDFS23343543_H_