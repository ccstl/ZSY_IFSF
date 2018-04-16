#include "common.h"


/********************************************************************\
* ������: Time_Process
* ˵��:
* ����:    ��ʱ��ת����7�ֽڵ�yyyymmddhhmmss
* ����:
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-9-18
\*********************************************************************/
void Time_Process()
{
    ftime(&tb);
    now=localtime(&tb.time);
    para.FSC_TIME[0]=((now->tm_year+1900)/1000)*16+((now->tm_year+1900)/100)%10;
    para.FSC_TIME[1]=((now->tm_year+1900)%100)/10*16+((now->tm_year+1900)%100)%10;
    para.FSC_TIME[2]=(now->tm_mon+1)/10*16+(now->tm_mon+1)%10;
    para.FSC_TIME[3]=(now->tm_mday)/10*16+(now->tm_mday)%10;
    para.FSC_TIME[4]=(now->tm_hour)/10*16+(now->tm_hour)%10;
    para.FSC_TIME[5]=(now->tm_min)/10*16+(now->tm_min)%10;
    para.FSC_TIME[6]=(now->tm_sec)/10*16+(now->tm_sec)%10;
}

void FCS_To_OPT_HeadandTail(Uchar cmdID,Ushort len)//֡ͷ+֡β
{
    char str[15];
    Ushort crc;
    //֡ͷ
    Uart_Tx_Buff[0] = 0xAA;       //��ʼ�� AAH
    Uart_Tx_Buff[1] = 0xFF;       //��ʼ�� FFH
    Uart_Tx_Buff[2] = para.NZN;   //ǹ�ź� ��Ŀ���豸
    Uart_Tx_Buff[3] = 0x00;       //Դ�豸
    Uart_Tx_Buff[4] = cmdID;      //������ 01H~90H
    Uart_Tx_Buff[5] = len>>8;
    Uart_Tx_Buff[6] = (Uchar)len;
    //֡β
    crc=ModbusCrc16(&Uart_Tx_Buff[2], len+5);
    Uart_Tx_Buff[len+7] = crc>>8;
    Uart_Tx_Buff[len+8] = (Uchar)crc;
    Uart_Tx_Buff[len+9] = 0xCC; //������CC
    Uart_Tx_Buff[len+10] = 0xFF; //������FF
    Uart_Tx_Flag=1;
    Uart_Data_Len=len+11;
    sprintf(str,"U_Send_%02X",cmdID);
    Print_HEX(str,Uart_Data_Len,Uart_Tx_Buff);
}

//********************************************************************
// ������: FCS_To_JKOPT_51
// ����:   FSC ��ѵ������
// ����:
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_51()
{
    Uchar tt=7,i=0;    
    Time_Process();
    for(i=0; i<7; i++)      //ʱ��
    {
        Uart_Tx_Buff[tt++]=para.FSC_TIME[i];
    }
    if(Stop_Opt==1)
        Uart_Tx_Buff[tt++]=0;
    else
        Uart_Tx_Buff[tt++]=FS_OK;                    //fs״̬
    Uart_Tx_Buff[tt++]=ReadNoPayFlag;                //��ȡδ֧�����ױ�ʶ
    Uart_Tx_Buff[tt++]=fp_id.Default_Fuelling_Mode;  //ģʽѡ��
    Uart_Tx_Buff[tt++]=BoartRequestFlag; //���忪ʼ����֧�����
    Uart_Tx_Buff[tt++]=price.VER;
    FCS_To_OPT_HeadandTail(0x51,tt-7);

    if(OPT_IS_Online)
        OPT_IS_Online--;
}

//********************************************************************
// ������: FCS_To_JKOPT_52
// ����:   FSC �ظ��������ϴ����׼�¼
// ����:
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2017-8-30 10:19:11
//*********************************************************************
void FCS_To_JKOPT_52()
{
    Uchar tt=7,i=0;  
    UnionU32 urdv;
    Uart_Tx_Buff[tt++]=1;//�Ƿ�ɹ���־
    Uart_Tx_Buff[tt++]=0;//��������
    urdv.data=para.POS_TTC;
    for(i=0;i<4;i++)
    {
        Uart_Tx_Buff[tt++]=urdv.rdcv[i];
    }
    FCS_To_OPT_HeadandTail(0x52,tt-7);
}

//********************************************************************
// ������: FCS_To_JKOPT_57
// ����:   FSC���������ۼ�
// ����:
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2017-8-30 10:52:34
//*********************************************************************
void FCS_To_JKOPT_57()
{
    Uchar tt=7;  
    Uart_Tx_Buff[tt++]=para.NZN;
    FCS_To_OPT_HeadandTail(0x57,tt-7);
}

//********************************************************************
// ������: FCS_To_JKOPT_58
// ����:   FSC��������ָ����ˮ
// ����:
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2017-8-30 11:06:18
//*********************************************************************
void FCS_To_JKOPT_58()
{
    Uchar tt=7;  
    Uart_Tx_Buff[tt++]=para.NZN;
    FCS_To_OPT_HeadandTail(0x58,tt-7);
}

//********************************************************************
// ������: FCS_To_JKOPT_5E
// ����:   �����������·�����
// ����:
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2017-8-30 10:52:34
//*********************************************************************
void FCS_To_JKOPT_5E()
{
    Uchar tt=7,i;  
    Time_Process();
    Uart_Tx_Buff[tt++]=price.VER;    //���۰汾��
    Uart_Tx_Buff[tt++]=para.devicetype; //�豸����
    for(i=0; i<7; i++)      //ʱ��
    {
        Uart_Tx_Buff[tt++]=para.FSC_TIME[i];
    }
    Uart_Tx_Buff[tt++]=4;//������Ŀ

    Uart_Tx_Buff[tt++]=price.Price[0];
    Uart_Tx_Buff[tt++]=price.Price[0]>>8;
    Uart_Tx_Buff[tt++]=price.Price[1];
    Uart_Tx_Buff[tt++]=price.Price[1]>>8;
    Uart_Tx_Buff[tt++]=price.Price[2];
    Uart_Tx_Buff[tt++]=price.Price[2]>>8;
    Uart_Tx_Buff[tt++]=price.Price[3];
    Uart_Tx_Buff[tt++]=price.Price[3]>>8;
    
    FCS_To_OPT_HeadandTail(0x5E,tt-7);
}



//********************************************************************
// ������: FCS_To_JKOPT_70
// ����:    FSC �ظ�������������Ȩ
// ����:   
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2017-8-30 11:02:44
//*********************************************************************
void FCS_To_JKOPT_70()
{
    Uchar tt=7,i=0;
    UnionU32 urdv;
    Uart_Tx_Buff[tt++]=Auth_Type; //��Ȩ����
    urdv.data=Auth_Value;
    for(i=0; i<4; i++)            //��Ȩֵ
    {
        Uart_Tx_Buff[tt++]=urdv.rdcv[i];
    }
    FCS_To_OPT_HeadandTail(0x70,tt-7);
}

//********************************************************************
// ������: FCS_To_JKOPT_71
// ����:    FSC �ظ�EPS���ݸ�������
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_71()
{
    Uchar tt=7,i=0;
    for(i=0; i<EPS_Data_Len; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_To_Uart[i];
        EPS_Back_Buf[i]=EPS_To_Uart[i];
    }
    EPS_Back_num = EPS_Data_Len;
    
    if((EPS_To_Uart[26]==0x02)&&
        (EPS_To_Uart[27]==0x00)&&
        (EPS_To_Uart[28]==0x02)&&
        (HuiSuoFlag==0))//  EPS�鿨����
    {
        YanKaFlag=1;
        OverTransFlag=0;
    }
    else if((EPS_To_Uart[26]==0x02)&&
        (EPS_To_Uart[27]==0x00)&&
        (EPS_To_Uart[28]==0x03)&&
        (YanKaFlag==1))//  EPS֪ͨ�����������
    {
        HuiSuoFlag=1;
        YanKaFlag=0;
    }
    else if((EPS_To_Uart[26]==0x02)&&
        (EPS_To_Uart[27]==0x00)&&
        (EPS_To_Uart[28]==0x04)&&
        (OverTransFlag==0))//  EPS���ؼ���������֧��
    {
        RequestPayFlag=1;
        HuiSuoFlag=0;
    }
    else if((EPS_To_Uart[26]==0x02)&&
        (EPS_To_Uart[27]==0x00)&&
        (EPS_To_Uart[28]==0x05))//  EPS���ؼ�����֪ͨ���׽��
    {
        OverTransFlag=1;
        RequestPayFlag=0;
    }
    bzero(EPS_To_Uart,LENGTH);
    FCS_To_OPT_HeadandTail(0x71,tt-7);
}


//********************************************************************
// ������: FCS_To_JKOPT_72
// ����:    FSC ֹͣ������
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2015-06-03
//*********************************************************************
void FCS_To_JKOPT_72()
{
    Uchar tt=7;
    Uart_Tx_Buff[tt++]=para.NZN;
    FCS_To_OPT_HeadandTail(0x72,tt-7);
}

//********************************************************************
// ������: FCS_To_JKOPT_73
// ����:    FSC �ظ��������ϴ�����
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2017-8-30 11:55:56
//*********************************************************************
void FCS_To_JKOPT_73()
{
    Uchar tt=7;
    Uart_Tx_Buff[tt++]=0;
    FCS_To_OPT_HeadandTail(0x73,tt-7);
}

//eps�������ݴ���
void EpsBackDataDispose()
{
    Uchar tt=7,i=0;
    for(i=0; i<EPS_Back_num; i++)
    {
        Uart_Tx_Buff[tt++]=EPS_Back_Buf[i];
    }
    FCS_To_OPT_HeadandTail(0x71,tt-7);
}


