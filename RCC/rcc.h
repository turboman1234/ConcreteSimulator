#ifndef __RCC_H
#define __RCC_H

#define PLL_M           16
#define PLL_N           200
#define PLL_P           2
#define PLL_Q           2

void InitRCC(void);

extern RCC_ClocksTypeDef MYCLOCKS;

#endif
