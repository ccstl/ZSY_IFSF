#include "common.h"

#define CLOCKID CLOCK_REALTIME


void ShutDownServer()
{
    FS_OK=0;
    Cli_Connect_FS=0;
    Ser_Connect_FS=0;
    Reconnect_EPS_Flag=0;
    Reconnect_FS_Flag=0;
    Disconnect_EPS_Flag=1;
    if(Cli_FS_fd>0)
    {
        close(Cli_FS_fd);
        Cli_FS_fd=-1;
    }
    if(Ser_FS_fd>0)
    {
        close(Ser_FS_fd);
        Ser_FS_fd=-1;
    }
    if(Cli_EPS_fd>0)
    {
        close(Cli_EPS_fd);
        Cli_EPS_fd=-1;
    }
}

//eps��־����
void EpsFlagDispose()
{
    if(FaQiYanKaFlag==1)//�������鿨��ʱ�䣬��ʱ����������ǽ���־����
    {
        FaQiYanKaTime++;
        if(FaQiYanKaTime>YANKATIMEOUT)
        {
            FaQiYanKaFlag=0;
            FaQiYanKaTime=0;
        }
    }
    
    if(YanKaFlag==1)//�����鿨��ʱ�䣬��ʱ����������ǽ���־����
    {
        YanKaCount++;
        if(YanKaCount>10)
        {
            YanKaFlag=0;
            YanKaCount=0;
        }
    }
    else    YanKaCount=0;
    
    if(HuiSuoFlag==1)//���������ʱ�䣬��ʱ����������ǽ���־����
    {
        HuiSuoCount++;
        if(HuiSuoCount>10)
        {
            HuiSuoFlag=0;
            HuiSuoCount=0;
        }
    }
    else  HuiSuoCount=0;

    if(RequestPayFlag==1)//��������֧����ʱ�䣬��ʱ����������ǽ���־����
    {
        RequestPayCount++;
        if(RequestPayCount>10)
        {
            RequestPayFlag=0;
            RequestPayCount=0;
        }
    }
    else    RequestPayCount=0;
    
    if(OverTransFlag==1)//����֪ͨ���׵�ʱ�䣬��ʱ����������ǽ���־����
    {
        OverTransCount++;
        if(OverTransCount>10)
        {
            OverTransFlag=0;
            OverTransCount=0;
        }
    }
    else    OverTransCount=0;

    //����EPS�׽����쳣���
    //����EPS�������ڷ��������ݺ󶼻������ر�socket��
    //���Ե������쳣���û���յ�eps�ر�socket�����ʱ�򣬴�����
    if(Cli_EPS_fd>0)
    {
        EpsfdErrorCount++;
        if(EpsfdErrorCount>20) Cli_EPS_fd = -1;
    }
    else
        EpsfdErrorCount=0;
}

//ֻ���һ��ǹ���������
void OnlyGunOneDispose()
{
    Uchar i;
    char str[10],tmp[9];
    if(shared->FtoUflag==1) shared->errcount++;
    if(shared->errcount > 5)//ֻ�е�һ�����̼��� ���쳣�������������
    {
        shared->FtoUflag=0;
        shared->errcount=0;
    } 
    
    if((FSbacknum1>0)&&(shared->FtoUflag==0))//����FS�·���δ���ü����������
    {
        memset(shared->FS_To_Uart,0,LENGTH);
        memcpy(shared->FS_To_Uart,FSbackbuf1,FSbacknum1);                    
        shared->FtoUflag=1;
        shared->FtoUnum=FSbacknum1; 
        FSbacknum1=0;
    }
    else if((FSbacknum2>0)&&(shared->FtoUflag==0))//����FS�·���δ���ü����������
    {
        memset(shared->FS_To_Uart,0,LENGTH);
        memcpy(shared->FS_To_Uart,FSbackbuf2,FSbacknum2);                    
        shared->FtoUflag=1;
        shared->FtoUnum=FSbacknum2; 
        FSbacknum2=0;
    }
    
    for(i=0;i<4;i++)
    {
        if(shared->pricever[i] != 0)//���������ļ��еĵ��ۼ�¼
        {                           
            sprintf(str,"Gun_Info%d",i+1);
            P_Log(conf.Gun_num,"===========%s update price===========\n",str);
            iniSetInt((const char*)str, "Pri_Ver", shared->pricever[i], 10);
            iniSetInt((const char*)str, "price", shared->price[i], 10);
            shared->pricever[i]=0;
        }
        if(shared->oilver[i] != 0)//���������ļ��е���Ʒ����
        {            
            sprintf(str,"Gun_Info%d",i+1);            
            P_Log(conf.Gun_num,"===========%s update oilcode===========\n",str);
            sprintf(tmp,"%02X%02X%02X%02X",shared->oilcode[i][0],
                shared->oilcode[i][1],shared->oilcode[i][2],shared->oilcode[i][3]);
            iniSetString((const char*)str, "oilcode", (const char *)tmp);
            shared->oilver[i]=0;
        }
    }        
}


/********************************************************************\
* ������: timer_thread
* ˵��:
* ����:    ��ʱ������
* ����:
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2014-9-12
\*********************************************************************/
void timer_thread(union sigval v)
{
    static Uchar st=3;
    Time_100ms++;
    F0_Time++;
    if(Gun_num==1)//ֻ���һ��ǹ���������
    {
        OnlyGunOneDispose();
    }
    if(F0_Time%5==0) LedOnOff(1);
    
    if(((Time_S%conf.Time_Reconnect)==0)&&(Cli_Connect_FS==0)&&(FS_OK==1)&&(Reconnect_FS_Flag==0))
    {
        Reconnect_FS_Flag=1;
    }
    if(Time_100ms%10==0)
    {
        Time_S++;
        Check_Time++;
        udptimeoutcount++;
        if((uploadtransagain==1)&&(Stop_Opt==0))
        {
            uploadtransagaincount++;
            if((uploadtransagaincount%16==0)&&(uploadtransagaincount<50)) //��ഫ3��
            {
                para = paraonline;
                Reply_FS_UploadOverFuel(); // 16��δ�յ��彻�ף����ٴ��ϴ����׼�¼
            }
        }
        if(shared->TCPOK==0) FS_OK = 0;//TCP����������
        if(udptimeoutcount>30)//�����ϴ�udp����ʱ�䳬��30s����Shutdown Server
        {   
            FS_OK=0;
            ReadNoPayFlag=0;
            P_Log(conf.Gun_num,"===============Shutdown Server!=============\n");
            if(Gun_num!=1)  ShutDownServer();
        }       
        EpsFlagDispose();   //eps��־����
        
        if(StartFuelling==9)
        {
            StartFuelling=0;
            st=3;//��ֹ��������������3״̬����ɲ�����ˮ���������ϴ���3״̬
        }
        if((para.ST!=8)&&(st!=para.ST)&&(para.ST!=0)&&(StartFuelling==0)) //״̬�ı䣬�����ϴ�
        {
            st=para.ST;
            Reply_FS_UploadStatus(para.ST);
        }
        else if(statusflag>0)
        {
            Reply_FS_UploadStatus(statusflag);
            statusflag=0;
        }
  
        if(Check_Netlink("eth0") == -1) //����δ���Ӻ�
        {
            Stop_Opt=1;
            if(Check_Time>30)   ShutDownServer();
        }
        else //��������
        {
            Check_Time=0;
            Stop_Opt=0;
            if((Reconnect_FS_Flag==1)&&(Ser_Connect_FS==1)&&(Cli_Connect_FS==0)&&(Cli_FS_fd<1))
            {
                FS_Tcp_Client_Creat();
            }
        } 
    }
    if(F0_Time%8==0)
    {
        LedOnOff(0);
        Lock(&Data_To_Uart);
        switch(CmdFlag)
        {
            case 0x52:
                FCS_To_JKOPT_52();
                break;
            case 0x57:
                FCS_To_JKOPT_57();
                break;
            case 0x58:
                FCS_To_JKOPT_58();
                break;
            case 0x5E:
                FCS_To_JKOPT_5E();
                break;
            case 0x70:
                FCS_To_JKOPT_70();
                break;
            case 0x71:
                FCS_To_JKOPT_71();
                break;
            case 0x72:
                FCS_To_JKOPT_72();
                break;
            case 0x73:
                FCS_To_JKOPT_73();
                break;
            default:
                FCS_To_JKOPT_51();
                break;
        }
        CmdFlag=0;
        Unlock(&Data_To_Uart);           
    }
    if((Reconnect_EPS_Flag==1)&&(Check_Netlink("eth0")==0))
    {
        EPS_Tcp_Client_Creat();
    }

    if(Time_S==8000)
    {
        Time_S=0;
    }
    if(Time_100ms==80000)
    {
        Time_100ms=0;
        F0_Time=1;
    }
}

void Creat_Timer()
{
    // XXX int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid);
    // clockid--ֵ��CLOCK_REALTIME,CLOCK_MONOTONIC��CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID
    // evp--��Ż���ֵ�ĵ�ַ,�ṹ��Ա˵���˶�ʱ�����ڵ�֪ͨ��ʽ�ʹ���ʽ��
    // timerid--��ʱ����ʶ��
    timer_t timerid;
    struct sigevent evp;
    memset(&evp, 0, sizeof(struct sigevent));   //�����ʼ��

    evp.sigev_value.sival_int = 111;            //Ҳ�Ǳ�ʶ��ʱ���ģ����timerid��ʲô���𣿻ص��������Ի��
    evp.sigev_notify = SIGEV_THREAD;            //�߳�֪ͨ�ķ�ʽ����פ���߳�
    evp.sigev_notify_function = timer_thread;   //�̺߳�����ַ

    if(timer_create(CLOCKID, &evp, &timerid) == -1)
    {
        perror("fail to timer_create");
        exit(-1);
    }

    // XXX int timer_settime(timer_t timerid, int flags, const struct itimerspec *new_value,struct itimerspec *old_value);
    // timerid--��ʱ����ʶ
    // flags--0��ʾ���ʱ�䣬1��ʾ����ʱ��
    // new_value--��ʱ�����³�ʼֵ�ͼ�����������it
    // old_value--ȡֵͨ��Ϊ0�������ĸ�������ΪNULL,����ΪNULL���򷵻ض�ʱ����ǰһ��ֵ

    //��һ�μ��it.it_value��ô��,�Ժ�ÿ�ζ���it.it_interval��ô��,����˵it.it_value��0��ʱ���װ��it.it_interval��ֵ
    struct itimerspec it;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_nsec = 100000000;//100ms
    it.it_value.tv_sec = 1;
    it.it_value.tv_nsec = 0;

    if(timer_settime(timerid, 0, &it, NULL) == -1)
    {
        perror("fail to timer_settime");
        exit(-1);
    }
    pause();
}
/*
 * int timer_gettime(timer_t timerid, struct itimerspec *curr_value);
 * ��ȡtimeridָ���Ķ�ʱ����ֵ������curr_value
 *
 */
