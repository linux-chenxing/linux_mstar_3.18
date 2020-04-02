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
/// @file   drvPCMCIA.h
/// @brief  PCMCIA Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_PCMCIA_H_
#define _DRV_PCMCIA_H_

#include <linux/kernel.h>
//-----------------------------------------------------------------------------
//  Macro and Define
//-----------------------------------------------------------------------------
// PCMCIA
#define MAX_PCMCIA_CONFIGS              6       //!< The maximum number of configurations supported by a PCMCIA card
#define MAX_PCMCIA_STRLEN               (64)    //!< The maximum name of vendor/manufacturer/info strings
#define MAX_CIS_SIZE                    0x100   //!< The maximum size of a CIS, that is understood by this driver
#define PCMCIA_HW_TIMEOUT               3000
#define PCMCIA_IRQ_ENABLE               1

// IRQ_force status bits
#define PCMCIA_MEM_READY                (0x04)  //The same pin as IRQ in I/O mode

// Command Interface Hardware Registers
#define PCMCIA_PHYS_REG_DATA            (0)
#define PCMCIA_PHYS_REG_COMMANDSTATUS   (1)
#define PCMCIA_PHYS_REG_SIZELOW         (2)
#define PCMCIA_PHYS_REG_SIZEHIGH        (3)

// Status Register Bits
#define PCMCIA_STATUS_DATAAVAILABLE     (0x80)  //!< PCMCIA Status Register Bit - The module wants to send data
#define PCMCIA_STATUS_FREE              (0x40)  //!< PCMCIA Status Register Bit - The module can accept data
#define PCMCIA_STATUS_IIR               (0x10)  //!< PCMCIA Status Register Bit - IIR
#define PCMCIA_STATUS_RESERVEDBITS      (0x2C)  //!< PCMCIA Status Register Bits - reserved
#define PCMCIA_STATUS_WRITEERROR        (0x02)  //!< PCMCIA Status Register Bit - Write error
#define PCMCIA_STATUS_READERROR         (0x01)  //!< PCMCIA Status Register Bit - Read error

// Command Register Bits
#define PCMCIA_COMMAND_DAIE             (0x80)  //!< PCMCIA Command register bit - DAIE
                                                // when this bit is set, the module asserts IREQ# each time it has data to send
#define PCMCIA_COMMAND_FRIE             (0x40)  //!< PCMCIA Command register bit - FRIE
                                                // when this bit is set, the module asserts IREQ# each time it is free to receive data
#define PCMCIA_COMMAND_RESERVEDBITS     (0x30)  //!< PCMCIA Command register bits - reserved
#define PCMCIA_COMMAND_RESET            (0x08)  //!< PCMCIA Command register bit - Reset
#define PCMCIA_COMMAND_SIZEREAD         (0x04)  //!< PCMCIA Command register bit - Size read
#define PCMCIA_COMMAND_SIZEWRITE        (0x02)  //!< PCMCIA Command register bit - Size Write
#define PCMCIA_COMMAND_HOSTCONTROL      (0x01)  //!< PCMCIA Command register bit - Host control

#define PCMCIAINFO_MANID_VALID          (0x00000001)
#define PCMCIAINFO_VERS1_VALID          (0x00000002)
#define PCMCIAINFO_FUNCID_VALID         (0x00000004)

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// Constant & Macro Definition
////////////////////////////////////////////////////////////////////////////////
typedef void (*IsrCallback)( void* wparam, void* lparam );

//-----------------------------------------------------------------------------
//  Enums
//-----------------------------------------------------------------------------
//! This enum contains the card types, that can be encoded in CISTPL_FUNCID (0x21)
typedef enum {
    E_PCMCIA_FUNC_CUSTOM        = 0,
    E_PCMCIA_FUNC_MEMORY        = 1,
    E_PCMCIA_FUNC_SERIAL        = 2,
    E_PCMCIA_FUNC_PARALLEL      = 3,
    E_PCMCIA_FUNC_DRIVE         = 4,
    E_PCMCIA_FUNC_VIDEO         = 5,
    E_PCMCIA_FUNC_NETWORK       = 6,
    E_PCMCIA_FUNC_AIMS          = 7,
    E_PCMCIA_FUNC_SCSI          = 8,
    E_PCMCIA_FUNC_CARDBUS       = 9,
    E_PCMCIA_FUNC_MANUFACTURER  = 0xFF      // manifacturer designed purpose
} PCMCIA_FUNCTYPE;

typedef enum {
    E_PCMCIA_MODULE_A           = 0,
    E_PCMCIA_MODULE_B           = 1,
    E_PCMCIA_MODULE_MAX         = 2
} PCMCIA_MODULE;

typedef enum {
    E_POWER_RESUME,
    E_POWER_SUSPEND,
    E_POWER_MECHANICAL,
    E_POWER_SOFT_OFF
} EN_POWER_MODE;
//-----------------------------------------------------------------------------
//  Structures
//-----------------------------------------------------------------------------
//! This structure contains information about exactly one possible PCMCIA card configuration.
typedef struct /* 13 Bytes */
{
    u32 dwEAAddr;            //!< EA-address
    u32 dwEALen;             //!< size of the EA area (zero for none)
    u16 wIRQData;            //!< supported IRQ number mask
    u8  bConfigIndex;        //!< cor //!< The value of bConfigIndex has to be written into the card configuration register to activate this configuration.
    u8  bIRQDesc1;           //!< Interrupt descriptor byte
    u8  fCITagsPresent;      //!< Bitfield which is 0x03 if both required CI tags are present. 0x01 for DVB_HOST, 0x02 for DVB_CI_MODULE
} PCMCIA_CONFIG;

//! This structure provides simple access to the PCMCIA card information
//! after decoding of the Card Information Structure. This decoding is performed by MDrv_CI_PCMCIA_ReadAttribMem()
typedef struct
{
    u32 ConfigOffset;                            //!< Offset of the Configuration byte in the Attribute Memory
    u32 dwValidFlags;                            //!< Bitmask that defines which of the other fields are valid
    u16 wManufacturerId;                         //!< 16Bit Manufacturer ID (PCMCIAINFO_MANID_VALID)
    u16 wCardID;                                 //!< 16Bit Card ID (PCMCIAINFO_MANID_VALID)

    /* 99 Bytes */
    u16 wPCMCIAStdRev;                           //!< PCMCIA Standard version supported by the card (PCMCIAINFO_VERS1_VALID)
    u8 pszManufacturerName[MAX_PCMCIA_STRLEN];   //!< Name of the card manufacturer (PCMCIAINFO_VERS1_VALID)
    u8 pszProductName[MAX_PCMCIA_STRLEN];        //!< Product name (PCMCIAINFO_VERS1_VALID)
    u8 pszProductInfo1[MAX_PCMCIA_STRLEN];       //!< (PCMCIAINFO_VERS1_VALID)
    u8 pszProductInfo2[MAX_PCMCIA_STRLEN];       //!< (PCMCIAINFO_VERS1_VALID)
    u8 bCI_PLUS;                                  //!< PCMCIA card CI Plus Compatibility Identification
    
    PCMCIA_FUNCTYPE FuncType;                       //!< Card function type (PCMCIAINFO_FUNCID_VALID)
    u8 bFuncIDSysInfo;                           //!< SysInitByte from the FuncID block (PCMCIAINFO_FUNCID_VALID)

    u8 bINT;                                     //!< PCMCIA card Support interrupt or not
    u8 bNumConfigs;
    /* 13 * MAX_PCMCIA_CONFIGS(=10) = 130 Bytes*/   //!< The number of configurations supported by the card. Exactly bNumConfigs entries are valid in the Config array.
    PCMCIA_CONFIG Config[MAX_PCMCIA_CONFIGS];       //!< The array of possible card configurations
} PCMCIA_INFO;

//! This structure provides ISR data for upper layer
//! DO NOT change the current data member
//! Only allow to add new member in the tail of the structure
typedef struct {
    bool bISRCardInsert;
    bool bISRCardRemove;    
    bool bISRCardData;    
} PCMCIA_ISR;

//! This file typedefs PCMCIA_HANDLE as void*.
//! Actual PCMCIA driver implementations can hide whatever they want inside this handle.
//! Higher level drivers pass
typedef void *PCMCIA_HANDLE;

//-----------------------------------------------------------------------------
//  Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Global Functions
//-----------------------------------------------------------------------------
// PCMCIA

// revision history
// V2
//    for multiple PCMCIA slot support
//

void MDrv_PCMCIA_Init(bool bCD_Reverse);

u32 MDrv_PCMCIA_SetPowerState(EN_POWER_MODE u16PowerState);

bool MDrv_PCMCIA_PollingV2( PCMCIA_MODULE eModule );

bool MDrv_PCMCIA_DetectV2( PCMCIA_MODULE eModule );
bool MDrv_PCMCIA_Detect( void );

void MDrv_PCMCIA_Set_HW_ResetDuration( u8 u8HW_ResetDuration );

void MDrv_PCMCIA_ResetHW_V2( PCMCIA_MODULE eModule );
//! This function is called by higher level drivers to check if a module is (still) present.
//! Usually, this check is performed by checking the card detect GPIO pins of a PCMCIA slot.

void MDrv_PCMCIA_ReadAttribMemV2( PCMCIA_MODULE eModule, u16 u16Addr, u8 *pDest );
//! This function decodes a raw attribute memory dump into an easily readable
//! PCMCIA_INFO structure. The PCMCIA/PnP task is responsible for reading the raw attribute memory.
//! This function parses the card info structure (CIS) and decodes the relevant parts. Callers should
//! check the dwValidFlags Bitmask before accessing other fields.

void MDrv_PCMCIA_ParseAttribMem( u8 *pAttribMem, u16 dwLen, PCMCIA_INFO *pInfo );
//! Initialization function. During system initialization, this function
//! is called once to initialize the driver.

void MDrv_PCMCIA_WriteAttribMemV2( PCMCIA_MODULE eModule, u16 wAddr, u8 bData );
//! This function is called to write the byte bData into the card IO memory at address
//! wAddr.

void MDrv_PCMCIA_WriteIOMemV2( PCMCIA_MODULE eModule, u16 wAddr, u8 bData );
//! This function is read one byte of from the card IO memory at address wAddr.

void MDrv_PCMCIA_WriteIOMemLongV2( PCMCIA_MODULE eModule, u16 u16Addr, u8 u8Value, u16 u16DataLen, u8* u8pWriteBuffer);

u8 MDrv_PCMCIA_ReadIOMemV2( PCMCIA_MODULE eModule, u16 wAddr );

bool MDrv_PCMCIA_IsModuleStillPluggedV2( PCMCIA_MODULE eModule );
//! This function is called to enable or disable the TS stream for PCMCIA common interface slots.

void MDrv_PCMCIA_Set_Detect_Trigger( bool bActiveHigh );
void MDrv_PCMCIA_Set_Detect_Enable( bool bEnable );
u32 MDrv_PCMCIA_Get_CD_IntervalV2( PCMCIA_MODULE eModule );
//bool MDrv_PCMCIA_GetLibVer( const MSIF_Version **ppVersion );
void MDrv_PCMCIA_SetCommandBitV2( PCMCIA_MODULE eModule, u8 u8CommandBit, bool bValue );
bool MDrv_PCMCIA_ResetInterfaceV2( PCMCIA_MODULE eModule );
bool MDrv_PCMCIA_IsDataAvailableV2( PCMCIA_MODULE eModule );
u16 MDrv_PCMCIA_ReadDataV2( PCMCIA_MODULE eModule, u8* u8pReadBuffer, u16 u16ReadBufferSize );
bool MDrv_PCMCIA_WriteDataV2( PCMCIA_MODULE eModule, u8* u8pWriteBuffer, u16 u16DataLen );
bool MDrv_PCMCIA_SwitchToIOmodeV2( PCMCIA_MODULE eModule, PCMCIA_INFO *pInfo );
u16 MDrv_PCMCIA_NegotiateBufferSizeV2( PCMCIA_MODULE eModule, PCMCIA_INFO *pInfo );
void MDrv_PCMCIA_WriteBufferSizeV2( PCMCIA_MODULE eModule, u16 u16BufferSize );
bool MDrv_PCMCIA_WaitForStatusBitV2( PCMCIA_MODULE eModule, u8 u8StatusBit );
u8 MDrv_PCMCIA_ReadyStatus( void );
void MDrv_PCMCIA_Exit( void );

#if PCMCIA_IRQ_ENABLE
void MDrv_PCMCIA_Enable_InterruptV2( PCMCIA_MODULE eModule, bool bEnable );
void MDrv_PCMCIA_Set_InterruptStatusV2( PCMCIA_MODULE eModule, bool Status );
bool MDrv_PCMCIA_Get_InterruptStatusV2( PCMCIA_MODULE eModule );
void MDrv_PCMCIA_InstarllIsrCallbackV2( PCMCIA_MODULE eModule, IsrCallback fnIsrCallback );
#endif


//-----------------------------------------------------------------------------
//  Function define for backward compatible
//-----------------------------------------------------------------------------
#define PCMCIA_DEFAULT_MODULE                   E_PCMCIA_MODULE_A
#if 0
    //#define MDrv_PCMCIA_Detect(p...)              MDrv_PCMCIA_DetectV2(E_PCMCIA_MODULE_A)
    #define MDrv_PCMCIA_Polling(p...)               MDrv_PCMCIA_PollingV2(PCMCIA_DEFAULT_MODULE)
    #define MDrv_PCMCIA_IsModuleStillPlugged(p...)  MDrv_PCMCIA_IsModuleStillPluggedV2(PCMCIA_DEFAULT_MODULE)
    #define MDrv_PCMCIA_SetCommandBit(p...)         MDrv_PCMCIA_SetCommandBitV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_ResetInterface(p...)        MDrv_PCMCIA_ResetInterfaceV2(PCMCIA_DEFAULT_MODULE)
    #define MDrv_PCMCIA_IsDataAvailable(p...)       MDrv_PCMCIA_IsDataAvailableV2(PCMCIA_DEFAULT_MODULE)
    #define MDrv_PCMCIA_ReadData(p...)              MDrv_PCMCIA_ReadDataV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_WriteData(p...)             MDrv_PCMCIA_WriteDataV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_SwitchToIOmode(p...)        MDrv_PCMCIA_SwitchToIOmodeV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_NegotiateBufferSize(p...)   MDrv_PCMCIA_NegotiateBufferSizeV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_WriteBufferSize(p...)       MDrv_PCMCIA_WriteBufferSizeV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_WaitForStatusBit(p...)      MDrv_PCMCIA_WaitForStatusBitV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_ResetHW(p...)               MDrv_PCMCIA_ResetHW_V2(PCMCIA_DEFAULT_MODULE)
    #define MDrv_PCMCIA_WriteAttribMem(p...)        MDrv_PCMCIA_WriteAttribMemV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_ReadAttribMem(p...)         MDrv_PCMCIA_ReadAttribMemV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_WriteIOMem(p...)            MDrv_PCMCIA_WriteIOMemV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_WriteIOMemLong(p...)        MDrv_PCMCIA_WriteIOMemLongV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_ReadIOMem(p...)             MDrv_PCMCIA_ReadIOMemV2(PCMCIA_DEFAULT_MODULE, p)
    #define MDrv_PCMCIA_Get_CD_Interval(p...)       MDrv_PCMCIA_Get_CD_IntervalV2(PCMCIA_DEFAULT_MODULE)

    #if PCMCIA_IRQ_ENABLE
        #define MDrv_PCMCIA_Enable_Interrupt(p...)      MDrv_PCMCIA_Enable_InterruptV2(PCMCIA_DEFAULT_MODULE, p)
        #define MDrv_PCMCIA_Set_InterruptStatus(p...)   MDrv_PCMCIA_Set_InterruptStatusV2(PCMCIA_DEFAULT_MODULE, p)
        #define MDrv_PCMCIA_Get_InterruptStatus(p...)   MDrv_PCMCIA_Get_InterruptStatusV2(PCMCIA_DEFAULT_MODULE)
        #define MDrv_PCMCIA_InstarllIsrCallback(p...)   MDrv_PCMCIA_InstarllIsrCallbackV2(PCMCIA_DEFAULT_MODULE, p)
    #endif    
#else
    
    bool MDrv_PCMCIA_Polling( void );
    bool MDrv_PCMCIA_IsModuleStillPlugged( void );
    void MDrv_PCMCIA_SetCommandBit( u8 u8CommandBit, bool bValue );
    bool MDrv_PCMCIA_ResetInterface( void );
    bool MDrv_PCMCIA_IsDataAvailable( void );
    u16 MDrv_PCMCIA_ReadData( u8* u8pReadBuffer, u16 u16ReadBufferSize );
    bool MDrv_PCMCIA_WriteData( u8* u8pWriteBuffer, u16 u16DataLen );
    bool MDrv_PCMCIA_SwitchToIOmode( PCMCIA_INFO *pInfo );
    u16 MDrv_PCMCIA_NegotiateBufferSize( PCMCIA_INFO *pInfo );
    void MDrv_PCMCIA_WriteBufferSize( u16 u16BufferSize );
    bool MDrv_PCMCIA_WaitForStatusBit( u8 u8StatusBit );
    void MDrv_PCMCIA_ResetHW( void );
    void MDrv_PCMCIA_WriteAttribMem( u16 wAddr, u8 bData );
    void MDrv_PCMCIA_ReadAttribMem( u16 u16Addr, u8 *pDest );
    void MDrv_PCMCIA_WriteIOMem( u16 wAddr, u8 bData );
    void MDrv_PCMCIA_WriteIOMemLong( u16 u16Addr, u8 u8Value, u16 u16DataLen, u8* u8pWriteBuffer);
    u8 MDrv_PCMCIA_ReadIOMem( u16 wAddr );
    u32 MDrv_PCMCIA_Get_CD_Interval( void );

    #if PCMCIA_IRQ_ENABLE
        void MDrv_PCMCIA_Enable_Interrupt( bool bEnable );
        void MDrv_PCMCIA_Set_InterruptStatus( bool Status );
        bool MDrv_PCMCIA_Get_InterruptStatus( void );
        void MDrv_PCMCIA_InstarllIsrCallback( IsrCallback fnIsrCallback );
    #endif
#endif

#ifdef __cplusplus
}
#endif

#endif // _DRV_PCMCIA_H_
