#ifndef POSITION_RECORDS_TABLE_VIEW_SDFS_H
#define POSITION_RECORDS_TABLE_VIEW_SDFS_H

#include <QTableView>

static int cst_column_long_short = 0;
static int cst_column_qty = 1;
static int cst_column_ava_price = 2;
static int cst_column_float_profit = 3;
static int cst_column_stop_loss_price = 4;
static int cst_column_stop_profit_price = 5;
//static int cst_column_data = 6;

class MockTradeDlg;
class PositionRecordsTableView : public QTableView
{
    Q_OBJECT

public:
    PositionRecordsTableView(MockTradeDlg * parent);

protected:

    //virtual bool edit(const QModelIndex & index, EditTrigger trigger, QEvent * event) override;
    virtual void keyPressEvent(QKeyEvent * event) override;
    virtual void currentChanged(const QModelIndex & current, const QModelIndex & previous) override;

private:
    MockTradeDlg *parent_;

};

#endif