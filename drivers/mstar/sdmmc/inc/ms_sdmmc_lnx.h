/***************************************************************************************************************
 *
 * FileName ms_sdmmc_lnx.h
 *     @author jeremy.wang (2012/01/10)
 * Desc:
 *     This file is the header file of ms_sdmmc_lnx.c.
 *
 ***************************************************************************************************************/

#ifndef __MS_SDMMC_LNX_H
#define __MS_SDMMC_LNX_H

#include <linux/cdev.h>
#include <linux/interrupt.h>
#include "hal_sd_base.h"

//***********************************************************************************************************
// Config Setting (Externel)
//***********************************************************************************************************
//###########################################################################################################
#if (D_PROJECT == D_PROJECT__iNF2) //For iNfinity2
//###########################################################################################################

    #define EN_DEV_TREE_SUP            (TRUE)

    #define D_IP1_IP                   EV_IP_FCIE1                    //SDIO
    #define D_IP1_PORT                 EV_PFCIE5_SDIO_PLL             //Port Setting for FCIE5 (SDIO)

    #define D_IP2_IP                   EV_IP_FCIE2                    //FCIE
    #define D_IP2_PORT                 EV_PFCIE5_SDIO                 //Port Setting for FCIE5 (FCIE)

    #define D_IP3_IP                   EV_IP_FCIE3                    //FCIE
    #define D_IP3_PORT                 EV_PFCIE5_SDIO                 //Port Setting for FCIE5 (FCIE)


    #define WT_POWERUP                 20 //(ms)
    #define WT_POWERON                 60 //(ms)
    #define WT_POWEROFF                80 //(ms)


    #define D_SDMMC1_MUTEX             EV_MUTEX1
    #define D_SDMMC2_MUTEX             EV_MUTEX2
    #define D_SDMMC3_MUTEX             EV_MUTEX3

    #define EV_SDMMC1_DOWN_LVL         (FALSE)
    #define EV_SDMMC2_DOWN_LVL         (FALSE)
    #define EV_SDMMC3_DOWN_LVL         (FALSE)

    #define EV_SDMMC1_SDIO_IRQ         (FALSE)
    #define EV_SDMMC2_SDIO_IRQ         (FALSE)
    #define EV_SDMMC3_SDIO_IRQ         (FALSE)
    #define EV_SDMMC1_SDIO_PRT         (FALSE)
    #define EV_SDMMC2_SDIO_PRT         (FALSE)
    #define EV_SDMMC3_SDIO_PRT         (FALSE)

    #include "../../../mstar/include/infinity2/irqs.h"

    #define V_IP1_MIEIRQ               (INT_IRQ_80_SDIO_INT)
    #define V_IP2_MIEIRQ               (INT_IRQ_101_SD_INT)
    #define V_IP3_MIEIRQ               0

    #define V_IP_MIEIRQ_PARA           IRQF_SHARED

    #define V_PAD1_CDZIRQ              (0+32)
    #define V_PAD2_CDZIRQ              (0+160)
    #define V_PAD3_CDZIRQ              0

    #define V_PAD1_PWRGPIO             (19)
    #define V_PAD2_PWRGPIO             0
    #define V_PAD3_PWRGPIO             0

    #define V_PAD1_CDZIRQ_PARA         IRQF_SHARED
    #define V_PAD2_CDZIRQ_PARA         IRQF_SHARED
    #define V_PAD3_CDZIRQ_PARA         IRQF_SHARED

    #define EN_PAD1_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD2_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD3_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD1_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD2_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD3_CDZIRQ_WAKEUP      (FALSE)

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__C4P)    //For C4P
//###########################################################################################################

    #define EN_DEV_TREE_SUP            (TRUE)

    #define D_IP1_IP                   EV_IP_FCIE1                    //SDIO
    #define D_IP1_PORT                 EV_PFCIE5_SDIO_PLL             //Port Setting for FCIE5 (SDIO)

    #define D_IP2_IP                   EV_IP_FCIE2                    //FCIE
    #define D_IP2_PORT                 EV_PFCIE5_SDIO_PLL             //Port Setting for FCIE5 (FCIE)

    #define D_IP3_IP                   EV_IP_FCIE3                    //FCIE
    #define D_IP3_PORT                 EV_PFCIE5_SDIO                 //Port Setting for FCIE5 (FCIE)


    #define WT_POWERUP                 20 //(ms)
    #define WT_POWERON                 60 //(ms)
    #define WT_POWEROFF                80 //(ms)


    #define D_SDMMC1_MUTEX             EV_MUTEX1
    #define D_SDMMC2_MUTEX             EV_MUTEX2
    #define D_SDMMC3_MUTEX             EV_MUTEX3

    #define EV_SDMMC1_DOWN_LVL         (FALSE)
    #define EV_SDMMC2_DOWN_LVL         (FALSE)
    #define EV_SDMMC3_DOWN_LVL         (FALSE)

    #define EV_SDMMC1_SDIO_IRQ         (FALSE)
    #define EV_SDMMC2_SDIO_IRQ         (FALSE)
    #define EV_SDMMC3_SDIO_IRQ         (FALSE)
    #define EV_SDMMC1_SDIO_PRT         (FALSE)
    #define EV_SDMMC2_SDIO_PRT         (FALSE)
    #define EV_SDMMC3_SDIO_PRT         (FALSE)

    #include "../../../mstar/include/cleveland/irqs.h"

    #define V_IP1_MIEIRQ               (INT_GIC_SDIO_INT0)
    #define V_IP2_MIEIRQ               (INT_GIC_SDIO_INT1)
    #define V_IP3_MIEIRQ               0

    #define V_IP_MIEIRQ_PARA           IRQF_SHARED

    #define V_PAD1_CDZIRQ              0
    #define V_PAD2_CDZIRQ              0
    #define V_PAD3_CDZIRQ              0

    #define V_PAD1_PWRGPIO             (19)
    #define V_PAD2_PWRGPIO             0
    #define V_PAD3_PWRGPIO             0

    #define V_PAD1_CDZIRQ_PARA         IRQF_SHARED
    #define V_PAD2_CDZIRQ_PARA         IRQF_SHARED
    #define V_PAD3_CDZIRQ_PARA         IRQF_SHARED

    #define EN_PAD1_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD2_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD3_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD1_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD2_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD3_CDZIRQ_WAKEUP      (FALSE)

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__iNF3)    //For iNF3 (Two Slots)
//###########################################################################################################

    #define EN_DEV_TREE_SUP            (TRUE)

    #define D_IP1_IP                   EV_IP_FCIE1                    //SDIO
    #define D_IP1_PORT                 EV_PFCIE5_SDIO                 //Port Setting for FCIE5 (SDIO)

    #define D_IP2_IP                   EV_IP_FCIE2                    //FCIE
    #define D_IP2_PORT                 EV_PFCIE5_FCIE                 //Port Setting for FCIE5 (FCIE)

    #define D_IP3_IP                   EV_IP_FCIE3                    //FCIE
    #define D_IP3_PORT                 EV_PFCIE5_FCIE                 //Port Setting for FCIE5 (FCIE)

    #define WT_POWERUP                 20 //(ms)
    #define WT_POWERON                 60 //(ms)
    #define WT_POWEROFF                80 //(ms)


    #define D_SDMMC1_MUTEX             EV_MUTEX1
    #define D_SDMMC2_MUTEX             EV_MUTEX2
    #define D_SDMMC3_MUTEX             EV_MUTEX3

    #define EV_SDMMC1_DOWN_LVL         (FALSE)
    #define EV_SDMMC2_DOWN_LVL         (FALSE)
    #define EV_SDMMC3_DOWN_LVL         (FALSE)

    #define EV_SDMMC1_SDIO_IRQ         (FALSE)
    #define EV_SDMMC2_SDIO_IRQ         (FALSE)
    #define EV_SDMMC3_SDIO_IRQ         (FALSE)
    #define EV_SDMMC1_SDIO_PRT         (FALSE)
    #define EV_SDMMC2_SDIO_PRT         (FALSE)
    #define EV_SDMMC3_SDIO_PRT         (FALSE)

    #include "../../../mstar/include/infinity3/irqs.h"

    #define V_IP1_MIEIRQ               (INT_IRQ_SDIO+32)
    #define V_IP2_MIEIRQ               (INT_IRQ_FCIE+32)
    #define V_IP3_MIEIRQ               0

    #define V_IP_MIEIRQ_PARA           IRQF_SHARED|IRQF_DISABLED

    #define V_PAD1_CDZIRQ              (INT_FIQ_SD_CDZ+32)
    #define V_PAD2_CDZIRQ              (INT_PMSLEEP_GPIO_7+160)
    #define V_PAD3_CDZIRQ              0

    #define V_PAD1_PWRGPIO             (19)
    #define V_PAD2_PWRGPIO             0
    #define V_PAD3_PWRGPIO             0

    #define V_PAD1_CDZIRQ_PARA         IRQF_SHARED|IRQF_DISABLED
    #define V_PAD2_CDZIRQ_PARA         IRQF_SHARED|IRQF_DISABLED
    #define V_PAD3_CDZIRQ_PARA         IRQF_SHARED|IRQF_DISABLED

    #define EN_PAD1_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD2_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD3_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD1_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD2_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD3_CDZIRQ_WAKEUP      (FALSE)

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__iNF)    //For iNfinity
//###########################################################################################################

    #define EN_DEV_TREE_SUP            (TRUE)

    #define D_IP1_IP                   EV_IP_FCIE1                    //SDIO
    #define D_IP1_PORT                 EV_PFCIE5_SDIO                 //Port Setting for FCIE5 (SDIO)

    #define D_IP2_IP                   EV_IP_FCIE2                    //FCIE
    #define D_IP2_PORT                 EV_PFCIE5_FCIE                 //Port Setting for FCIE5 (FCIE)

    #define D_IP3_IP                   EV_IP_FCIE3                    //FCIE
    #define D_IP3_PORT                 EV_PFCIE5_FCIE                 //Port Setting for FCIE5 (FCIE)


    #define WT_POWERUP                 20 //(ms)
    #define WT_POWERON                 60 //(ms)
    #define WT_POWEROFF                80 //(ms)


    #define D_SDMMC1_MUTEX             EV_MUTEX1
    #define D_SDMMC2_MUTEX             EV_MUTEX2
    #define D_SDMMC3_MUTEX             EV_MUTEX3

    #define EV_SDMMC1_DOWN_LVL         (FALSE)
    #define EV_SDMMC2_DOWN_LVL         (FALSE)
    #define EV_SDMMC3_DOWN_LVL         (FALSE)

    #define EV_SDMMC1_SDIO_IRQ         (FALSE)
    #define EV_SDMMC2_SDIO_IRQ         (FALSE)
    #define EV_SDMMC3_SDIO_IRQ         (FALSE)
    #define EV_SDMMC1_SDIO_PRT         (FALSE)
    #define EV_SDMMC2_SDIO_PRT         (FALSE)
    #define EV_SDMMC3_SDIO_PRT         (FALSE)

    #include "../../../mstar/include/infinity3/irqs.h"

    #define V_IP1_MIEIRQ               (INT_IRQ_SDIO+32)
    #define V_IP2_MIEIRQ               (INT_IRQ_FCIE+32)
    #define V_IP3_MIEIRQ               0

    #define V_IP_MIEIRQ_PARA           IRQF_SHARED|IRQF_DISABLED

    #define V_PAD1_CDZIRQ              (INT_FIQ_SD_CDZ+32)
    #define V_PAD2_CDZIRQ              (INT_PMSLEEP_GPIO_7+160)
    #define V_PAD3_CDZIRQ              0

    #define V_PAD1_PWRGPIO             (19)
    #define V_PAD2_PWRGPIO             0
    #define V_PAD3_PWRGPIO             0

    #define V_PAD1_CDZIRQ_PARA         IRQF_SHARED|IRQF_DISABLED
    #define V_PAD2_CDZIRQ_PARA         IRQF_SHARED|IRQF_DISABLED
    #define V_PAD3_CDZIRQ_PARA         IRQF_SHARED|IRQF_DISABLED

    #define EN_PAD1_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD2_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD3_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD1_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD2_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD3_CDZIRQ_WAKEUP      (FALSE)


//###########################################################################################################
#else    //Templete Description
//###########################################################################################################
/*
    #define EN_DEV_TREE_SUP            (TRUE)

    #define D_IP1_IP                   EV_IP_FCIE1                    //SDIO
    #define D_IP1_PORT                 EV_PFCIE5_SDIO                 //Port Setting for FCIE5 (SDIO)

    #define D_IP2_IP                   EV_IP_FCIE2                    //FCIE
    #define D_IP2_PORT                 EV_PFCIE5_FCIE                 //Port Setting for FCIE5 (FCIE)

    #define D_IP3_IP                   EV_IP_FCIE3                    //FCIE
    #define D_IP3_PORT                 EV_PFCIE5_FCIE                 //Port Setting for FCIE5 (FCIE)


    #define WT_POWERUP                 20 //(ms)
    #define WT_POWERON                 60 //(ms)
    #define WT_POWEROFF                80 //(ms)


    #define D_SDMMC1_MUTEX             EV_MUTEX1
    #define D_SDMMC2_MUTEX             EV_MUTEX2
    #define D_SDMMC3_MUTEX             EV_MUTEX3

    #define EV_SDMMC1_DOWN_LVL         (FALSE)
    #define EV_SDMMC2_DOWN_LVL         (FALSE)
    #define EV_SDMMC3_DOWN_LVL         (FALSE)

    #define EV_SDMMC1_SDIO_IRQ         (FALSE)
    #define EV_SDMMC2_SDIO_IRQ         (FALSE)
    #define EV_SDMMC3_SDIO_IRQ         (FALSE)
    #define EV_SDMMC1_SDIO_PRT         (FALSE)
    #define EV_SDMMC2_SDIO_PRT         (FALSE)
    #define EV_SDMMC3_SDIO_PRT         (FALSE)

    #include "../../../mstar/include/infinity3/irqs.h"

    #define V_IP1_MIEIRQ               (INT_IRQ_SDIO+32)
    #define V_IP2_MIEIRQ               (INT_IRQ_FCIE+32)
    #define V_IP3_MIEIRQ               0

    #define V_IP_MIEIRQ_PARA           IRQF_SHARED|IRQF_DISABLED

    #define V_PAD1_CDZIRQ              (INT_FIQ_SD_CDZ+32)
    #define V_PAD2_CDZIRQ              (INT_PMSLEEP_GPIO_7+160)
    #define V_PAD3_CDZIRQ              0

    #define V_PAD1_PWRGPIO             (19)
    #define V_PAD2_PWRGPIO             0
    #define V_PAD3_PWRGPIO             0

    #define V_PAD1_CDZIRQ_PARA         IRQF_SHARED|IRQF_DISABLED
    #define V_PAD2_CDZIRQ_PARA         IRQF_SHARED|IRQF_DISABLED
    #define V_PAD3_CDZIRQ_PARA         IRQF_SHARED|IRQF_DISABLED

    #define EN_PAD1_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD2_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD3_CDZIRQ_SHARD       (FALSE)
    #define EN_PAD1_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD2_CDZIRQ_WAKEUP      (FALSE)
    #define EN_PAD3_CDZIRQ_WAKEUP      (FALSE)
*/
//###########################################################################################################
#endif
//###########################################################################################################

//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC_SLOTNUMS)
    #define V_SDMMC_SLOTNUMS       CONFIG_MSTAR_SDMMC_SLOTNUMS
#elif defined(CONFIG_MS_SDMMC_SLOTNUMS)
    #define V_SDMMC_SLOTNUMS       CONFIG_MS_SDMMC_SLOTNUMS
#else
    #define V_SDMMC_SLOTNUMS       1
#endif

//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC1_IP)
    #define V_SLOT0_IP       CONFIG_MSTAR_SDMMC1_IP
#elif defined(CONFIG_MS_SDMMC1_IP)
    #define V_SLOT0_IP       CONFIG_MS_SDMMC1_IP
#else
    #define V_SLOT0_IP       0
#endif
#if defined(CONFIG_MSTAR_SDMMC2_IP)
    #define V_SLOT1_IP       CONFIG_MSTAR_SDMMC2_IP
#elif defined(CONFIG_MS_SDMMC2_IP)
    #define V_SLOT1_IP       CONFIG_MS_SDMMC2_IP
#else
    #define V_SLOT1_IP       1
#endif
#if defined(CONFIG_MSTAR_SDMMC3_IP)
    #define V_SLOT2_IP       CONFIG_MSTAR_SDMMC3_IP
#elif defined(CONFIG_MS_SDMMC3_IP)
    #define V_SLOT2_IP       CONFIG_MS_SDMMC3_IP
#else
    #define V_SLOT2_IP       2
#endif

//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC1_PAD)
    #define V_SLOT0_PAD      CONFIG_MSTAR_SDMMC1_PAD
#elif defined(CONFIG_MS_SDMMC1_PAD)
    #define V_SLOT0_PAD       CONFIG_MS_SDMMC1_PAD
#else
    #define V_SLOT0_PAD       0
#endif
#if defined(CONFIG_MSTAR_SDMMC2_PAD)
    #define V_SLOT1_PAD       CONFIG_MSTAR_SDMMC2_PAD
#elif defined(CONFIG_MS_SDMMC2_IP)
    #define V_SLOT1_PAD       CONFIG_MS_SDMMC2_PAD
#else
    #define V_SLOT1_PAD       1
#endif
#if defined(CONFIG_MSTAR_SDMMC3_PAD)
    #define V_SLOT2_PAD       CONFIG_MSTAR_SDMMC3_PAD
#elif defined(CONFIG_MS_SDMMC3_PAD)
    #define V_SLOT2_PAD       CONFIG_MS_SDMMC3_PAD
#else
    #define V_SLOT2_PAD       2
#endif

//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC_REVCDZ) || defined(CONFIG_MS_SDMMC_REVCDZ)
    #define EN_SDMMC_CDZREV        (TRUE)
#else
    #define EN_SDMMC_CDZREV        (FALSE)
#endif
//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC_TCARD) || defined(CONFIG_MS_SDMMC_TCARD)
    #define EN_SDMMC_TCARD         (TRUE)
#else
    #define EN_SDMMC_TCARD         (TRUE)
#endif

#if defined(CONFIG_MSTAR_SDMMC_REVWP) || defined(CONFIG_MS_SDMMC_REVWP)
    #define EN_SDMMC_WPREV         (TRUE)
#else
    #define EN_SDMMC_WPREV         (FALSE)
#endif
//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC1_MAXCLK)
    #define V_SDMMC1_MAX_CLK       CONFIG_MSTAR_SDMMC1_MAXCLK
#elif defined(CONFIG_MS_SDMMC1_MAXCLK)
    #define V_SDMMC1_MAX_CLK       CONFIG_MS_SDMMC1_MAXCLK
#else
    #define V_SDMMC1_MAX_CLK       400000
#endif

#if defined(CONFIG_MSTAR_SDMMC1_MAXDLVL)
    #define V_SDMMC1_MAX_DLVL      CONFIG_MSTAR_SDMMC1_MAXDLVL
#elif defined(CONFIG_MS_SDMMC1_MAXDLVL)
    #define V_SDMMC1_MAX_DLVL      CONFIG_MS_SDMMC1_MAXDLVL
#else
    #define V_SDMMC1_MAX_DLVL      0
#endif

#if defined(CONFIG_MSTAR_SDMMC1_PASSLVL)
    #define V_SDMMC1_PASS_LVL      CONFIG_MSTAR_SDMMC1_PASSLVL
#elif defined(CONFIG_MS_SDMMC1_PASSLVL)
    #define V_SDMMC1_PASS_LVL      CONFIG_MS_SDMMC1_PASSLVL
#else
    #define V_SDMMC1_PASS_LVL      0
#endif


#if defined(CONFIG_MSTAR_SDMMC1_INTCDZ) || defined(CONFIG_MS_SDMMC1_INTCDZ)
    #define EN_SDMMC1_INTCDZ       (TRUE)
#else
    #define EN_SDMMC1_INTCDZ       (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC1_FAKECDZ) || defined(CONFIG_MS_SDMMC1_FAKECDZ)
    #define EN_SDMMC1_FAKECDZ      (TRUE)
#else
    #define EN_SDMMC1_FAKECDZ      (FALSE)
#endif
//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC2_MAXCLK)
    #define V_SDMMC2_MAX_CLK       CONFIG_MSTAR_SDMMC2_MAXCLK
#elif defined(CONFIG_MS_SDMMC2_MAXCLK)
    #define V_SDMMC2_MAX_CLK       CONFIG_MS_SDMMC2_MAXCLK
#else
    #define V_SDMMC2_MAX_CLK       400000
#endif

#if defined(CONFIG_MSTAR_SDMMC2_MAXDLVL)
    #define V_SDMMC2_MAX_DLVL      CONFIG_MSTAR_SDMMC2_MAXDLVL
#elif defined(CONFIG_MS_SDMMC2_MAXDLVL)
    #define V_SDMMC2_MAX_DLVL      CONFIG_MS_SDMMC2_MAXDLVL
#else
    #define V_SDMMC2_MAX_DLVL      0
#endif

#if defined(CONFIG_MSTAR_SDMMC2_PASSLVL)
    #define V_SDMMC2_PASS_LVL      CONFIG_MSTAR_SDMMC2_PASSLVL
#elif defined(CONFIG_MS_SDMMC2_PASSLVL)
    #define V_SDMMC2_PASS_LVL      CONFIG_MS_SDMMC2_PASSLVL
#else
    #define V_SDMMC2_PASS_LVL      0
#endif



#if defined(CONFIG_MSTAR_SDMMC2_INTCDZ) || defined(CONFIG_MS_SDMMC2_INTCDZ)
    #define EN_SDMMC2_INTCDZ       (TRUE)
#else
    #define EN_SDMMC2_INTCDZ       (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC2_FAKECDZ) || defined(CONFIG_MS_SDMMC2_FAKECDZ)
    #define EN_SDMMC2_FAKECDZ      (TRUE)
#else
    #define EN_SDMMC2_FAKECDZ      (FALSE)
#endif
//------------------------------------------------------------------------------------------------
#if defined(CONFIG_MSTAR_SDMMC3_MAXCLK)
    #define V_SDMMC3_MAX_CLK       CONFIG_MSTAR_SDMMC3_MAXCLK
#elif defined(CONFIG_MS_SDMMC3_MAXCLK)
    #define V_SDMMC3_MAX_CLK       CONFIG_MS_SDMMC3_MAXCLK
#else
    #define V_SDMMC3_MAX_CLK       400000
#endif

#if defined(CONFIG_MSTAR_SDMMC3_MAXDLVL)
    #define V_SDMMC3_MAX_DLVL      CONFIG_MSTAR_SDMMC3_MAXDLVL
#elif defined(CONFIG_MS_SDMMC3_MAXDLVL)
    #define V_SDMMC3_MAX_DLVL      CONFIG_MS_SDMMC3_MAXDLVL
#else
    #define V_SDMMC3_MAX_DLVL      0
#endif

#if defined(CONFIG_MSTAR_SDMMC3_PASSLVL)
    #define V_SDMMC3_PASS_LVL      CONFIG_MSTAR_SDMMC3_PASSLVL
#elif defined(CONFIG_MS_SDMMC3_PASSLVL)
    #define V_SDMMC3_PASS_LVL      CONFIG_MS_SDMMC3_PASSLVL
#else
    #define V_SDMMC3_PASS_LVL      0
#endif



#if defined(CONFIG_MSTAR_SDMMC3_INTCDZ) || defined(CONFIG_MS_SDMMC3_INTCDZ)
    #define EN_SDMMC3_INTCDZ       (TRUE)
#else
    #define EN_SDMMC3_INTCDZ       (FALSE)
#endif

#if defined(CONFIG_MSTAR_SDMMC3_FAKECDZ) || defined(CONFIG_MS_SDMMC3_FAKECDZ)
    #define EN_SDMMC3_FAKECDZ      (TRUE)
#else
    #define EN_SDMMC3_FAKECDZ      (FALSE)
#endif

//***********************************************************************************************************
//***********************************************************************************************************
typedef enum
{
    EV_SDMMC1 = 0,
    EV_SDMMC2 = 1,
    EV_SDMMC3 = 2,

} SlotEmType;

typedef enum
{
    EV_MUTEX1  = 0,
    EV_MUTEX2  = 1,
    EV_MUTEXS  = 2,
    EV_MUTEX3  = 3,
    EV_NOMUTEX = 4,

} MutexEmType;

struct ms_sdmmc_host
{
    struct platform_device  *pdev;
    struct ms_sdmmc_slot *sdmmc_slot[3];
};

struct ms_sdmmc_slot
{
    struct mmc_host     *mmc;

    unsigned int    slotNo;         //Slot No.
    unsigned int    mieIRQNo;       //MIE IRQ No.
    unsigned int    cdzIRQNo;       //CDZ IRQ No.
    unsigned int    cdzGPIONo;      //CDZ GPIO No.
    unsigned int    pwrGPIONo;      //PWR GPIO No.
    unsigned int    pmrsaveClk;     //Power Saving Clock

    unsigned int    initFlag;       //First Time Init Flag
    unsigned int    sdioFlag;       //SDIO Device Flag

    unsigned int    currClk;        //Current Clock
    unsigned int    currRealClk;    //Current Real Clock
    unsigned char   currWidth;      //Current Bus Width
    unsigned char   currTiming;     //Current Bus Timning
    unsigned char   currPowrMode;   //Current PowerMode
    unsigned char   currBusMode;    //Current Bus Mode
    unsigned short  currVdd;        //Current Vdd
    unsigned char   currDDR;        //Current DDR
    unsigned char   currDownLevel;  //Current Down Level
    unsigned char   currTimeoutCnt; //Current Timeout Count

    int read_only;                  //WP
    int card_det;                   //Card Detect

    /****** DMA buffer used for transmitting *******/
    u32 *dma_buffer;
    dma_addr_t dma_phy_addr;

    /****** ADMA buffer used for transmitting *******/
    u32 *adma_buffer;
    dma_addr_t adma_phy_addr;

    /***** Tasklet for hotplug ******/
    struct tasklet_struct   hotplug_tasklet;

};  /* struct ms_sdmmc_hot*/


#endif // End of __MS_SDMMC_LNX_H

