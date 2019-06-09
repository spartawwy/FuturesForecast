#ifndef TRAIN_TRADE_DSFK3423_DLG_H_
#define TRAIN_TRADE_DSFK3423_DLG_H_

#include <QtWidgets/QWidget>

#include "ui_traintrade.h"

//class KLineWall;
//class MainWindow;
class TrainDlg;
class TrainTradeDlg : public QWidget
{
    Q_OBJECT

public:

    TrainTradeDlg(TrainDlg *train_dlg, bool is_close);

    Ui::TrainTradeForm  ui;

    void is_close(bool val) { is_close_ = val; }
    bool is_long() { return ui.radioBtn_long->isChecked(); }
    void SetDate(int date) { date_ = date; }
    void SetHhmm(int hhmm) { hhmm_ = hhmm; }

//private slots:
    virtual void closeEvent(QCloseEvent *) override;
    virtual void showEvent(QShowEvent *) override;

    void SetStatusBar(const QString & val);

public slots:

    void OnBtnAllQuantity();
    void OnBtnHalfQuantity();
    void OnBtnOneThirdQuantity();
    void OnBtnOneFifthQuantity();

private:

    void _onBtnQuantity(double val);

    TrainDlg *train_dlg_;
    bool is_close_;
    int  date_;
    int  hhmm_;

    friend class TrainDlg;
};

#endif // TRAIN_TRADE_DSFK3423_DLG_H_
