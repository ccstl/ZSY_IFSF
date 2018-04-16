#ifndef _UART_H_
#define _UART_H_


Uchar   Recv70_Flag;
Uchar   Recv57_Flag;
Uchar   Recv72_Flag;
Uchar   Status_Code;
int     Uart_Data_Len;
Uchar   Uart_Tx_Flag;

Uchar   Auth_Type;//��Ȩ���ͣ�0=��1=����
long    Auth_Value;//��Ȩ��
Uchar   HaveAuth;   //0-δ��Ȩ��1-����Ȩ


Uchar   StartFuelling;   //0:started 6״̬��ʼ������1:fuelling 8״̬
Uchar   uploadtransagain;//���߽���ʱ�����յ����׼�¼��3��֮��û���յ���ȡδ֧�����ף������ϴ�һ��
Uchar   uploadtransagaincount;

void set_speed(int fd, int speed);
int set_Parity(int fd, int databits, int stopbits, int parity);
void Reply_FS_UploadStatus(Uchar st);
void Deal_Uart_data();
void Deal_FS_To_Uart_data();
void Deal_EPS_To_Uart_data();
int Uart_Pthread_Creat();
void Reply_FS_UploadRealtimeData();
void Reply_FS_UploadOverFuel();
void Reply_FS_ReadTotal();
void Reply_FS_Auth_OPT();
void Reply_FS_Stop_OPT();
void Reply_FS_CantAuth_OPT();
/*�ظ�fs��۳ɹ�*/
void Reply_FS_DownLoad_Price_Success();
/*��FP�ж�ȡδ֧������*/
void Reply_FS_ReadNO_Pay_Trans(Uchar flag);
/*�����ϴ�5,6,8״̬*/
void Reply_FS_Upload568Status();





#endif

