#include "code_list_wall.h"

#include <QStandardItemModel>
#include <qevent.h> 

#include <TLib/core/tsystem_utility_functions.h>

#include "futures_forecast_app.h"
#include "mainwindow.h"
#include "tool_bar.h"
#include "favorite_codes_serial.h"

static const int cst_code_index = 0;
static const int cst_name_index = 1;
static const int cst_is_index_index = 2;
static const int cst_nmarket_index = 3;
static const int cst_col_num = 4;

CodeListWall::CodeListWall(FuturesForecastApp *app, QWidget *parent) : app_(app)
{
    ui.setupUi(this);

    // init ui -----------
    QPalette pal = this->palette();
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

bool CodeListWall::Init()
{
    ui.tbview_codes->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.tbview_codes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tbview_codes->setSelectionMode(QAbstractItemView::SingleSelection);

    QStandardItemModel * model = new QStandardItemModel(0, cst_col_num, this);
    
    model->setHorizontalHeaderItem(cst_code_index, new QStandardItem(QString::fromLocal8Bit("代码")));
    model->horizontalHeaderItem(cst_code_index)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_name_index, new QStandardItem(QString::fromLocal8Bit("名称")));
    model->horizontalHeaderItem(cst_name_index)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_is_index_index, new QStandardItem(QString::fromLocal8Bit("是否指数")));
    model->horizontalHeaderItem(cst_is_index_index)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_nmarket_index, new QStandardItem(QString::fromLocal8Bit("市场代码")));
    model->horizontalHeaderItem(cst_nmarket_index)->setTextAlignment(Qt::AlignCenter);

    ui.tbview_codes->setModel(model);

    ui.tbview_codes->setColumnWidth(cst_code_index, 100);
    ui.tbview_codes->setColumnWidth(cst_is_index_index, 0);
    ui.tbview_codes->setColumnWidth(cst_nmarket_index, 0);

    ui.tbview_codes->setSelectionBehavior(QAbstractItemView::SelectRows);

    bool ret = QObject::connect(ui.tbview_codes, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotRowDoubleClicked(const QModelIndex &)));  
     
    AddCode("SC1907", QString::fromLocal8Bit("上海原油1907"), false, MARKET_SH_FUTURES);
 
    serial_ = std::make_shared<FavCodesSerial>(app_->work_dir());
    return true;
}

void CodeListWall::AddCode(const QString &code, const QString &name, bool is_index, int nmarket)
{
    // http://c.biancheng.net/view/1869.html
    QStandardItemModel &model = *(QStandardItemModel*)ui.tbview_codes->model();
    for( int i = 0; i < model.rowCount(); ++i )
    { 
        if( model.data(model.index(i, cst_code_index)).toString() == code )
            return;
    }
    auto item_code = new QStandardItem(code);
    auto item_name = new QStandardItem(name);
    auto item_is_index = new QStandardItem();
    item_is_index->setData(is_index);
    auto item_nmarket = new QStandardItem();
    item_nmarket->setData(nmarket);

    model.insertRow(model.rowCount());
    int row_index = model.rowCount() - 1;
    model.setItem(row_index, cst_code_index, item_code);
    model.setItem(row_index, cst_name_index, item_name);
    model.setItem(row_index, cst_is_index_index, item_is_index);
    model.setItem(row_index, cst_nmarket_index, item_nmarket);
    bool ck = model.item(model.rowCount() - 1, cst_is_index_index)->data().toBool();
    // model.data(model.index(model.rowCount() - 1, cst_is_index_index)).toBool() is not model.item(model.rowCount() - 1, cst_is_index_index)->data()
    
}

void CodeListWall::keyPressEvent(QKeyEvent *e) 
{
    e->ignore();
}

void CodeListWall::slotRowDoubleClicked(const QModelIndex &)
{
    QStandardItemModel *model = (QStandardItemModel*)ui.tbview_codes->model(); 
    QModelIndex index = ui.tbview_codes->currentIndex();  
    if( index.isValid() )  
    {  
        auto code = model->data(model->index(index.row(), cst_code_index)).toString();
        QString name;
        auto val = model->data(model->index(index.row(), cst_name_index));
        if( val.isValid() )
            name = val.toString();
        auto is_index = model->item(index.row(), cst_is_index_index)->data().toBool();
        int nmarket = model->item(index.row(), cst_nmarket_index)->data().toInt();
        app_->main_window()->ResetKLineWallCode(code, name, is_index, nmarket);
        app_->main_window()->SetMainView(WallType::KLINE);
        app_->main_window()->tool_bar()->SetShowSubKwallBtn(false);
        /*QStandardItem* aItem = model->itemFromIndex(index);
        if( aItem )
        {
            auto val = aItem->data().toString();
            val = val;
        }*/
    }

}