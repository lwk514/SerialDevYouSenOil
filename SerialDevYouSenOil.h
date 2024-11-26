/*
 *Copyright (c) 2022, 深圳市锐明技术股份有限公司软件二部
 *ALL rights reserved
 *
 *文件名称：SerialDevYouSenOil.h
 *文件标识：
 *摘 要：火车版本友森油杆
 *
 *当前版本：1.0
 *作 者：xdzhan
 *完成日期：2024年09月19日
 *
 */
#ifndef _SERIALDEV_YOUSENOIL_H_
#define	_SERIALDEV_YOUSENOIL_H_

#include "SerialPortBase.h"
#include "PeripheralDefines.h"
#include "TruckMessage.h"

#define MAX_TRANS_BUFSIZE      (1024)

#define DATA_RECV_HEAD         '*'
#define DATA_RECV_END1         0x0D
#define DATA_RECV_END2         0x0A
#define DATA_LEN                (16)

class CSerialDevYouSenOil : public CSerialPortBase
{
public:
	CSerialDevYouSenOil(CMsgClientManage *pMsgManage, CServeMsgManage *pServerMsg, unsigned int nMainId, unsigned int nSubId, SerialPortCreateParam_t &param, Json::Value &jsonValue);
	virtual ~CSerialDevYouSenOil();

public:
	virtual xint32_t perip_base_init();
	virtual xint32_t perip_base_uninit();
	virtual xint32_t perip_base_cycle_running(xuint64_t milliSecond);
	virtual xint32_t serial_base_analyze_data(xbyte_t *pBufferData, xint32_t nBufferLen);

private:
	xint32_t RecvSerialData(char *pData, int dataLen);
	xint32_t HandleSerialData(char *iPacketData, int iDataLen);
    xint32_t AnalySerialData(char *Data);
    xint32_t CharToHEX(unsigned char a);                                                           // ASCII码转十进制
    xuint32_t adValueToMillimeters(xuint32_t adValue);

private:
	xbyte_t m_strRecvBuff[MAX_TRANS_BUFSIZE];
	xint32_t m_RecvBuffLen;
    msgLiquidHeightData_t m_liquidheightdata;
};

#endif // _SERIALDEV_YOUSENOIL_H_
