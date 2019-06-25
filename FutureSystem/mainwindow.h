#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include <QtWidgets/QTableWidgetItem>
 
#include "stockinput_dlg.h"

//#define  USE_TITLE_VCM
//#define  USE_STATUS_BAR 

enum class WallIndex : unsigned char
{
    MAIN,
    SUB,
};

namespace Ui 
{
    class MainWindow;
}
enum class WallType : unsigned char
{
    KLINE,
    CODE_LIST,
};

class MockTradeDlg;
class TrainDlg;
class FuturesForecastApp;
class TitleBar;
class ToolBar;
class KLineWall;
class CodeListWall;
class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:

    MainWindow(FuturesForecastApp *app, QWidget *parent = 0);
    ~MainWindow();

    bool Initialize();
    ToolBar* tool_bar() { return tool_bar_; }

    void SetCurKlineWallIndex(WallIndex index);
    KLineWall * MainKlineWall() { return kline_wall_main; }
    KLineWall * SubKlineWall() { return kline_wall_sub; }
    KLineWall * CurKlineWall() { if( cur_kline_index_ == WallIndex::MAIN ) return kline_wall_main; else return kline_wall_sub; }
    //void DoCurKlineWallIndexChange();
    //KLineWall * kline_wall(WallIndex index) { if( index == WallIndex::MAIN ) return kline_wall_main; else return kline_wall_sub; }

    void SetMainView(WallType wall_type);
    void ResetKLineWallCode(const QString &code, const QString &cn_name, bool is_index, int nmarket);

    void UncheckBtnABDownPen();
    void UncheckBtnABUpPen();
    void UncheckBtnABCDownPen();
    void UncheckBtnABCUpPen();

    void AddCode2CodeList(const QString &code, const QString &cn_name, bool is_index, int nmarket);

    void StockInputDlgRet();

    void PopTrainDlg();

    bool is_train_mode(){ return is_train_mode_; }
    void is_train_mode(bool val) { is_train_mode_ = val; }

    void UpdateStockData();
    void UpdateStockQuote();

    void show_sig(bool val) { show_sig_ = val; }
    bool show_sig() { return show_sig_; }

    void PopMokeTradeDlg();

    void EmitSigQuoteData(double sell1, double buy1, int sell_vol, int buy_vol) { emit sigQuoteData(sell1, buy1, sell_vol, buy_vol); }

signals:

    void sigQuoteData(double, double, int, int);

protected:

    virtual void closeEvent(QCloseEvent * event) override;
#ifdef USE_TITLE_VCM
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif
    virtual bool eventFilter(QObject *o, QEvent *e) override;

    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void changeEvent(QEvent *e) override;
    
    void updateDateTime();

private:
      
    Ui::MainWindow *ui;
    FuturesForecastApp *app_;
   
    QTimer *timer;
    int timer_update_kwall_inter_;

    // cutomer qt controler --------
    ToolBar        *tool_bar_;
    TitleBar       *title_;
    KLineWall      *kline_wall_main;
    KLineWall      *kline_wall_sub;
    CodeListWall   *code_list_wall_;
    //DayKLineDialog *dayKLineDialog;
    WallIndex cur_kline_index_;

    StockInputDlg  stock_input_dlg_;

    TrainDlg *train_dlg_;
    volatile bool  is_train_mode_;

    MockTradeDlg *mock_trade_dlg_;

    bool show_sig_;

private slots:

    void onTimer();

    //void on_tableWidget_itemDoubleClicked(QTableWidgetItem* item);
    void on_actionExit_triggered(); 
    void onMainKwallCycleChange(int /*index*/);
    void onSubKwallCycleChange(int /*index*/);
    
    friend class TrainDlg;
};

#endif // MAINWINDOW_H
