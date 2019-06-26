#ifndef MOCK_TRADE_DLG_DSFS3SDFSD_H_
#define MOCK_TRADE_DLG_DSFS3SDFSD_H_

#include <mutex>

#include <QtWidgets/QWidget>

#include "stkfo_common.h"
#include "train_trade.h"

#include "ui_mocktrade.h"

class MockTradeDlg : public QWidget
{
    Q_OBJECT

public:

    MockTradeDlg();

    void SetStatusBar(const QString & val);

public slots:

    void slotHandleQuote(double /*cur_price*/, double /*sell1*/, double /*bull1*/, int /*sell_vol1*/, int /*bull_vol1*/);

    void slotOpenSell();
    void slotOpenBuy();
    void slotPositionClose();
    void slotBtnCondition();

private:
    void _OpenBuySell(bool is_buy);

private:

    Ui::MockTradeForm  ui;
     
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