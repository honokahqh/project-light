#include "APP.h"
#include "Modbus_CORE.h"
MBS_PortFrameTypes MBS_Buf;
volatile uint8_t mbs_has_data;
/**
 * Cmd_MBS_Cmd_Request
 * @brief �յ�Lora����0x60����ʱ����-�ж��Ƿ�Ϊ������ַ
 * @brief ����Ǳ�����ֱַ�Ӵ���ظ�,�����MBS��������
 * @brief v101������Ϊ��ͨ�п�ʹ��,����Ҫת���ӻ�����
 * @author Honokahqh
 * @date 2023-08-05
 */
void MBS_CorePoll()
{
    if (MBS_Buf._rxLen >= 4 && mb_crc16(MBS_Buf._rxBuff, MBS_Buf._rxLen) == 0)
    {
        for (uint8_t i = 0; i < MBS_NODE_NUM; i++)
        {
            if (MBS_Buf._rxBuff[MBS_FRAME_SLAVER_ADDR] == mbsNode[i].mbsAddr)
            {
                MBS_CoreAnalyze(mbsNode[i]);
                mbs_has_data = 1;
            }
        }
    }
}

/**
 * MBS_CoreAnalyze
 * @brief mbs�ӻ�Э�����
 * @author Honokahqh
 * @date 2023-08-05
 */
void MBS_CoreAnalyze(mbsNode_t mbsNode) // ֻʵ����������---03H�����ּĴ���---10Hд������ּĴ���
{
    switch (MBS_Buf._rxBuff[1]) /* ��2���ֽ� ������ */
    {
#if MBS_FUNCTION_01_ENABLE
    case 0x01:
        MBS_Function01H(mbsNode);
        break;
#endif
#if MBS_FUNCTION_02_ENABLE
    case 0x02:
        MBS_Function02H();
        break;
#endif
#if MBS_FUNCTION_03_ENABLE
    case 0x03:
        MBS_Function03H(mbsNode);
        break;
#endif
#if MBS_FUNCTION_04_ENABLE
    case 0x04:
        MBS_Function04H();
        break;
#endif
#if MBS_FUNCTION_05_ENABLE
    case 0x05:
        MBS_Function05H(mbsNode);
        break;
#endif
#if MBS_FUNCTION_06_ENABLE
    case 0x06:
        MBS_Function06H(mbsNode);
        break;
#endif
#if MBS_FUNCTION_0F_ENABLE
    case 0x0F:
        MBS_Function0FH(mbsNode);
        break;
#endif
#if MBS_FUNCTION_10_ENABLE
    case 0x10:
        MBS_Function10H(mbsNode);
        break;
#endif
    default:
        MBS_PortSendAck(mbsNode.mbsAddr, MBS_EX_ILLEGAL_FUNCTION);
        break;
    }
}
void MBS_PortSendWithCRC(uint8 *buf, uint8 len)
{
    uint16 crc;
    crc = mb_crc16(buf, len);
    buf[len++] = crc;
    buf[len++] = crc >> 8;
    MBS_PhysicalSendBuff(buf, len);
}
/*
*****************************************************
* Method:       MBS_PortSendAck
* Description:	����Ӧ����Ϣ
* Author:       @Draven
* Date:  		2017/05/08
* Returns:      void
* Parameter:   	MBS_EX_STATE ackCode
* History:
*****************************************************
*/
void MBS_PortSendAck(uint16_t mbs_addr, MBS_EX_STATE ackCode)
{
    if (ackCode)
    {
        MBS_Buf._txBuff[MBS_FRAME_SLAVER_ADDR] = mbs_addr;
        MBS_Buf._txBuff[MBS_FRAME_FUNCTION_CODE] = MBS_Buf._rxBuff[MBS_FRAME_FUNCTION_CODE] | 0x80;
        MBS_Buf._txBuff[MBS_FRAME_FUNCTION_CODE + 1] = (uint8)ackCode; // ���ʹ�����
        MBS_Buf._txLen = 3;
    }
    else
    {
        uint8 i;
        for (i = 0; i < 6; i++)
        {
            MBS_Buf._txBuff[i] = MBS_Buf._rxBuff[i];
        }
        MBS_Buf._txLen = 6;
    }
    MBS_PortSendWithCRC(MBS_Buf._txBuff, MBS_Buf._txLen);
}
void MBS_PhysicalSendBuff(uint8 *buf, uint8 len)
{
    uart_send_data(buf, len);
}
