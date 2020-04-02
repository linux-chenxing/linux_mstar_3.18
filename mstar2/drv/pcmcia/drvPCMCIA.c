//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
/// file    drvPCMCIA.c
/// @brief  PCMCIA Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////

/*****************************************************************************/
/*                       Header Files                                        */
/*****************************************************************************/
// Common Definition
#include <linux/kernel.h>

#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include "mdrv_types.h"
#include "mdrv_system.h"
#include "drvPCMCIA.h"
#include "halPCMCIA.h"
#include "chip_int.h"

/*****************************************************************************/
/*                       Define                                              */
/*****************************************************************************/
#define PCMCIA_MAX_DETECT_COUNT         1
#define PCMCIA_MAX_POLLING_COUNT        20000
#define PCMCIA_DEFAULT_RESET_DURATION   20
#define PCMCIA_MAX_RETRY_COUNT       100      //  PCMCIA hardware register maximum access times

#define PCMCIA_DEBUG_ENABLE             false
#define PCMCIA_DUMP_REG                 false

#define MsOS_DelayTask          mdelay
#define MsOS_DisableInterrupt   disable_irq
#define MsOS_EnableInterrupt    enable_irq
/*****************************************************************************/
/*                       Global Variables                                    */
/*****************************************************************************/

#if PCMCIA_IRQ_ENABLE
static bool  _gbPCMCIA_Irq[E_PCMCIA_MODULE_MAX] = {DISABLE};
static bool  _gbPCMCIA_IrqStatus[E_PCMCIA_MODULE_MAX] = {false};
static int pcmcia_irq = E_IRQ_PCM;
#endif
static u8    _gu8PCMCIA_Command[E_PCMCIA_MODULE_MAX] = {0};
static u8    _gu8HW_ResetDuration    = PCMCIA_DEFAULT_RESET_DURATION;

static struct mutex Pcmcia_Mutex;

/*****************************************************************************/
/*                       Macro                                               */
/*****************************************************************************/
#if PCMCIA_DEBUG_ENABLE
#define PCMCIA_DEBUG( x )           printk x
#else
#define PCMCIA_DEBUG( x )
#endif

#define pcmcia_min( x, y )          ( (u8)x < (u8)y ? (u8)x : (u8)y )

#define PCM_ENTER()     mutex_lock(&Pcmcia_Mutex)
#define PCM_EXIT()      mutex_unlock(&Pcmcia_Mutex)    

#define PCMCIA_FIRE_COMMAND         BIT0
#define PCMCIA_CLEAN_STATE_RD_DONE  BIT1
#define PCMCIA_STATE_RD_DONE        BIT0
#define PCMCIA_STATE_BUS_IDLE       BIT1
#define PCMCIA_DETECT_PIN_MODULEA   BIT2
#define PCMCIA_DETECT_PIN_MODULEB   BIT3

/* Table 2-6 Tuple Summary Tabl (Spec P.24)*/
/* Layer 1 Tuples */
#define CISTPL_NULL                 0x00
#define CISTPL_DEVICE               0x01
#define CISTPL_LONGLINK_CB          0x02
#define CISTPL_INDIRECT             0x03
#define CISTPL_CONFIG_CB            0x04
#define CISTPL_CFTABLE_ENTRY_CB     0x05
#define CISTPL_LONGLINK_MFC         0x06
#define CISTPL_BAR                  0x07
#define CISTPL_PWR_MGMNT            0x08
#define CISTPL_EXTDEVICE            0x09
#define CISTPL_CHECKSUM             0x10
#define CISTPL_LONGLINK_A           0x11
#define CISTPL_LONGLINK_C           0x12
#define CISTPL_LINKTARGET           0x13
#define CISTPL_NO_LINK              0x14
#define CISTPL_VERS_1               0x15
#define CISTPL_ALTSTR               0x16
#define CISTPL_DEVICE_A             0x17
#define CISTPL_JEDEC_C              0x18
#define CISTPL_JEDEC_A              0x19
#define CISTPL_CONFIG               0x1A
#define CISTPL_CFTABLE_ENTRY        0x1B
#define CISTPL_DEVICE_OC            0x1C
#define CISTPL_DEVICE_OA            0x1D
#define CISTPL_DEVICE_GEO           0x1E
#define CISTPL_DEVICE_GEO_A         0x1F
#define CISTPL_MANFID               0x20
#define CISTPL_FUNCID               0x21
#define CISTPL_FUNCE                0x22
#define CISTPL_END                  0xFF
/* Layer 2 Tuples */
#define CISTPL_SWIL                 0x23
#define CISTPL_VERS_2               0x40
#define CISTPL_FORMAT               0x41
#define CISTPL_GEOMETRY             0x42
#define CISTPL_BYTEORDER            0x43
#define CISTPL_DATE                 0x44
#define CISTPL_BATTERY              0x45
#define CISTPL_FORMAT_A             0x47
/* Layer 3 Tuples */
#define CISTPL_ORG                  0x46
/* Layer 4 Tuples */
#define CISTPL_SPCL                 0x90

/*****************************************************************************/
/*                       Local Variables                                     */
/*****************************************************************************/
static bool _gbHighActive;
static bool _gbCardInside[E_PCMCIA_MODULE_MAX];
static bool _gbPCMCIA_Detect_Enable;
static u8    _gu8PCMCIACurModule = PCMCIA_DEFAULT_MODULE;
#if PCMCIA_IRQ_ENABLE
static IsrCallback _fnIsrCallback[E_PCMCIA_MODULE_MAX] = {NULL};
static PCMCIA_ISR _gPCMCIA_ISR;
#endif

/*****************************************************************************/
/*                       Local Functions                                     */
/*****************************************************************************/
static bool _MDrv_PCMCIA_ReadReg( u8 u8Addr, u8 *pu8Value );
static bool _MDrv_PCMCIA_WriteReg( u8 u8Addr, u8 u8Value );

static bool _MDrv_PCMCIA_ReadReg( u8 u8Addr, u8 *pu8Value )
{
    if ( NULL == pu8Value )
    {
        return false;
    }

    *pu8Value = HAL_PCMCIA_Read_Byte( u8Addr );
#if PCMCIA_DUMP_REG
    printk( "R: Addr %02X, Value %02X\n", u8Addr, *pu8Value );
#endif

    return true;
}

static bool _MDrv_PCMCIA_WriteReg( u8 u8Addr, u8 u8Value )
{
    HAL_PCMCIA_Write_Byte( u8Addr , u8Value );
#if PCMCIA_DUMP_REG
    printk( "W: Addr %02X, Value %02X\n", u8Addr, u8Value );
#endif

    return true;
}

static void _MDrv_PCMCIA_SwitchModule(PCMCIA_MODULE eModule)
{
    u8 u8Reg = 0x0;
    
    if(_gu8PCMCIACurModule == eModule)
        return;
        
    _MDrv_PCMCIA_ReadReg( REG_PCMCIA_MODULE_VCC_OOB, (u8 *)&u8Reg );   
    u8Reg &= ~(BIT0|BIT1);

    //  MODULE_SEL[1:0]	1:0	Module select.
    //  00: No destination selected.
    //  01: Select module A.
    //  10: Select module B.
    //  11: Reserved.
    
    if(eModule == E_PCMCIA_MODULE_A)
    {
        u8Reg |= BIT0;
    }
    else
    {
        u8Reg |= BIT1;        
    }
    
    _MDrv_PCMCIA_WriteReg( REG_PCMCIA_MODULE_VCC_OOB, u8Reg );     
    _gu8PCMCIACurModule = eModule;
}

#if PCMCIA_IRQ_ENABLE
static irqreturn_t _MDrv_PCMCIA_Isr( int irq, void *devid )
{
    ISR_STS IsrSts;
    PCMCIA_MODULE eModule = E_PCMCIA_MODULE_A;
    memset(&_gPCMCIA_ISR, 0x00, sizeof(_gPCMCIA_ISR));
    memset(&IsrSts, 0x00, sizeof(ISR_STS));

    //MsOS_DisableInterrupt(pcmcia_irq);
    /* MASK PCMCIA IRQ */
    HAL_PCMCIA_GetIntStatus(&IsrSts);
    HAL_PCMCIA_MaskInt(0x0, true);
    HAL_PCMCIA_ClrInt(0x0);
 
    if(IsrSts.bCardAInsert || IsrSts.bCardARemove || IsrSts.bCardAData) 
    {
        if (_gbPCMCIA_Irq[E_PCMCIA_MODULE_A])
        {
            _gbPCMCIA_IrqStatus[E_PCMCIA_MODULE_A] = true;
        }   
        if(IsrSts.bCardAInsert)
        {
            _gPCMCIA_ISR.bISRCardInsert=true;
        }     
        if(IsrSts.bCardARemove)
        {
            _gPCMCIA_ISR.bISRCardRemove=true;
        }     
        if(IsrSts.bCardAData)
        {
            _gPCMCIA_ISR.bISRCardData=true;
        }     
        if (NULL != _fnIsrCallback[E_PCMCIA_MODULE_A])
        {
            eModule = E_PCMCIA_MODULE_A;
            _fnIsrCallback[E_PCMCIA_MODULE_A]((void*)(&_gPCMCIA_ISR), (void*)&eModule);
        }        
    }    
    else if(IsrSts.bCardBInsert || IsrSts.bCardBRemove || IsrSts.bCardBData) 
    {
        if (_gbPCMCIA_Irq[E_PCMCIA_MODULE_B])
        {
            _gbPCMCIA_IrqStatus[E_PCMCIA_MODULE_B] = true;
        }   
        if(IsrSts.bCardBInsert)
        {
            _gPCMCIA_ISR.bISRCardInsert=true;
        }     
        if(IsrSts.bCardBRemove)
        {
            _gPCMCIA_ISR.bISRCardRemove=true;
        }     
        if(IsrSts.bCardBData)
        {
            _gPCMCIA_ISR.bISRCardData=true;
        }     
        if (NULL != _fnIsrCallback[E_PCMCIA_MODULE_B])
        {
            eModule = E_PCMCIA_MODULE_B;
            _fnIsrCallback[E_PCMCIA_MODULE_B]((void*)(&_gPCMCIA_ISR), (void*)&eModule);
        }        
    }    
    else
    {
        printk("[PCMCIA] IRQ but nothing happen\n");
        //MS_ASSERT( 0 );
    }


    /* Enable HK PCMCIA IRQ */
    //MsOS_EnableInterrupt(pcmcia_irq);
    
    /* UNMASK PCMCIA IRQ */
    HAL_PCMCIA_MaskInt(0x0, false);
    return IRQ_HANDLED;
}
#endif

void _MDrv_PCMCIA_Exit( bool bSuspend )
{
    if(false == bSuspend)
    {
    #if PCMCIA_IRQ_ENABLE
        MsOS_DisableInterrupt( pcmcia_irq );                             // Disable PCMCIA interrupt
        //MsOS_DetachInterrupt( pcmcia_irq );                              // Detach PCMCIA interrupt
    #endif
    }
    else
    {
        // suspend        
    #if PCMCIA_IRQ_ENABLE
        MsOS_DisableInterrupt( pcmcia_irq );                             // Disable PCMCIA interrupt
    #endif        
    }
}

/*****************************************************************************/
/*                       Global Functions                                    */
/*****************************************************************************/
void MDrv_PCMCIA_Init( bool bHighActiveTrigger )
{

    u8 u8Reg;
    u32 i = 0;
            
    _gbHighActive = bHighActiveTrigger;

    mutex_init(&Pcmcia_Mutex);

    for(i = 0; i < E_PCMCIA_MODULE_MAX; i++)
    {
        _gbCardInside[i] = false;
        _gu8PCMCIA_Command[i] = 0;
        _fnIsrCallback[i] = NULL;
        _gbPCMCIA_Irq[i] = DISABLE;
        _gbPCMCIA_IrqStatus[i] = false;
    }
    _gu8PCMCIACurModule = PCMCIA_DEFAULT_MODULE;
    _gbPCMCIA_Detect_Enable = true;    
    
    /* Initailze PCMCIA Registers. */
    _MDrv_PCMCIA_ReadReg( REG_PCMCIA_MODULE_VCC_OOB, (u8 *)&u8Reg );

    u8Reg = ( BIT6 | BIT0 ); // reg_module_sel(BIT 1~0): module select
                                    //                          01: select module A
                                    // reg_single_card(BIT4):   Only support single card
                                    //                          1: support 1 card only
                                    // reg_vcc_en(BIT5):        0: VCC Disable
                                    // reg_oob_en(BIT6):        1: OOB enable

    _MDrv_PCMCIA_WriteReg( REG_PCMCIA_MODULE_VCC_OOB, u8Reg );
   

#if PCMCIA_IRQ_ENABLE
    _MDrv_PCMCIA_WriteReg( REG_PCMCIA_INT_MASK_CLEAR, 0x7C );           // unmask cardA insert/ remove
    //MsOS_DisableInterrupt( pcmcia_irq );                             // Disable PCMCIA interrupt
    //MsOS_DetachInterrupt( E_INT_IRQ_PCM );                              // Detach PCMCIA interrupt
    
    //MsOS_AttachInterrupt( E_INT_IRQ_PCM, (InterruptCb)_MDrv_PCMCIA_Isr );// Attach PCMCIA interrupt
    if(request_irq(pcmcia_irq, _MDrv_PCMCIA_Isr, SA_INTERRUPT, "PCMCIA_IRQ", NULL))
    {
        printk("[K][%s] request pcmcia irq failed\n", __FUNCTION__);
    }
           
    //MsOS_EnableInterrupt( pcmcia_irq );                              // Enable PCMCIA interrupt
#endif

    return;
}

u32 MDrv_PCMCIA_SetPowerState(EN_POWER_MODE u16PowerState)
{
       switch (u16PowerState)
       {
           case E_POWER_RESUME:
                //_MDrv_PCMCIA_InitHW(true);
           break;
           case E_POWER_SUSPEND:
                _MDrv_PCMCIA_Exit(true);
                break;
           case E_POWER_MECHANICAL:                
           case E_POWER_SOFT_OFF:
           default:
                break;
       }
       return true;
}


void MDrv_PCMCIA_Exit( void )
{
    _MDrv_PCMCIA_Exit(false);
}

bool MDrv_PCMCIA_DetectV2( PCMCIA_MODULE eModule )
{
    u8 u8value = 0;
    
    u8 u8DetectPin = 
        (eModule == E_PCMCIA_MODULE_A) ? PCMCIA_DETECT_PIN_MODULEA : PCMCIA_DETECT_PIN_MODULEB;
 
//    if(eModule >= E_PCMCIA_MODULE_MAX)
//    {
//        printk("ERROR: Module 0x%x not support\n", (int)eModule);
//        return false;
//    }
        
    if(!_gbPCMCIA_Detect_Enable)
        return false;

    _MDrv_PCMCIA_ReadReg( REG_PCMCIA_READ_DATA_DONE_BUS_IDLE, &u8value );
    if((u8value & u8DetectPin) != 0)
        return (true == _gbHighActive);
    else
        return (false == _gbHighActive);
}

bool _MDrv_PCMCIA_PollingV2( PCMCIA_MODULE eModule )
{
    bool bCardDetect;
    bool bModuleStatusChange = false;
    
    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return false;
    }
    
    bCardDetect = MDrv_PCMCIA_DetectV2(eModule);
    
    if(_gbCardInside[eModule] != bCardDetect)
        bModuleStatusChange = true;

    _gbCardInside[eModule] = bCardDetect;

    if(bModuleStatusChange)
    {
        if(_gbCardInside[eModule])
        {
            PCMCIA_DEBUG( ( "Card detected\n" ) );
        }
        else
        {
            PCMCIA_DEBUG( ( "Card removed\n" ) );
        }
    }

    return bModuleStatusChange;
}

void _MDrv_PCMCIA_Set_HW_ResetDuration( u8 u8HW_ResetDuration )
{
    _gu8HW_ResetDuration = u8HW_ResetDuration;
}

void MDrv_PCMCIA_ResetHW_V2( PCMCIA_MODULE eModule)
{
    u8 u8Reg = 0;
    u8 bit = (eModule == E_PCMCIA_MODULE_A) ? BIT2 : BIT3;

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return;
    }
    
    PCM_ENTER();
    
#if PCMCIA_IRQ_ENABLE
    MDrv_PCMCIA_Enable_Interrupt( DISABLE );
#endif

    _MDrv_PCMCIA_ReadReg( REG_PCMCIA_MODULE_VCC_OOB, (u8 *)&u8Reg );
    u8Reg |= bit;           //                          1: RESET = HIGH
                            
                            
    _MDrv_PCMCIA_WriteReg( REG_PCMCIA_MODULE_VCC_OOB, u8Reg );
    MsOS_DelayTask( _gu8HW_ResetDuration );   // MUST...for HW reset
    u8Reg &= ~bit;          //                          0: RESET = LOW

    _MDrv_PCMCIA_WriteReg( REG_PCMCIA_MODULE_VCC_OOB, u8Reg );

    /* Comment? */  // FIXME_Alec
    _MDrv_PCMCIA_WriteReg( 0x18, 0xBD );
    _MDrv_PCMCIA_WriteReg( 0x19, 0x00 );
    _MDrv_PCMCIA_WriteReg( 0x1A, 0x31 );

    /* Reset PCMCIA IRQ Mask. */
#if PCMCIA_IRQ_ENABLE
    _MDrv_PCMCIA_WriteReg( REG_PCMCIA_INT_MASK_CLEAR, 0x7C );  //unmask IRQ cardA insert/ remove
#else
    _MDrv_PCMCIA_WriteReg( REG_PCMCIA_INT_MASK_CLEAR, 0x7F );
#endif
    PCM_EXIT();
}

void MDrv_PCMCIA_WriteAttribMemV2( PCMCIA_MODULE eModule, u16 u16Addr, u8 u8Value)
{
    PCMCIA_Map_Info_t       stMapInfo;
    u32 u32Ret = 0;    

//    printk("%s, u16Addr %d, value 0x%x\n", __FUNCTION__, u16Addr, u8Value);

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return;
    }
    
    stMapInfo.u16Addr = u16Addr ;
    stMapInfo.u8Value = u8Value ;
    stMapInfo.u8Type = 1 ;
    
    PCM_ENTER();
    _MDrv_PCMCIA_SwitchModule(eModule);
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
    u32Ret = MDrv_SYS_PCMCIA_WRITE((u32)&stMapInfo, false);
#elif defined(CONFIG_ARM64)
    u32Ret = MDrv_SYS_PCMCIA_WRITE((unsigned long)&stMapInfo, false);
#endif    
    PCM_EXIT();

    if(0 != u32Ret)
    {
        printk("[%s][%d] write data fail\n", __FUNCTION__, __LINE__);
    }
}

void MDrv_PCMCIA_ReadAttribMemV2( PCMCIA_MODULE eModule, u16 u16Addr, u8 *pDest )
{
    PCMCIA_Map_Info_t       stMapInfo;
    u32 u32Ret = 0;    

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return;
    }
    
    stMapInfo.u16Addr = u16Addr ;
    stMapInfo.u8Type = 1 ;

    PCM_ENTER();
    _MDrv_PCMCIA_SwitchModule(eModule);
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
    u32Ret = MDrv_SYS_PCMCIA_READ((u32)&stMapInfo, false);
#elif defined(CONFIG_ARM64)
    u32Ret = MDrv_SYS_PCMCIA_READ((unsigned long)&stMapInfo, false);
#endif        
    if(u32Ret == 0)
    {
        *pDest = stMapInfo.u8Value;
    }
    else
    {
        *pDest = 0xFF;
    }
    PCM_EXIT();
//    printk("%s, u16Addr %d, value 0x%x\n", __FUNCTION__, u16Addr, *pDest);

}

void MDrv_PCMCIA_WriteIOMemV2( PCMCIA_MODULE eModule, u16 u16Addr, u8 u8Value)
{
    PCMCIA_Map_Info_t       stMapInfo;
    u32 u32Ret = 0;    

//    printk("%s, u16Addr %d, value 0x%x\n", __FUNCTION__, u16Addr, u8Value);
    
    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return;
    }
    
    stMapInfo.u16Addr = u16Addr ;
    stMapInfo.u8Value = u8Value;
    stMapInfo.u8Type = 2;
    
    PCM_ENTER();
    _MDrv_PCMCIA_SwitchModule(eModule);
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
    u32Ret = MDrv_SYS_PCMCIA_WRITE((u32)&stMapInfo, false);
#elif defined(CONFIG_ARM64)
    u32Ret = MDrv_SYS_PCMCIA_WRITE((unsigned long)&stMapInfo, false);
#endif        
    PCM_EXIT();

    if(0 != u32Ret)
    {
        printk("[%s][%d] write data fail\n", __FUNCTION__, __LINE__);
    }
}

void MDrv_PCMCIA_WriteIOMemLongV2( PCMCIA_MODULE eModule, u16 u16Addr, u8 u8Value, u16 u16DataLen, u8* u8pWriteBuffer)
{
    PCMCIA_Map_Info_t       stMapInfo;
    u32 u32Ret = 0;

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return;
    }
    stMapInfo.u16Addr = u16Addr ;
    stMapInfo.u8Value = u8Value;
    stMapInfo.u8Type = 3;
    stMapInfo.u16DataLen = u16DataLen;
    stMapInfo.u8pWriteBuffer = u8pWriteBuffer;
    PCM_ENTER();
    _MDrv_PCMCIA_SwitchModule(eModule);    
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
    u32Ret = MDrv_SYS_PCMCIA_WRITE((u32)&stMapInfo, false);
#elif defined(CONFIG_ARM64)
    u32Ret = MDrv_SYS_PCMCIA_WRITE((unsigned long)&stMapInfo, false);
#endif    
    PCM_EXIT();
    
    if(0 != u32Ret)
    {
        printk("[%s][%d] write data fail\n", __FUNCTION__, __LINE__);
    }    
}

//! This function is read one byte of from the card IO memory at address wAddr.
u8 MDrv_PCMCIA_ReadIOMemV2( PCMCIA_MODULE eModule, u16 u16Addr )
{
    PCMCIA_Map_Info_t       stMapInfo;
    u32 u32Ret = 0;
    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return 0;
    }
    stMapInfo.u16Addr = u16Addr ;
    stMapInfo.u8Value = 0;
    stMapInfo.u8Type = 2 ;
    PCM_ENTER();
    _MDrv_PCMCIA_SwitchModule(eModule);    
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
    u32Ret = MDrv_SYS_PCMCIA_READ((u32)&stMapInfo, false);
#elif defined(CONFIG_ARM64)
    u32Ret = MDrv_SYS_PCMCIA_READ((unsigned long)&stMapInfo, false);
#endif        

    PCM_EXIT();

    if(0 == u32Ret)
    {
//        printk("%s, u16Addr %d, value 0x%x\n", __FUNCTION__, u16Addr, stMapInfo.u8Value);
        
        return stMapInfo.u8Value;
    }
    else
    {
        return 0xFF;
    }
}


#if 0

bool _MDrv_PCMCIA_IsModuleStillPluggedV2( PCMCIA_MODULE eModule )
{
    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return false;
    }
    
    return ( _gbCardInside[eModule] );
}

void _MDrv_PCMCIA_Set_Detect_Trigger( bool bHighActive )
{
    _gbHighActive = bHighActive;
    return;
}

void _MDrv_PCMCIA_Set_Detect_Enable( bool bEnable )
{
    _gbPCMCIA_Detect_Enable = bEnable;
}

void _MDrv_PCMCIA_ParseAttribMem( u8 *pAttribMem, u16 dwLen, PCMCIA_INFO *pInfo )
{
    u8 bTag          = 0;
    u8 bTagLen       = 0;
    u16 i            = 0;
    u8 FieldSize     = 0;
    //u8 LastIndex     = 0;
    //u8 MaskByte      = 0;
    //u8 SubTagByte    = 0;
    u8 SubTagLen     = 0;
    //u16 STCI_IFN     = 0;
    u8 fInterface    = 0;
    char CiDetStr[20];

    if ( ( NULL == pAttribMem ) || ( NULL == pInfo ) )
    {
        return;
    }

    memset( pInfo, 0x00, sizeof( PCMCIA_INFO ) );
    do
    {
        bTag = pAttribMem[0];
        bTagLen = pAttribMem[1];
        if ( (u16)bTagLen + 2 > dwLen )
        {
            return ;
        }

        dwLen -= ( bTagLen + 2 );
        PCMCIA_DEBUG( ( "Parse_PCMCIA_AttribMem:bTag[0x%02X] dwLen[%d bytes]\n", bTag, dwLen ) );
        switch ( bTag )
        {
            case CISTPL_NULL:
                PCMCIA_DEBUG( ( "Parse_PCMCIA_AttribMem case 0x00\n" ) );
                break;

            case CISTPL_DEVICE:
                PCMCIA_DEBUG( ( "CISTPL_DEVICE\n" ) );
                // Memory type
                // Access speed
                // Memory Size
                break;
            case CISTPL_LINKTARGET:
                PCMCIA_DEBUG( ( "CISTPL_LINKTARGET\n" ) );
                // "CIS"
                break;
            case CISTPL_NO_LINK:
                //PCMCIA_DEBUG(("CISTPL_NOLINK\n"));
                break;
            case CISTPL_VERS_1:
                pInfo->wPCMCIAStdRev = (u16)pAttribMem[2] << 8 | (u16)pAttribMem[3];
                pInfo->dwValidFlags |= PCMCIAINFO_VERS1_VALID;
                i = 4;
                if ( bTagLen < 2 )
                {
                    break;
                } // error
                memcpy( pInfo->pszManufacturerName, pAttribMem + i,
                        pcmcia_min( bTagLen + 2 - i, MAX_PCMCIA_STRLEN - 1 ) );
                pInfo->pszManufacturerName[pcmcia_min( bTagLen + 2 - i, MAX_PCMCIA_STRLEN - 1 )] = '\0';
                i += strlen( ( char* )pAttribMem + i ) + 1;
                if ( i < bTagLen + 2 )
                {
                    memcpy( pInfo->pszProductName, pAttribMem + i,
                            pcmcia_min( bTagLen + 2 - i, MAX_PCMCIA_STRLEN - 1 ) );
                    pInfo->pszProductName[pcmcia_min( bTagLen + 2 - i, MAX_PCMCIA_STRLEN - 1 )] = '\0';
                    i += strlen( ( char* )pAttribMem + i ) + 1;
                }

                if ( 0xFF != *( pAttribMem + i ) )
                {
                    if ( i < bTagLen + 2 )
                    {
                        memcpy( pInfo->pszProductInfo1, pAttribMem + i,
                                pcmcia_min( bTagLen + 2 - i, MAX_PCMCIA_STRLEN - 1 ) );
                        pInfo->pszProductInfo1[pcmcia_min( bTagLen + 2 - i, MAX_PCMCIA_STRLEN - 1 )] = '\0';
                        i += strlen( ( char* )pAttribMem + i ) + 1;

                        /* $compatible[ciplus=1]$  */
                        {
                            u8 u8aCI_PLUS_STRING1[] = "$compatible[ciplus=";
                            u8 u8aCI_PLUS_STRING2[] = "$COMPATIBLE[CIPLUS=";
                            if ( ( 0 == memcmp( pInfo->pszProductInfo1, u8aCI_PLUS_STRING1, sizeof( u8aCI_PLUS_STRING1 ) - 1 ) ) ||
                                 ( 0 == memcmp( pInfo->pszProductInfo1, u8aCI_PLUS_STRING2, sizeof( u8aCI_PLUS_STRING2 ) - 1 ) ) )
                            {
                                PCMCIA_DEBUG( ( "CI+ CAM\n" ) );
                                pInfo->bCI_PLUS = 1;
                            }
                        }
                    }

                    if ( i < bTagLen + 2 )
                    {
                        memcpy( pInfo->pszProductInfo2, pAttribMem + i,
                                pcmcia_min( bTagLen + 2 - i, MAX_PCMCIA_STRLEN - 1 ) );
                        pInfo->pszProductInfo2[pcmcia_min( bTagLen + 2 - i, MAX_PCMCIA_STRLEN - 1 )] = '\0';
                    }
                }
                #if 0//PCMCIA_DEBUG_ENABLE
                    printk( "ManufacturerName %s \n", pInfo->pszManufacturerName );
                    printk( "ProductName %s \n", pInfo->pszProductName );
                    printk( "ProductInfo1 %s \n", pInfo->pszProductInfo1 );
                    printk( "ProductInfo2 %s \n", pInfo->pszProductInfo2 );
                #endif
                break;
            case CISTPL_DEVICE_A:
                //PCMCIA_DEBUG( ( "CISTPL_DEVICE_A\n" ) );
                // Like 0x01... for device(s) in attribute memory
                // Memory type
                // Access speed
                // Memory Size
                break;
            case CISTPL_CONFIG:
                PCMCIA_DEBUG( ( "CISTPL_CONFIG\n" ) );
                /*
                {
                    u8 bNumAddrBytes;
                    u8 bNumConfigRegs;
                    u8 bLastIndex;
                    bNumAddrBytes = (pAttribMem[2]&0x03)+1;
                    bNumConfigRegs = ((pAttribMem[2]>>2)&0x0F)+1;
                    bLastIndex = pAttribMem[3]&0x3F;
                    for ( i = 0; i < bNumAddrBytes; i++ )
                    {
                        pInfo->ConfigOffset = pInfo->ConfigOffset<<8;
                        pInfo->ConfigOffset |= pAttribMem[4+i];
                        PCMCIA_DEBUG( ( "ConfigOffset %08lX\n", pInfo->ConfigOffset ) );
                    }
                    i++;
                }
                */
                FieldSize = pAttribMem[2];
                //LastIndex = pAttribMem[3];
                if ( FieldSize == 0 )
                {
                    pInfo->ConfigOffset = pAttribMem[4];
                }
                else if ( FieldSize == 1 )
                {
                    pInfo->ConfigOffset = (u32)pAttribMem[5] << 8 | (u32)pAttribMem[4];
                }
                PCMCIA_DEBUG( ( "ConfigOffset 0x%lx\n", pInfo->ConfigOffset ) );
                //MaskByte = pAttribMem[5 + FieldSize];
                //SubTagByte = pAttribMem[6 + FieldSize];
                SubTagLen = pAttribMem[7 + FieldSize];
                //STCI_IFN = (u16)pAttribMem[9 + FieldSize] << 8 | (u16)pAttribMem[8 + FieldSize];
                memcpy( CiDetStr, pAttribMem + 10 + FieldSize, pcmcia_min( SubTagLen - 2, sizeof( CiDetStr ) - 1 ) );
                CiDetStr[pcmcia_min( SubTagLen - 2, sizeof( CiDetStr ) - 1 )] = 0;
                //PCMCIA_DEBUG( ( "FieldSize %d, LastIndex %d MaskByte %02X SubTag %02X ID %02X %s\n", FieldSize, LastIndex, MaskByte, SubTagByte, STCI_IFN, CiDetStr ) );
                break;
            case CISTPL_CFTABLE_ENTRY:
                PCMCIA_DEBUG( ( "CISTPL_CFTABLE_ENTRY\n" ) );
                {
                    //u8 bIF = 0;
                    u8 bFlags = 0;
                    u16 j = 0;

                    pInfo->bINT = false; // It's MUST because there are multiple CISTPL_CFTABLE_ENTRYs.

                    pInfo->Config[pInfo->bNumConfigs].bConfigIndex = ( pAttribMem[2] & 0x3F );
                    PCMCIA_DEBUG(("Config Entry value 0x02%X\n",pInfo->Config[pInfo->bNumConfigs].bConfigIndex));
                    if ( pAttribMem[2] & 0x40 )
                    {
                        ;
                    } // Default
                    if ( pAttribMem[2] & 0x80 )
                    {
                        //bIF = pAttribMem[3];
                        i = 4;
                    }
                    else
                    {
                        i = 3;
                    }
                    bFlags = pAttribMem[i]; // TPCE_FS: Feature Selection Byte
                    i++;

                    if ( bFlags & (BIT0 | BIT1) ) // BIT0 | BIT1 = TPCE_FS: Power
                    {/* TPCE_PD: Power Description Structure */
                        u8 bPowerBits = pAttribMem[i];   // Parameter Selection Byte

                        //PCMCIA_DEBUG( ( "PowerDesc %02X\n", bPowerBits ) );
                        i++;
                        for ( j = 0; j < 7; j++ )
                        {
                            if ( ( bPowerBits >> j ) & 0x01 )
                            {
                                i++;
                            }
                            while ( pAttribMem[i] & 0x80 )
                            {
                                i++;
                            } // extension byte
                        }
                    }

                    if ( bFlags & BIT2 ) // BIT2 = Timing
                    {/* TPCE_TD: Configuration Timing Information */
                        //PCMCIA_DEBUG( ( "TimingDesc %02X\n", pAttribMem[i] ) );
                        i++;
                    }

                    if ( bFlags & BIT3 ) // BIT3 = IO Space
                    {/* TPCE_IO: I/O Space Addresses Required For This Configuration */
                        if ( pAttribMem[i] & 0x80 ) // BIT7 = Range
                        {
                            /* Range = 1: The I/O Space definition byte is followed by an I/O Range Descriptor byte, and
                                          one or more I/O Address Range Description fields.
                            */
                            u8 bAddrBytes;
                            u8 bLenBytes;
                            //u8 bNumDesc;
                            u32 dwEALen = 0;
                            u32 dwEAAddr = 0;
                            //bNumDesc = pAttribMem[i + 1] & 0x0F;            // I/O Range Descriptor Byte: Number of I/O Address Ranges (Bit 0~3))
                            bAddrBytes = ( pAttribMem[i + 1] >> 4 ) & 0x03; // I/O Range Descriptor Byte: Size of Address (Bit 4~5)
                            bLenBytes = ( pAttribMem[i + 1] >> 6 ) & 0x03;  // I/O Range Descriptor Byte: Size of Length (Bit 6~7)
                            //PCMCIA_DEBUG( ( "EADesc %02X %d %d %d\n", pAttribMem[i + 1], bNumDesc, bAddrBytes, bLenBytes ) );
                            i += 2;
                            switch ( bAddrBytes )
                            {
                                case 1:
                                    dwEAAddr = pAttribMem[i];
                                    break;
                                case 2:
                                    dwEAAddr = (u32)pAttribMem[i] | (u32)pAttribMem[i + 1] << 8;
                                    break;
                            }
                            pInfo->Config[pInfo->bNumConfigs].dwEAAddr = dwEAAddr;
                            i += bLenBytes;
                            switch ( bLenBytes )
                            {
                                case 1:
                                    dwEALen = pAttribMem[i];
                                    break;
                                case 2:
                                    dwEALen = (u32)pAttribMem[i] | (u32)pAttribMem[i + 1] << 8;
                                    break;
                            }
                            pInfo->Config[pInfo->bNumConfigs].dwEALen = dwEALen;
                            i += bAddrBytes;
                            //PCMCIA_DEBUG( ( "Addr %04X Len %04X", dwEAAddr, dwEALen ) );
                        }
                        else
                        {
                            i++;
                        }
                    }

                    if ( bFlags & BIT4 ) // BIT4 = IRQ
                    {/* TPCE_IR: Interrupt Request Description Structure */
                        PCMCIA_DEBUG( ( "IrqDesc: 0x%02X\n", pAttribMem[i] ) );
                        pInfo->Config[pInfo->bNumConfigs].bIRQDesc1 = pAttribMem[i];
                        if ( ( pAttribMem[i] & BIT5) && ( !(pAttribMem[i] & (~BIT5) ) ) )
                        {
                            pInfo->bINT = ENABLE;
                        }
                        else if ( pAttribMem[i] & BIT4 )
                        {
                            pInfo->Config[pInfo->bNumConfigs].wIRQData = (u16)pAttribMem[i + 1] << 8 |
                                                                         (u16)pAttribMem[i + 2];
                            i += 2;
                        }
                        i++;
                    }
                    if ( bFlags & 0x60 )
                    {
                        PCMCIA_DEBUG( ( "MemoryDesc\n" ) );
                        i++;
                    }
                    if ( bFlags & 0x80 )
                    {
                        PCMCIA_DEBUG( ( "MixedDesc\n" ) );
                        i++;
                    }

                    while ( i < ( bTagLen + 2 ) )
                    {
                        PCMCIA_DEBUG( ( "SubTag 0x%02X %d %d\n", pAttribMem[i], i, bTagLen ) );
                        if ( pAttribMem[i] == 0xc0 )
                        {
                            if ( strcmp( ( char* )pAttribMem + i + 2, "DVB_HOST" ) == 0 )
                            {
                                pInfo->Config[pInfo->bNumConfigs].fCITagsPresent |= 0x01;
                            }
                            PCMCIA_DEBUG( ( "%s\n", pAttribMem + i + 2 ) );
                        }
                        if ( pAttribMem[i] == 0xc1 )
                        {
                            if ( strcmp( ( char* )pAttribMem + i + 2, "DVB_CI_MODULE" ) == 0 )
                            {
                                pInfo->Config[pInfo->bNumConfigs].fCITagsPresent |= 0x02;
                            }
                            PCMCIA_DEBUG( ( "%s\n", pAttribMem + i + 2 ) );
                        }
                        i += pAttribMem[i + 1] + 2;
                    }

                    pInfo->bNumConfigs++;
                }
                fInterface = ( pAttribMem[2] >> 6 ) & 0x01;
                if ( fInterface )
                {
                    //PCMCIA_DEBUG( ( "IF %02X ", pAttribMem[3] ) );
                }

                #if 0
                PCMCIA_DEBUG( ( "\n" ) );

                for ( i = 0; i < bTagLen; i++ )
                {
                    PCMCIA_DEBUG( ( "%02X ", pAttribMem[2 + i] ) );
                }
                PCMCIA_DEBUG( ( "\n" ) );
                for ( i = 0; i < bTagLen; i++ )
                {
                    PCMCIA_DEBUG( ( "%c  ", pAttribMem[2 + i] ) );
                }
                PCMCIA_DEBUG( ( "\n" ) );
                #endif
                break;
            case CISTPL_DEVICE_OC:
                PCMCIA_DEBUG( ( "CISTPL_DEVICE_OC\n" ) );
                break;
            case CISTPL_DEVICE_OA:
                PCMCIA_DEBUG( ( "CISTPL_DEVICE_OA\n" ) );
                break;
            case CISTPL_MANFID:
                PCMCIA_DEBUG( ( "CISTPL_MANFID\n" ) );
                pInfo->dwValidFlags |= PCMCIAINFO_MANID_VALID;
                pInfo->wManufacturerId = (u16)pAttribMem[2] << 8 | (u16)pAttribMem[3];
                pInfo->wCardID = (u16)pAttribMem[4] << 8 | (u16)pAttribMem[5];
                PCMCIA_DEBUG( ( "Manufacturer code %04X Product Code %04X\n",
                                pInfo->wManufacturerId, pInfo->wCardID ) );
                #if 0//PCMCIA_DUMP_CISMANID
                printk( "wManufacturerId %x \n", pInfo->wManufacturerId );
                printk( "CardID %x \n", pInfo->wCardID );
                #endif
                break;
            case CISTPL_FUNCID:
                PCMCIA_DEBUG( ( "CISTPL_FUNCID\n" ) );
                pInfo->dwValidFlags |= PCMCIAINFO_FUNCID_VALID;
                pInfo->FuncType = ( PCMCIA_FUNCTYPE )pAttribMem[2];
                pInfo->bFuncIDSysInfo = pAttribMem[3];
                break;
            case CISTPL_VERS_2:
                // CISTPL_VERS2
                PCMCIA_DEBUG( ( "Parse_PCMCIA_AttribMem case 0x40\n" ) );
                break;
            default:
                #if 0
                PCMCIA_DEBUG( ( "Parse_PCMCIA_AttribMem case else\n" ) );
                PCMCIA_DEBUG( ( "Tag %02X, Len %d\n", bTag, bTagLen ) );
                for ( i = 0; i < bTagLen; i++ )
                {
                    PCMCIA_DEBUG( ( "%02X ", pAttribMem[2 + i] ) );
                }
                PCMCIA_DEBUG( ( "\n" ) );
                for ( i = 0; i < bTagLen; i++ )
                {
                    PCMCIA_DEBUG( ( "%c", pAttribMem[2 + i] ) );
                }
                PCMCIA_DEBUG( ( "\n" ) );
                #endif
                break;
        }
        pAttribMem += ( 2 + (u16)bTagLen );
    }
    while ( ( bTag != 0x14 ) && ( bTag != 0xFF ) && ( dwLen ) );

    return;
}

bool MDrv_PCMCIA_ResetInterfaceV2( PCMCIA_MODULE eModule )
{
    bool bRet = true;

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return false;
    }

    _gu8PCMCIA_Command[eModule] = 0x00;  // Reset CI Command

    MsOS_DelayTask( 300 );  // The waiting time in for loop of _MDrv_PCMCIA_WaitForStatusBit() is the key factor.
    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_RESET, true );    // Up RS

    bRet = _MDrv_PCMCIA_WaitForStatusBitV2( eModule, PCMCIA_STATUS_FREE );

    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_RESET, false );   // Down RS
    
    // For some specific CAMs, it sets the Free bit after RS bit is clear.
    if(bRet == false)
    {
        bRet = MDrv_PCMCIA_WaitForStatusBitV2( eModule, PCMCIA_STATUS_FREE );
    }
    return bRet;
}

void _MDrv_PCMCIA_SetCommandBitV2( PCMCIA_MODULE eModule, u8 u8CommandBit, bool bValue )
{
    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return;
    }
        
    if ( bValue )
        _gu8PCMCIA_Command[eModule] |= u8CommandBit;
    else
        _gu8PCMCIA_Command[eModule] &= ( ~u8CommandBit );

    _MDrv_PCMCIA_WriteIOMemV2( eModule, PCMCIA_PHYS_REG_COMMANDSTATUS, _gu8PCMCIA_Command[eModule] );
}

bool _MDrv_PCMCIA_IsDataAvailableV2( PCMCIA_MODULE eModule )
{
    u8 u8StatusBit = 0;

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return false;
    }
    u8StatusBit = _MDrv_PCMCIA_ReadIOMemV2( eModule, PCMCIA_PHYS_REG_COMMANDSTATUS );

    return ( u8StatusBit & PCMCIA_STATUS_DATAAVAILABLE );
}

u16 _MDrv_PCMCIA_ReadDataV2( PCMCIA_MODULE eModule, u8* u8pReadBuffer, u16 u16ReadBufferSize )
{
    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return false;
    }    
    
    PCMCIA_Map_Info_t       stMapInfo;
    stMapInfo.u16DataLen = u16ReadBufferSize;
    stMapInfo.u8pReadBuffer = u8pReadBuffer;
    u32 u32Ret = UTOPIA_STATUS_FAIL;

    PCM_ENTER();
    _MDrv_PCMCIA_SwitchModule(eModule);

    u32Ret = ioctl(SYS_fd, IOCTL_SYS_PCMCIA_READ_DATA , &stMapInfo);
    
    PCM_EXIT();
    
    if(UTOPIA_STATUS_SUCCESS != u32Ret)
    {
        return 0;
    }
    else
    {
        return stMapInfo.u16DataLen;
    }
}

bool _MDrv_PCMCIA_WriteDataV2( PCMCIA_MODULE eModule, u8* u8pWriteBuffer, u16 u16DataLen )
{
    u16  u16TryLoop = 0;
    u16 i = 0;
    
    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return false;
    }    

    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_HOSTCONTROL, true );      // Up HC
    while ( ( !( _MDrv_PCMCIA_ReadIOMemV2( eModule, PCMCIA_PHYS_REG_COMMANDSTATUS ) & PCMCIA_STATUS_FREE ) ) &&
            ( u16TryLoop < PCMCIA_MAX_RETRY_COUNT ) )
    {
        _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_HOSTCONTROL, false ); // Down HC
        MsOS_DelayTask( 1 );
        _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_HOSTCONTROL, true );  // Up HC
        u16TryLoop++;
    }

    if ( PCMCIA_MAX_RETRY_COUNT == u16TryLoop )
    {
        printk( "MDrv_PCMCIA_WriteData: not \"free\", retry %d times fail!\r\n", u16TryLoop);
        return false;
    }

    _MDrv_PCMCIA_WriteIOMemV2( eModule, PCMCIA_PHYS_REG_SIZELOW, (u8)( u16DataLen & 0xFF ) );
    _MDrv_PCMCIA_WriteIOMemV2( eModule, PCMCIA_PHYS_REG_SIZEHIGH, (u8)( u16DataLen >> 8 ) );
    _MDrv_PCMCIA_WriteIOMemLongV2( eModule, PCMCIA_PHYS_REG_DATA, u8pWriteBuffer[i], u16DataLen, u8pWriteBuffer );

    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_HOSTCONTROL, false );     // Down HC
    return true;
}

bool _MDrv_PCMCIA_SwitchToIOmodeV2( PCMCIA_MODULE eModule, PCMCIA_INFO *pInfo )
{
    u8 i = 0;

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return false;
    }    
    
    MsOS_DelayTask( 1000 );
    
    for ( i = 0; i < pInfo->bNumConfigs; i++ )
    {
        if ( pInfo->Config[i].fCITagsPresent == 0x03 )
        {
            _MDrv_PCMCIA_WriteAttribMemV2( eModule, (u16)( pInfo->ConfigOffset ), pInfo->Config[i].bConfigIndex );    //switch to i/o mode

            return true;
        }
    }

    return false;
}

u16 _MDrv_PCMCIA_NegotiateBufferSizeV2( PCMCIA_MODULE eModule, PCMCIA_INFO *pInfo )
{
    u16 u16BufferSize;

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return 0;
    }    
    
    if ( NULL == pInfo )
    {
        PCMCIA_DEBUG( ( "ERROR: MDrv_PCMCIA_NegotiateBufferSize: NULL == pInfo!\n" ) );
        return 0;
    }

    // write size read
    PCMCIA_DEBUG( ( "Write Size Read\n" ) );
    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_SIZEREAD, true );         // Up SR
    if ( !_MDrv_PCMCIA_WaitForStatusBitV2( eModule, PCMCIA_STATUS_DATAAVAILABLE ) )
    {
        PCMCIA_DEBUG( ( "ERROR: MDrv_PCMCIA_NegotiateBufferSize NG!\n" ) );
        _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_SIZEREAD, false );    // Down SR
        return 0;
    }

    // Dump Buffersize
    // read the size (this is always 0002, byteswapped)
    u16BufferSize = (u16)_MDrv_PCMCIA_ReadIOMemV2( eModule, PCMCIA_PHYS_REG_SIZEHIGH ) << 8 |
                    (u16)_MDrv_PCMCIA_ReadIOMemV2( eModule, PCMCIA_PHYS_REG_SIZELOW );
    PCMCIA_DEBUG( ( "Datalen %04X\n", u16BufferSize ) );
    // if the module returned an invalid data size, initiate a reset
    if ( u16BufferSize != 0x0002 )
    {
        PCMCIA_DEBUG( ( "ERROR: MDrv_PCMCIA_NegotiateBufferSize: Invalid BufferSize!\n" ) );
        _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_SIZEREAD, false );    // Down SR
        return 0;
    }
    u16BufferSize = (u16)_MDrv_PCMCIA_ReadIOMemV2( eModule, PCMCIA_PHYS_REG_DATA ) << 8;
    u16BufferSize |= (u16)_MDrv_PCMCIA_ReadIOMemV2( eModule, PCMCIA_PHYS_REG_DATA );
    PCMCIA_DEBUG( ( "BufferSize %02X\n", u16BufferSize ) );

    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_SIZEREAD, false );        // Down SR

    return u16BufferSize;
}

void _MDrv_PCMCIA_WriteBufferSizeV2( PCMCIA_MODULE eModule, u16 u16BufferSize )
{
    u16 u16TryLoop = 0;

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return;
    }    
    
    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_SIZEWRITE, true );        // Up SW
    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_HOSTCONTROL, true );      // Up HC

    while ( ( !( _MDrv_PCMCIA_ReadIOMemV2( eModule, PCMCIA_PHYS_REG_COMMANDSTATUS ) & PCMCIA_STATUS_FREE ) ) &&
            ( u16TryLoop < PCMCIA_MAX_RETRY_COUNT ) )
    {
        _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_HOSTCONTROL, false ); // Down HC
        MsOS_DelayTask( 1 );
        _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_HOSTCONTROL, true );  // Up HC
        u16TryLoop++;
    }

    if ( PCMCIA_MAX_RETRY_COUNT == u16TryLoop )
    {
        printk( "ERROR: MDrv_PCMCIA_WriteBufferSize NG!\n" );
    }

    _MDrv_PCMCIA_WriteIOMemV2( eModule, PCMCIA_PHYS_REG_SIZELOW, 0x02 );
    _MDrv_PCMCIA_WriteIOMemV2( eModule, PCMCIA_PHYS_REG_SIZEHIGH, 0x00 );
    _MDrv_PCMCIA_WriteIOMemV2( eModule, PCMCIA_PHYS_REG_DATA, (u8)( u16BufferSize >> 8 ) );
    _MDrv_PCMCIA_WriteIOMemV2( eModule, PCMCIA_PHYS_REG_DATA, (u8)( u16BufferSize ) );

    MsOS_DelayTask( 50 );
    
    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_HOSTCONTROL, false );     // Down HC
    _MDrv_PCMCIA_SetCommandBitV2( eModule, PCMCIA_COMMAND_SIZEWRITE, false );       // Down SW

    PCMCIA_DEBUG( ( "Write Buffersize: 0x%04X bytes\n", u16BufferSize ) );
    MsOS_DelayTask( 100 );//Black Viacess
}

#endif

u8 MDrv_PCMCIA_ReadyStatus( void )
{
    u8 u8Reg = 0,  u8Reg2 = 0, temp = 0;

    _MDrv_PCMCIA_ReadReg( REG_PCMCIA_INT_MASK_CLEAR, (u8 *) &temp );
    _MDrv_PCMCIA_WriteReg( REG_PCMCIA_INT_MASK_CLEAR, 0x7B );

    _MDrv_PCMCIA_ReadReg( REG_PCMCIA_STAT_INT_RAW_INT, (u8 *) &u8Reg );
        printk("REG_PCMCIA_STAT_INT_RAW_INT %x\n",u8Reg );

    _MDrv_PCMCIA_ReadReg( REG_PCMCIA_STAT_INT_RAW_INT1, (u8 *)&u8Reg2 );
        printk("REG_PCMCIA_STAT_INT_RAW_INT1 %x\n",u8Reg2 );

    _MDrv_PCMCIA_WriteReg( REG_PCMCIA_INT_MASK_CLEAR, temp );

    return ( u8Reg );

}

#if PCMCIA_IRQ_ENABLE


void MDrv_PCMCIA_Set_InterruptStatusV2( PCMCIA_MODULE eModule, bool Status )
{
    _gbPCMCIA_IrqStatus[eModule] = Status;
}

bool MDrv_PCMCIA_Get_InterruptStatusV2( PCMCIA_MODULE eModule )
{
    return _gbPCMCIA_IrqStatus[eModule];
}

void MDrv_PCMCIA_Enable_InterruptV2( PCMCIA_MODULE eModule, bool bEnable )
{
    u8 u8Reg;

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return;
    }  
        
    MDrv_PCMCIA_Set_InterruptStatusV2( eModule, false );

    if ( ENABLE == bEnable )
    {
        _gbPCMCIA_Irq[eModule] = ENABLE;

        /* Enable MPU PCMCIA IRQ. */
        MsOS_EnableInterrupt( pcmcia_irq );

        /* Enable IP PCMCIA IRQ. */
        _MDrv_PCMCIA_ReadReg( REG_PCMCIA_INT_MASK_CLEAR, (u8 *)&u8Reg );
        if(eModule == E_PCMCIA_MODULE_A)
        {
            u8Reg &= ( ~BIT2 );
            u8Reg &= ( ~BIT1 );
            u8Reg &= ( ~BIT0 );
        }
        else 
        { // Module B
            u8Reg &= ( ~BIT5 );
            u8Reg &= ( ~BIT4 );
            u8Reg &= ( ~BIT3 );
        }
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_INT_MASK_CLEAR, u8Reg );
    }
    else
    {
        _gbPCMCIA_Irq[eModule] = DISABLE;

        /* Here DON"T Disable MPU PCMCIA IRQ. */
        /* Disable IP PCMCIA IRQ. */
        _MDrv_PCMCIA_ReadReg( REG_PCMCIA_INT_MASK_CLEAR, (u8 *)&u8Reg );
        if(eModule == E_PCMCIA_MODULE_A)
        {
            u8Reg |= BIT2; //Don't mask cardA insert/remove
        }
        else
        { // Module B
            u8Reg |= BIT5;
        }
        _MDrv_PCMCIA_WriteReg( REG_PCMCIA_INT_MASK_CLEAR, u8Reg );
    }
}

void MDrv_PCMCIA_InstarllIsrCallbackV2( PCMCIA_MODULE eModule, IsrCallback fnIsrCallback )
{
    _fnIsrCallback[eModule] = fnIsrCallback;
}
#endif

bool _MDrv_PCMCIA_WaitForStatusBitV2( PCMCIA_MODULE eModule, u8 u8StatusBit )
{
    u16 i = 0;

    if(eModule >= E_PCMCIA_MODULE_MAX)
    {
        printk("ERROR: Module 0x%x not support\n", (int)eModule);
        return false;
    }    
    
    for ( i = 0; i < 500; i++ )
    {
        if ( MDrv_PCMCIA_ReadIOMemV2( eModule, PCMCIA_PHYS_REG_COMMANDSTATUS ) & u8StatusBit )
        {
            return true;
        }
        MsOS_DelayTask( 15 );
        if(!MDrv_PCMCIA_DetectV2(eModule))
        {
            PCMCIA_DEBUG( ( "ERROR: Card Removed\n" ) );
            return false;
        }
    }

    PCMCIA_DEBUG( ( "ERROR: Waiting Timeout for PCMCIA Status Bit\n" ) );

    return false;
}

// backward compatible

#if PCMCIA_IRQ_ENABLE

void MDrv_PCMCIA_Enable_Interrupt( bool bEnable )
{
    MDrv_PCMCIA_Enable_InterruptV2(PCMCIA_DEFAULT_MODULE, bEnable);
}

void MDrv_PCMCIA_Set_InterruptStatus( bool Status )
{
    MDrv_PCMCIA_Set_InterruptStatusV2(PCMCIA_DEFAULT_MODULE, Status);
}

bool MDrv_PCMCIA_Get_InterruptStatus( void )
{
    return MDrv_PCMCIA_Get_InterruptStatusV2(PCMCIA_DEFAULT_MODULE);
}

void MDrv_PCMCIA_InstarllIsrCallback( IsrCallback fnIsrCallback )
{
    MDrv_PCMCIA_InstarllIsrCallbackV2(PCMCIA_DEFAULT_MODULE, fnIsrCallback);
}

#endif // PCMCIA_IRQ_ENABLE

#if 0

bool MDrv_PCMCIA_Polling( void )
{
    return MDrv_PCMCIA_PollingV2(PCMCIA_DEFAULT_MODULE);
}

bool MDrv_PCMCIA_IsModuleStillPlugged( void )
{
    return MDrv_PCMCIA_IsModuleStillPluggedV2(PCMCIA_DEFAULT_MODULE);
}

void MDrv_PCMCIA_SetCommandBit( u8 u8CommandBit, bool bValue )
{
    MDrv_PCMCIA_SetCommandBitV2(PCMCIA_DEFAULT_MODULE, u8CommandBit, bValue);
}

bool MDrv_PCMCIA_ResetInterface( void )
{
    return MDrv_PCMCIA_ResetInterfaceV2(PCMCIA_DEFAULT_MODULE);
}

bool MDrv_PCMCIA_IsDataAvailable( void )
{
    return MDrv_PCMCIA_IsDataAvailableV2(PCMCIA_DEFAULT_MODULE);
}

u16 MDrv_PCMCIA_ReadData( u8* u8pReadBuffer, u16 u16ReadBufferSize )
{
    return MDrv_PCMCIA_ReadDataV2(PCMCIA_DEFAULT_MODULE, u8pReadBuffer, u16ReadBufferSize);
}

bool MDrv_PCMCIA_WriteData( u8* u8pWriteBuffer, u16 u16DataLen )
{
    return MDrv_PCMCIA_WriteDataV2(PCMCIA_DEFAULT_MODULE, u8pWriteBuffer, u16DataLen);
}

bool MDrv_PCMCIA_SwitchToIOmode( PCMCIA_INFO *pInfo )
{
    return MDrv_PCMCIA_SwitchToIOmodeV2(PCMCIA_DEFAULT_MODULE, pInfo);
}

u16 MDrv_PCMCIA_NegotiateBufferSize( PCMCIA_INFO *pInfo )
{
    return MDrv_PCMCIA_NegotiateBufferSizeV2(PCMCIA_DEFAULT_MODULE, pInfo);
}

void MDrv_PCMCIA_WriteBufferSize( u16 u16BufferSize )
{
    MDrv_PCMCIA_WriteBufferSizeV2(PCMCIA_DEFAULT_MODULE, u16BufferSize);
}

bool MDrv_PCMCIA_WaitForStatusBit( u8 u8StatusBit )
{
    return MDrv_PCMCIA_WaitForStatusBitV2(PCMCIA_DEFAULT_MODULE, u8StatusBit);
}

u32 MDrv_PCMCIA_Get_CD_Interval( void )
{
    return MDrv_PCMCIA_Get_CD_IntervalV2(PCMCIA_DEFAULT_MODULE);
}

#endif

void MDrv_PCMCIA_ResetHW( void )
{
    MDrv_PCMCIA_ResetHW_V2(PCMCIA_DEFAULT_MODULE);
}

void MDrv_PCMCIA_WriteAttribMem( u16 wAddr, u8 bData )
{
    MDrv_PCMCIA_WriteAttribMemV2(PCMCIA_DEFAULT_MODULE, wAddr, bData);
}

void MDrv_PCMCIA_ReadAttribMem( u16 u16Addr, u8 *pDest )
{
    MDrv_PCMCIA_ReadAttribMemV2(PCMCIA_DEFAULT_MODULE, u16Addr, pDest);
}

void MDrv_PCMCIA_WriteIOMem( u16 wAddr, u8 bData )
{
    MDrv_PCMCIA_WriteIOMemV2(PCMCIA_DEFAULT_MODULE, wAddr, bData);
}

void MDrv_PCMCIA_WriteIOMemLong( u16 u16Addr, u8 u8Value, u16 u16DataLen, u8* u8pWriteBuffer)
{
    MDrv_PCMCIA_WriteIOMemLongV2(PCMCIA_DEFAULT_MODULE, u16Addr, u8Value, u16DataLen, u8pWriteBuffer);
}

u8 MDrv_PCMCIA_ReadIOMem( u16 wAddr )
{
    return MDrv_PCMCIA_ReadIOMemV2(PCMCIA_DEFAULT_MODULE, wAddr);
}


bool MDrv_PCMCIA_Detect( void )
{
    return MDrv_PCMCIA_DetectV2(PCMCIA_DEFAULT_MODULE);
}
