#ifndef WINNER_API_SDF32DF_H_
#define WINNER_API_SDF32DF_H_
 
//APIʹ������Ϊ: Ӧ�ó����ȵ���WinnerHisHq_Connect����Ӯ����ʷ������,Ȼ��ſ��Ե��������ӿڻ�ȡ��������,Ӧ�ó���Ӧ���д��������������, �ӿ����̰߳�ȫ��
//������ߣ���������api������api�᷵���Ѿ����ߵĴ�����Ϣ��Ӧ�ó���Ӧ���ݴ˴�����Ϣ�������ӷ�������

//3.������������˵��
/// <summary>
/// ����Ӯ�����������
/// </summary>
/// <param name="IP">������IP,����ȯ��ͨ���������¼���桰ͨѶ���á���ť�ڲ��</param>
/// <param name="Port">�������˿�</param>
/// <param name="Result">��APIִ�з��غ�  </param>
/// <param name="ErrInfo">��APIִ�з��غ�������������˴�����Ϣ˵����һ��Ҫ����256�ֽڵĿռ䡣û����ʱΪ���ַ�����</param>
/// <returns>�ɹ�����0, ʧ�ܷ�������</returns>
typedef int (__cdecl* WinnerHisHq_ConnectDelegate)(char* IP, int Port, char* Result, char* ErrInfo);

/// <summary>
/// �Ͽ�ͬ������������
/// </summary>
typedef void(__cdecl* WinnerHisHq_DisconnectDelegate)();

typedef struct _t_quote_atom_data
{
    // 'code','date','time','price','change','volume','amount','type'
    char code[16];
    //int  date; //yyyymmdd
    __int64  time; //HHMMSS
    double price; // .2f
    double price_change;
    int  vol;
    unsigned char bid_type; // 0: buy_pan  1 :sell_pan
}T_QuoteAtomData;

#ifndef T_K_Data
typedef struct _t_k_data
{
    int yyyymmdd;
    int hhmmdd;
    double open;
    double close;
    double high;
    double low;
    int vol;
}T_K_Data;

#define T_K_Data T_K_Data
#endif
typedef void (*FenbiCallBack)(T_QuoteAtomData *quote_data, bool is_end, void *para);
typedef struct _t_fenbi_call_back
{
    FenbiCallBack  call_back_func;
    void *para;
    int date;
    unsigned int serial;
    _t_fenbi_call_back() : call_back_func(nullptr), para(nullptr), date(0), serial(0){} 
    _t_fenbi_call_back(const _t_fenbi_call_back &lh) : call_back_func(lh.call_back_func), para(lh.para), date(lh.date), serial(lh.serial){} 
}T_FenbiCallBack;

 //----------------------------
typedef void (*KDataCallBack)(T_K_Data *k_data, bool is_end, void *para);
typedef struct _t_kdata_call_back
{
    KDataCallBack  call_back_func;
    void *para;
    int date;
    unsigned int serial;
    _t_kdata_call_back() : call_back_func(nullptr), para(nullptr), date(0), serial(0){} 
    _t_kdata_call_back(const _t_kdata_call_back &lh) : call_back_func(lh.call_back_func), para(lh.para), date(lh.date), serial(lh.serial){} 
}T_KDataCallBack;


 
/// ��ȡ��ʷ��ʱ����
/// </summary>
/// <param name="Zqdm">֤ȯ����</param>
/// <param name="Date">����, ����2014��1��1��Ϊ����20140101</param>
/// <param name="FenbiCallBack"> �ص�����</param>
/// <param name="ErrInfo">��APIִ�з��غ�������������˴�����Ϣ˵����һ��Ҫ����256�ֽڵĿռ䡣û����ʱΪ���ַ�����</param>
/// <returns>�ɹ�����0, ʧ�ܷ������� -1--δ����</returns>
typedef int (__cdecl* WinnerHisHq_GetHisFenbiDataDelegate)(char* Zqdm, int Date, T_FenbiCallBack *call_back_para, char* ErrInfo);

/// <summary>
/// ������ȡ��ʷ��ʱ����
/// </summary>
/// <param name="Zqdm">֤ȯ����</param>
/// <param name="Date">����, ����2014��1��1��Ϊ����20140101</param>
/// <param name="FenbiCallBack"> �ص�����</param>
/// <param name="ErrInfo">��APIִ�з��غ�������������˴�����Ϣ˵����һ��Ҫ����256�ֽڵĿռ䡣û����ʱΪ���ַ�����</param>
/// <returns>�ɹ�����0, ʧ�ܷ������� -1--δ����</returns>
typedef int (__cdecl* WinnerHisHq_GetHisFenbiDataBatchDelegate)(char* Zqdm, int date_begin, int date_end, T_FenbiCallBack *call_back_para, char* ErrInfo);

enum class PeriodType : unsigned char
{
    PERIOD_5M = 1,
    PERIOD_15M,
    PERIOD_30M,
    PERIOD_HOUR,
    PERIOD_DAY,
    PERIOD_WEEK,
    PERIOD_MON,
    PERIOD_YEAR,
};

enum class FqType : unsigned char
{
    FQ_NO, 
    FQ_BEFORE,  //ǰ��Ȩ
    FQ_AFTER,   //��Ȩ
};
/// <summary>
/// ��ȡ֤ȯָ����Χ�ĵ�K������
/// </summary>
/// <param name="Category">K������, 0->5����K�� 1->15����K�� 2->30����K�� 3->1СʱK�� 4->��K�� 5->��K�� 6->��K�� 7->1���� 8->1����K�� 9->��K�� 10->��K�� 11->��K��< / param>
/// <param name="Market">�г�����, 0->���� 1->�Ϻ�</param>
/// <param name="Zqdm">֤ȯ����</param>
/// <param name="Start">��Χ�Ŀ�ʼλ��,���һ��K��λ����0, ǰһ����1, ��������</param>
/// <param name="Count">��Χ�Ĵ�С��APIִ��ǰ,��ʾ�û�Ҫ�����K����Ŀ, APIִ�к�,������ʵ�ʷ��ص�K����Ŀ, ���ֵ800</param>
/// <param name="Result">��APIִ�з��غ�Result�ڱ����˷��صĲ�ѯ����, ��ʽΪ������ݣ�������֮��ͨ��\n�ַ��ָ������֮��ͨ��\t�ָ���һ��Ҫ����1024*1024�ֽڵĿռ䡣����ʱΪ���ַ�����</param>
/// <param name="ErrInfo">��APIִ�з��غ�������������˴�����Ϣ˵����һ��Ҫ����256�ֽڵĿռ䡣û����ʱΪ���ַ�����</param>
/// <returns>�ɹ�����0, ʧ�ܷ��� < 0</returns>
//typedef bool(__cdecl* TdxHq_GetSecurityBarsDelegate)(byte Category, byte Market, char* Zqdm, short Start, short& Count, char* Result, char* ErrInfo);
/// <summary>

typedef int (__cdecl* WinnerHisHq_GetKDataDelegate)(char* Zqdm, PeriodType type, int date_begin, int date_end
                                                           , T_KDataCallBack *call_back_para, bool is_index, char *ErrInfo);




#endif // WINNER_API_SDF32DF_H_