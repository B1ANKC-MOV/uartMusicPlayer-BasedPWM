#define APLLCON0        *((volatile unsigned int *)0xE0100100)  
#define MPLLCON         *((volatile unsigned int *)0xE0100108)  
#define EPLLCON0        *((volatile unsigned int *)0xE0100110)  
#define VPLLCON         *((volatile unsigned int *)0xE0100120)  
#define CLK_SRC0        *((volatile unsigned int *)0xE0100200)  
#define CLK_DIV0        *((volatile unsigned int *)0xE0100300)  
#define CLK_DIV1        *((volatile unsigned int *)0xE0100304)  
#define CLK_DIV2        *((volatile unsigned int *)0xE0100308)  
#define CLK_DIV3        *((volatile unsigned int *)0xE010030C)  
  
void clock_init()  
{  
    /* 1、设置PLL_LOCK寄存器（这里使用默认值） */  
    /* 2、设置PLL_CON寄存器（使用芯片手册推荐的值） */  
    APLLCON0    = (1 << 0) | (3 << 8) | (125 << 16) | (1 << 31);    /* FOUTAPLL = 1000MHz */  
    MPLLCON     = (1 << 0) | (12 << 8) | (667 << 16) | (1 << 31);   /* FOUTMPLL = 667MHz */  
    EPLLCON0    = (2 << 0) | (3 << 8) | (48 << 16) | (1 << 31);   /* FOUTEPLL = 96MHz */  
    VPLLCON     = (3 << 0) | (6 << 8) | (108 << 16) | (1 << 31);    /* FOUTVPLL = 54MHz */  
      
    /* 3、选择PLL为时钟输出 */  
    /* MOUT_MSYS = SCLKAPLL = 1000MHz 
    ** MOUT_DSYS = SCLKMPLL = 667MHz 
    ** MOUT_PSYS = SCLKMPLL = 667MHz 
    */  
    CLK_SRC0 = (1 << 0) | (1 << 4) | (1 << 8) | (1 << 12);  
      
    /* 4、设置系统时钟分频值 */  
    /* freq(ARMCLK) = MOUT_MSYS / (APLL_RATIO + 1) = 1000MHz / (0 + 1) = 1000MHz 
    ** freq(HCLK_MSYS) = ARMCLK / (HCLK_MSYS_RATIO + 1) = 1000MHz / (4 + 1) = 200MHz 
    ** freq(PCLK_MSYS) = HCLK_MSYS / (PCLK_MSYS_RATIO + 1) = 200MHz / (1 + 1) = 100MHz 
    ** freq(HCLK_DSYS) = MOUT_DSYS / (HCLK_DSYS_RATIO + 1) = 667 / (3 + 1) = 166MHz 
    ** freq(PCLK_DSYS) = HCLK_DSYS / (PCLK_DSYS_RATIO + 1) = 166 / (1 + 1) = 83MHz 
    ** freq(HCLK_PSYS) = MOUT_PSYS / (HCLK_PSYS_RATIO + 1) = 667 / (4 + 1) = 133MHz 
    ** freq(PCLK_PSYS) = HCLK_PSYS / (PCLK_PSYS_RATIO + 1) = 133 / (1 + 1) = 66MHz 
    */  
    CLK_DIV0 = (0 << 0) | (4 << 8) | (1 << 12) | (3 << 16) | (1 << 20) | (4 << 24) | (1 << 28);  
}
