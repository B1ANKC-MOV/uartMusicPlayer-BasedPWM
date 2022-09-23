#define GPA0CON     *((volatile unsigned int *)0xE0200000)  
#define ULCON0      *((volatile unsigned int *)0xE2900000)  
#define UCON0       *((volatile unsigned int *)0xE2900004)  
#define UFCON0      *((volatile unsigned int *)0xE2900008)  
#define UTRSTAT0    *((volatile unsigned int *)0xE2900010)  
#define UTXH0       *((volatile unsigned int *)0xE2900020)  
#define URXH0       *((volatile unsigned int *)0xE2900024)  
#define UBRDIV0     *((volatile unsigned int *)0xE2900028)  
#define UDIVSLOT0   *((volatile unsigned int *)0xE290002C)  
  
/* 
** UART0初始化 
*/  
void uart_init()  
{  
    /* 
    ** 配置GPA0_0为UART_0_RXD 
    ** 配置GPA0_1为UART_0_TXD 
    */  
    GPA0CON &= ~0xFF;  
    GPA0CON |= 0x22;  //0b 00100010
  
    /* 8-bits/One stop bit/No parity/Normal mode operation */
    /*每次8位，1个停止位，无奇偶验证，正常发送模式（非红外）*/  
    ULCON0 = 0x3 | (0 << 2) | (0 << 3) | (0 << 6);  // 0b 0 0xx 0 11
 
    /* Interrupt request or polling mode/Normal transmit/Normal operation/PCLK/*/
    /*发送和接受引脚采用中断和轮询查询模式，正常发送，常规操作，时钟选择为PCLK*/  
    UCON0 = 1 | (1 << 2) | (0 << 10);  
  
    /* 禁止FIFO */  
    UFCON0 = 0;  
  
    /* 
    ** 波特率计算：115200bps 
    ** PCLK = 66MHz 
    ** DIV_VAL = (66000000/(115200 x 16))-1 = 35.8 - 1 = 34.8 
    ** UBRDIV0 = 34 (DIV_VAL的整数部分) 
    ** (num of 1's in UDIVSLOTn)/16 = 0.8 (DIV_VAL的小数部分)
    ** (num of 1's in UDIVSLOTn) = 12 
    ** UDIVSLOT0 = 0xDDDD (在数据手册上880页查表) 
    */  
    UBRDIV0 = 34;//波特率分度值  
    UDIVSLOT0 = 0xDDDD;  
}  
  
void uart_send_byte(unsigned char byte)  
{  
    while (!(UTRSTAT0 & (1 << 2)));   /* 等待发送缓冲区为空 */  
    UTXH0 = byte;                   /* 发送一字节数据 */         
}  
  
unsigned char uart_recv_byte()  
{  
    while (!(UTRSTAT0 & 1));    /* 等待接收缓冲区有数据可读 */  
    return URXH0;               /* 接收一字节数据 */         
}  
  
void uart_send_string(char *str)  
{  
    char *p = str;  
    while (*p)  
        uart_send_byte(*p++);  
}