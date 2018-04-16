#include     "common.h"
#define FALSE 1
#define TRUE 0

int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200};
int name_arr[] = {115200, 57600, 38400,  19200,  9600,  4800,  2400, 1200};

extern char *secname; 


/********************************************************************\
* ������: set_speed
* ˵��:
* ����:    ���ô��ڲ�����
* ����:     fd:�ļ�������
            speed:������
* ����ֵ:   0:�ɹ�����0:ʧ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-8-22
\*********************************************************************/
void set_speed(int fd, int speed)
{
    int   i;
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for(i = 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if(speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if(status != 0)
                perror("tcsetattr fd1");
            return;
        }
        tcflush(fd, TCIOFLUSH);
    }
}

/********************************************************************\
* ������: set_Parity
* ˵��:
* ����:    ���ô�������λ��ֹͣλ��У�鷽ʽ
* ����:     fd:�ļ�������
            databits:����λ
            stopbits:ֹͣλ
            parity:У�鷽ʽ
* ����ֵ:   0:�ɹ�����0:ʧ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-8-22
\*********************************************************************/
int set_Parity(int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    if(tcgetattr(fd, &options)  !=  0)
    {
        perror("SetupSerial 1");
        return(FALSE);
    }
    options.c_cflag &= ~CSIZE;
    switch(databits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unsupported data size\n");
            return (FALSE);
    }
    switch(parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;
        case 'S':
        case 's':
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported parity\n");
            return (FALSE);
    }
    switch(stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported stop bits\n");
            return (FALSE);
    }

    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    /* Set input parity option */

    if(parity != 'n')
        options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;

    tcflush(fd, TCIFLUSH); /* Update the options and do it NOW */
    if(tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("SetupSerial 3");
        return (FALSE);
    }
    return (TRUE);
}

//********************************************************************
// ������: Dispose_Recv51
// ����:    FSC ����������ϴ���ѵ
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2017-8-29 17:28:15
//*********************************************************************
void Dispose_Recv51()
{
    Uchar tt=7,i,dst[30],tmp[3];
    UnionU32 urdv;
    //�豸����
    bzero(dst, 30);
    switch(Uart_Rx_Buff[tt])
    {
        case 0:
            sprintf((char *)dst,"LNG-%d",conf.Gun_num);
        break;
        case 1:
            sprintf((char *)dst,"CNG-%d",conf.Gun_num);
        break;
        case 2:
            sprintf((char *)dst,"������-%d",conf.Gun_num);
        break;
        case 3:
            sprintf((char *)dst,"ж����-%d",conf.Gun_num);
        break;
    }
    
    //״̬
    para.ST=Uart_Rx_Buff[tt+1];
    switch(para.ST)
    {
        case 1:
            para.ST=2;//��Ҫ1(������)״̬
            P_Log(conf.Gun_num,"%s Disable\n",dst);
            break;
        case 2:
            P_Log(conf.Gun_num,"%s Closed\n",dst);
            break;
        case 3:
            HaveAuth=0;  
            if(StartFuelling==8) StartFuelling=0;//��ֹ�ڼ����󣬸մ���5,6,8״̬�ͱ���λ��ػ�����ɴ�״̬δ����
            P_Log(conf.Gun_num,"%s Idle\n",dst);
            break;
        case 4:
            P_Log(conf.Gun_num,"%s Auth request\n",dst);
            break;
        case 5:
            P_Log(conf.Gun_num,"%s Has authorized\n",dst);
            break;
        case 6:
            P_Log(conf.Gun_num,"%s Start fuel 1\n",dst);
            break;
        case 7:
            P_Log(conf.Gun_num,"%s Start fuel 2\n",dst);
            break;
        case 8:
            P_Log(conf.Gun_num,"%s Fueling...\n",dst);           
            break;
        case 9:
            P_Log(conf.Gun_num,"%s Paying...\n",dst);
            break;
        default:
            P_Log(conf.Gun_num,"%s Error ST\n",dst);
            break;
    }        
    
    if(para.ST==8)//������
    {   
        //���׵���
        para.PRC=(Ushort)Uart_Rx_Buff[tt+3]<<8|Uart_Rx_Buff[tt+2];
        
        //��ǰ������
        for(i = 0; i < 4; i++)
        {
            urdv.rdcv[i] =Uart_Rx_Buff[tt+4+i];
        }
        para.VOL=urdv.data;    
        
        //��ǰ�������
        for(i = 0; i < 4; i++)
        {
            urdv.rdcv[i] = Uart_Rx_Buff[tt+8+i];
        }
        para.AMN=urdv.data;

        //IC������
        bzero(dst, 30);
        bzero(tmp, 3);
        for(i = 0; i < 10; i++)
        {
            para.ASN[i] = Uart_Rx_Buff[tt+12+i];
            sprintf((char *)tmp, "%02X", para.ASN[i]);
            strcat((char *)dst, (const char*)tmp);
        }    
        P_Log(conf.Gun_num,"Card ASN:%s\n", dst);

        //��ǰѹ��
        para.CUR_PR=(Ushort)Uart_Rx_Buff[tt+23]<<8|Uart_Rx_Buff[tt+22];

        ////////////�ϴ���fuel///////////////
        if(FS_OK==1)
        {
            if((StartFuelling==0)&&(Uart_To_FS_Flag==0)&&(para.VOL>1))
            {
                Reply_FS_Upload568Status();
                StartFuelling=8;
            }
            else if((StartFuelling==8)&&(Uart_To_FS_Flag==0))
            {
                para.ST=8;
                Reply_FS_UploadRealtimeData();
            }
        }
        /////////////////end////////////////
        P_Log(conf.Gun_num,"CUR_PRC:%.2f\n", para.PRC / 100.0);
        P_Log(conf.Gun_num,"CUR_VOL:%.2f\n", para.VOL / 100.0);    
        P_Log(conf.Gun_num,"CUR_AMN:%.2f\n", para.AMN / 100.0);
        P_Log(conf.Gun_num,"CUR_PRE:%dkp\n", para.CUR_PR);
    }
    
    //δ֧��������
    for(i = 0; i < 4; i++)
    {
        urdv.rdcv[i] = Uart_Rx_Buff[tt+24+i];
    }
    para.NO_Payment_Trans=urdv.data;
    P_Log(conf.Gun_num,"NO_Payment_Trans:%d\n",para.NO_Payment_Trans);

    //������Ϣ
    for(i = 0; i < 4; i++)
    {
        urdv.rdcv[i] = Uart_Rx_Buff[tt+28+i];
    }
    para.syserror=urdv.data;                  
    P_Log(conf.Gun_num,"syserror:%d\n",para.syserror);
    
}

//********************************************************************
// ������: Dispose_Recv52
// ����:    FSC ����������ϴ���ˮ
// ����:   ��
// ����ֵ:   ��
// ������:   Yang Chao Xu
// ����ʱ��: 2017-8-30 09:49:52
//*********************************************************************
void Dispose_Recv52()
{
    Uchar   i,tt=7,dst[30], tmp[3];
    UnionU32 urdv;

    P_Log(conf.Gun_num,"===========Dispose_Recv52 Recv===========\n");

    //UID������¼ǹ��ˮ��
    for(i=0; i<4; i++)
    {
        urdv.rdcv[i]=Uart_Rx_Buff[tt+i];
    }
    para.POS_TTC=urdv.data;
    P_Log(conf.Gun_num,"POS_TTC=%d\n", para.POS_TTC);

    //IC������
    bzero(dst, 30);
    bzero(tmp, 3);
    for(i = 0; i < 10; i++)
    {
        para.ASN[i] = Uart_Rx_Buff[tt+i+5];
        sprintf((char *)tmp, "%02X", para.ASN[i]);
        strcat((char *)dst, (const char *)tmp);
    }
    P_Log(conf.Gun_num,"Card ASN:%s\n", dst);

    //����ʱ��
    bzero(dst, 30);
    bzero(tmp, 3);
    for(i = 0; i < 7; i++)
    {
        para.TIME[i] = Uart_Rx_Buff[i+25+tt];
        sprintf((char *)tmp, "%02X", para.TIME[i]);
        strcat((char *)dst, (const char *)tmp);
    }
    P_Log(conf.Gun_num,"Start_time:%s\n", dst);

    //ֹͣʱ��
    bzero(dst, 30);
    bzero(tmp, 3);
    for(i = 0; i < 7; i++)
    {
        para.END_TIME[i] = Uart_Rx_Buff[i+32+tt];
        sprintf((char *)tmp, "%02X", para.END_TIME[i]);
        strcat((char *)dst, (const char *)tmp);
    }
    P_Log(conf.Gun_num,"End_time:%s\n", dst);

    //���μ�������
    para.PRC=(Ushort)Uart_Rx_Buff[40+tt]<<8|Uart_Rx_Buff[39+tt];
    P_Log(conf.Gun_num,"PRC:%.2f\n", para.PRC / 100.0);
   
    //���μ�����
    for(i = 0; i < 4; i++)
    {
        urdv.rdcv[i] = Uart_Rx_Buff[i+41+tt];
    }
    para.VOL=urdv.data;
    P_Log(conf.Gun_num,"VOL:%.2f\n", para.VOL / 100.0);
    
    //���μ������
    for(i = 0; i < 4; i++)
    {
        urdv.rdcv[i] = Uart_Rx_Buff[i+45+tt];
    }
    para.AMN=urdv.data;
    P_Log(conf.Gun_num,"AMN:%.2f\n", para.AMN / 100.0);

    //�ۼ���������
    for(i = 0; i < 4; i++)
    {
        urdv.rdcv[i] = Uart_Rx_Buff[i+61+tt];
    }
    para.V_TOT=urdv.data;
    P_Log(conf.Gun_num,"V_TOT:%.2f\n", para.V_TOT / 100.0);

    //�ۼ����ѽ��
    for(i = 0; i < 4; i++)
    {
        urdv.rdcv[i] = Uart_Rx_Buff[i+65+tt];
    }
    para.V_AMN=urdv.data;
    P_Log(conf.Gun_num,"V_AMN:%.2f\n", para.V_AMN / 100.0);
    
    //����ǰѹ��
    para.STAR_PRE=(Ushort)Uart_Rx_Buff[70+tt]<<8|Uart_Rx_Buff[69+tt];
    P_Log(conf.Gun_num,"STAR_PRE:%.2f\n", para.STAR_PRE / 100.0);

    //������ѹ��
    para.STOP_PRE=(Ushort)Uart_Rx_Buff[72+tt]<<8|Uart_Rx_Buff[71+tt];
    P_Log(conf.Gun_num,"STOP_PRE:%.2f\n", para.STOP_PRE / 100.0);

    //��¼����
    para.T_TYPE = Uart_Rx_Buff[84+tt];
    if(para.T_TYPE==1)
        P_Log(conf.Gun_num,"Online Record\n");
    else
        P_Log(conf.Gun_num,"Offline Record\n");
    if(para.VOL==0)//����0����
    {
    }
    else if(para.T_TYPE==1)
    {
        HaveAuth=0;
        StartFuelling=9;//���׼�¼�ϴ�����0
        Reply_FS_UploadOverFuel();//�ϴ����׼�¼        
        uploadtransagain=1;
        uploadtransagaincount=0;
        paraonline = para;
    }
    else if(para.T_TYPE==0)//�ѻ�����
    {
        ReadNoPayFlag=0;//�ϴ���ȴ��´ζ�ȡ����1        
        if(para.NO_Payment_Trans>0)
            para.NO_Payment_Trans--;
        Reply_FS_ReadNO_Pay_Trans(0);
    }
    CmdFlag=0x52;//�ظ��ϴ�����
}

/********************************************************************\
* ������: Dispose_Recv57
* ˵��:
* ����:    FS���������ۼ�
* ����:    ��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��:2017-8-30 11:07:54
\*********************************************************************/
void Dispose_Recv57()
{
    Uchar   i,tt=7;
    UnionU32 urdv;
    P_Log(conf.Gun_num,"===========Dispose_Recv57 Recv===========\n");

    //UID������¼ǹ��ˮ��
    for(i=0; i<4; i++)
    {
        urdv.rdcv[i]=Uart_Rx_Buff[tt+i];
    }
    para.POS_TTC=urdv.data;
    P_Log(conf.Gun_num,"POS_TTC=%d\n", para.POS_TTC);

    //�ۼ���������
    for(i = 0; i < 4; i++)
    {
        urdv.rdcv[i] = Uart_Rx_Buff[i+4+tt];
    }
    para.V_TOT=urdv.data;
    P_Log(conf.Gun_num,"V_TOT:%.2f\n", para.V_TOT / 100.0);

    //�ۼ����ѽ��
    for(i = 0; i < 4; i++)
    {
        urdv.rdcv[i] = Uart_Rx_Buff[i+8+tt];
    }
    para.V_AMN=urdv.data;
    P_Log(conf.Gun_num,"V_AMN:%.2f\n", para.V_AMN / 100.0);
    Reply_FS_ReadTotal();
}

/********************************************************************\
* ������: Dispose_Recv5E
* ˵��:
* ����:    FS�·����۳ɹ�
* ����:    ��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��:2017-8-30 11:07:54
\*********************************************************************/
void Dispose_Recv5E()
{
    P_Log(conf.Gun_num,"===========Dispose_Recv5E Recv===========\n");
    para.devicetype=Uart_Rx_Buff[7];
    CmdFlag=0x5E;//�ظ��·�����
    Reply_FS_DownLoad_Price_Success();
    P_Log(conf.Gun_num,"===========DownLoad Price Success===========\n");
}

/********************************************************************\
* ������: Dispose_Recv70
* ˵��:
* ����:    ������������Ȩ
* ����:    ��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2015-06-04
\*********************************************************************/
void Dispose_Recv70()
{
    Uchar   Flag, i,tt=7;
    UnionU32 urdv;
    long Value;
    P_Log(conf.Gun_num,"===========Dispose_Recv70 Recv===========\n");
    P_Log(conf.Gun_num,"Gun NO:%d\n", para.NZN);
    Flag = Uart_Rx_Buff[tt];     //��Ȩ���
    if(Flag == 0)
    {
        P_Log(conf.Gun_num,"Auth Result: Success\n");
    }
    else
    {
        P_Log(conf.Gun_num,"Auth Result: Failed\n");
    }

    if(Auth_Type == 0)
    {
        P_Log(conf.Gun_num,"Auth Type: Amount \n");
    }
    else if(Auth_Type == 1)
    {
        P_Log(conf.Gun_num,"Auth Type: Volume \n");
    }
    else
    {
        P_Log(conf.Gun_num,"Auth Type: Other \n");
    }
    for(i=0; i<4; i++)//��Ȩֵ
    {
        urdv.rdcv[i]=Uart_Rx_Buff[tt+2+i];
    }
    Value = urdv.data;
    P_Log(conf.Gun_num,"Auth Value:%ld\n", Auth_Value);
    P_Log(conf.Gun_num,"Flag=%d Value:%ld\n", Flag, Value);
    if((Flag == 0) && (Value == Auth_Value))
    {
        HaveAuth=1;
        Reply_FS_Auth_OPT();
    }
    else
    {
        Reply_FS_CantAuth_OPT();
    }
}



/********************************************************************\
* ������: Dispose_Recv71
* ����:    ��������EPS������( ֻ��ת��)
* ����:    ��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��:2015-06-04
\*********************************************************************/
void Dispose_Recv71()
{
    Uchar   i = 0;
    if((Cli_EPS_fd < 1) && (Reconnect_EPS_Flag == 0))
    {
        Reconnect_EPS_Flag = 1;
    }    
    EPS_Data_Len = Uart_Data_Len - 11;
    if((Uart_Rx_Buff[33]==0x02)&&
        (Uart_Rx_Buff[34]==0x00)&&
        (Uart_Rx_Buff[35]==0x02))//�����鿨
    {
        P_Log(conf.Gun_num,"===========Verify===========\n");
        para.VOL=0;//�������鿨����֮ǰ�Ľ������㣬��ֹ��ȡδ֧���������ض�
        if(YanKaFlag==1) EpsBackDataDispose();//֮ǰ�Ѿ��յ����鿨������
        if((FaQiYanKaFlag==1)&&(FaQiYanKaTime<YANKATIMEOUT))    return;//��ֹ����鿨�����MAC2����)            
        OverTransFlag=0;
        BoartRequestFlag=0;
        FaQiYanKaFlag=1;          
    }
    else if((Uart_Rx_Buff[33]==0x02)&&
        (Uart_Rx_Buff[34]==0x00)&&
        (Uart_Rx_Buff[35]==0x03))//����֪ͨ����
    {
        P_Log(conf.Gun_num,"===========GreyLock===========\n");
        FaQiYanKaFlag=0;
        FaQiYanKaTime=0;
        if(HuiSuoFlag==1) EpsBackDataDispose();//֮ǰ�յ��˻������
    }
    else if((Uart_Rx_Buff[33]==0x02)&&
        (Uart_Rx_Buff[34]==0x00)&&
        (Uart_Rx_Buff[35]==0x04))//����֧������
    {
        P_Log(conf.Gun_num,"===========Pay===========\n");
        HuiSuoFlag=0;
        if(RequestPayFlag==1) EpsBackDataDispose();//֮ǰ�յ�������֧�����
    }
    else if((Uart_Rx_Buff[33]==0x02)&&
        (Uart_Rx_Buff[34]==0x00)&&
        (Uart_Rx_Buff[35]==0x05))//����֪ͨ���׽��
    {
        P_Log(conf.Gun_num,"===========Over===========\n");
        para.VOL=0;//������֪ͨ���׽������Ϊһ�������Ľ���
        BoartRequestFlag=0;
        if(OverTransFlag==1) EpsBackDataDispose();//֮ǰ�յ���֪ͨ���׽��
    }
    while(i < EPS_Data_Len)
    {
        Uart_To_EPS[i] = Uart_Rx_Buff[i + 7];
        i++;
    }
    Uart_To_EPS_Flag = 1;
}



/********************************************************************\
* ������: Dispose_Recv72
* ˵��:
* ����:    �������ظ�ֹͣ��������
* ����:    ��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2015-06-04
\*********************************************************************/
void Dispose_Recv72()
{
    P_Log(conf.Gun_num,"===========Dispose_Recv72 Recv===========\n");
    if(Uart_Rx_Buff[7] == 0)   //ֹͣ�������ɹ�
    {
        P_Log(conf.Gun_num,"Stop Dispenser Success\n");
        Reply_FS_Stop_OPT();
    }
    else
        P_Log(conf.Gun_num,"Stop Dispenser Failure\n");
    StartFuelling=0;
}

/********************************************************************\
* ������: Dispose_Recv73
* ˵��:
* ����:    �������ϴ�����
* ����:    ��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2015-06-12
\*********************************************************************/
void Dispose_Recv73()
{
//    unsigned char   FS_IP[4];           //FS IP��ַ4
//    unsigned int    FS_Port;            //FS �˿ں�6
//    unsigned char   FS_NODE[2];         //FS�ڵ�8
//    unsigned char   EPS_IP[4];          //EPS IP��ַ12
//    unsigned int    EPS_Port;           //EPS�˿ں�14
//    unsigned char   Local_NODE[2];      //���ؽڵ�16
//    unsigned char   Local_IP[4];        //���� IP ��ַ 20
//    unsigned char   Local_Mask[4];      //��������24
//    unsigned char   Local_Gateway[4];   //��������28
//    unsigned char   Local_Broad_IP[4];  //���ع㲥 IP ��ַ 32
//    unsigned int    Local_Broad_Port;   //���ع㲥 IP �˿�34

    Uint    tmp_port;
    Uchar   i,reboot_flag=0,tt=7;
    char    Tmp[6],dst[20];

    P_Log(conf.Gun_num,"===========Dispose_Recv73 Recv===========\n");

    //FS IP��ַ
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[tt+3-i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[tt+3-i]);
        strcat(dst, Tmp);
    }
    if(strcmp(conf.FS_IP,dst)!=0)
    {
        bzero(conf.FS_IP,sizeof(conf.FS_IP));
        strncpy(conf.FS_IP,dst,strlen(dst));
        iniSetString("FS_EPS", "FS_IP", conf.FS_IP);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"FS_IP=%s\n", dst);

    //fs port
    bzero(Tmp, 6);
    tmp_port=(Uint)(Uart_Rx_Buff[tt+5])<<8|Uart_Rx_Buff[tt+4];
    if(conf.FS_Port!=tmp_port)
    {
        conf.FS_Port=tmp_port;
        iniSetInt("FS_EPS", "FS_Port", conf.FS_Port, 10);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"FS_Port=%d\n",tmp_port);

    //FS NODE
    bzero(Tmp,6);
    sprintf(Tmp,"%02d.%02d",Uart_Rx_Buff[tt+7],Uart_Rx_Buff[tt+6]);
    if(strncmp(Tmp,conf.IFSF_Node,5)!=0) //��ȡFS�Ľڵ�
    {
        strcpy(conf.IFSF_Node,Tmp);
        iniSetString("FS_EPS", "IFSF_Node", conf.IFSF_Node);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"IFSF_Node=%s\n", Tmp);

    // unsigned char   EPS_IP[4]
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[tt+8+3-i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[tt+8+3-i]);
        strcat(dst, Tmp);
    }
    if(strcmp(conf.EPS_IP,dst)!=0)
    {
        bzero(conf.EPS_IP,sizeof(conf.EPS_IP));
        strncpy(conf.EPS_IP,dst,strlen(dst));
        iniSetString("FS_EPS", "EPS_IP", conf.EPS_IP);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"EPS_IP=%s\n", dst);

    //unsigned int    EPS_Port
    bzero(Tmp, 6);
    tmp_port=(Uint)(Uart_Rx_Buff[tt+13])<<8|Uart_Rx_Buff[tt+12];
    sprintf(Tmp,"%d",tmp_port);
    if(conf.EPS_Port!=tmp_port)
    {
        conf.EPS_Port=tmp_port;
        iniSetInt("FS_EPS", "EPS_Port", conf.EPS_Port, 10);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"EPS_Port=%d\n",tmp_port);

    //unsigned char   Local_NODE[2];
    bzero(Tmp,6);
    sprintf(Tmp,"%02d.%02d",Uart_Rx_Buff[tt+15],Uart_Rx_Buff[tt+14]);
    if(strncmp(Tmp,conf.My_Node,5)!=0) //��ȡFS�Ľڵ�
    {
        strcpy(conf.My_Node,Tmp);
        iniSetString(secname, "My_Node", conf.My_Node);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"My_Node=%s\n", Tmp);

    //unsigned char   Local_IP[4];
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[tt+16+3-i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[tt+16+3-i]);
        strcat(dst, Tmp);
    }
    if(strcmp(conf.My_IP,dst)!=0)
    {
        bzero(conf.My_IP,sizeof(conf.My_IP));
        strncpy(conf.My_IP,dst,strlen(dst));
        iniSetString("Board_Info", "My_IP", conf.My_IP);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"My_IP=%s\n", dst);

    // unsigned char   Local_Mask[4]
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[tt+20+3-i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[tt+20+3-i]);
        strcat(dst, Tmp);
    }
    if(strcmp(conf.My_Mask,dst)!=0)
    {
        bzero(conf.My_Mask,sizeof(conf.My_Mask));
        strncpy(conf.My_Mask,dst,strlen(dst));
        iniSetString("Board_Info", "My_Mask", conf.My_Mask);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"My_Mask=%s\n", dst);

    // unsigned char   Local_Gateway[4]
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[tt+24+3-i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[tt+24+3-i]);
        strcat(dst, Tmp);
    }
    if(strcmp(conf.My_Gateway,dst)!=0)
    {
        bzero(conf.My_Gateway,sizeof(conf.My_Gateway));
        strncpy(conf.My_Gateway,dst,strlen(dst));
        iniSetString("Board_Info", "My_Gateway", conf.My_Gateway);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"My_Gateway=%s\n", dst);

    //unsigned char   Local_Broad_IP[4]
    bzero(dst, 20);
    for(i=0; i<4; i++)
    {
        bzero(Tmp, 6);
        if(i<3)
            sprintf(Tmp, "%d.", Uart_Rx_Buff[tt+28+3-i]);
        else if(i==3)
            sprintf(Tmp, "%d", Uart_Rx_Buff[tt+28+3-i]);
        strcat(dst, Tmp);
    }
    if(strcmp((const char *)conf.My_Broadcast_IP,(const char *)dst)!=0)
    {
        bzero(conf.My_Broadcast_IP,sizeof(conf.My_Broadcast_IP));
        strncpy(conf.My_Broadcast_IP,dst,strlen(dst));
        iniSetString("Board_Info", "My_Broadcast_IP", (const char *)conf.My_Broadcast_IP);       
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"My_Broadcast_IP=%s\n", dst);

    //unsigned int    Local_Broad_Port
    bzero(Tmp, 6);
    tmp_port=(Uint)(Uart_Rx_Buff[tt+33])<<8|Uart_Rx_Buff[tt+32];
    if(conf.My_Broadcast_Port!=tmp_port)
    {
        conf.My_Broadcast_Port=tmp_port;
        iniSetInt("Board_Info", "My_Broadcast_Port", conf.My_Broadcast_Port, 10);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"My_Broadcast_Port=%d\n",tmp_port);

    //gun_num
    tmp_port = Uart_Rx_Buff[tt+34];

    if(conf.Gun_num!=tmp_port)
    {
        para.NZN = tmp_port;
        conf.Gun_num = tmp_port;
        bzero(Tmp, 6);
        sprintf((char *)Tmp, "%d", tmp_port);
        iniSetInt((const char *)secname, "Gun_num", conf.Gun_num, 10);
        reboot_flag=1;
    }
    P_Log(conf.Gun_num,"Gun_num=%d\n",tmp_port);

    CmdFlag=0x73;//�ظ��������ϴ�����
    if(reboot_flag==1)
    {
        sleep(2);
        system("reboot");
    }
}
/********************************************************************\
* ������: Deal_Uart_Recv_Data
* ˵��:     �����ڽ��յ�����(�������͵�FS����EPS)
* ����:     �������ݴ���
* ����:     ��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-01
\*********************************************************************/
void Deal_Uart_Recv_Data()
{
    Ushort len,crc,recv_crc;   
    if((OPT_IS_Online==0)&&(Uart_Rx_Buff[3]==0x19))//�ѻ�����ʱֱ�ӷ���
    {
        return;
    }
    OPT_IS_Online=conf.OPT_IS_Online;
    len = Uart_Rx_Buff[6] + 11;//��Ϣ�ܳ���
    if(len != Uart_Data_Len)
    {
        P_Log(conf.Gun_num,"Data recv length error! Uart Recv len=%d,OPT send len=%d\n", Uart_Data_Len, len);
    }
    crc = ModbusCrc16(Uart_Rx_Buff + 2, len - 6);//����CRC
    recv_crc = ((Ushort)Uart_Rx_Buff[len - 4]<<8 )| Uart_Rx_Buff[len - 3];//�յ���CRC

    if((crc != recv_crc)||(len != Uart_Data_Len))
    {
        P_Log(conf.Gun_num,"Data recv CRC error! Uart Recv CRC=%04X,Local CRC=%04X\n", recv_crc, crc);        
    }
    else
    {
        switch(Uart_Rx_Buff[4])
        {
            case 0x51:
                Dispose_Recv51();
                break; 
            case 0x52:
                Dispose_Recv52();
                break;
            case 0x57:
                Dispose_Recv57();
                break; 
            case 0x5E:
                Dispose_Recv5E();
                break;
            case 0x70:
                Dispose_Recv70();
                break;
            case 0x71:
                Dispose_Recv71();
                break; 
            case 0x72:
                Dispose_Recv72();
                break;
            case 0x73:
                Dispose_Recv73();
                break;
            default:
                break;
        }
    }

}


/*��ʼʱ��������״̬20*/
void Reply_FS_ReadStatus()
{
    //01.03.02.01.00.0E.00.03.01.20.14.
    //02.01.01.03.00.2E.00.05.01.21.14.01.02.02.01.01.03.00.EE.00.03.01.20.00.
    Uchar tt = 7;
    Uart_To_FS[tt++] = 0x05;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x14;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = para.ST;                     //������״̬
    fp_id.FP_State = para.ST;
    Uart_To_FS[tt++] = FS_To_Uart[2];
    Uart_To_FS[tt++] = FS_To_Uart[3];
    Uart_To_FS[tt++] = FS_To_Uart[0];
    Uart_To_FS[tt++] = FS_To_Uart[1];
    Uart_To_FS[tt++] = 0x0;
    Uart_To_FS[tt++] = (0xE0 | (0x1F & FS_To_Uart[5]));
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x03;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x20;
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}


/*��������״̬�Ƿ�ı�*/
void Reply_FS_StaIsChanged()
{
    Uchar i, tt = 7;
    //01.03.02.01.00.1E.00.03.01.21.14.
    //02.01.01.03.00.3E.00.05.01.21.14.01.02 ״̬δ�ı�
    /*
       ״̬�ı�
       02.01.01.03.00.80.00.18.01.21.
       64.00.14.01.06.15.01.01.16.02.00.00.50.08.00.00.00.00.00.00.00.00
     */
//    if(HaveAuth==1)
//    {
//        para.ST=5;
//    }
    if((para.ST == 0x03) || (para.ST == 0x05)||(para.ST == 0x02))   //״̬δ�ı�
    {
        Uart_To_FS[tt++] = 0x05;                        //���ȵ�λ
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x14;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = para.ST;                     //������״̬
    }
    else     //״̬�ı�
    {
        Uart_To_FS[tt++] = 0x18;                        //���ȵ�λ
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x64;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x14;                        //������״̬ID
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = para.ST;                     //������״̬
        fp_id.FP_State = para.ST;
        Uart_To_FS[tt++] = 0x15;                        //��ǹ״̬ID
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x01;                        //��ǹ״̬
        Uart_To_FS[tt++] = 0x16;                        //fp������ID
        Uart_To_FS[tt++] = 0x02;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x50;
        Uart_To_FS[tt++] = 0x08;
        for(i = 0; i < 8; i++)
        {
            Uart_To_FS[tt++] = 0x00;
        }
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*�����ϴ�״̬*/
void Reply_FS_UploadStatus(Uchar st)
{
    Uchar i, tt = 0;
    if((conf.MY_NODE[0]==0)&&(conf.MY_NODE[1]==0))
        return;
    //02.01.01.03.00.80.00.18.01.21.64.00.14.01.03.15.01.00.16.02.00.00.50.08.00.00.00.00.00.00.00.00
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x18;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //������״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = st;                          //������״̬
    Uart_To_FS[tt++] = 0x15;                        //��ǹ״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;                        //��ǹ״̬
    Uart_To_FS[tt++] = 0x16;                        //fp������ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*�����ϴ�5,6,8״̬*/
void Reply_FS_Upload568Status()
{
    Uchar i, tt = 0;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x18;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //������״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x05;                        //������״̬
    Uart_To_FS[tt++] = 0x15;                        //��ǹ״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;                        //��ǹ״̬
    Uart_To_FS[tt++] = 0x16;                        //fp������ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x18;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //������״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x06;                        //������״̬
    Uart_To_FS[tt++] = 0x15;                        //��ǹ״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;                        //��ǹ״̬
    Uart_To_FS[tt++] = 0x16;                        //fp������ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x18;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //������״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x08;                        //������״̬
    Uart_To_FS[tt++] = 0x15;                        //��ǹ״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;                        //��ǹ״̬
    Uart_To_FS[tt++] = 0x16;                        //fp������ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }   
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}


/*ѡ����Ʒ��ַ*/
void Reply_FS_SelectOilAddr()
{
    //01.03.02.01.00.5A.00.05.01.81.04.01.01.    01��Ӧ41H...08��Ӧ48H
    //02.01.01.03.00.FA.00.05.01.81.05.04.02.
    Uchar tt = 7;
    if(c_dat.Auth_State_Mode == 1)   //ѡ����
    {
        Uart_To_FS[tt++] = 0x05;                        //���ȵ�λ
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = FS_To_Uart[9];
        Uart_To_FS[tt++] = 0x05;
        Uart_To_FS[tt++] = 0x04;
        Uart_To_FS[tt++] = 0x02;
    }
    else
    {
        //0201010100FC0003018100
        if(FS_To_Uart[10] == 0x01)
        {
            m_id.Meter_Type = FS_To_Uart[12];
        }
        else if(FS_To_Uart[10] == 0x04)
        {
            m_id.PR_Id = FS_To_Uart[12]; //1��Ӧ41H...8��Ӧ48H
        }
        Uart_To_FS[tt++] = 0x03;                        //���ȵ�λ
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = FS_To_Uart[9];
        Uart_To_FS[tt++] = 0x00;
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*дC_DAT���ݿ��е�0B��16��17*/
void Reply_FS_Write0B1617()
{
    //01.03.02.01.00.5F.00.0B.01.01.0B.01.00.16.01.00.17.01.00.
    //02.01.01.03.00.FF.00.09.01.01.05.0B.00.16.02.17.02.
    //02.01.01.01.00.E2.00.03.01.01.00
    Uchar tt = 7;
    if(c_dat.Auth_State_Mode == 1)
    {
        Uart_To_FS[tt++] = 0x09;                        //���ȵ�λ
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x05;                        //��Ϣȷ��״̬
        Uart_To_FS[tt++] = 0x0B;
        Uart_To_FS[tt++] = c_dat.Auth_State_Mode;
        Uart_To_FS[tt++] = 0x16;
        Uart_To_FS[tt++] = c_dat.Min_Fuelling_Vol;
        Uart_To_FS[tt++] = 0x17;
        Uart_To_FS[tt++] = c_dat.Min_Display_Vol;
    }
    else
    {
        c_dat.Auth_State_Mode = FS_To_Uart[12];
        c_dat.Min_Fuelling_Vol = FS_To_Uart[15];
        c_dat.Min_Display_Vol = FS_To_Uart[18];
        Uart_To_FS[tt++] = 0x03;                        //���ȵ�λ
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = 0x00;                        //��Ϣȷ��״̬
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*���ۼ�*/
void Reply_FS_ReadTotal()
{
    Uchar i,tt = 0;
    //01.04.02.01.00.1A.00.05.02.21.10.14.15.
    //02.01.01.04.00.3A.00.04.02.21.10.00.
    //02.01.01.04.00.3A.00.15.02.21.11.14.07.0A.00.00.02.12.64.33.15.07.0A.00.00.10.57.93.64.
    //�ظ�10
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Recv57_Flag;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x04;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x10;
    Uart_To_FS[tt++] = 0x00;
    //�ظ�14,15
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Recv57_Flag;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x15;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x11;
    Uart_To_FS[tt++] = 0x14;
    Uart_To_FS[tt++] = 0x07;
    HEXtoFsData_New(para.V_TOT, 2, 0, 6);
    for(i = 0; i < 7; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0x15;
    Uart_To_FS[tt++] = 0x07;
    HEXtoFsData_New(para.V_AMN, 2, 0, 6);
    for(i = 0; i < 7; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*
**��FP�ж�ȡδ֧������
flag=1������¼
flag=0�ѻ���¼
*/
void Reply_FS_ReadNO_Pay_Trans(Uchar flag)
{
    /* //zsy ����         
            01.04.02.01.00.1C.00.0E.04.21.21.36.94.05.06.07.08.0A.CC.CD.EA.EB
            02.01.01.04.00.3C.00.40.
            04.21.21.36.94.
            05.05.06.00.00.03.63.
            06.05.06.00.00.01.21.
            07.04.04.00.03.00.
            08.01.01.
            0A.04.00.00.00.03.
            CC.07.0A.00.00.02.12.22.84.//ǰ����
            CD.07.0A.00.00.02.12.24.05.
            EA.04.04.00.00.24.
            EB.04.04.00.00.24. 
            //zsy �ѻ�        
            //01.04.02.01.00.05.00.0F.04.20.21.00.00.C8.05.06.07.08.0A.CA.CB.EA.EB. 
            02.01.01.04.00.25.00.3B.
            04.21.21.36.99.
            C8.00.
            05.05.06.00.00.12.00.
            06.05.06.00.00.02.00.
            07.04.04.00.06.00.
            08.01.01.
            0A.04.00.00.00.03.
            CA.04.20.17.11.24.
            CB.03.14.45.43.
            EA.04.04.00.00.24.
            EB.04.04.00.00.24. 
            */
    Uchar i, tt = 0;
   
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0;                               //��Ϣ����
    Uart_To_FS[tt++] = Deal_Status_Code(Status_Code); //��Ϣ״̬��
    Uart_To_FS[tt++] = 0;                               //���ȸ�λ

    if(flag==0)//�ѻ���¼������
        Uart_To_FS[tt++] = 0x3B;    //����tt=7
    else
        Uart_To_FS[tt++] = 0x40;    //����tt=7
    Uart_To_FS[tt++] = 0x04;    //���ݿ����tt=8
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x21;
    HEXtoBCD(para.POS_TTC, 2);
    Uart_To_FS[tt++] = HEX_BCD[0];
    Uart_To_FS[tt++] = HEX_BCD[1];

    if(flag==0)//�ѻ���¼������
    {
        Uart_To_FS[tt++] = 0xC8;
        Uart_To_FS[tt++] = 0x00;
    }
    
    Uart_To_FS[tt++] = 0x05;    //���׽��
    Uart_To_FS[tt++] = 0x05;
    HEXtoFsData_New(para.AMN, 2, 0, 4);
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    
    Uart_To_FS[tt++] = 0x06;    //�������
    Uart_To_FS[tt++] = 0x05;
    HEXtoFsData_New(para.VOL, 2, 0, 4);
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    
    Uart_To_FS[tt++] = 0x07;    //���׵���
    Uart_To_FS[tt++] = 0x04;
    HEXtoFsData_New(para.PRC, 2, 0, 3);
    for(i = 0; i < 4; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0x08;    //����ǹ��
    Uart_To_FS[tt++] = 0x01;
    //Uart_To_FS[tt++] = para.NZN;//ǹ�Ŵ���17ʱ������
    Uart_To_FS[tt++] = 0x01;
    
    Uart_To_FS[tt++] = 0x0A;    //������Ʒ���� ��������ֵ
    Uart_To_FS[tt++] = 0x04;
    for(i = 0; i < 4; i++)
    {
        Uart_To_FS[tt++] = price.O_TYPE[i];
    }
    
    if(flag==0)//�ѻ���¼������
    {
        Uart_To_FS[tt++] = 0xCA;    //��������
        Uart_To_FS[tt++] = 0x04;
        for(i = 0; i < 4; i++)
        {
            Uart_To_FS[tt++] = para.TIME[i];
        }
        Uart_To_FS[tt++] = 0xCB;    //����ʱ��
        Uart_To_FS[tt++] = 0x03;
        for(i = 4; i < 7; i++)
        {
            Uart_To_FS[tt++] = para.TIME[i];
        } 
    }
    else//������¼
    {
        Uart_To_FS[tt++] = 0xCC;    //����ǰ����
        Uart_To_FS[tt++] = 0x07;
        HEXtoFsData_New(para.V_TOT-para.VOL, 2, 0, 6);
        for(i = 0; i < 7; i++)
        {
            Uart_To_FS[tt++] = HEX_FsData[i];
        }
        
        Uart_To_FS[tt++] = 0xCD;    //���׺�����
        Uart_To_FS[tt++] = 0x07;
        HEXtoFsData_New(para.V_TOT, 2, 0, 6);
        for(i = 0; i < 7; i++)
        {
            Uart_To_FS[tt++] = HEX_FsData[i];
        }
    }
    
    Uart_To_FS[tt++] = 0xEA;
    Uart_To_FS[tt++] = 0x04;
    HEXtoFsData_New(para.STAR_PRE, 2, 0, 3);
    for(i = 0; i < 4; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0xEB;
    Uart_To_FS[tt++] = 0x04;
    HEXtoFsData_New(para.STOP_PRE, 2, 0, 3);
    for(i = 0; i < 4; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;        
}

/*д��ǹ���ݿ�*/
void Reply_FS_WriteOilDataBase()
{
    Uchar tt = 7;
    //01.03.02.01.00.5B.00.09.02.21.11.01.01.01.07.01.01.
    //02.01.01.03.00.FB.00.08.02.21.11.05.01.00.07.02.�ܾ�
    //02.01.01.03.00.FB.00.05.02.21.11.00 ����
    fp_id_ln_id.Meter_1_Id=0;
    if(fp_id_ln_id.Meter_1_Id != 0)
    {
        Uart_To_FS[tt++] = 0x08;                        //���ȵ�λ
        Uart_To_FS[tt++] = 0x02;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x11;
        Uart_To_FS[tt++] = 0x05;
        Uart_To_FS[tt++] = 0x01;
        Uart_To_FS[tt++] = fp_id_ln_id.PR_Id;
        Uart_To_FS[tt++] = 0x07;
        Uart_To_FS[tt++] = fp_id_ln_id.Meter_1_Id;
    }
    else
    {
        fp_id_ln_id.PR_Id = FS_To_Uart[13];
        fp_id_ln_id.Meter_1_Id = FS_To_Uart[16];
        Uart_To_FS[tt++] = 0x04;                        //���ȵ�λ
        Uart_To_FS[tt++] = 0x02;
        Uart_To_FS[tt++] = 0x21;
        Uart_To_FS[tt++] = 0x11;
        Uart_To_FS[tt++] = 0x00;
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;

}

/*��һ���رյ�FP*/
void Reply_FS_OpenFP()
{
    Uchar i, len = 0, tt = 7;
    len = FS_To_Uart[8];
    Uart_To_FS[tt++] = 2 + len;                     //���ȵ�λ
    Uart_To_FS[tt++] = len;
    for(i = 0; i < len; i++)
    {
        Uart_To_FS[tt++] = FS_To_Uart[9 + i];
    }
    Uart_To_FS[tt++] = 0x00;
    
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x18;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //������״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x03;                        //������״̬
    Uart_To_FS[tt++] = 0x15;                        //��ǹ״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x00;                        //��ǹ״̬
    Uart_To_FS[tt++] = 0x16;                        //fp������ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }
   
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}


/*����ָ����Ʒ\����ģʽ�µĵ���*/
void Reply_FS_DownLoad_Price()
{
    //01.03.02.01.00.43.00.0D.06.61.00.00.00.08.11.02.04.04.00.04.90.
    //02.01.01.03.00.E3.00.08.06.61.00.00.00.08.11.00.
    //01.0B.02.01.00.47.00.0D.06.61.00.00.00.04.11.02.04.04.00.09.00.
    Uchar i, j, tt = 7, tmp[20];
    PRICE pri;  
    Uart_To_FS[tt++] = 0x08;                        //���ȵ�λ
    for(i = 0; i < 7; i++)
    {
        Uart_To_FS[tt++] = FS_To_Uart[8 + i];
    }
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
    //�õ�FS����buf
    bzero(tmp, 20);
    for(i = 0; i < 4; i++)
    {
        tmp[i] = FS_To_Uart[17 + i];
    }
    //��fs����תΪ������
    j = FS_To_Uart[14] & 0x0F;
    pri.Price[j - 1] = FSDataToLong(4, tmp);
    P_Log(conf.Gun_num,"===========price[%d]=%d============\n", j - 1, pri.Price[j - 1]);
    //if(pri.Price[j - 1] != price.Price[j - 1])
    //{        
    price.Price[j - 1] = pri.Price[j - 1];   
    price.VER++;//�ͼ۰汾�ż�1
    if(price.VER > 200) price.VER = 1;
    conf.Pri_Ver = price.VER;
    para.PRC_VER = price.VER;        
    shared->pricever[Gun_num-1] = para.PRC_VER;
    shared->price[Gun_num-1] = price.Price[j - 1];
    //}

    Time_Process();
    for(i = 0; i < 6; i++)              //FSCʱ��
    {
        price.V_D_T[i] = para.FSC_TIME[i];
    }
    price.FIE_NU = 1;                   //�ͼۼ�¼��
    price.NZN = para.NZN;
    price.PRC_N = 8;
}

/*�ظ�fs��۳ɹ�*/
void Reply_FS_DownLoad_Price_Success()
{
    Uchar tt = 0;
    /*02.01.01.01.00.80.00.0C.03.21.41.99.64.00.01.01.99.05.01.02*/
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;                //���ȸ�λ
    Uart_To_FS[tt++] = 0x0C;
    Uart_To_FS[tt++] = 0x03;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x41;
    Uart_To_FS[tt++] = 0x99;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x99;
    Uart_To_FS[tt++] = 0x05;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = para.ST;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*�ϴ�ʵʱ����*/
void Reply_FS_UploadRealtimeData()
{
    Uchar tt = 0, i;
    /*
    02.01.01.03.00.80.00.12.01.21.
    66.00.
    22.05.06.00.00.01.41.
    23.05.06.00.00.00.17.
    */
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;                //���ȸ�λ
    Uart_To_FS[tt++] = 0x12;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x66;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x22;
    HEXtoFsData_New(para.AMN, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0x23;
    HEXtoFsData_New(para.VOL, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*�ϴ����׼�¼*/
void Reply_FS_UploadOverFuel()
{
    Uchar i, tt = 0;
    /*
    //�������
    02.01.01.03.00.80.00.20.04.21.21.29.97.
    64.00.
    01.02.29.97.
    15.01.02.
    14.02.00.00.
    05.05.06.00.00.71.13.
    06.05.06.00.00.08.57
    */
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;                //���ȸ�λ
    Uart_To_FS[tt++] = 0x20;
    Uart_To_FS[tt++] = 0x04;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x21;
    HEXtoBCD(para.POS_TTC, 2);
    Uart_To_FS[tt++] = HEX_BCD[0];
    Uart_To_FS[tt++] = HEX_BCD[1];
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = HEX_BCD[0];
    Uart_To_FS[tt++] = HEX_BCD[1];
    Uart_To_FS[tt++] = 0x15;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x02;        //����״̬��������Ҫ�޸�
    Uart_To_FS[tt++] = 0x14;
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x05;
    P_Log(conf.Gun_num,"==============AMN=%d===============\n", para.AMN);
    HEXtoFsData_New(para.AMN, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    Uart_To_FS[tt++] = 0x06;
    P_Log(conf.Gun_num,"===============VOL=%d===============\n", para.VOL);
    HEXtoFsData_New(para.VOL, 2, 0, 4);
    Uart_To_FS[tt++] = 0x05;
    for(i = 0; i < 5; i++)
    {
        Uart_To_FS[tt++] = HEX_FsData[i];
    }
    
    //�ϴ�����״̬
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x80;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x18;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x64;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x14;                        //������״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x03;                        //������״̬
    Uart_To_FS[tt++] = 0x15;                        //��ǹ״̬ID
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x01;                        //��ǹ״̬
    Uart_To_FS[tt++] = 0x16;                        //fp������ID
    Uart_To_FS[tt++] = 0x02;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x50;
    Uart_To_FS[tt++] = 0x08;
    for(i = 0; i < 8; i++)
    {
        Uart_To_FS[tt++] = 0x00;
    }
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;    
}

/*��Ȩ������*/
void Reply_FS_Auth_OPT()
{
    //01.0B.02.01.00.47.00.08.01.21.1E.02.02.01.3E.00  //�޿�������Ȩ
    //01.03.02.01.00.47.00.0B.01.21.1E.02.02.01.19.01.01.3E.00 //�п���������Ȩ
    //02.01.01.04.00.F1.00.03.01.21.00
    Uchar tt = 0;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Recv70_Flag;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x03;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*�ܾ���Ȩ������*/
void Reply_FS_CantAuth_OPT()
{
    //01.03.02.01.00.4C.00.12.01.21.1B.05.05.00.10.00.00.1E.02.02.01.19.01.01.3E.00
    //02.01.01.03.00.EC.00.0B.01.21.05.1B.00.1E.00.19.00.3E.03
    Uchar tt = 0;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Recv70_Flag;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x0B;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x05;
    Uart_To_FS[tt++] = 0x1B;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x1E;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x19;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x3E;
    Uart_To_FS[tt++] = 0x03;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
    statusflag=3;
}


/*ֹͣ������*/
void Reply_FS_Stop_OPT()
{
    //01.03.02.01.00.5B.00.04.01.21.40.00
    //02.01.01.03.00.FB.00.03.01.21.00
    Uchar tt = 0;
    Uart_To_FS[tt++] = conf.FS_NODE[0];
    Uart_To_FS[tt++] = conf.FS_NODE[1];
    Uart_To_FS[tt++] = conf.MY_NODE[0];
    Uart_To_FS[tt++] = conf.MY_NODE[1];
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = Recv72_Flag;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x03;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*������׼�¼*/
void Reply_FS_ClearTran()
{
    //01.09.02.01.00.5B.00.07.04.21.21.16.21.1E.00. 
    Uchar tt = 7;
    Uart_To_FS[tt++] = 0x06;
    Uart_To_FS[tt++] = 0x04;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = FS_To_Uart[11];
    Uart_To_FS[tt++] = FS_To_Uart[12];
    Uart_To_FS[tt++] = 0x00;

    para.VOL=0;//������׺������㣬��ֹ�ڶ��δ��˵�һ�ε�ֵ
    para.AMN=0;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/*Ĭ�ϻظ�fuel��������Ϣ*/
void Reply_FS_OtherMsg()
{
    //01.02.02.01.00.44.00.04.01.21.3D.00
    //02.01.01.02.00.E4.00.03.01.21.00
    //01.03.02.01.00.46.00.09.02.21.11.01.01.01.07.01.01    
    //02.01.01.03.00.E6.00.04.02.21.11.00
    Uchar i, len = 0, tt = 7;  
    len = FS_To_Uart[8];
    Uart_To_FS[tt++] = 2 + len;                     //���ȵ�λ
    Uart_To_FS[tt++] = len;
    for(i = 0; i < len; i++)
    {
        Uart_To_FS[tt++] = FS_To_Uart[9 + i];
    }
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

//����ͨѶ���ݿ�1
void Reply_FS_DownloadComm1()
{
    //01.03.02.01.02.4B.00.06.01.00.0B.02.02.01.
    //02.01.01.03.00.EB.00.03.01.00.00
    Uchar tt = 7;
    FS_OK=2;
    para.VOL=0;//����ʱ����
    HaveAuth=0;//��������ʱ���֮ǰ��������Ȩ�ı�ʶ
    ReadNoPayFlag=0;//����֮ǰ���ܴ��ڵĶ�ȡδ֧�����ױ��
    Uart_To_FS[tt++] = 0x03;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

//����ͨѶ���ݿ�2
void Reply_FS_DownloadComm2()
{
    //01.03.02.01.02.01.00.03.01.00.04
    //02.01.01.03.00.21.00.05.01.00.04.01.0A.
    Uchar tt = 7;
    FS_OK=2;
    para.VOL=0;//����ʱ����
    HaveAuth=0;//��������ʱ���֮ǰ��������Ȩ�ı�ʶ
    ReadNoPayFlag=0;//����֮ǰ���ܴ��ڵĶ�ȡδ֧�����ױ��
    Uart_To_FS[tt++] = 0x05;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x00;
    Uart_To_FS[tt++] = 0x04;
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x0A;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

//�����������Ʒ����
void Reply_FS_DownloadOilCode()
{
    //01.03.02.01.00.51.00.08.01.41.02.04.00.00.00.00. (41-48)
    //02.01.01.03.00.F1.00.03.01.41.00.
    Uchar i,tt = 7;
    char tmp[9];
    if(FS_To_Uart[15] != 0)   //������Ʒ����
    {
        for(i = 0; i < 4; i++)
        {
            price.O_TYPE[i] = FS_To_Uart[12 + i];
        }
        sprintf(tmp,"%02X%02X%02X%02X",price.O_TYPE[0],price.O_TYPE[1],price.O_TYPE[2],price.O_TYPE[3]);
        if(strncmp(tmp,(char *)conf.oilcode,8)!=0)//��Ʒ���벻��
        {
            for(i = 0; i < 4; i++)
            {
                shared->oilcode[Gun_num-1][i] = FS_To_Uart[12 + i];
            }
            shared->oilver[Gun_num-1] = 1;
        }
    }
    Uart_To_FS[tt++] = 0x03;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = FS_To_Uart[9];
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

//�´ν���ģʽ
void Reply_FS_NextMode()
{
    //01.03.02.01.00.5C.00.05.01.21.07.01.02.
    //02.01.01.03.00.FC.00.03.01.21.00.
    Uchar tt = 7;    
    fp_id.Default_Fuelling_Mode = FS_To_Uart[12];
    Uart_To_FS[tt++] = 0x03;                        //���ȵ�λ
    Uart_To_FS[tt++] = 0x01;
    Uart_To_FS[tt++] = 0x21;
    Uart_To_FS[tt++] = 0x00;
    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

//������Ȩ
void DealAuthOPT()
{
     /*
            01.0B.02.01.00.59.00.12.
            01.21.
            1B.05.06.00.99.00.00.
            19.01.01.
            1E.02.02.01.
            3E.00.    
            //�޿�������Ȩ
            01.0B.02.01.00.47.00.08.
            01.21.
            1E.02.02.01.
            3E.00  
            */
    Uchar i,tmp[10];;
    if(FS_To_Uart[10] != 0x1E)
    {
        if(FS_To_Uart[10] == 0x1B)
        {
            Auth_Type = 0;
        }
        else
        {
            Auth_Type = 1;
        }
        for(i = 0; i < FS_To_Uart[11]; i++)
        {
            tmp[i] = FS_To_Uart[12 + i];
        }
        Auth_Value = FSDataToLong(FS_To_Uart[11], tmp);
    }
    else
    {
        Auth_Type = 2; //������ֱ�Ӽ���
        Auth_Value = 0;
    }
    Recv70_Flag = Deal_Status_Code(FS_To_Uart[5]);  //��Ϣ״̬��
    CmdFlag=0x70;//fs��Ȩ������        
}

//����򿪡��رա�ֹͣ������
void DealOpenCloseStopOPT()
{
    if(FS_To_Uart[10] == 0x3C)      //��һ���رյ�FP
    {
        Reply_FS_OtherMsg();        
        FS_OK = 1;  
        if(HaveAuth==0)//δ��Ȩ���ϴ�3״̬
            statusflag=3;//�ڴ�һ��FPʱ������֮ǰ�Ѿ��ǿ���״̬������ı�һ��״̬��ȷ�������ϴ�3״̬
    }
    else if(FS_To_Uart[10] == 0x3D) //�ر�һ��FP
    {
        Reply_FS_OtherMsg();
        statusflag=2;
        FS_OK = 2;
    }
    else if((FS_To_Uart[10] == 0x40)||(FS_To_Uart[10] == 0x3F))     //ֹͣ����������
    {
        Recv72_Flag = Deal_Status_Code(FS_To_Uart[5]);  //��Ϣ״̬��
        CmdFlag=0x72;//�·�ֹͣ����������
    }
}

//�ظ��ѻ�����
void Reply_FS_OfflineTrans()
{
    //01.04.02.01.00.57.00.07.04.21.21.37.09.C9.00
    //02.01.01.04.00.F7.00.06.04.21.21.06.14.00 ���н���
    //02.01.01.04.00.E0.00.06.04.21.20.00.00.00 û�н���
    Uchar tt = 7;
    Status_Code = FS_To_Uart[5];      //��Ϣ״̬�ֱ���
    Uart_To_FS[tt++] = 0x06;
    Uart_To_FS[tt++] = 0x04;
    Uart_To_FS[tt++] = 0x21;    
    P_Log(conf.Gun_num,"===============NO_Payment_Trans=%d=============\n",para.NO_Payment_Trans);
    if(para.NO_Payment_Trans>0)//���н���
    {        
        ReadNoPayFlag=1;//��ʱ�����ٴζ�ȡ
        Status_Code &= 0x1F;  //�ı�FS״̬��Ϊ�����´��ϴ�����״̬�ֲŶ�
        Uart_To_FS[tt++] = 0x21;
        //Uart_To_FS[tt++] = 0x06;//���˾��ô˴�������Ӧ�ô���ˮ��
        //Uart_To_FS[tt++] = 0x14;
        Uart_To_FS[tt++] = FS_To_Uart[11];
        Uart_To_FS[tt++] = FS_To_Uart[12];
        Uart_To_FS[tt++] = 0x00;        
    }
    else //û�н���
    {
        ReadNoPayFlag=0;
        Uart_To_FS[tt++] = 0x20;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x00;
        Uart_To_FS[tt++] = 0x00;
    }

    FS_Data_Len = tt;
    Uart_To_FS_Flag = 1;
}

/********************************************************************\
* ������: Deal_FS_To_Uart_data
* ˵��:     ����FS���͸�������������
* ����:     FS���������ݴ���
* ����:     ��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-09-01
\*********************************************************************/
void Deal_FS_To_Uart_data()
{
    Uchar tt, data_len, database_len, data_base_name;
    tt = 0;
    Uart_To_FS[tt++] = FS_To_Uart[2];
    Uart_To_FS[tt++] = FS_To_Uart[3];
    Uart_To_FS[tt++] = FS_To_Uart[0];
    Uart_To_FS[tt++] = FS_To_Uart[1];
    Uart_To_FS[tt++] = 0;                               //��Ϣ����
    Uart_To_FS[tt++] = Deal_Status_Code(FS_To_Uart[5]); //��Ϣ״̬��    
    Uart_To_FS[tt++] = 0;                               //���ȸ�λ
    data_len = FS_To_Uart[7];                           //���ݳ���
    database_len = FS_To_Uart[8];                       //���ݿⳤ��
    data_base_name = FS_To_Uart[9];                     //��һ�����ݿ�����
    fp_id.FP_State = para.ST;
    conf.FS_NODE[0] = FS_To_Uart[2];
    conf.FS_NODE[1] = FS_To_Uart[3];

    if((data_len == 0x06) && 
        (database_len == 0x01) && 
        (data_base_name == 0))   //����ͨѶ���ݿ�1
    {
        Reply_FS_DownloadComm1();
    }
    else if((data_len == 0x03) && 
        (database_len == 0x01) && 
        (data_base_name == 0))   //����ͨѶ���ݿ�2
    {
        Reply_FS_DownloadComm2();
    }
    else if((data_len == 0x03) && 
        (database_len == 0x01) && 
        (data_base_name == 0x20))     //��������״̬
    {
        Reply_FS_ReadStatus();
    }
    else if((data_len == 0x03) && 
        (database_len == 0x01) && 
        (data_base_name == 0x21))     //��������״̬
    {
        Reply_FS_StaIsChanged();
    }
    else if((data_len == 0x08) && 
        (database_len == 0x01) &&
        (data_base_name > 0x40))     //�����������Ʒ����
    {
        Reply_FS_DownloadOilCode();
    }
    else if((data_len == 0x05) && 
        (database_len == 0x01) && 
        (data_base_name == 0x81))     //ѡ����Ʒ��ַ
    {
        Reply_FS_SelectOilAddr();
    }
    else if((data_len == 0x0B) && 
        (database_len == 0x01) && 
        (data_base_name == 0x01) && 
        (FS_To_Uart[10] == 0x0B))     //дC_DAT���ݿ��е�0B��16��17
    {
        Reply_FS_Write0B1617();
    }
    else if((data_len == 0x06) && 
        (database_len == 0x02) && 
        (data_base_name == 0x21) && 
        (FS_To_Uart[10] == 0x11)&& 
        (FS_To_Uart[11] == 0x01))     //������Ʒ
    {       
        Reply_FS_WriteOilDataBase();        
    }
    else if((data_len == 0x05) && 
        (database_len == 0x02) && 
        (data_base_name == 0x21) && 
        (FS_To_Uart[10] == 0x10)&& 
        (FS_To_Uart[11] == 0x14))     //���ۼ�
    {
        Recv57_Flag = Deal_Status_Code(FS_To_Uart[5]);  //��Ϣ״̬��
        CmdFlag=0x57;//���ۼ�
    }
    else if((data_len == 0x0E) &&
        (database_len == 0x04) && 
        (data_base_name == 0x21) && 
        (FS_To_Uart[10] == 0x21))  //���ߣ���FP�ж�ȡδ֧������
    {
        if((paraonline.T_TYPE!=0)||(paraonline.NO_Payment_Trans<2))//�Ѿ��ϴ����׺������fs��ȡ
        {           
            BoartRequestFlag=1;//����1ʱ�����忪ʼ����֧��
            Status_Code = FS_To_Uart[5];      //��Ϣ״̬�ֱ���
            para = paraonline;
            Reply_FS_ReadNO_Pay_Trans(1);
        }
    }
    else if((data_len == 0x0F) &&
        (database_len == 0x04) && 
        (data_base_name == 0x20) && 
        (FS_To_Uart[10] == 0x21))     //�ѻ�����FP�ж�ȡδ֧������
    {
        if((ReadNoPayFlag==0)&&(para.NO_Payment_Trans!=0))
        {
            Status_Code = FS_To_Uart[5];      //��Ϣ״̬�ֱ���
            ReadNoPayFlag=1;//��ʼ�������ȡ�ѻ����׼�¼
        }
        else if(para.NO_Payment_Trans==0)//�ϵ�ʱ��ȡ��û��δ֧������
        {
            Reply_FS_OtherMsg();
        }
    }
    else if((data_len == 0x09) && 
        (database_len == 0x02) && 
        (data_base_name == 0x21) && 
        (FS_To_Uart[10] == 0x11))     //д��ǹ���ݿ�
    {
        Reply_FS_WriteOilDataBase();
    }
    else if((data_len == 0x05) && 
        (database_len == 0x01) && 
        (data_base_name == 0x21) && 
        (FS_To_Uart[10] == 0x07))     //�´ν���ģʽ
    {
        Reply_FS_NextMode();
    }
    else if((data_len == 0x04) && 
        (database_len == 0x01) && 
        (data_base_name == 0x21))    //����򿪡��رա�ֹͣ������
    {
        DealOpenCloseStopOPT();
    }
    else if((data_len == 0x0D) && 
        (database_len == 0x06) && 
        (data_base_name == 0x61))     //���ص���
    {
        Reply_FS_DownLoad_Price();
    }
    else if((database_len == 0x01) && 
        (data_base_name == 0x21) && 
        ((FS_To_Uart[10] == 0x1B) || 
        (FS_To_Uart[10] == 0x1C) || 
        (FS_To_Uart[10] == 0x1E)))     //��Ȩ���/��Ȩ����/������ֱ�Ӽ���
    {
        DealAuthOPT();
    }
    else if((data_len == 0x07) && 
        (database_len == 0x04) && 
        (data_base_name == 0x21) && 
        (FS_To_Uart[10] == 0x21))     
    {
        if(FS_To_Uart[13]==0x1E)//������׼�¼
        {            
            uploadtransagain=0;
            Reply_FS_ClearTran();
        }
        else if(FS_To_Uart[13]==0xC9)//��ȡ�ѻ�����
        {
            Reply_FS_OfflineTrans();
        }
    }
    else
    {
        Reply_FS_OtherMsg();
    }
}

void Uart_Receive_Thread_Select()
{
    int nread, ret;
    char str[10];
    fd_set readfds;
    struct timeval timeout;
    timeout.tv_sec = 0;         //������
    timeout.tv_usec = 0;
    while(1)
    {
        FD_ZERO(&readfds);       //ÿ��ѭ����Ҫ��ռ��ϣ�select��������ļ����Ƿ��пɶ��ģ��Ӷ��ܼ���������仯
        FD_SET(Uart_fd, &readfds);
        ret = select(Uart_fd + 1, &readfds, NULL, NULL, &timeout); //select��������ļ����Ƿ��пɶ���
        usleep(60 * 1000); //�ȴ���������
        if(ret < 0)
        {
            P_Log(conf.Gun_num,"select error!\n");
        }
        //�����ļ����������ϣ�һֱ��ѭ���������������ı仯
        ret = FD_ISSET(Uart_fd, &readfds);
        if(ret > 0)
        {
            bzero(Uart_Rx_Buff, LENGTH);
            if((nread = read(Uart_fd, Uart_Rx_Buff, LENGTH)) > 0)   //��������
            {
                Uart_Data_Len = nread;
                sprintf(str,"U_Recv_%02X",Uart_Rx_Buff[4]);
                Print_HEX(str, nread, Uart_Rx_Buff);
                Lock(&Uart_Recv_Data);
                Deal_Uart_Recv_Data();
                Unlock(&Uart_Recv_Data);
            }
        }
    }
    return;
}



/********************************************************************\
* ��������: Uart_send_Thread
* ˵��:
* ����:     �������ݷ��ʹ���
* ����:     ��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��:  2014-8-22
\*********************************************************************/
void Uart_send_Thread(void)
{
    while(1)
    {
        if(Uart_Tx_Flag == 1)
        {
            Uart_Tx_Flag = 0;
            Lock(&Data_To_Uart);
            write(Uart_fd, Uart_Tx_Buff, Uart_Data_Len);
            bzero(Uart_Tx_Buff, LENGTH);
            Unlock(&Data_To_Uart);
        }
        usleep(10 * 1000);
    }
    return;
}


/********************************************************************\
* ������: Uart_Pthread_Creat
* ˵��:
* ����:  ���ڵĽ��ܺͷ����̴߳���
* ����:
* ����ֵ:   0:�ɹ���0:ʧ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-8-22
\*********************************************************************/
int Uart_Pthread_Creat()
{
    int err;
    pthread_t receiveid, send;
    err = pthread_create(&receiveid, NULL, (void*)Uart_Receive_Thread_Select, NULL); //���������߳�
    if(err != 0)
    {
        P_Log(conf.Gun_num,"can't create Uart_Receive thread thread: %s\n", strerror(err));
        return 1;
    }
    else
        P_Log(conf.Gun_num,"create Uart_Receive thread thread OK\n");
    err = pthread_create(&send, NULL, (void*)Uart_send_Thread, NULL); //���������߳�
    if(err != 0)
    {
        P_Log(conf.Gun_num,"can't create Uart_send thread: %s\n", strerror(err));
        return 1;
    }
    else
        P_Log(conf.Gun_num,"create Uart_send thread OK\n");

    return 0;
}
