#include "stkfo_common.h"
#include <algorithm>

#include <qtextcodec.h>
#include <qdebug.h>

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
    if( pinyin == "SZZS" ) //��ָ֤��
    {
        code = "000001";
        name = "��ָ֤��";
    }
    else if( pinyin == "SZCZ" ) // ���ڳ�ָ
    {
        code = "399001"; 
        name = "���ڳ�ָ";
    }
    else if( pinyin == "ZXBZ" ) //��С��ָ��
    { 
        code = "399005"; 
        name = "��С��ָ";
    }
    else if( pinyin == "CYBZ" ) //��ҵ��ָ��
    {
        code = "399006";
        name = "��ҵ��ָ";
    }
    else if( pinyin == "SZ50" ) // ��֤50
    {
        code = "000016"; 
        name = "��֤50";
    }
    else if( pinyin == "HS300" ) // ����300 
    {
        code = "000300"; 
        name = "����300";
    }
    else 
        return false;
    return true;
}


bool TransIndexCode2Name(const std::string &code, std::string &name)
{ 
    if( code == "999999" ) //��ָ֤�� 000001
    { 
        name = "��ָ֤��";
    }
    else if( code == "399001" ) // ���ڳ�ָ
    { 
        name = "���ڳ�ָ";
    }
    else if( code == "399005" ) //��С��ָ��
    {  
        name = "��С��ָ";
    }
    else if( code == "399006" ) //��ҵ��ָ��
    { 
        name = "��ҵ��ָ";
    }
    else if( code == "000016" ) // ��֤50
    { 
        name = "��֤50";
    }
    else if( code == "000300" ) // ����300 
    { 
        name = "����300";
    }
    else 
        return false;
    return true;
}

std::string TransIndex2TusharedCode(const std::string &code)
{
    if( code == "999999" || code == "SZZS" ) //��ָ֤��
        return "000001";
    else if( code == "399001" || code == "SZCZ" ) // ���ڳ�ָ
        return "399001"; 
    else if( code == "399005" || code == "ZXBZ" ) //��С��ָ��
        return "399005"; 
    else if( code == "399006" || code == "CYBZ" ) //��ҵ��ָ��
        return "399006";
    else if( code == "000016" || code == "SZ50" ) // ��֤50
        return "000016"; 
    else if( code == "000300" || code == "HS300" ) // ����300 
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
    return QString::number(val, 'f', 2);
}

QString ToQString(int val)
{ 
   return ToQString(double(val));
}