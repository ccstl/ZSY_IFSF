#include "common.h"


/***************************************************************************
**�������ƣ� ModbusCrc16
**�������ܣ�������CRC16����
**��ڲ�����buf:��Ҫ����CRC�����ݣ�len:��Ҫ��������ݳ���
**����ֵ�������CRC
**�����ߣ����
**�������ڣ�2016-3-30 15:45:03
**
**�޸��ߣ�
**�޸����ڣ�
**�޸����ݣ�
***************************************************************************/
Ushort ModbusCrc16(Uchar *buf, Ushort Len)
{
    Ushort IX, IY, CRC;
    CRC = 0xFFFF;
    if(Len <= 0)    return 0;
    for(IX = 0; IX < Len; IX++)
    {
        CRC = CRC ^ (Ushort)(buf[IX]);
        for(IY = 0; IY < 8; IY++)
        {
            if((CRC &1) != 0)
                CRC = (CRC >> 1) ^ 0xA001;
            else
                CRC = CRC >> 1;
        }
    }
    return CRC;
}


