#include "position_records_table_view.h"

#include <QStandardItemModel>
#include <QKeyEvent>
#include <QDebug>

#include "mock_trade_dlg.h"


PositionRecordsTableView::PositionRecordsTableView(QWidget *wid_parent, MockTradeDlg * parent) : QTableView(wid_parent)
    , parent_(parent)
{

}

void PositionRecordsTableView::keyPressEvent(QKeyEvent * event)
{
    if( parent_->is_closed() )
        return;
    qDebug() << __FUNCTION__ << " key:" << event->key() << " text:" << event->text() << " cur_index:" << currentIndex();
    auto model = static_cast<QStandardItemModel *>(this->model());
    if( !model || model->rowCount() <= 0 || currentIndex().row() < 0 )
        return;
    do
    {
        auto key_val = event->key();
        if( /*key_val == Qt::Key_Enter || */ key_val == Qt::Key_Escape ||  key_val == Qt::Key_Tab )
        { 
            //this->update(currentIndex());
            int trade_id = model->item(currentIndex().row(), cst_column_long_short)->data().toInt();
            if ( currentIndex().column() == cst_column_stop_profit_price )
            {
                // wwy: steal get old content. why ?
                QString new_str = model->item(currentIndex().row(), cst_column_stop_profit_price)->text().trimmed();
                double stop_price = 0.0;
                bool is_illegal_price = false;
                if( !TransToDouble(new_str.toLocal8Bit().data(), stop_price) )
                {
                    is_illegal_price = true;
                    parent_->SetStatusBar(QString::fromLocal8Bit("价格为非法字符!"));
                
                }else if( !parent_->IsLegalStopPrice(trade_id, stop_price, true) )
                {
                    is_illegal_price = true;
                    parent_->SetStatusBar(QString::fromLocal8Bit("止赢价不合理!"));
                }
                if( is_illegal_price )
                {
                    double old_price = parent_->GetStopProfit(trade_id);
                    model->item(currentIndex().row(), cst_column_stop_profit_price)->setText(QString::number(old_price));
                    break;
                }
                parent_->UpDateStopProfitOrLossIfNecessary(currentIndex().row(), true);
            }else if( currentIndex().column() == cst_column_stop_loss_price )
            { 
                QString new_str = model->item(currentIndex().row(), cst_column_stop_loss_price)->text().trimmed();
                double stop_price = 0.0;
                bool is_illegal_price = false;
                if( !TransToDouble(new_str.toLocal8Bit().data(), stop_price) )
                {
                    is_illegal_price = true;
                    parent_->SetStatusBar(QString::fromLocal8Bit("价格为非法字符!"));
                }else if( !parent_->IsLegalStopPrice(trade_id, stop_price, false) )
                {
                    is_illegal_price = true;
                    parent_->SetStatusBar(QString::fromLocal8Bit("止损价不合理!"));
                }
                if( is_illegal_price )
                {
                    double old_price = parent_->GetStopProfit(trade_id);
                    model->item(currentIndex().row(), cst_column_stop_loss_price)->setText(QString::number(old_price));
                    break;
                }
                 
                parent_->UpDateStopProfitOrLossIfNecessary(currentIndex().row(), false);
            }
        } 
    }while(0);
    QTableView::keyPressEvent(event);
}

void PositionRecordsTableView::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
    qDebug() << "current :" << current << " previous:" << previous;
    if( parent_->is_closed() )
        return;
    auto model = static_cast<QStandardItemModel *>(this->model());
    if( !model || model->rowCount() <= 0 || currentIndex().row() < 0 )
        return;
    do 
    {
        int trade_id = model->item(currentIndex().row(), cst_column_long_short)->data().toInt();

        if( previous.column() == cst_column_stop_profit_price )
        {
            double stop_price = 0.0;
            bool is_illegal_price = false;
            QString new_str = model->item(currentIndex().row(), cst_column_stop_profit_price)->text().trimmed();
            if( !TransToDouble(new_str.toLocal8Bit().data(), stop_price) )
            {
                is_illegal_price = true;
                parent_->SetStatusBar(QString::fromLocal8Bit("价格为非法字符!"));
            }else if( !parent_->IsLegalStopPrice(trade_id, stop_price, true) )
            {
                is_illegal_price = true;
                parent_->SetStatusBar(QString::fromLocal8Bit("止赢价不合理!"));
            }
            if( is_illegal_price )
            {
                double old_price = parent_->GetStopProfit(trade_id);
                model->item(currentIndex().row(), cst_column_stop_profit_price)->setText(QString::number(old_price));
                break;
            }
            parent_->UpDateStopProfitOrLossIfNecessary(currentIndex().row(), true);

        }else if( previous.column()  == cst_column_stop_loss_price )
        {  
            double stop_price = 0.0;
            bool is_illegal_price = false;
            QString new_str = model->item(currentIndex().row(), cst_column_stop_loss_price)->text().trimmed();
            if( !TransToDouble(new_str.toLocal8Bit().data(), stop_price) )
            {
                is_illegal_price = true;
                parent_->SetStatusBar(QString::fromLocal8Bit("价格为非法字符!"));
            }else if( !parent_->IsLegalStopPrice(trade_id, stop_price, false) )
            {
                is_illegal_price = true;
                parent_->SetStatusBar(QString::fromLocal8Bit("止损价不合理!"));
            }
            if( is_illegal_price )
            {
                double old_price = parent_->GetStopProfit(trade_id);
                model->item(currentIndex().row(), cst_column_stop_loss_price)->setText(QString::number(old_price));
                break;
            }
            parent_->UpDateStopProfitOrLossIfNecessary(currentIndex().row(), false);
        }
    } while (0); 
    QTableView::currentChanged(current, previous);
}