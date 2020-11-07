#ifndef ZHIBIAO_SDF3SDFSD_H_
#define ZHIBIAO_SDF3SDFSD_H_

#include "stkfo_common.h"

class MomentumZhibiao : public ZhiBiaoAtom
{
public:
    static void Caculate(T_HisDataItemContainer &data_items_in_container);
    static void Caculate(T_HisDataItemContainer &data_items_in_container, int end_span);

    MomentumZhibiao():val0_(0.0), val1_(0.0),val2_(0.0), val3_(0.0) {}
      
    virtual void val0( double val) override { val0_ = val;} 
    virtual double val0() override { return val0_;}

    virtual void val1( double val) override { val1_ = val;} 
    virtual double val1(){ return val1_;}

    virtual void val2( double val) override { val2_ = val;} 
    virtual double val2(){ return val2_;}

    virtual void val3( double val) override { val3_ = val;} 
    virtual double val3(){ return val3_;}

private:
    double val0_; // short 
    double val1_; // long
    double val2_; // DEA
    double val3_; // MACD
};


class ExpmaZhibiao : public ZhiBiaoAtom
{
public:
    static void Caculate(T_HisDataItemContainer &data_items_in_container);
    static void Caculate(T_HisDataItemContainer &data_items_in_container, int end_span);

    ExpmaZhibiao():val0_(0.0), val1_(0.0),val2_(0.0), val3_(0.0) {}

    virtual void val0( double val) override { val0_ = val;} 
    virtual double val0() override { return val0_;}

    virtual void val1( double val) override { val1_ = val;} 
    virtual double val1(){ return val1_;}

    virtual void val2( double val) override { val2_ = val;} 
    virtual double val2(){ return val2_;}

    virtual void val3( double val) override { val3_ = val;} 
    virtual double val3(){ return val3_;}

    virtual void val4( double val) override { val4_ = val;} 
    virtual double val4(){ return val4_;}

    virtual void val5( double val) override { val5_ = val;} 
    virtual double val5(){ return val5_;}

private:
    double val0_; // quick line val 
    double val1_; // slow line val  

    double val2_; // quick line pos y of kwall 0
    double val3_; // slow line pos y of kwall 0

    double val4_; // quick line pos y of kwall 1
    double val5_; // slow line pos y of kwall 1
};
#endif