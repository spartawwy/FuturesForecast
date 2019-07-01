#include "position_records_table_view.h"

#include <QStandardItemModel>
#include <QKeyEvent>
#include <QDebug>

#include "mock_trade_dlg.h"


PositionRecordsTableView::PositionRecordsTableView(MockTradeDlg * parent) : QTableView(parent)
    , parent_(parent)
{

}

void PositionRecordsTableView::keyPressEvent(QKeyEvent * event)
{
    if( parent_->is_closed() )
        return;
    qDebug() << __FUNCTION__ << " key:" << event->key() << " text:" << event->text() << " cur_index:" << currentIndex();
    auto model = static_cast<QStandardItemModel *>(this->model());
    if( !model || model->rowCount() <= 0 )
        return;
    auto key_val = event->key();
    if( /*key_val == Qt::Key_Enter || */ key_val == Qt::Key_Escape ||  key_val == Qt::Key_Tab )
    { 
        this->update(currentIndex());
        
        int trade_id = model->item(currentIndex().row(), cst_column_long_short)->data().toInt();
        if ( currentIndex().column() == cst_column_stop_profit_price )
        {
            // wwy: steal get old content. why ?
            QString new_str = model->item(currentIndex().row(), cst_column_stop_profit_price)->text().trimmed();
            if( !IsDouble(new_str.toLocal8Bit().data()) )
            {
                double old_price = parent_->GetStopProfit(trade_id);
                model->item(currentIndex().row(), cst_column_stop_profit_price)->setText(QString::number(old_price));
                return;
            } 
            
            parent_->UpDateStopProfitOrLossIfNecessary(currentIndex().row(), true);
        }else if( currentIndex().column() == cst_column_stop_loss_price )
        { 
            QString new_str = model->item(currentIndex().row(), cst_column_stop_loss_price)->text().trimmed();
            if( !IsDouble(new_str.toLocal8Bit().data()) )
            {
                double old_price = parent_->GetStopLoss(trade_id);
                model->item(currentIndex().row(), cst_column_stop_loss_price)->setText(QString::number(old_price));
                return;
            } 
            //this->update();
            parent_->UpDateStopProfitOrLossIfNecessary(currentIndex().row(), false);
        }
    } 
}

void PositionRecordsTableView::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
    qDebug() << "current :" << current << " previous:" << previous;
    if( parent_->is_closed() )
        return;
    auto model = static_cast<QStandardItemModel *>(this->model());
    if( !model || model->rowCount() <= 0 )
        return;

    int trade_id = model->item(currentIndex().row(), cst_column_long_short)->data().toInt();

    if( previous.column() == cst_column_stop_profit_price )
    {
        if( !IsDouble(model->item(currentIndex().row(), cst_column_stop_profit_price)->text().trimmed().toLocal8Bit().data()) )
        {
            double old_price = parent_->GetStopProfit(trade_id);
            model->item(currentIndex().row(), cst_column_stop_profit_price)->setText(QString::number(old_price));
            return;
        } 

        parent_->UpDateStopProfitOrLossIfNecessary(currentIndex().row(), true);

    }else if( previous.column()  == cst_column_stop_loss_price )
    { 
        if( !IsDouble(model->item(currentIndex().row(), cst_column_stop_loss_price)->text().trimmed().toLocal8Bit().data()) )
        {
            double old_price = parent_->GetStopLoss(trade_id);
            model->item(currentIndex().row(), cst_column_stop_loss_price)->setText(QString::number(old_price));
            return;
        } 

        parent_->UpDateStopProfitOrLossIfNecessary(currentIndex().row(), false);
    }
}