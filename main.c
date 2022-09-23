#define TCFG0 (*(volatile unsigned int *)0xE2500000)
#define TCFG1 (*(volatile unsigned int *)0xE2500004)
#define TCON 	(*(volatile unsigned int *)0xE2500008)
#define TCNTB0 (*(volatile unsigned int *)0xE250000c)
#define TCMPB0 (*(volatile unsigned int *)0xE2500010)
#define TINT_CSTAT *((volatile unsigned int *)0xE2500044)
#define VIC0IRQSTATUS *((volatile unsigned int *)0xF2000000)
#define VIC0VECTADDR21 *((volatile unsigned int *)0xF2000154)
#define GPJ2CON 	(*(volatile unsigned long *) 0xE0200280)
#define GPJ2DAT 	(*(volatile unsigned long *) 0xE0200284)
#define GPH0CON     (*(volatile unsigned int *)0xE0200C00)
#define GPH0DAT     (*(volatile unsigned int *)0xE0200C04)
#define	GPD0CON     (*(volatile unsigned long *)0xE02000A0)
#define GPD0DAT     (*(volatile unsigned long *)0xE02000A4)
#define	GPC1CON		(*((volatile unsigned long *)0xE0200060))
#define	GPC1DAT		(*((volatile unsigned long *)0xE0200064))
#define	GPC1PUD		(*((volatile unsigned long *)0xE0200068))
#define VIC0INTSELECT       *((volatile unsigned int *)0xF200000C)
#define VIC0INTENABLE       *((volatile unsigned int *)0xF2000010)
#define VIC0ADDRESS    		*((volatile unsigned int *)0xF2000F00)
#define VIC0VECTADDR2    	*((volatile unsigned int *)0xF2000108)
#define VIC0VECTADDR3    	*((volatile unsigned int *)0xF200010C)
#define EXT_INT_0_CON    	*((volatile unsigned int *)0xE0200E00)
#define EXT_INT_0_MASK    	*((volatile unsigned int *)0xE0200F00)
#define EXT_INT_0_PEND      *((volatile unsigned int *)0xE0200F40)
#define VIC0INTENCLEAR       *((volatile unsigned int *)0xF2000014)
#define TCNTB2	(*(volatile unsigned long *)0xE2500024)
#define TCMPB2 	(*(volatile unsigned long *)0xE2500028)
#define WTCON	(*(volatile unsigned long *)0xE2700000)
#define WTDAT	(*(volatile unsigned long *)0xE2700004)
#define WTCNT	(*(volatile unsigned long *)0xE2700008)
#define WTCLRINT	(*(volatile unsigned long *)0xE270000C)

int Ledposition=0;//灯的位置
int flag=0;
int tone=0;//选择音调
extern void IRQ_handle(void);
extern void key_isr(void);
extern void uart_send_byte(unsigned char byte);
extern unsigned char uart_recv_byte();
extern void uart_send_string(char *str);
int Songlength;//歌曲长度 
//对于《欢乐颂》的曲调和节拍用数组封装，一拍是8,半拍是4,1/4拍是2,
int Ode_to_joy[64] = {3,3,4,5,5,4,3,2,1,1,2,3,3,2,2,3,3,4,5,5,4,3,2,1,1,2,3,2,1,1,2,2,3,1,2,3,4,3,1,2,3,4,3,2,1,2,1,3,3,3,4,5,5,4,3,4,2,1,1,2,3,2,1,1};
int Ode_to_joy_rhythm[64] = {8,8,8,8,8,8,8,8,8,8,8,8,12,4,16,8,8,8,8,8,8,8,8,8,8,8,8,12,4,16,8,8,8,8,8,4,4,8,8,8,4,4,8,8,8,8,8,8,8,8,8,8,8,8,8,4,4,8,8,8,8,12,4,16};

void delay(volatile unsigned int t)//延迟***
{
    volatile unsigned int t2 = 0xFFFF;
    while (t--)
        for (; t2; t2--);
}

void timer0_init(void)//LED中断***
{
    TCNTB0 = 63162;//一秒触发一次中断
    TCMPB0 = 32581;//PWM占空比为50% TCMPB0=50%TCNTB0
    TCFG0 |= 65;//time0 的 Prescaler value（预分频器值）=65
    TCFG1 |= 0x04;//16分频
    TCON |= (1 << 1);//手动更新
    TCON = 0x09;
}

void timer2_init(volatile unsigned int tcn2,volatile unsigned int tcm2)//蜂鸣器中断***
{
    //此处选用一分频，声音的音色容易分辨
    GPD0CON &= ~(0xf << 8);
    GPD0CON |= (2 << 8);//发声
    TCFG0 &= ~(0xff << 8);
    TCFG0 |= (65 << 8);//（预分频器值）=65
    TCFG1 &= ~(0xf << 8);
    TCFG1 |= 0 << 8;//1分频
    TCON |= (1 << 15);//自动加载
    //每个音调的TB和PB
    TCNTB2 = tcn2;
    TCMPB2 = tcm2;
    TCON |= (1 << 13);//手动更新
    TCON &= ~(1 << 13);//清零
    TCON |= (1 << 12);//开启定时器
}

//音调设置 不同的占空比表示不同的音色 
void doo(void)
{
    timer2_init(2500,50);
}
void re(void)
{
    timer2_init(2200,50);
}
void mi(void)
{
    timer2_init(2000,50);
}
void fa(void)
{
    timer2_init(1800,50);
}
void so(void)
{
    timer2_init(1600,50);
}
void la(void)
{
    timer2_init(1400,50);
}
void xi(void)
{
    timer2_init(1300,40);
}
//中断初始化
void init_irq(void)
{
    TINT_CSTAT |= 1;
}
//清除中断
void clear_irq(void)
{
    TINT_CSTAT |= (1<<5);
}
//中断使能在key函数内***
void init_int(void)
{
    VIC0INTSELECT |= ~(1<<21);
    VIC0ADDRESS = 0x0;//清中断服务程序入口寄存器
    /* 当触发中断CPU就会自动跳转到这个地址去执行*/
    VIC0VECTADDR21 = (int)IRQ_handle;
}
void clear_vectaddr(void)
{
    VIC0ADDRESS = 0x0;
}
unsigned long get_irqstatus(void)
{
    return VIC0IRQSTATUS;
}

void irq_handler()//IRQ中断响应//***
{
    volatile unsigned char status = ((get_irqstatus() & (1<<21))>>21)&0x1;
    clear_vectaddr();
    clear_irq();//清除中断 
    //音乐中断 蜂鸣器
    if (status == 0x1)
    {

        if(flag==1)
        {
            led_key();
        }
        else if(flag==3)
        {
            // 当一个音调延时结束后，立即将中断清除，不影响下一个发音
            if(tone==1)
            {
                flag=0;
                doo();
                VIC0INTENCLEAR |= (1<<21);
            }

            if(tone==2)
            {
                flag=0;
                re();
                VIC0INTENCLEAR |= (1<<21);
            }
            if(tone==3)
             {
                flag=0;
                mi();
                VIC0INTENCLEAR |= (1<<21);
             }
            if(tone==4)
            {
                flag=0;
                fa();
                VIC0INTENCLEAR |= (1<<21);
            }
            if(tone==5)
            {
                flag=0;
                so();
                VIC0INTENCLEAR |= (1<<21);
            }
            if(tone==6)
            {
                flag=0;
                la();
                VIC0INTENCLEAR |= (1<<21);
            }
            if(tone==7)
            {
                flag=0;
                xi();
                VIC0INTENCLEAR |= (1<<21);
            }
        }
    }

}

//对每个音调的节奏（调整TB和PB）
void Control_tone_rhythm(int num)//通过num数值，控制灯亮的时间间隔 2s //***
{
    TCNTB0 = 63162/10*num;
    TCMPB0 = 32581/10*num;
}
void buzzer_init(void)
{
    GPD0CON |= 1<<8;
}

void buzzer_on(void)
{
    GPD0DAT |= 1<<2;
}

void buzzer_off(void)
{
    GPD0DAT &= ~(1<<2);
}
void buzzer2_off(void)
{
    GPD0CON &= ~(0xf << 8);
}

//音乐
void play_music(int a, int b)
{
    tone = a;
    flag = 3;
    Control_tone_rhythm(b);
    VIC0INTENABLE |= (1 << 21);
    while (flag == 3);
}

//中断函数
void key_handle()
{
    volatile unsigned char key_code = EXT_INT_0_PEND & 0xC;
    VIC0ADDRESS = 0;
    EXT_INT_0_PEND |= 0xC;
    if (key_code == 0x04)      /* SW4 */
    {
        Ledposition=0;
        flag=1;
        Control_tone_rhythm(20);
        VIC0INTENABLE |= (1<<21);
    }

}
//led初始化
void ledinit()
{
    GPJ2CON &= ~(0xFFFF << 0);
    GPJ2CON |= ((0x1 << 0) | (0x1 << 4) | (0x1 << 8) | (0x1 << 12));
    GPJ2DAT |= 0xFF << 0;
}
//配置中断引脚P108-109 
void keyinit()
{
    GPH0CON |= 0xFF << 8;
    /* 清空低四位*/
    EXT_INT_0_CON &= ~(0xFF << 8);
    EXT_INT_0_CON |= (4 << 8) | (4 << 12);
    EXT_INT_0_MASK &= ~0xC;
}
void keyinit_2()
{
    VIC0INTSELECT &= ~0xC;
    VIC0INTENABLE |= 0xC;
    VIC0VECTADDR2 = (int)key_isr;
    VIC0VECTADDR3 = (int)key_isr;
}

void led_key()//按键SW4
{

    if (Ledposition == 0)
    {
        uart_send_string("\r\nMarquee\r\n");
        uart_send_string("\r\n LED 0\n");
        GPJ2DAT = 0xe;
        Ledposition = 1;
    }
    else if (Ledposition == 1)
    {
        uart_send_string("\r\n LED 1\n");
        GPJ2DAT = 0xd;
        Ledposition = 2;
    }
    else if (Ledposition == 2)
    {
        uart_send_string("\r\n LED 2\n");
        GPJ2DAT = 0xb;
        Ledposition = 3;
    }
    else if (Ledposition == 3)
    {
        uart_send_string("\r\n LED 3\n");
        GPJ2DAT = 0x7;
        Ledposition = 4;
    }
    else if (Ledposition == 4)
    {
        uart_send_string("\r\n LED ALL\n");
        GPJ2DAT = 0;
        Ledposition = 0;
        VIC0INTENCLEAR |= (1 << 21);
    }
}
//初始化全部
void init_all()
{
    buzzer_init();
    ledinit();
    keyinit();
    timer0_init();
    init_irq();
    init_int();
    keyinit_2();
}

char t[10]="play";
int k=0;

int main()
{
    char byte;
    init_all();
    while(1)
    {

        byte = uart_recv_byte();
        uart_send_byte(byte);
	if(byte==t[k])k++;
	else k=0;
      
        if (k==4)
        {
            uart_send_string("\r\n------------- Playing music Ode to joy -------------\r\n");
            Songlength = 64;
            int i;
            for (i = 0; i <= Songlength; i ++ )
            {
                play_music(Ode_to_joy[i], Ode_to_joy_rhythm[i] );
            }
        }
        delay(1000);

    }

    return 0;
}

