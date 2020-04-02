/* ir-nec-decoder.c - handle NEC IR Pulse/Space protocol
 *
 * Copyright (C) 2010 by Mauro Carvalho Chehab <mchehab@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <linux/bitrev.h>
#include <linux/module.h>
#include "../ir_core.h"
#include "../ir_common.h"

static u8 u8InitFlag_nec = FALSE;
static u32 scancode = 0;
static u32 speed = 0;
static u32 mapnum = 0;
static unsigned long  KeyTime = 0;
static IR_NEC_Spec_t nec;

/**
 * ir_nec_decode() - Decode one NEC pulse or space
 * @dev:	the struct rc_dev descriptor of the device
 * @duration:	the struct ir_raw_event descriptor of the pulse/space
 *
 * This function returns -EINVAL if the pulse violates the state machine
 */
static int ir_nec_decode(struct mstar_ir_dev *dev, struct ir_raw_data ev)
{
    IR_NEC_Spec_t *data = &nec;
    struct ir_scancode *sc = &dev->raw->this_sc;
    struct ir_scancode *prevsc = &dev->raw->prev_sc;
    u8 i = 0;
    u8 u8Addr = 0;
    u8 u8AddrInv =0;
    u8 u8Cmd =0;
    u8 u8CmdInv =0;

    if (!(dev->enabled_protocols & (1<<IR_TYPE_NEC)))
        return -EINVAL;
    switch (data->eStatus)
    {

    case STATE_INACTIVE:
        if (!ev.pulse)
            break;
        if (eq_margin(ev.duration,NEC_HEADER_PULSE_LWB,NEC_HEADER_PULSE_UPB))
        {
            data->u8BitCount = 0;
            data->eStatus = STATE_HEADER_SPACE;
            //IRDBG_INFO("NEC_HEADER_PULSE\n");
            return 0;
        }
        else
            break;

    case STATE_HEADER_SPACE:
        if (ev.pulse)
            break;
        if (eq_margin(ev.duration,NEC_HEADER_SPACE_LWB,NEC_HEADER_SPACE_UPB))
        {
            data->eStatus = STATE_BIT_PULSE;
            return 0;
        }
        else if (eq_margin(ev.duration,NEC_REPEAT_SPACE_LWB,NEC_REPEAT_SPACE_UPB))
        {//repeat
            //IRDBG_INFO("[NEC] TIME =%ld\n",(MIRC_Get_System_Time()-KeyTime));
            if (prevsc->scancode_protocol == (1<<IR_TYPE_NEC) && ((u32)(MIRC_Get_System_Time()- KeyTime) <= NEC_REPEAT_TIMEOUT))
            {
                KeyTime = MIRC_Get_System_Time();
                if (((speed != 0)&&( data->u8RepeatTimes >= (speed - 1))) 
                        || ((speed == 0)&&(data->u8RepeatTimes >= dev->speed)))
                {
#ifdef CONFIG_MIRC_INPUT_DEVICE
                    sc->scancode = scancode;
                    sc->scancode_protocol = (1<<IR_TYPE_NEC);
                    dev->map_num = mapnum;
                    dev->raw->u8RepeatFlag = 1;
#else
                    sc->scancode_protocol = (1<<IR_TYPE_NEC);
                    sc->scancode = scancode|0x01;//repeat
#endif
                    memset(data,0,sizeof(IR_NEC_Spec_t));
                    return 1;

                }
                data->u8RepeatTimes ++;

                //IRDBG_INFO("[NEC] repeattimes =%d \n",data->u8RepeatTimes);
            }
            else
            {
                scancode = 0;
                mapnum = NUM_KEYMAP_MAX;
            }
            data->eStatus = STATE_INACTIVE;
            return 0;
        }
        break;

    case STATE_BIT_PULSE:
        if (!ev.pulse)
            break;
        if (!eq_margin(ev.duration,NEC_BIT_PULSE_LWB,NEC_BIT_PULSE_UPB))
            break;

        data->eStatus = STATE_BIT_SPACE;
        return 0;

    case STATE_BIT_SPACE:
        if (ev.pulse)
            break;
        data->u32DataBits <<= 1;

        if (eq_margin(ev.duration,NEC_BIT_1_SPACE_LWB,NEC_BIT_1_SPACE_UPB))
            data->u32DataBits |= 1;
        else if (!eq_margin(ev.duration,NEC_BIT_0_SPACE_LWB,NEC_BIT_0_SPACE_UPB))
            break;
        data->u8BitCount++;

        if (data->u8BitCount == NEC_NBITS)
        {
            u8Addr    = bitrev8((data->u32DataBits >> 24) & 0xff);
            u8AddrInv = bitrev8((data->u32DataBits >> 16) & 0xff);
            u8Cmd	  = bitrev8((data->u32DataBits >>  8) & 0xff);
            u8CmdInv  = bitrev8((data->u32DataBits >>  0) & 0xff);
            
            IRDBG_INFO("[NEC] u8Addr = %x u8AddrInv = %x u8Cmd = %x u8CmdInv = %x\n",u8Addr,u8AddrInv,u8Cmd,u8CmdInv);            
            for (i= 0;i<dev->support_num;i++)
            {
                if(dev->support_ir[i].eIRType == IR_TYPE_NEC)
                {
                    if((((u8Addr<<8) | u8AddrInv) == dev->support_ir[i].u32HeadCode))
                    {
                        if (u8Cmd == (u8)~u8CmdInv) 
                        {
#ifdef CONFIG_MIRC_INPUT_DEVICE
                            sc->scancode = (u8Addr<<16) | (u8AddrInv<<8) | u8Cmd;
                            sc->scancode_protocol = (1<<IR_TYPE_NEC);
                            scancode = sc->scancode;
                            speed = dev->support_ir[i].u32IRSpeed;
                            dev->map_num = dev->support_ir[i].u32HeadCode;
                            mapnum = dev->map_num;        
                            dev->raw->u8RepeatFlag = 0;
#else
                            sc->scancode = (u8Cmd<<8) |0x00;
                            sc->scancode |= (0x01UL << 28);
                            sc->scancode_protocol = (1<<IR_TYPE_NEC);
                            scancode = sc->scancode;
                            speed = dev->support_ir[i].u32IRSpeed;
#endif                  
                            KeyTime = MIRC_Get_System_Time();
                            memset(data,0,sizeof(IR_NEC_Spec_t));
                            return 1;
                        }
                    }
                }
            }

        }
        else
            data->eStatus = STATE_BIT_PULSE;

        return 0;
    default:
        break;
    }

    data->eStatus = STATE_INACTIVE;
    return -EINVAL;
}

static struct ir_decoder_handler nec_handler = {
    .protocols	= (1<<IR_TYPE_NEC),
    .decode		= ir_nec_decode,
};

int nec_decode_init(void)
{
    if(u8InitFlag_nec == FALSE)
    {
        scancode = 0;
        mapnum = 0;
        KeyTime = 0;
        memset(&nec,0,sizeof(IR_NEC_Spec_t));
        MIRC_Decoder_Register(&nec_handler);
        IR_PRINT("[IR Log] NEC Spec Protocol Init\n");
        u8InitFlag_nec = TRUE;
    }
    else
    {
        IR_PRINT("[IR Log] NEC Spec Protocol Has been Initialized\n");
    }
    return 0;
}

void nec_decode_exit(void)
{
    if(u8InitFlag_nec == TRUE)
    {
        MIRC_Decoder_UnRegister(&nec_handler);
        u8InitFlag_nec = FALSE;
    }
}
