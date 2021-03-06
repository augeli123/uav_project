#include"c_serial.h"
void show_menu_pwm();
void checkpwm_input();
void generate_pwmbuffer();
void setPWM();
static int fd;
uint8_t pwm_message[12]={0};  
uint8_t ack_message[12]={0};
int TIMx_CH[4]={0};
uint16_t CRC_NUM=0;
uint16_t arr[4]={0};
int len;
/*CRC_Generator*/
static unsigned char auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40 };
/* Table of CRC values for loworder byte */
static unsigned char auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40 };
/*
 * Compute CRC-16
 */
uint16_t crc16(uint8_t* buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */
    int index = 0;

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^(uint8_t) buffer[index++]; /* calculate the CRC  */
        crc_hi = crc_lo ^ auchCRCHi[i];
        crc_lo =auchCRCLo[i];
    }
    return (crc_hi << 8 | crc_lo);
}
int main(int argc, char **argv)
{
                             //文件描述符
    int err;                           //返回调用函数的状态
    int len;
//    int i;
//    char rcv_buf[12];
//    char send_buf[12]={0};
    char ch;
    if(argc != 2)
       {
              printf("Usage: %s /dev/ttyUSBn\n",argv[0]);
              return FALSE;
       }
    fd = UART0_Open(fd,argv[1]); //打开串口，返回文件描述符

    if(UART0_Init(fd,9600,0,8,1,'N')!=-1)
       {  
		 printf("Set Port Exactly!\n");
	}else{
                printf("set Port error!\n");
		return 0;
      }
             

    do {       show_menu_pwm();
               checkpwm_input();
               generate_pwmbuffer();
               setPWM();
               ch=getchar();
               }while(ch!='#');

//    if(0 == strcmp(argv[2],"0"))
//           {
//                  for(i = 0;i < 10;i++)
//                         {
//                                len = UART0_Send(fd,send_buf,10);
//                                if(len > 0)
//                                       printf(" %d send data successful\n",i);
//                                else
//                                       printf("send data failed!\n");

//                                sleep(2);
//                         }
//                  UART0_Close(fd);
//           }
//    else
//           {

//           while (1) //循环读取数据
//                  {
//                     len = UART0_Recv(fd, rcv_buf,9);
//                     if(len > 0)
//                            {
//                       rcv_buf[len] = '\0';
//                                   printf("receive data is %s\n",rcv_buf);
//                       printf("len = %d\n",len);
//                            }
//                     else
//                            {
//                                   printf("cannot receive data\n");
//                            }
//                     sleep(2);
//              }
       UART0_Close(fd);

}
void show_menu_pwm()
{

    printf("Now you can set the PWM:\n");
     printf("PWM1 PWM2 PWM3 PWM4\n");
    scanf("%d %d %d %d",&TIMx_CH[0],&TIMx_CH[1],&TIMx_CH[2],&TIMx_CH[3]);
    
}
void checkpwm_input()
{
    if(TIMx_CH[0]<200||TIMx_CH[0]>20000||TIMx_CH[1]<200||TIMx_CH[1]>20000||TIMx_CH[2]<200||TIMx_CH[2]>20000||TIMx_CH[3]<200||TIMx_CH[3]>20000)
    {
        printf("Please set everychannel between 200 and 200000 (200HZ-20KHZ)\n");
        show_menu_pwm();
    }
}
void generate_pwmbuffer()
{
    memset(pwm_message,0,12);//每次生成前先清该串
    pwm_message[0]=0xff;
    pwm_message[1]=0xee;
    for(int i=0;i<4;i++)
    {
        arr[i]=8000000/(uint16_t)TIMx_CH[i];
        pwm_message[2*i+2]=(arr[i]>>8);
        pwm_message[2*i+3]=(arr[i]);
    }
    CRC_NUM=crc16(pwm_message,10); //not 12!!!
    pwm_message[10]=(CRC_NUM>>8);
    pwm_message[11]=(CRC_NUM);
    //printf("%d\n",(int)CRC_NUM);
}
void setPWM()
{

    len = UART0_Send(fd,pwm_message,12);
    if(len ==12)
          printf("send data successful\n");
    else
          printf("send data failed!\n");

    sleep(1);
    len =UART0_Recv(fd,ack_message,12);
    if(len >0)
          printf("read data successful\n");
    else
          printf("read data failed!\n");



}
