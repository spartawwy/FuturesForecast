#ifndef MOCK_TRADE_DLG_DSFS3SDFSD_H_
#define MOCK_TRADE_DLG_DSFS3SDFSD_H_

#include <mutex>

#include <QtWidgets/QWidget>

#include "stkfo_common.h"
#include "train_trade.h"

#include "ui_mocktrade.h"

class MainWindow;
class MockTradeDlg : public QWidget
{
    Q_OBJECT

public:

    MockTradeDlg(MainWindow *main_win);

    void SetStatusBar(const QString & val);
     
    void UpDateStopProfitOrLossIfNecessary(int row_index, bool is_profit);
    double GetStopProfit(int trade_id);
    double GetStopLoss(int trade_id);

public slots:

    void slotHandleQuote(double /*cur_price*/, double /*sell1*/, double /*bull1*/, int /*sell_vol1*/, int /*bull_vol1*/);

    void slotOpenSell();
    void slotOpenBuy();
    void slotPositionClose();
    void slotBtnCondition();

private:

    void closeEvent(QCloseEvent * /*event*/) override;

    void ResetUi();

    void _OpenBuySell(bool is_buy);

    bool JudgeDoForceClose(double price);

private:

    Ui::MockTradeForm  ui;
    MainWindow *main_win_;

    AccountInfo  account_info_;

    T_Quote_Data  quote_data_;
    double cur_price_;

    double force_close_low_;
    double force_close_high_;

    double fee_rate_;
     
    std::vector<TradeRecordAtom>  trade_records_;

    std::mutex quote_data_mutex_;
};

#endif // MOCK_TRADE_DLG_DSFS3SDFSD_H_