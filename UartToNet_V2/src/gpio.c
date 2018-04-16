#include "common.h"

/********************************************************************\
* ��������: OpenLedDev
* ����:    ��GPIO�豸
* ����:     ��
* ����ֵ:   0-�ɹ���-1-ʧ��
* ������:   Yang Chao Xu
* ����ʱ��: 2017-10-9 17:20:07
\*********************************************************************/
int OpenLedDev()
{	
    if(conf.Led_ON==0) return -1;//�����ļ��п���led�Ƿ���˸
    Gpio_fd = open(DEV_GPIO, O_RDWR);	
    if (Gpio_fd < 0) 
    {       
        P_Log(conf.Gun_num,"OpenLedDev Error, device open fail! %d\n", Gpio_fd);
        return -1;  
    }
    return 0;
}

/********************************************************************\
* ��������: LedOnOff
* ����:    ����LED��
* ����:     0-��1-��
* ����ֵ:   ��
* ������:   Yang Chao Xu
* ����ʱ��: 2017-10-9 17:20:51
\*********************************************************************/
void LedOnOff(int state)
{
    if(conf.Led_ON==0) return;//�����ļ��п���led�Ƿ���˸
    am335x_gpio_arg arg;
    arg.pin = GPIO_TO_PIN(0, 20);
    arg.data = state;
    ioctl(Gpio_fd, IOCTL_GPIO_SETOUTPUT, &arg);
}


