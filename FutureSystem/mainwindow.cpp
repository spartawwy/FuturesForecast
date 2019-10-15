﻿
#include <windowsx.h>
#include <qt_windows.h> 


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <string.h>

#include <QString>
#include <QDateTime>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QComboBox>
#include <QMessageBox>
#include <QTimer>
#include <qdesktopwidget.h>

#include <qdebug.h>

#include "futures_forecast_app.h"
#include "kline_wall.h"
#include "title_bar.h"
#include "tool_bar.h"
#include "code_list_wall.h"
#include "train_dlg.h"
#include "mock_trade_dlg.h"

#define MAKE_SUB_WALL 

static const int cst_win_width = 1000;
static const int cst_win_height = 500;
static const int cst_update_kwall_inter = 10;

using namespace std;

MainWindow::MainWindow(FuturesForecastApp *app, QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , app_(app)
    , tool_bar_(nullptr)
    , title_(nullptr)
    , kline_wall_main(nullptr)
    , kline_wall_sub(nullptr)
    , code_list_wall_(nullptr)
    , cur_kline_index_(WallIndex::MAIN)
    , stock_input_dlg_(this, app->data_base())
    , timer_update_kwall_inter_(0)
    , train_dlg_(nullptr)
    , is_train_mode_(false)
    , is_mock_trade_(false)
    , mock_trade_dlg_(nullptr)
    , show_sig_(false)
    , cur_quote_price_(0.0)
{
    ui->setupUi(this); 
}

MainWindow::~MainWindow()
{
//#ifdef USE_STATUS_BAR
//    delete timer;
//#endif
//    delete ui;
}


bool MainWindow::Initialize()
{  
    // https://blog.csdn.net/qq_28093585/article/details/78517358
    this->setWindowFlags(Qt::FramelessWindowHint);  
    this->setGeometry(100, 100, cst_win_width, cst_win_height);
#ifndef _DEBUG
    auto cur_date = QDate::currentDate().year() * 10000 + QDate::currentDate().month() * 100 + QDate::currentDate().day();
    if( cur_date > 20191225 )
    {
        QMessageBox::information(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("软件已经到期!请联系249564063@qq.com")); 
        return false;
    }
#endif
    QWidget *wd = new QWidget(this);  
    QVBoxLayout *layout_all = new QVBoxLayout;  
    layout_all->setContentsMargins(0,0,0,0);  
    layout_all->setSpacing(0);  
    // title ---------
    title_ = new TitleBar(this);
    layout_all->addWidget(title_);  

    // tool bar ---------
    tool_bar_ = new ToolBar(this);
    tool_bar_->SetMainKwallCurCycleType(DEFAULT_MAINKWALL_TYPE_PERIOD);
    bool ret = connect(tool_bar_->main_cycle_comb(), SIGNAL(currentIndexChanged(int)), this, SLOT(onMainKwallCycleChange(int)));
    
    tool_bar_->SetSubKwallCurCycleType(DEFAULT_SUBKWALL_TYPE_PERIOD);
    ret = connect(tool_bar_->sub_cycle_comb(), SIGNAL(currentIndexChanged(int)), this, SLOT(onSubKwallCycleChange(int)));
    ret = ret;

    // view area ----------
    QHBoxLayout * view_layout = new QHBoxLayout;
    view_layout->setContentsMargins(0,0,0,0);  
    view_layout->setSpacing(1);  

    kline_wall_main = new KLineWall(app_, this, (int)WallIndex::MAIN, DEFAULT_MAINKWALL_TYPE_PERIOD);
    if( !kline_wall_main->Init() )
        return false;
    kline_wall_main->setMouseTracking(true);
    //kline_wall_main->ResetTypePeriod(DEFAULT_MAINKWALL_TYPE_PERIOD);
    kline_wall_main->setFocusPolicy(Qt::StrongFocus);
    view_layout->addWidget(kline_wall_main);
#ifdef MAKE_SUB_WALL
    kline_wall_sub = new KLineWall(app_, this, (int)WallIndex::SUB, DEFAULT_SUBKWALL_TYPE_PERIOD);
    if( !kline_wall_sub->Init() )
        return false;
    kline_wall_sub->setMouseTracking(true);
    //kline_wall_sub->ResetTypePeriod(DEFAULT_SUBKWALL_TYPE_PERIOD);
    kline_wall_sub->setFocusPolicy(Qt::StrongFocus);
    view_layout->addWidget(kline_wall_sub);

    kline_wall_sub->setVisible(false);
#endif
    // end of view area-------
     
    code_list_wall_ = new CodeListWall(app_, this);
    code_list_wall_->Init();

    layout_all->addWidget(tool_bar_);  
    layout_all->addLayout(view_layout);  
    layout_all->addWidget(code_list_wall_);  

    code_list_wall_->hide();

    wd->setLayout(layout_all);  
    this->setCentralWidget(wd);  

    train_dlg_ = new TrainDlg(kline_wall_main, this);
    train_dlg_->setWindowFlags(train_dlg_->windowFlags() | Qt::WindowStaysOnTopHint/*Qt::Dialog*/ );
    train_dlg_->hide();

    mock_trade_dlg_ = new MockTradeDlg(this);
    mock_trade_dlg_->setWindowFlags(train_dlg_->windowFlags() | Qt::WindowStaysOnTopHint/*Qt::Dialog*/ );
    mock_trade_dlg_->hide();

    bool result = connect(this, SIGNAL(sigQuoteData(double, double, double, int, int, int)), mock_trade_dlg_, SLOT(slotHandleQuote(double, double, double, int, int, int)));

    //-------------------------

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();
       
#ifdef USE_STATUS_BAR
    
    ui->labelCurrentTime->setText(
            QDateTime::currentDateTime().toString("yyyy-MM-dd HH:MM:ss"));
     
    ui->statusBar->showMessage("hello",2000); 
    ui->statusBar->addPermanentWidget(ui->labelCurrentTime);
#endif

    auto desktop_wid = QApplication::desktop();
    //获取设备屏幕大小
    QRect screenRect = desktop_wid->screenGeometry();

    this->setGeometry(0, 0, screenRect.width() * 0.8, screenRect.height() * 0.8);

    /*g_nActScreenX = screenRect.width();
    g_nActScreenY = screenRect.height();*/

    return true;
}

void MainWindow::SetCurKlineWallIndex(WallIndex index)
{
    if( cur_kline_index_ != index )
    { 
        CurKlineWall()->ResetDrawState(DrawAction::NO_ACTION);
    } 
    
    cur_kline_index_ = index; 
}

void MainWindow::SetMainView(WallType wall_type)
{
    kline_wall_main->hide();
    switch(wall_type)
    {    
    case WallType::KLINE: 
        code_list_wall_->hide();
        if( kline_wall_sub )
            kline_wall_sub->hide();
        kline_wall_main->show(); 
        break;
    case WallType::CODE_LIST: 
        kline_wall_main->hide();
        code_list_wall_->show(); 
        break;
    default:break;
    }
}

void MainWindow::ResetKLineWallCode(const QString &code, const QString &cn_name, bool is_index, int nmarket)
{
    kline_wall_main->ResetStock(code, cn_name, is_index, nmarket);
}
 
void MainWindow::closeEvent(QCloseEvent * event)
{
    if( train_dlg_ )
        train_dlg_->hide();
    auto ret_button = QMessageBox::question(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("确定退出系统?"),
        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if( ret_button == QMessageBox::Cancel )
        event->ignore();
    else
        app_->Stop();
}

void MainWindow::UncheckBtnABDownPen()
{
    if( tool_bar_ ) 
        tool_bar_->UncheckBtnABDownPen(); 
}

void MainWindow::UncheckBtnABUpPen()
{
    if( tool_bar_ ) 
        tool_bar_->UncheckBtnABUpPen(); 
}

void MainWindow::UncheckBtnABCDownPen()
{
    if( tool_bar_ ) 
        tool_bar_->UncheckBtnABCDownPen(); 
}

void MainWindow::UncheckBtnABCUpPen()
{
    if( tool_bar_ ) 
        tool_bar_->UncheckBtnABCUpPen(); 
}

void MainWindow::AddCode2CodeList(const QString &code, const QString &cn_name, bool is_index, int nmarket)
{
    code_list_wall_->AddCode(code , cn_name, is_index, nmarket);
}


void MainWindow::StockInputDlgRet()
{
    bool is_index = false;

    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString tgt_tag = stock_input_dlg_.ui.stock_input->text().section('/', 0, 0, flag);
    QString stock_code = tgt_tag.toLocal8Bit().data();
    /*if( !IsStrNum(stock_code) || stock_code.size() != 6 )
    return;*/
    QString stock_name = stock_input_dlg_.ui.stock_input->text().section('/', 1, 1, flag);

    QString type = stock_input_dlg_.ui.stock_input->text().section('/', 2, 2, flag);
    is_index = type == "1";
    QString stock_code_changed;
	stock_input_dlg_.ui.stock_input->text().clear();

	auto first_code = stock_code.toLocal8Bit().data();
	/*if( *first_code != '\0' && *first_code != '\2' && *first_code == '\3' && *first_code == '\6' )
		return;
	if( stock_code.length() != 6 || !IsNumber(stock_code.toLocal8Bit().data()) )
		return;*/
    QString maket = stock_input_dlg_.ui.stock_input->text().section('/', 3, 3, flag);
    int nmakert = maket.toInt();
    stock_code_changed = stock_code.toLocal8Bit().data();

    kline_wall_main->ResetStock(stock_code_changed, stock_name, is_index, nmakert);
    if( kline_wall_sub )
        kline_wall_sub->ResetStock(stock_code_changed, stock_name, is_index, nmakert);
}
 
void MainWindow::UpdateStockData(int target_date, int cur_hhmm)
{
    if( kline_wall_main )
        kline_wall_main->UpdateIfNecessary(target_date, cur_hhmm);
    if( kline_wall_sub )
        kline_wall_sub->UpdateIfNecessary(target_date, cur_hhmm);
}

// ps : cause 1m is updated high frequent, so only update main k line wall
void MainWindow::UpdateStockQuote()
{
    if( kline_wall_main )
        kline_wall_main->UpdateStockQuote();
}

void MainWindow::PopTrainDlg()
{
    assert(train_dlg_);

    if( tool_bar()->main_cycle_comb()->currentIndex() != COMBO_PERIOD_5M_INDEX )
    {
        // ps: it will trigger onMainKwallCycleChange
        tool_bar()->main_cycle_comb()->setCurrentIndex(COMBO_PERIOD_5M_INDEX); // set current to period day
    }
    if( tool_bar()->sub_cycle_comb()->currentIndex() != COMBO_PERIOD_1M_INDEX )
    {
        // ps: it will trigger onSubKwallCycleChange
        tool_bar()->sub_cycle_comb()->setCurrentIndex(COMBO_PERIOD_1M_INDEX);
    }
    tool_bar()->main_cycle_comb()->setEnabled(false);

    is_train_mode(true);
    train_dlg_->showNormal();
}

void MainWindow::MinimizeTrainDlg()
{
    train_dlg_->showMinimized();
}

void MainWindow::PopMokeTradeDlg()
{
    mock_trade_dlg_->showNormal();
}

void MainWindow::MinimizeMockTradeDlg()
{
    mock_trade_dlg_->showMinimized();
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) 
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
 
bool MainWindow::eventFilter(QObject *o, QEvent *e)
{ 
    if( o == kline_wall_main )
    {
        switch ( e->type() )
        {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            return true;
        default: return false; // ndchk
        }
        
    }else
        return false;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if( is_train_mode_ )
        return;
    switch(e->key())
    {
        case Qt::Key_F3:
        { 
            /*kline_wall_main->ResetStock("999999", QString::fromLocal8Bit("上证指数"), true);
            if( kline_wall_sub )
            {
                kline_wall_sub->ResetStock("999999", QString::fromLocal8Bit("上证指数"), true);
                if( kline_wall_sub->isVisible() )
                {
                    kline_wall_main->update();
                    kline_wall_sub->update();
                }
            }*/
        } break;
        case Qt::Key_F5:
        {
            kline_wall_main->show();
            code_list_wall_->hide();
        } break;
        case Qt::Key_F6:
        {
            kline_wall_main->hide();
            if( kline_wall_sub )
                kline_wall_sub->hide();
            code_list_wall_->show();
        } break;
        case Qt::Key_0: case Qt::Key_1: case Qt::Key_2: case Qt::Key_3: case Qt::Key_4:  
        case Qt::Key_5: case Qt::Key_6: case Qt::Key_7: case Qt::Key_8: case Qt::Key_9: 
        case Qt::Key_A: case Qt::Key_B: case Qt::Key_C: case Qt::Key_D: case Qt::Key_E:
        case Qt::Key_F: case Qt::Key_G: case Qt::Key_H: case Qt::Key_I: case Qt::Key_J:
        case Qt::Key_K: case Qt::Key_L: case Qt::Key_M: case Qt::Key_N: case Qt::Key_O:
        case Qt::Key_P: case Qt::Key_Q: case Qt::Key_R: case Qt::Key_S: case Qt::Key_T:
        case Qt::Key_U: case Qt::Key_V: case Qt::Key_W: case Qt::Key_X: case Qt::Key_Y:
        case Qt::Key_Z:
		{
            if( (e->modifiers() & Qt::ControlModifier) )
                break;
            //qDebug() << "MainWindow::keyPressEvent " << e->key() << "\n";
            stock_input_dlg_.ui.stock_input->clear();
            char tmpbuf[8] = {0};
            if( e->key() >= Qt::Key_0 && e->key() <= Qt::Key_9 )
                sprintf_s(tmpbuf, sizeof(tmpbuf), "%c", (char)e->key());
            else
                sprintf_s(tmpbuf, sizeof(tmpbuf), "%c", char(e->key()+32));
            stock_input_dlg_.ui.stock_input->setText(tmpbuf);
            stock_input_dlg_.showNormal();
        } break;

        default:
            break;
    }
    e->ignore();
}

void MainWindow::onTimer()
{
    // updateDateTime();  1015-1030
    //int hhmmss = QDateTime::currentDateTime().toString("hhmmss").toInt();
    int hh = QDateTime::currentDateTime().time().hour();
    int mm = QDateTime::currentDateTime().time().minute();
    int ss = QDateTime::currentDateTime().time().second();

    int min_5_amain_second = (5 - (hh * 60 + mm) % 5) * 60 - ss; 
    int min_15_amain_second = (15 - (hh * 60 + mm) % 15) * 60 - ss; 

    
    /*if( hh > 9 )
    {
        amain_second = (5 - ((hh - 9) * 60 + mm) % 5) * 60 - ss; 

    }else
    {
        amain_second = (5 - ((hh - 9) * 60 + mm) % 5) * 60 - ss; 
    }
*/
    //hhmm / 100 * 60 - 9 * 60
    QString content = QString("%1                                         %2                 5M: %3     \t\t     15M: %4")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss"))
        .arg(QString::number(cur_quote_price(), 'f', 1))
        .arg(min_5_amain_second)
        .arg(min_15_amain_second);

    statusBar()->showMessage(content);

}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::updateDateTime()
{
#ifdef USE_STATUS_BAR
    ui->labelCurrentTime->setText(
            QDateTime::currentDateTime().toString("yyyy-MM-dd HH:MM:ss"));
#endif
}
  
void MainWindow::onMainKwallCycleChange(int /*index*/)
{
    assert(kline_wall_main);
    tool_bar_->main_cycle_comb()->clearFocus();
    kline_wall_main->ResetTypePeriod( TypePeriod(tool_bar_->main_cycle_comb()->currentData().toInt()) );
}

void MainWindow::onSubKwallCycleChange(int /*index*/)
{
    if( !kline_wall_sub )
        return;
    tool_bar_->sub_cycle_comb()->clearFocus();
    if( is_train_mode() && tool_bar_->sub_cycle_comb()->currentIndex() >= COMBO_PERIOD_5M_INDEX )
    {
        tool_bar_->sub_cycle_comb()->setCurrentIndex(COMBO_PERIOD_1M_INDEX);
        return;
    }else
    {
        kline_wall_sub->ResetTypePeriod( TypePeriod(tool_bar_->sub_cycle_comb()->currentData().toInt()) );
        if( kline_wall_main->k_cur_train_date() > 0 )
        {
            kline_wall_sub->ShowDurationKlines(kline_wall_main->k_cur_train_date(), kline_wall_main->k_cur_train_hhmm());
            kline_wall_sub->SetTrainStartDateTime(TypePeriod(tool_bar_->sub_cycle_comb()->currentData().toInt())
                , kline_wall_main->k_cur_train_date(), kline_wall_main->k_cur_train_hhmm());
        }
    }
}


