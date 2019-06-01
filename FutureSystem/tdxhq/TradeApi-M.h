#ifndef __TRADE_API_M_H
#define __TRADE_API_M_H

#include <Windows.h>

#define MAX_RESULT_SIZE   1024*1024
#define MAX_ERRINFO_SIZE  1024


#ifdef __cplusplus
extern "C" {
#endif

//
// ��ͨ����ʵ��
//
int WINAPI OpenTdx(
    short nClientType,
    const char *pszClientVersion,
    char nCliType,
    char nVipTermFlag,
    char *pszErrInfo);

//
// �ر�ͨ����ʵ��
//
void WINAPI CloseTdx();

//
// �����˻���¼
//
// ����:
//     nQsid           - ȯ�̱�ʶ
//     pszHost         - ȯ�̽��׷�����IP
//     nPort           - ȯ�̽��׷������˿�
//     pszVersion      - ����ͨ���ſͻ��˵İ汾��
//     nYybId          - Ӫҵ�����룬�뵽��ַ http://www.chaoguwaigua.com/downloads/qszl.htm ��ѯ
//     nAccountType    - ��¼�˺�����
//     pszAccountNo    - �����ĵ�¼�˺ţ�ȯ��һ��ʹ���ʽ��ʻ���ͻ���
//     pszTradeAccount - �����˺ţ�һ�����¼�ʺ���ͬ. ���¼ȯ��ͨ�����������ѯ�ɶ��б��ɶ��б��ڵ��ʽ��ʺž��ǽ����ʺ�, �����ѯ���������վ���ȵ��ʴ���Ŀ
//     pszJyPassword   - ��������
//     pszTxPassword   - ͨѶ����
//     pszErrInfo      - ��APIִ�з��غ�������������˴�����Ϣ˵����һ��Ҫ����256�ֽڵĿռ䡣û����ʱΪ���ַ�����
//
// ����ֵ:
//     �ͻ���ID��ʧ��ʱ����0
//
int WINAPI Logon(
    int nQsid,
    const char *pszHost,
    short nPort,
    const char *pszVersion,
    short nYybId,
    char nAccountType,
    const char *pszAccountNo,
    const char *pszTradeAccount,
    const char *pszJyPassword,
    const char *pszTxPassword,
    char *pszErrInfo);

//
// �����˻���¼
//
// ����:
//     nQsid           - ȯ�̱�ʶ
//     pszHost         - ȯ�̽��׷�����IP
//     nPort           - ȯ�̽��׷������˿�
//     pszVersion      - ����ͨ���ſͻ��˵İ汾��
//     nYybId          - Ӫҵ�����룬�뵽��ַ http://www.chaoguwaigua.com/downloads/qszl.htm ��ѯ
//     nAccountType    - ��¼�˺�����
//     pszAccountNo    - �����ĵ�¼�˺ţ�ȯ��һ��ʹ���ʽ��ʻ���ͻ���
//     pszTradeAccount - �����˺ţ�һ�����¼�ʺ���ͬ. ���¼ȯ��ͨ�����������ѯ�ɶ��б��ɶ��б��ڵ��ʽ��ʺž��ǽ����ʺ�, �����ѯ���������վ���ȵ��ʴ���Ŀ
//     pszJyPassword   - ��������
//     pszTxPassword   - ͨѶ����
//     pszQsInfoEx     - ���һЩȯ�̵ĸ�����Ϣ, ȱʡ��д0, ������û��������
//     pszErrInfo      - ��APIִ�з��غ�������������˴�����Ϣ˵����һ��Ҫ����256�ֽڵĿռ䡣û����ʱΪ���ַ�����
//
// ����ֵ:
//     �ͻ���ID��ʧ��ʱ����0
//
int WINAPI LogonEx(
    int nQsid,
    const char *pszHost,
    short nPort,
    const char *pszVersion,
    short nYybId,
    char nAccountType,
    const char *pszAccountNo,
    const char *pszTradeAccount,
    const char *pszJyPassword,
    const char *pszTxPassword,
    const char* pszOtherInfo,
    char *pszErrInfo);

//
// �����˻�ע��
//
// ����:
//     nClientID - �ͻ���ID
//
void WINAPI Logoff(int nClientID);

//
//
//
bool WINAPI IsConnectOK(int nClientID);

//
//
//
const char * WINAPI GetMachineInfo(int nClientID, char *pszBuff, int size);

//
//  0 - �ɹ�
// -1 - �������� ERR_PARAM_CHECK
// -2 - �ڴ���� ERR_MEMORY
// -3 - �߼����� ERR_LOGIC
//

//
// ��ѯ���ֽ�������
//
// ����:
//     nClientID   - �ͻ���ID
//     nCategory   - ��ʾ��ѯ��Ϣ�����࣬
//                  0 �ʽ�
//                  1 �ɷ�
//                  2 ����ί��
//                  3 ���ճɽ�
//                  4 �ɳ���
//                  5 �ɶ�����
//                  6 �������
//                  7 ��ȯ���
//                  8 ����֤ȯ
//                  9
//                 10
//                 11
//                 12 ���깺�¹ɲ�ѯ
//                 13 �¹��깺��Ȳ�ѯ
//                 14 ��Ų�ѯ
//                 15 ��ǩ��ѯ
//
//     pszResult   - ��APIִ�з��غ�Result�ڱ����˷��صĲ�ѯ����, ��ʽΪ������ݣ�
//                   ������֮��ͨ��\n�ַ��ָ������֮��ͨ��\t�ָ���һ��Ҫ����1024*1024�ֽڵĿռ䡣
//                   ����ʱΪ���ַ�����
//     pszErrInfo  - ͬLogon������ErrInfo˵��
//
int WINAPI QueryData(
    int nClientID,
    int nCategory,
    char *pszResult,
    char *pszErrInfo);

int WINAPI QueryDataEx(
    int nClientID,
    int nCategory,
    int nBatchNum,
    char *pszSeekStr,
    char *pszResult,
    char *pszErrInfo);

//
// ��ί�н���֤ȯ
//
// ����:
//     nClientID  - �ͻ���ID
//     nCategory  - ��ʾί�е����࣬
//                  0 ����
//                  1 ����
//                  2 ��������
//                  3 ��ȯ����
//                  4 ��ȯ��ȯ
//                  5 ��ȯ����
//                  6 ��ȯ��ȯ
//                  7 ����Ʒ����
//                  8 ����Ʒ����
//     nPriceType - ��ʾ���۷�ʽ
//                  0 �Ϻ��޼�ί�� �����޼�ί��
//                  1 (�м�ί��)���ڶԷ����ż۸�
//                  2 (�м�ί��)���ڱ������ż۸�
//                  3 (�м�ί��)���ڼ�ʱ�ɽ�ʣ�೷��
//                  4 (�м�ί��)�Ϻ��嵵����ʣ�� �����嵵����ʣ��
//                  5 (�м�ί��)����ȫ��ɽ�����
//                  6 (�м�ί��)�Ϻ��嵵����ת�޼�
//     pszGddm    - �ɶ�����, �����Ϻ���Ʊ���Ϻ��Ĺɶ����룻�������ڵĹ�Ʊ�������ڵĹɶ�����
//     pszZqdm    - ֤ȯ����
//     fPrice     - ί�м۸�
//     nQuantity  - ί������
//     pszResult  - ͬ��,���к���ί�б������
//     pszErrInfo - ͬ��
//
int WINAPI SendOrder(
    int nClientID,
    int nCategory,
    int PriceType,
    const char *pszGddm,
    const char *pszZqdm,
    float fPrice,
    int nQuantity,
    char *pszResult,
    char *pszErrInfo);

//
// ��ί��
//
// ����:
//     nClientID     - �ͻ���ID
//     nExchangeID   - ������ID�� �Ϻ�1������0(����֤ȯ��ͨ�˻�������2)
//     pszHth        - ��ʾҪ����Ŀ��ί�еı��
//     pszResult     - ͬ��
//     pszErrInfo    - ͬ��
//
int WINAPI CancelOrder(
    int nClientID,
    char nMarket,
    const char *pszHth,
    char *pszResult,
    char *pszErrInfo);

//
// ����:
//     nClientID     - �ͻ���ID
//     nExchangeID   - ������ID�� �Ϻ�1������0(����֤ȯ��ͨ�˻�������2)
//     pszHth        - ��ʾҪ����Ŀ��ί�еı��
//     pszGddm       - �ɶ�����
//     pszResult     - ͬ��
//     pszErrInfo    - ͬ��
//
int WINAPI CancelOrderEx(
    int nClientID,
    char nMarket,
    const char *pszHth,
    const char *pszGddm,
    char *pszResult,
    char *pszErrInfo);

//
// ��ȡ֤ȯ��ʵʱ�嵵����
//
// ����:
//     nClientID  - �ͻ���ID
//     pszZqdm    - ֤ȯ����
//     pszResult  - ͬ��
//     pszErrInfo - ͬ��
//
int WINAPI GetQuote(
    int nClientID,
    const char *pszZqdm,
    char *pszResult,
    char *pszErrInfo);

//
//
//
int WINAPI GetTradableQuantity(
    int nClientID,
    char nCategory,
    int nPriceType,
    const char *pszGddm,
    const char *pszZqdm,
    float fPrice,
    char *pszResult,
    char *pszErrInfo);

//
// ������ȯ�˻�ֱ�ӻ���
//
int WINAPI Repay(
    int nClientID,
    const char *pszAmount,
    char *pszResult,
    char *pszErrInfo);

//
//
//
int WINAPI QueryHistoryData(
    int nClientID,
    int nCategory,
    const char *pszBeginDate,
    const char *pszEndDate,
    char *pszResult,
    char *pszErrInfo);

int WINAPI QueryHistoryDataEx(
    int nClientID,
    int nCategory,
    int nBatchNum,
    char *pszSeekStr,
    const char *pszBeginDate,
    const char *pszEndDate,
    char *pszResult,
    char *pszErrInfo);

//
// ���˻�������ѯ���ཻ������
//
int WINAPI QueryDatas(
    int nClientID,
    int nCategory[],
    int nCount,
    char* pszResultOK[],
    char* pszResultFail[],
    char* pszErrInfo);

//
// ���˻������µ�
//
int WINAPI SendOrders(
    int nClientID,
    int nCategory[],
    int nPriceType[],
    const char *pszGddm[],
    const char *pszZqdm[],
    float fPrice[],
    int nQuantity[],
    int nCount,
    char* pszResultOK[],
    char* pszResultFail[],
    char* pszErrInfo);

//
// ���˻���������
//
int WINAPI CancelOrders(
    int nClientID,
    const char nMarket[],
    const char *pszOrderID[],
    int nCount,
    char* pszResultOK[],
    char* pszResultFail[],
    char* pszErrInfo);

//
// ���˻�������ȡ�嵵����
//
int WINAPI GetQuotes(
    int nClientID,
    const char *pszZqdm[],
    int nCount,
    char* pszResultOK[],
    char* pszResultFail[],
    char* pszErrInfo);

//
// һ������
//
int WINAPI QuickIPO(int nClientID);

//
// һ������
//
int WINAPI QuickIPODetail(
    int nClientID,
    int nCount,
    char* pszResultOK[],
    char* pszResultFail[],
    char* pszErrInfo);

//
// ������ͬ�˻���ѯ���ཻ������
//
int WINAPI QueryMultiAccountsDatas(
    int nClientID[],
    int nCategory[],
    int nCount,
    char* pszResultOK[],
    char* pszResultFail[]);

//
// ������ͬ�˻��µ�
//
int WINAPI SendMultiAccountsOrders(
    int nClientID[],
    int nCategory[],
    int nPriceType[],
    const char *pszGddm[],
    const char *pszZqdm[],
    float fPrice[],
    int nQuantity[],
    int nCount,
    char *pszResult[],
    char *pszErrInfo[]);

//
// ������ͬ�˻�����
//
int WINAPI CancelMultiAccountsOrders(
    int nClientID[],
    char nMarket[],
    const char *pszOrderID[],
    int nCount,
    char *pszResult[],
    char *pszErrInfo[]);

//
// ������ͬ�˻���ȡ�嵵����
//
int WINAPI GetMultiAccountsQuotes(
    int nClientID[],
    const char *pszZqdm[],
    int nCount,
    char *pszResult[],
    char *pszErrInfo[]);


#ifdef __cplusplus
}
#endif

#endif
