#ifndef __GPIO_H__
#define __GPIO_H__

#define GPIO0_ADDR_BASE     0x44E07000
#define GPIO0_ADDR_END      0x44E07FFF
#define GPIO0_ADDR_SIZE     (GPIO0_ADDR_END - GPIO0_ADDR_BASE + 1)

#define GPIO1_ADDR_BASE     0x4804C000
#define GPIO1_ADDR_END      0x4804CFFF
#define GPIO1_ADDR_SIZE     (GPIO1_ADDR_END - GPIO1_ADDR_BASE + 1)

#define GPIO_OE_OFFSET              0x134
#define GPIO_CLEARDATAOUT_OFFSET    0x190
#define GPIO_SETDATAOUT_OFFSET      0x194
#define GPIO_DATAIN_OFFSET          0x138

#define GPIO0_30    (1 << 30)   // LED - P9_11
#define GPIO1_28    (1 << 28)   // Button - P9_12

#endif  //__GPIO_H__