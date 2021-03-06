#include "common.h"

//数据发送
void UartSendDataToFS()
{
    int num;
    //gun1
    if((FS_Data_Len>0)&&(Uart_To_FS[0]!=0))
    {
        if((num = send(Cli_FS_fd, Uart_To_FS, FS_Data_Len, 0)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_FS_Thread Failed to sent string.\n");
            Cli_Connect_FS=0;
        }
        else
        {                
            Print_HEX("U_To_F Send>>>>>>>",num,Uart_To_FS);
        } 
        FS_Data_Len=0;
    }
    else
        FS_Data_Len=0;
    //gun2
    if((shared->UtoFnum2>0)&&(shared->Uart_To_FS2[0]!=0))
    {
        if((num = send(Cli_FS_fd, shared->Uart_To_FS2, shared->UtoFnum2, 0)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_FS_Thread Failed to sent string.\n");
            Cli_Connect_FS=0;
        }
        else
        {                
            Print_HEX("U_To_F Send>>>>>>>",num,shared->Uart_To_FS2);
        } 
        shared->UtoFnum2=0;
    }
    else
        shared->UtoFnum2=0;
    //gun3
    if((shared->UtoFnum3>0)&&(shared->Uart_To_FS3[0]!=0))
    {
        if((num = send(Cli_FS_fd, shared->Uart_To_FS3, shared->UtoFnum3, 0)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_FS_Thread Failed to sent string.\n");
            Cli_Connect_FS=0;
        }
        else
        {                
            Print_HEX("U_To_F Send>>>>>>>",num,shared->Uart_To_FS3);
        } 
        shared->UtoFnum3=0;
    }
    else
        shared->UtoFnum3=0;
    //gun4
    if((shared->UtoFnum4>0)&&(shared->Uart_To_FS4[0]!=0))
    {
        if((num = send(Cli_FS_fd, shared->Uart_To_FS4, shared->UtoFnum4, 0)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_FS_Thread Failed to sent string.\n");
            Cli_Connect_FS=0;
        }
        else
        {                
            Print_HEX("U_To_F Send>>>>>>>",num,shared->Uart_To_FS4);
        } 
        shared->UtoFnum4=0;
    }
    else
        shared->UtoFnum4=0;
}

/********************************************************************\
* 函数名：: Cli_Uart_To_FS_Thread
* 说明:
* 功能:     加气机发送给FS的数据处理
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-26
\*********************************************************************/
void *Cli_Uart_To_FS_Thread(void *arg)
{    
    while(1)
    {                
        UartSendDataToFS();
        if(Cli_Connect_FS==0)
        {
            close(Cli_FS_fd);
            Cli_FS_fd=-1;
            return(0);
        }
        if(Cli_FS_fd<0) return 0;
        if(Ser_FS_fd<0)//服务器断开后也断开客户端
        {
            if(Cli_FS_fd>0)
            {
                close(Cli_FS_fd);
                Cli_Connect_FS = 0;
                Cli_FS_fd=-1;
            }
            return 0;
        }
        usleep(1*1000);
    }
    close(Cli_FS_fd);
}

/********************************************************************\
* 函数名：: Cli_EPS_To_Uart_Thread
* 说明:
* 功能:    EPS发送给 加气机的数据处理
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-26
\*********************************************************************/
void *Cli_EPS_To_Uart_Thread(void *arg)
{
    int num;
    /* Try to connect the server */
    while(1)       // Check remoter command
    {        
        num = recv(Cli_EPS_fd, EPS_To_Uart, LENGTH, 0);
        switch(num)
        {
            case -1:
                P_Log(conf.Gun_num,"ERROR:Cli_EPS_To_Uart_Thread Receive string error!\n");
                Disconnect_EPS_Flag=1;
                break;
            case  0:
                P_Log(conf.Gun_num,"Info:EPS_Server close the socket!\n");
                Disconnect_EPS_Flag=1;
                break;
            default:
                P_Log(conf.Gun_num,"OK:Cli_EPS_To_Uart_Thread Receviced numbytes = %d\n", num);
                EPS_Data_Len=num;
                Print_HEX("E_To_U Recv",num,EPS_To_Uart);
                CmdFlag=0x71;//下发EPS命令
                break;
        }
        if(Disconnect_EPS_Flag==1)
        {
            Disconnect_EPS_Flag=0;
            Is_Connect_EPS=0;
            close(Cli_EPS_fd);           
            Cli_EPS_fd=-1;
            return(0);
        }
        if(Cli_EPS_fd<0) return 0;
    }
    close(Cli_EPS_fd);
}

/********************************************************************\
* 函数名：: Cli_Uart_To_EPS_Thread
* 说明:
* 功能:     加气机发送给EPS的数据处理
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-26
\*********************************************************************/
void *Cli_Uart_To_EPS_Thread(void *arg)
{
    int num;
    while(1)
    {
        if((Uart_To_EPS_Flag==1)&&(Cli_EPS_fd>0))
        {
            Uart_To_EPS_Flag=0;
            if((num = send(Cli_EPS_fd, Uart_To_EPS, EPS_Data_Len, 0)) == -1)
            {
                P_Log(conf.Gun_num,"ERROR:Cli_Uart_To_EPS_Thread Failed to sent string.\n");
                Disconnect_EPS_Flag=1;
            }
            bzero(Uart_To_EPS,LENGTH);
        }
        if(Disconnect_EPS_Flag==1)
        {
            Disconnect_EPS_Flag=0;
            Is_Connect_EPS=0;
            close(Cli_EPS_fd);
            Cli_EPS_fd=-1;
            return(0);
        }
        if(Cli_EPS_fd<0) return 0;//服务器端关闭连接后，需要手动退出
        usleep(10*1000);
    }
    close(Cli_EPS_fd);
}

/********************************************************************\
* 函数名：: EPS_Tcp_Client_Creat
* 说明:
* 功能:     EPS客户端创建
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-26
\*********************************************************************/
int EPS_Tcp_Client_Creat()
{
    const char *argv=(const char *)conf.EPS_IP;
    int sockfd,err;                        // Socket file descriptor
    pthread_t Cli_Send,Cli_Receive;
    pthread_attr_t attr;//线程属性
    struct sockaddr_in remote_addr;    // Host address information

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:EPS_Tcp_Client_Creat Failed to obtain Socket Descriptor!\n");
        return (0);
    }
    /* Fill the socket address struct */
    remote_addr.sin_family = AF_INET;                       // Protocol Family
    remote_addr.sin_port = htons(conf.EPS_Port);            // Port number
    err=inet_pton(AF_INET, argv, &remote_addr.sin_addr);    // Net Address
    if(err <= 0)
    {
        if(err == 0)
            fprintf(stderr, "EPS_Tcp_Client_Creat Not in presentation format\n");
        else
            perror("EPS_Tcp_Client_Creat inet_pton error\n");
        return (0);
    }
    bzero(&(remote_addr.sin_zero), 8);                  // Flush the rest of struct

    /* Try to connect the remote */
    if(connect(sockfd, (struct sockaddr *)&remote_addr,  sizeof(struct sockaddr)) == -1)
    {
        EPS_Error++;
        if(EPS_Error>5){
            Reconnect_EPS_Flag=0;
        }
        P_Log(conf.Gun_num,"ERROR:EPS_Tcp_Client_Creat Failed to connect to the host!\n");
        sleep(5);
        return (0);
    }
    else
    {
        EPS_Error=0;
        P_Log(conf.Gun_num,"OK:EPS_Tcp_Client_Creat Have connected to the %s\n",argv);
    }
    //创建线程
    Cli_EPS_fd=sockfd;
    Reconnect_EPS_Flag=0;
    Is_Connect_EPS=1;//连接上了eps
    pthread_attr_init(&attr);//线程属性初始化
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//设置为分离线程，这样可以在线程结束时马上释放资源
    err=pthread_create(&Cli_Send,&attr,(void*)Cli_Uart_To_EPS_Thread,NULL);//创建接收线程
    if(err != 0)
        P_Log(conf.Gun_num,"EPS_Tcp_Client_Creat can't create Cli_Uart_To_EPS_Thread thread: %s\n", strerror(err));
    else
        P_Log(conf.Gun_num,"EPS_Tcp_Client_Creat create Cli_Uart_To_EPS_Thread thread OK\n");
    pthread_attr_destroy(&attr);//去掉线程属性
    
    pthread_attr_init(&attr);//线程属性初始化
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//设置为分离线程，这样可以在线程结束时马上释放资源
    err=pthread_create(&Cli_Receive,&attr,(void*)Cli_EPS_To_Uart_Thread,NULL);//创建接收线程
    if(err != 0)
        P_Log(conf.Gun_num,"EPS_Tcp_Client_Creat can't create Cli_EPS_To_Uart_Thread thread: %s\n", strerror(err));
    else
        P_Log(conf.Gun_num,"EPS_Tcp_Client_Creat create Cli_EPS_To_Uart_Thread thread OK\n");
    
    pthread_attr_destroy(&attr);//去掉线程属性
    return (0);
}

/********************************************************************\
* 函数名：: FS_Tcp_Client_Creat
* 说明:
* 功能:     FS客户端创建
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-26
\*********************************************************************/
int FS_Tcp_Client_Creat()
{
    const char *argv=(const char *)conf.FS_IP;
    int sockfd,err;                        // Socket file descriptor
    pthread_t Cli_Send;
    struct sockaddr_in remote_addr;    // Host address information

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:FS_Tcp_Client_Creat Failed to obtain Socket Descriptor!\n");
        return (0);
    }
    /* Fill the socket address struct */
    remote_addr.sin_family = AF_INET;                       // Protocol Family
    remote_addr.sin_port = htons(conf.FS_Port);             // Port number
    err=inet_pton(AF_INET, argv, &remote_addr.sin_addr);    // Net Address
    if(err <= 0)
    {
        if(err == 0)
            fprintf(stderr, "FS_Tcp_Client_Creat Not in presentation format\n");
        else
            perror("FS_Tcp_Client_Creat inet_pton error\n");
        return (0);
    }
    bzero(&(remote_addr.sin_zero), 8);                  // Flush the rest of struct
    /* Try to connect the remote */
    if((Cli_Connect_FS==0)&&((FS_OK==1)||FS_OK==2))
    {
        if(connect(sockfd, (struct sockaddr *)&remote_addr,  sizeof(struct sockaddr)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:FS_Tcp_Client_Creat Failed to connect to the host!\n");
        }
        else
        {
            P_Log(conf.Gun_num,"OK:FS_Tcp_Client_Creat Have connected to the %s\n",argv);
        }
        Cli_Connect_FS=1;//连接上了fs�
        Reconnect_FS_Flag=0;
        //创建线程
        Cli_FS_fd=sockfd;
        err=pthread_create(&Cli_Send,NULL,(void*)Cli_Uart_To_FS_Thread,NULL);//创建向FS发送数据线程
        if(err != 0)
            P_Log(conf.Gun_num,"FS_Tcp_Client_Creat can't create Cli_Uart_To_FS_Thread thread: %s\n", strerror(err));
        else
            P_Log(conf.Gun_num,"FS_Tcp_Client_Creat create Cli_Uart_To_FS_Thread thread OK\n");        
    }
    return 0;
}

/********************************************************************\
* 函数名: CopyToShm
* 功能:     拷贝FS数据到共享内存中 
* 输入:     数据长度
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2017-11-20 12:01:27
\*********************************************************************/
void CopyToShm(int num)    
{
    //num=24,两帧数据
    //01.0B.02.01.00.5D.00.04.01.21.3C.00.01.0A.02.01.00.53.00.04.01.21.3C.00
    //num=36,三帧数据
    //01.09.02.01.00.5E.00.04.01.21.3D.00.01.0B.02.01.00.41.00.04.01.21.3D.00.01.0A.02.01.00.56.00.04.01.21.3D.00.
    if(shared->FtoUflag==0)
    {          
        memset(shared->FS_To_Uart,0,LENGTH);
        memcpy(shared->FS_To_Uart,FS_To_Uart,num);                    
        shared->FtoUflag=1;
        shared->FtoUnum=num;                        
    }
    else if((shared->FtoUflag==1)&&(num>0)&&(FSbacknum1==0))//处理之前FS发给串口的数据还没处理完，进行备份
    {
        memset(FSbackbuf1,0,LENGTH);
        memcpy(FSbackbuf1,FS_To_Uart,num);  
        FSbacknum1=num;
    }
    else if((FSbacknum1>0)&&(FSbacknum2==0))//若fs下发数据过快，则之前的备份数据可能还没处理完，再次备份
    {        
        memset(FSbackbuf2,0,LENGTH);
        memcpy(FSbackbuf2,FS_To_Uart,num);  
        FSbacknum2=num;
    }  
}

/********************************************************************\
* 函数名: FS_Tcp_Server_Creat
* 说明: 用于fs 客户端连接加气机
* 功能:     FS服务端创建
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-26
\*********************************************************************/
int FS_Tcp_Server_Creat()
{
    int sockfd;                         // Socket file descriptor
    int nsockfd;                        // New Socket file descriptor
    int num;
    int sin_size;                       // to store struct size
    int opt=1;                          //设置端口重用(wait状态)
    int keepalive = 1;                  // 开启keepalive属性
    int keepidle = 30;                  // 如该连接在30秒内没有任何数据往来,则进行探测
    int keepinterval = 5;               // 探测时发包的时间间隔为5 秒
    int keepcount = 3;                  // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

    struct sockaddr_in addr_local;
    struct sockaddr_in addr_remote;

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:FS_Tcp_Server_Creat Failed to obtain Socket Despcritor.\n");
        return (0);
    }
    else
    {
        P_Log(conf.Gun_num,"OK:FS_Tcp_Server_Creat Obtain Socket Despcritor sucessfully.\n");
    }
    setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR,(void *)&opt,sizeof(opt));
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive , sizeof(keepalive));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount));
    /* Fill the local socket address struct */
    addr_local.sin_family = AF_INET;                // Protocol Family
    addr_local.sin_port = htons(conf.My_FS_Port);   // Port number
    addr_local.sin_addr.s_addr  = INADDR_ANY;       // AutoFill local address
    bzero(&(addr_local.sin_zero), 8);               // Flush the rest of struct

    /*  Blind a special Port */
    if(bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:FS_Tcp_Server_Creat Failed to bind Port %d.\n",conf.My_FS_Port);
        return (0);
    }
    else
    {
        P_Log(conf.Gun_num,"OK:FS_Tcp_Server_Creat Bind the Port %d sucessfully.\n",conf.My_FS_Port);
    }

    /*  Listen remote connect/calling */
    if(listen(sockfd,BACKLOG) == -1)
    {
        P_Log(conf.Gun_num,"ERROR:FS_Tcp_Server_Creat Failed to listen Port %d.\n", conf.My_FS_Port);
        return (0);
    }
    else
    {
        P_Log(conf.Gun_num,"OK:FS_Tcp_Server_Creat Listening the Port %d sucessfully.\n", conf.My_FS_Port);
    }
    OPT_Server_OK=1;
    while(1)
    {
        FS_OK=0;
        sin_size = sizeof(struct sockaddr_in);
        /*  Wait a connection, and obtain a new socket file despriptor for single connection */
        if((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, (socklen_t *)&sin_size)) == -1)
        {
            P_Log(conf.Gun_num,"ERROR:FS_Tcp_Server_Creat Obtain new Socket Despcritor error.\n");
            continue;
        }
        else
        {
            P_Log(conf.Gun_num,"OK:FS_Tcp_Server_Creat Server has got connect from %s.\n", inet_ntoa(addr_remote.sin_addr));
        }
        Ser_Connect_FS=1;
        Ser_FS_fd=nsockfd;
        FS_OK=2;
        FS_Tcp_Client_Creat();
        shared->TCPOK=1;
        while(1)
        {
            num = recv(Ser_FS_fd, FS_To_Uart, LENGTH, 0);
            switch(num)
            {
                case -1:
                    P_Log(conf.Gun_num,"ERROR:Ser_FS_To_Uart_Thread Receive string error!\n");
                    Ser_Connect_FS=0;
                    break;
                case  0:
                    Ser_Connect_FS=0;
                    break;
                default:                    
                    break;
            }
            if(Ser_Connect_FS==0)
            {
                close(Ser_FS_fd);
                Ser_FS_fd=-1;
                break;
            }
            else
            {
                //拷贝到共享内存中     
                CopyToShm(num);
                Print_HEX("F_To_U Recv<<<<<<<",num,FS_To_Uart);
            }
        }
        shared->TCPOK=0;
    }
    close(Ser_FS_fd);
    Ser_FS_fd=-1;
    OPT_Server_OK=0;
    return -1;
}


/********************************************************************\
* 函数名：: Tcp_Pthread_Creat
* 说明:
* 功能:     客户端TCP线程创建
* 输入:     无
* 返回值:   无
* 创建人:   Yang Chao Xu
* 创建时间: 2014-8-26
\*********************************************************************/
int Tcp_Pthread_Creat()
{
    if(Gun_num==1)//只有第一条枪创建TCP服务器
    {
        int err;
        pthread_t Tcp_Server;
        err=pthread_create(&Tcp_Server,NULL,(void*)FS_Tcp_Server_Creat,NULL);//创建接收线程
        if(err != 0)
        {
            P_Log(conf.Gun_num,"can't create FS_Tcp_Server_Creat thread: %s\n", strerror(err));
            return 1;
        }
        else
            P_Log(conf.Gun_num,"create FS_Tcp_Server_Creat thread OK\n");
        return 0;
    }
    else 
        return 0;
}

