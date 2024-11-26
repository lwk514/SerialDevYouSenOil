#include "SerialDevYouSenOil.h"

extern "C" CPeripheralBase *SerialPeripheralCreate(CMsgClientManage *pMsgManage, CServeMsgManage *pServeMsg, unsigned int nMainId, unsigned int nSubId, SerialPortCreateParam_t &param, Json::Value &jsonValue)
{
	// 如果想起独立线程的话，将pMsgManage置为空即可，不使用外部传入的
	return new CSerialDevYouSenOil(pMsgManage, pServeMsg, nMainId, nSubId, param, jsonValue);
}

extern "C" bool SerialPeripheralRelease(CPeripheralBase *pPeripheral)
{
	if (NULL != pPeripheral)
	{
		pPeripheral->perip_base_stop_run();

		delete pPeripheral;
	}
	return true;
}

CSerialDevYouSenOil::CSerialDevYouSenOil(CMsgClientManage *pMsgManage, CServeMsgManage *pServeMsg, unsigned int nMainId, unsigned int nSubId, SerialPortCreateParam_t &param, Json::Value &jsonValue)
	: CSerialPortBase(pMsgManage, pServeMsg, nMainId, nSubId, param, jsonValue)
{
	RM_TRUCK_LOG_CRITICAL("-----new CSerialDevYouSenOil begin--------\n");
	m_RecvBuffLen = 0;
	memset(m_strRecvBuff, 0x00, sizeof(MAX_TRANS_BUFSIZE));
    memset(&m_liquidheightdata, 0x00, sizeof(msgLiquidHeightData_t));
}

CSerialDevYouSenOil::~CSerialDevYouSenOil()
{
}

xint32_t CSerialDevYouSenOil::perip_base_init()
{

	return 0;
}

xint32_t CSerialDevYouSenOil::perip_base_uninit()
{
	return 0;
}

xint32_t CSerialDevYouSenOil::perip_base_cycle_running(xuint64_t milliSecond)
{


	return 0;
}

xint32_t CSerialDevYouSenOil::serial_base_analyze_data(xbyte_t *pBufferData, xint32_t nBufferLen)
{
	RM_TRUCK_LOG_MESSAGE("Rec data: \n");
	RM_TRUCK_HEX_MESSAGE(pBufferData, nBufferLen);
	RM_TRUCK_LOG_MESSAGE("\n");

	RecvSerialData(pBufferData, nBufferLen);

	return 0;
}

xint32_t CSerialDevYouSenOil::RecvSerialData(char *pData, int dataLen)
{
	if (NULL == pData)
	{
		RM_TRUCK_LOG_ERROR("pData is NULL\n");
		return -1;
	}

	if (dataLen >= MAX_TRANS_BUFSIZE)
	{
		RM_TRUCK_LOG_ERROR("recv data length is too large, length = %d\n", dataLen);
		return -1;
	}

	if ((m_RecvBuffLen + dataLen) >= MAX_TRANS_BUFSIZE)
	{
		RM_TRUCK_LOG_ERROR("(M_RecvBufferLen + dataLen) >= MAX_TRANS_BUFSIZE\n");
		m_RecvBuffLen = 0;
		memset(m_strRecvBuff, 0x00, MAX_TRANS_BUFSIZE);
	}

	memcpy(m_strRecvBuff + m_RecvBuffLen, pData, dataLen);
	m_RecvBuffLen += dataLen;

	while (m_RecvBuffLen > 0)
	{
		HandleSerialData(m_strRecvBuff, m_RecvBuffLen);
		break;
	}
	return 0;
}

xint32_t CSerialDevYouSenOil::HandleSerialData(char *iPacketData, int iDataLen)
{
	if (NULL == iPacketData)
	{
		RM_TRUCK_LOG_ERROR("iPacketData is NULL\n");
		return -1;
	}

	if (iDataLen > 0)
	{
		bool isFull = true;

		for (xint32_t index = 0; index < iDataLen; index++)
		{
			// 判断协议固定头 *     DATA_RECV_HEAD是已经定义好的宏
			if (index + 1 < iDataLen && DATA_RECV_HEAD == iPacketData[index])
			{
				// 如果数据不全，则删除前面处理数据，并将当前剩余数据保存到缓冲区
				if (index + DATA_LEN -1 > m_RecvBuffLen)  // 当前剩余数据小于36
				{
					RM_TRUCK_LOG_ERROR("The Data is not full\n");
					m_RecvBuffLen -= index;   
					memcpy(m_strRecvBuff, m_strRecvBuff + index, m_RecvBuffLen);

					isFull = false;
					break;
				}
                else if (DATA_RECV_END1 == iPacketData[index + DATA_LEN - 2] && DATA_RECV_END2 == iPacketData[index + DATA_LEN - 1])
                {
                    AnalySerialData(iPacketData + index);
                    index += (DATA_LEN - 1);
                }
			}
		}

		if(isFull)
		{
			m_RecvBuffLen = 0;
			memset(m_strRecvBuff, 0x00, sizeof(m_strRecvBuff));
		}
		serial_base_clear_port_data();
	}

	return 0;
}


xint32_t CSerialDevYouSenOil::AnalySerialData(char *Data)
{

    /*
        帧格式：包头  标识  ID号 未知数据  AD值       检验和 包尾
                *    CFV   01  00       0000         8A    ..
    */
    // 获取校验和
    xuint32_t checksum = 0;
    checksum = (CharToHEX(Data[12]) << 4) + CharToHEX(Data[13]);

    // 计算校验和
    xuint32_t calsum = 0;
    for (xuint32_t i = 0; i <= 11; i++)
    {
        calsum += Data[i];
    }
    calsum &= 0xFF; // 取最后两位值

    // 判断校验和
    if (calsum != checksum)
    {
        RM_TRUCK_LOG_ERROR("checksum:%d error, calsum:%d\n", checksum, calsum);
        return 0;
    }

    // 获取ID号
    xuint32_t id = 0;
    id = (CharToHEX(Data[4]) << 4) + CharToHEX(Data[5]);

    // 获取AD值
    xuint32_t advalue = 0;
    for (xuint32_t i = 8; i <= 11; i ++)
    {
        advalue = (advalue << 4) + CharToHEX(Data[i]);
    }

    // AD值转化为距离 单位 0.1mm
    xuint32_t liquidheight = adValueToMillimeters(advalue);

    RM_TRUCK_LOG_MESSAGE("id:%u advalue:%u liquidheight:%u(0.1mm) checksum:%u calsum:%u\n", id, advalue, liquidheight,checksum, calsum);

    m_liquidheightdata.liquidheight = liquidheight;
    m_liquidheightdata.ucAnalysisState = 1;

    // TODO:加入策略，当前传感器支持数据平滑处理

    m_pMsgManage->MSGSendMessage(0, MESSAGE_TRUCK_BUSINESS_YOUSEN_LIQUIDHEIGHT, 0, (xbyte_t *)&m_liquidheightdata, sizeof(m_liquidheightdata), m_nMsgId);

    return 0;
}

xint32_t CSerialDevYouSenOil::CharToHEX(unsigned char a)
{
    if ((a >='A') && (a <= 'F'))
    {
        return a - 'A' + 10;
    }
    else if ((a >= 'a') && (a <= 'f'))
    {
        return a - 'a' + 10;
    }
    else if ((a >= '0') && (a <= '9'))
    {
        return a - '0';
    }

	return 0;
}


xuint32_t CSerialDevYouSenOil::adValueToMillimeters(xuint32_t adValue) 
{
    // AD值范围 0-65535 距离范围 0-1m
    const double scaleFactor = 10000.0 / 65535.0;

    return static_cast<double>(adValue) * scaleFactor;
}