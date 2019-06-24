#ifndef MOCK_TRADE_DLG_DSFS3SDFSD_H_
#define MOCK_TRADE_DLG_DSFS3SDFSD_H_

#include <QtWidgets/QWidget>

#include "stkfo_common.h"

#include "ui_mocktrade.h"

class MockTradeDlg : public QWidget
{
public:

    Q_OBJECT

    MockTradeDlg();

private:

    Ui::MockTradeForm  ui;

};

#endif // MOCK_TRADE_DLG_DSFS3SDFSD_H_