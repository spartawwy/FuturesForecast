#include "stkfo_common.h"

#include <boost/lexical_cast.hpp>

#include <algorithm>

#include <qtextcodec.h>
#include <qdebug.h>
#include <cmath>
#include <cassert>

bool IsNumber(const std::string& str)
{
	//bool ret = true;
	//std::for_each( std::begin(str), std::end(str), [&ret](char entry)
	for( unsigned int i = 0; i < str.length(); ++i )
	{
		if( str.at(i) < '0' || str.at(i) > '9' )
		  return false;
	}
	return true;
	/*{
		if( entry < '\0' || entry > '\9' )
		{
			ret = false;
		}
	}*/
}


bool TransIndexPinYin2CodeName(const std::string &pinyin, std::string &code, std::string &name)
{ 
    if( pinyin == "SZZS" ) //上证指数
    {
        code = "000001";
        name = "上证指数";
    }
    else if( pinyin == "SZCZ" ) // 深圳成指
    {
        code = "399001"; 
        name = "深圳成指";
    }
    else if( pinyin == "ZXBZ" ) //中小板指数
    { 
        code = "399005"; 
        name = "中小板指";
    }
    else if( pinyin == "CYBZ" ) //创业板指数
    {
        code = "399006";
        name = "创业板指";
    }
    else if( pinyin == "SZ50" ) // 上证50
    {
        code = "000016"; 
        name = "上证50";
    }
    else if( pinyin == "HS300" ) // 沪深300 
    {
        code = "000300"; 
        name = "沪深300";
    }
    else 
        return false;
    return true;
}


bool TransIndexCode2Name(const std::string &code, std::string &name)
{ 
    if( code == "999999" ) //上证指数 000001
    { 
        name = "上证指数";
    }
    else if( code == "399001" ) // 深圳成指
    { 
        name = "深圳成指";
    }
    else if( code == "399005" ) //中小板指数
    {  
        name = "中小板指";
    }
    else if( code == "399006" ) //创业板指数
    { 
        name = "创业板指";
    }
    else if( code == "000016" ) // 上证50
    { 
        name = "上证50";
    }
    else if( code == "000300" ) // 沪深300 
    { 
        name = "沪深300";
    }
    else 
        return false;
    return true;
}

std::string TransIndex2TusharedCode(const std::string &code)
{
    if( code == "999999" || code == "SZZS" ) //上证指数
        return "000001";
    else if( code == "399001" || code == "SZCZ" ) // 深圳成指
        return "399001"; 
    else if( code == "399005" || code == "ZXBZ" ) //中小板指数
        return "399005"; 
    else if( code == "399006" || code == "CYBZ" ) //创业板指数
        return "399006";
    else if( code == "000016" || code == "SZ50" ) // 上证50
        return "000016"; 
    else if( code == "000300" || code == "HS300" ) // 沪深300 
        return "000300"; 
    return code;
}

FractalType  MaxFractalType(int val)
{
    if( (val & int(FractalType::TOP_FAKE)) == int(FractalType::TOP_FAKE) )
        return FractalType::TOP_FAKE;
    else if( (val & int(FractalType::TOP_AXIS_T_11)) == int(FractalType::TOP_AXIS_T_11) )
        return FractalType::TOP_AXIS_T_11;
    else if( (val & int(FractalType::TOP_AXIS_T_9)) == int(FractalType::TOP_AXIS_T_9) )
        return FractalType::TOP_AXIS_T_9;
    else if( (val & int(FractalType::TOP_AXIS_T_7)) == int(FractalType::TOP_AXIS_T_7) )
        return FractalType::TOP_AXIS_T_7;
    else if( (val & int(FractalType::TOP_AXIS_T_5)) == int(FractalType::TOP_AXIS_T_5) )
        return FractalType::TOP_AXIS_T_5;
    else if( (val & int(FractalType::TOP_AXIS_T_3)) == int(FractalType::TOP_AXIS_T_3) )
        return FractalType::TOP_AXIS_T_3;

    else if( (val & int(FractalType::BTM_AXIS_T_11)) == int(FractalType::BTM_AXIS_T_11) )
        return FractalType::BTM_AXIS_T_11;
    else if( (val & int(FractalType::BTM_AXIS_T_9)) == int(FractalType::BTM_AXIS_T_9) )
        return FractalType::BTM_AXIS_T_9;
    else if( (val & int(FractalType::BTM_AXIS_T_7)) == int(FractalType::BTM_AXIS_T_7) )
        return FractalType::BTM_AXIS_T_7;
    else if( (val & int(FractalType::BTM_AXIS_T_5)) == int(FractalType::BTM_AXIS_T_5) )
        return FractalType::BTM_AXIS_T_5;
    else if( (val & int(FractalType::BTM_AXIS_T_3)) == int(FractalType::BTM_AXIS_T_3) )
        return FractalType::BTM_AXIS_T_3;
    else if( (val & int(FractalType::BTM_FAKE)) == int(FractalType::BTM_FAKE) )
        return FractalType::BTM_FAKE;
    else
        return FractalType::UNKNOW_FRACTAL;
}


FractalType  BtmestFractalType(int val)
{
    if( (val & int(FractalType::BTM_AXIS_T_11)) == int(FractalType::BTM_AXIS_T_11) )
        return FractalType::BTM_AXIS_T_11;
    else if( (val & int(FractalType::BTM_AXIS_T_9)) == int(FractalType::BTM_AXIS_T_9) )
        return FractalType::BTM_AXIS_T_9;
    else if( (val & int(FractalType::BTM_AXIS_T_7)) == int(FractalType::BTM_AXIS_T_7) )
        return FractalType::BTM_AXIS_T_7;
    else if( (val & int(FractalType::BTM_AXIS_T_5)) == int(FractalType::BTM_AXIS_T_5) )
        return FractalType::BTM_AXIS_T_5;
    else if( (val & int(FractalType::BTM_AXIS_T_3)) == int(FractalType::BTM_AXIS_T_3) )
        return FractalType::BTM_AXIS_T_3;
    else if( (val & int(FractalType::BTM_FAKE)) == int(FractalType::BTM_FAKE) )
        return FractalType::BTM_FAKE;
    else
        return FractalType::UNKNOW_FRACTAL;
}

bool IsTopFake(int val)
{
    return  (val & int(FractalType::TOP_FAKE)) == int(FractalType::TOP_FAKE);
}

bool IsBtmFake(int val)
{
    return  (val & int(FractalType::BTM_FAKE)) == int(FractalType::BTM_FAKE);
}

bool IsTopFractal(int type)
{
    if( MaxFractalType(type) >= FractalType::TOP_AXIS_T_3 )
        return true;
    else
        return false;
}

bool IsBtmFractal(int type)
{
    return BtmestFractalType(type) != FractalType::UNKNOW_FRACTAL;
}

bool IsStrAlpha(const std::string& str)
{
    try
    {
        auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
        { 
            if( val < 0 || val > 99999 ) 
                return 0;
            return isalpha(val);
        });
        return iter == str.end();
    }catch(...)
    {
        return false;
    }

}

KGreenRedType KGGetGreenRedType(const T_StockHisDataItem &item, TypePeriod type_period)
{
    KGreenRedType  gr_type = KGreenRedType::UNKNOW_TYPE;
    double small = 0.0;
    double mid = 0.0;
    double big = 0.0;
    switch(type_period)
    {
    case TypePeriod::PERIOD_1M: small = 0.1; mid = 0.5; big = 1.0; break;
    case TypePeriod::PERIOD_5M: small = 0.2; mid = 0.5; big = 1.5; break;
    case TypePeriod::PERIOD_15M: small = 0.5; mid = 1.5; big = 3.5;break;
    case TypePeriod::PERIOD_30M: small = 0.5; mid = 4.0; big = 8.0;break;
    case TypePeriod::PERIOD_HOUR: small = 1.0; mid = 4.0; big = 8.0;break;
    case TypePeriod::PERIOD_DAY: small = 2.5; mid = 7.0; big = 9.0;break;
    case TypePeriod::PERIOD_WEEK: small = 7.0; mid = 10.0; big = 15.0;break;
    default: assert(false);
    }
    double body_high = 0.0; 
    if( item.close_price > item.open_price + EPSINON )
    {
        body_high = item.close_price - item.open_price;
        if( body_high < small )
            gr_type = KGreenRedType::TEN_CROSS;
        else if( body_high < mid )
            gr_type = KGreenRedType::SMALL_RED;
        else if( body_high < big )
            gr_type = KGreenRedType::MID_RED;
        else
            gr_type = KGreenRedType::STRONG_RED;
    }else if( item.close_price < item.open_price - EPSINON )
    {
        body_high = item.open_price - item.close_price;
        if( body_high < small )
            gr_type = KGreenRedType::TEN_CROSS;
        else if( body_high < mid )
            gr_type = KGreenRedType::SMALL_GREEN;
        else if( body_high < big )
            gr_type = KGreenRedType::MID_GREEN;
        else
            gr_type = KGreenRedType::STRONG_GREEN;
    }else
        gr_type = KGreenRedType::TEN_CROSS;

    return gr_type;
}

//void ClearTopFractal(int &val)
void ClearTopFractal(T_KlineDataItem &k_data_item)
{
    int vals[] = {(int)FractalType::TOP_AXIS_T_3, (int)FractalType::TOP_AXIS_T_5, (int)FractalType::TOP_AXIS_T_7
        , (int)FractalType::TOP_AXIS_T_9, (int)FractalType::TOP_AXIS_T_11, (int)FractalType::TOP_FAKE};
    for( int i = 0; i < sizeof(vals) / sizeof(vals[0]); ++i )
    {
        int tmp_val = int(vals[i]);
        tmp_val ^= 0xffffffff;
        k_data_item.type &= tmp_val;
    }
    qDebug() << __FUNCTION__ << "  " << k_data_item.stk_item.date << ":" << k_data_item.stk_item.hhmmss << "\n";
}

//void ClearBtmFractal(int &val)
void ClearBtmFractal(T_KlineDataItem &k_data_item)
{
    int vals[] = {(int)FractalType::BTM_AXIS_T_3, (int)FractalType::BTM_AXIS_T_5, (int)FractalType::BTM_AXIS_T_7
        , (int)FractalType::BTM_AXIS_T_9, (int)FractalType::BTM_AXIS_T_11, (int)FractalType::BTM_FAKE};
    for( int i = 0; i < sizeof(vals) / sizeof(vals[0]); ++i )
    {
        int tmp_val = int(vals[i]);
        tmp_val ^= 0xffffffff;
        k_data_item.type &= tmp_val;
    }
    qDebug() << __FUNCTION__ << "  " << k_data_item.stk_item.date << ":" << k_data_item.stk_item.hhmmss << "\n";
}

bool IsStrNum(const std::string& str)
{
    try
    {
        auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
        { 
            if( val < 0 || val > 99999 ) 
                return 0;
            return isalnum(val);
        });
        return iter == str.end();
    }catch(...)
    {
        return false;
    }

}
bool IsDouble(const std::string& str)
{
    bool ret = true;
    try
    {
        double val = boost::lexical_cast<double>(str.c_str());
    }catch (boost::exception &)
    {
        ret = false;
    }catch(...)
    {
        ret = false;
    }
    return ret;
}

bool TransToDouble(const std::string& str, double &ret_val)
{
    ret_val = 0.0;
    bool ret = true;
    try
    {
        ret_val = boost::lexical_cast<double>(str.c_str());
    }catch (boost::exception &)
    {
        ret = false;
    }catch(...)
    {
        ret = false;
    }
    return ret;
}

double ProcDecimal(double val, unsigned int decimal)
{
    int temp = pow(10, decimal);
    int64_t big_val = int64_t(fabs(val * temp) + 0.5) * (val < 0 ? - 1 : 1); //4舍5入
    return double(big_val) / temp;
}

bool Equal(double lh, double rh)
{
    return fabs(lh-rh) < 0.0001;
}

int cur_hhmm()
{
    time_t rawtime;
    struct tm timeinfo;
    time( &rawtime );
    localtime_s(&timeinfo, &rawtime); 
    return timeinfo.tm_hour * 100 + timeinfo.tm_min;
}

void utf8ToGbk(std::string& strUtf8)
{
    QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
    QTextCodec* gbkCodec = QTextCodec::codecForName("gbk");

    QString strUnicode = utf8Codec->toUnicode(strUtf8.c_str());
    QByteArray ByteGbk = gbkCodec->fromUnicode(strUnicode);

    strUtf8 = ByteGbk.data();
}

void gbkToUtf8(std::string& strGbk)
{
    QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
    QTextCodec* gbkCodec = QTextCodec::codecForName("gbk");

    QString strUnicode = gbkCodec->toUnicode(strGbk.c_str());
    QByteArray ByteUtf8 = utf8Codec->fromUnicode(strUnicode);
    strGbk = ByteUtf8.data();
}

QString ToQString(double val)
{ 
    return QString::number(val, 'f', DEFAULT_DECIMAL);
}

QString ToQString(int val)
{ 
   //return ToQString(double(val));
    return QString::number(val);
}