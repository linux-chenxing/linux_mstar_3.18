////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#include "MsCommon.h"


#define _RV1(addr, value)   (((addr) >> 16) & 0xFF), (((addr) >> 8) & 0xFF), (MS_U8)(addr), (MS_U8)(value)
#define _END_OF_TBL2_       0xFF, 0xFF, 0xFF


MS_U8  tVE_ENCODER_NTSC_TBL[] =
{
//video encoder
    _RV1(L_BK_VE_ENC(0x00), 0x01),// hsync start
    _RV1(H_BK_VE_ENC(0x00), 0x7F),// hsync end
    _RV1(L_BK_VE_ENC(0x01), 0x94),// burst start
    _RV1(H_BK_VE_ENC(0x01), 0xD7),// burst end
    _RV1(L_BK_VE_ENC(0x02), 0x00),
    _RV1(H_BK_VE_ENC(0x02), 0x00),
    _RV1(L_BK_VE_ENC(0x03), 0x07),
    _RV1(H_BK_VE_ENC(0x03), 0x00),
    _RV1(L_BK_VE_ENC(0x04), 0x00),// contrast
    _RV1(H_BK_VE_ENC(0x04), 0x4C),// contrast
    _RV1(L_BK_VE_ENC(0x06), 0x00),
    _RV1(H_BK_VE_ENC(0x06), 0x00),
    _RV1(L_BK_VE_ENC(0x09), 0xB4),// h total
    _RV1(H_BK_VE_ENC(0x09), 0x06),
    _RV1(L_BK_VE_ENC(0x0A), 0x00),// brightness
    _RV1(H_BK_VE_ENC(0x0A), 0x80),// brightness
    _RV1(L_BK_VE_ENC(0x0B), 0x1F),// burst phase step
    _RV1(H_BK_VE_ENC(0x0B), 0x7C),
    _RV1(L_BK_VE_ENC(0x0C), 0xF0),
    _RV1(H_BK_VE_ENC(0x0C), 0x21),
    _RV1(L_BK_VE_ENC(0x0D), 0xD0),
    _RV1(H_BK_VE_ENC(0x0D), 0x00),
    _RV1(L_BK_VE_ENC(0x0E), 0x00),
    _RV1(H_BK_VE_ENC(0x0E), 0x00),
    _RV1(L_BK_VE_ENC(0x25), 0x00),// av st
    _RV1(H_BK_VE_ENC(0x25), 0x01),
    _RV1(L_BK_VE_ENC(0x26), 0x8D),// av end
    _RV1(H_BK_VE_ENC(0x26), 0x06),
    _RV1(L_BK_VE_ENC(0x27), 0x2A),// sync tip level & pad level
    _RV1(H_BK_VE_ENC(0x27), 0x10),
    _RV1(L_BK_VE_ENC(0x28), 0xF0),// sync step & blank level
    _RV1(H_BK_VE_ENC(0x28), 0x32),
    _RV1(L_BK_VE_ENC(0x29), 0x13),// burst amp & step
    _RV1(H_BK_VE_ENC(0x29), 0x70),
    _RV1(L_BK_VE_ENC(0x2A), 0x41),//chroma gain
    _RV1(H_BK_VE_ENC(0x2A), 0x5B),
    _RV1(L_BK_VE_ENC(0x2D), 0x20),// Y clamp
    _RV1(H_BK_VE_ENC(0x2D), 0x53),
    _RV1(L_BK_VE_ENC(0x2E), 0x00),
    _RV1(H_BK_VE_ENC(0x2E), 0x00),
    _RV1(L_BK_VE_ENC(0x2F), 0xFA),
    _RV1(H_BK_VE_ENC(0x2F), 0x00),
    _RV1(L_BK_VE_ENC(0x78), 0x00),// disable MV

//video source
    _RV1(L_BK_VE_SRC(0x42), 0xE0),// Frame line num
    _RV1(H_BK_VE_SRC(0x42), 0x01),
    _RV1(L_BK_VE_SRC(0x45), 0x60),// Field Size
    _RV1(H_BK_VE_SRC(0x45), 0x54),

    _END_OF_TBL2_,
};

MS_U8  tVE_ENCODER_NTSC_443_TBL[] =
{
    _RV1(L_BK_VE_ENC(0x00), 0x01),// hsync st
    _RV1(H_BK_VE_ENC(0x00), 0x7F),// hsync end
    _RV1(L_BK_VE_ENC(0x01), 0x8E),// burst st
    _RV1(H_BK_VE_ENC(0x01), 0xD1),// burst end
    _RV1(L_BK_VE_ENC(0x02), 0x00),
    _RV1(H_BK_VE_ENC(0x02), 0x00),
    _RV1(L_BK_VE_ENC(0x03), 0x07),
    _RV1(H_BK_VE_ENC(0x03), 0x00),
    _RV1(L_BK_VE_ENC(0x04), 0x00),// contrast
    _RV1(H_BK_VE_ENC(0x04), 0x4C),// contrast
    _RV1(L_BK_VE_ENC(0x09), 0xB4),// h total
    _RV1(H_BK_VE_ENC(0x09), 0x06),
    _RV1(L_BK_VE_ENC(0x0A), 0x00),// brightness
    _RV1(H_BK_VE_ENC(0x0A), 0x80),// brightness
    _RV1(L_BK_VE_ENC(0x0B), 0xCB),// burst phase step
    _RV1(H_BK_VE_ENC(0x0B), 0x8A),
    _RV1(L_BK_VE_ENC(0x0C), 0x09),
    _RV1(H_BK_VE_ENC(0x0C), 0x2A),
    _RV1(L_BK_VE_ENC(0x0D), 0x2E),
    _RV1(H_BK_VE_ENC(0x0D), 0x05),
    _RV1(L_BK_VE_ENC(0x0E), 0xB2),
    _RV1(H_BK_VE_ENC(0x0E), 0x01),
    _RV1(L_BK_VE_ENC(0x25), 0x00),// av st
    _RV1(H_BK_VE_ENC(0x25), 0x01),
    _RV1(L_BK_VE_ENC(0x26), 0x8D),// av end
    _RV1(H_BK_VE_ENC(0x26), 0x06),
    _RV1(L_BK_VE_ENC(0x27), 0x2A),// sync tip level & pad level
    _RV1(H_BK_VE_ENC(0x27), 0x10),
    _RV1(L_BK_VE_ENC(0x28), 0xF0),// sync step & blank level
    _RV1(H_BK_VE_ENC(0x28), 0x32),
    _RV1(L_BK_VE_ENC(0x29), 0x13),// burst amp & step
    _RV1(H_BK_VE_ENC(0x29), 0x70),
    _RV1(L_BK_VE_ENC(0x2A), 0x41),//chroma gain
    _RV1(H_BK_VE_ENC(0x2A), 0x5B),
    _RV1(L_BK_VE_ENC(0x2D), 0x20),// Y clamp
    _RV1(H_BK_VE_ENC(0x2D), 0x53),
    _RV1(L_BK_VE_ENC(0x2F), 0xFA),
    _RV1(H_BK_VE_ENC(0x2F), 0x00),

     _RV1(L_BK_VE_ENC(0x78), 0x00),// disable MV

    _RV1(L_BK_VE_SRC(0x42), 0xE0),// Frame line num
    _RV1(H_BK_VE_SRC(0x42), 0x01),
    _RV1(L_BK_VE_SRC(0x45), 0x60),// Field Size
    _RV1(H_BK_VE_SRC(0x45), 0x54),
    _END_OF_TBL2_
};

MS_U8  tVE_ENCODER_NTSC_J_TBL[] =
{
    _RV1(L_BK_VE_ENC(0x00), 0x01),// hsync st
    _RV1(H_BK_VE_ENC(0x00), 0x7F),// hsync end
    _RV1(L_BK_VE_ENC(0x01), 0x8E),// burst st
    _RV1(H_BK_VE_ENC(0x01), 0xD1),// burst end
    _RV1(L_BK_VE_ENC(0x02), 0x00),
    _RV1(H_BK_VE_ENC(0x02), 0x00),
    _RV1(L_BK_VE_ENC(0x03), 0x07),
    _RV1(H_BK_VE_ENC(0x03), 0x00),
    _RV1(L_BK_VE_ENC(0x04), 0x00),// contrast
    _RV1(H_BK_VE_ENC(0x04), 0x52),// brightness
    _RV1(L_BK_VE_ENC(0x09), 0xB4),// H total
    _RV1(H_BK_VE_ENC(0x09), 0x06),
    _RV1(L_BK_VE_ENC(0x0A), 0x00),// brightness
    _RV1(H_BK_VE_ENC(0x0A), 0x80),// brightness
    _RV1(L_BK_VE_ENC(0x0B), 0x1F),// burst phase step
    _RV1(H_BK_VE_ENC(0x0B), 0x7C),
    _RV1(L_BK_VE_ENC(0x0C), 0xF0),
    _RV1(H_BK_VE_ENC(0x0C), 0x21),
    _RV1(L_BK_VE_ENC(0x0D), 0xD0),// lower stage fraction
    _RV1(H_BK_VE_ENC(0x0D), 0x00),
    _RV1(L_BK_VE_ENC(0x0E), 0x00),// 625 stage fraction
    _RV1(H_BK_VE_ENC(0x0E), 0x00),
    _RV1(L_BK_VE_ENC(0x25), 0x00),// av st
    _RV1(H_BK_VE_ENC(0x25), 0x01),
    _RV1(L_BK_VE_ENC(0x26), 0x8D),// av end
    _RV1(H_BK_VE_ENC(0x26), 0x06),
    _RV1(L_BK_VE_ENC(0x27), 0x00),// sync tip level & pad level
    _RV1(H_BK_VE_ENC(0x27), 0x10),
    _RV1(L_BK_VE_ENC(0x28), 0xF0),// sync step & blank level
    _RV1(H_BK_VE_ENC(0x28), 0x32),
    _RV1(L_BK_VE_ENC(0x29), 0x13),// burst amp & burst step
    _RV1(H_BK_VE_ENC(0x29), 0x70),
    _RV1(L_BK_VE_ENC(0x2A), 0x46),//chroma gain
    _RV1(H_BK_VE_ENC(0x2A), 0x62),
    _RV1(L_BK_VE_ENC(0x2D), 0x20),// Y clamp
    _RV1(H_BK_VE_ENC(0x2D), 0x53),
    _RV1(L_BK_VE_ENC(0x2F), 0xFA),
    _RV1(H_BK_VE_ENC(0x2F), 0x00),

    _RV1(L_BK_VE_ENC(0x78), 0x00),// disable MV

    _RV1(L_BK_VE_SRC(0x42), 0xE0),// Frame line number
    _RV1(H_BK_VE_SRC(0x42), 0x01),
    _RV1(L_BK_VE_SRC(0x45), 0x60),// Field Size
    _RV1(H_BK_VE_SRC(0x45), 0x54),
    _END_OF_TBL2_,
};

MS_U8  tVE_ENCODER_PAL_M_TBL[] =
{
    _RV1(L_BK_VE_ENC(0x00), 0x01),// hsync st
    _RV1(H_BK_VE_ENC(0x00), 0x7E),// hsync end
    _RV1(L_BK_VE_ENC(0x01), 0x9B),// burst st
    _RV1(H_BK_VE_ENC(0x01), 0xDE),// burst end
    _RV1(L_BK_VE_ENC(0x02), 0x00),
    _RV1(H_BK_VE_ENC(0x02), 0x00),
    _RV1(L_BK_VE_ENC(0x03), 0x07),
    _RV1(H_BK_VE_ENC(0x03), 0x00),
    _RV1(L_BK_VE_ENC(0x04), 0x00),// contrast
    _RV1(H_BK_VE_ENC(0x04), 0x50),// contrast
    _RV1(L_BK_VE_ENC(0x06), 0x02),
    _RV1(L_BK_VE_ENC(0x09), 0xB4),// H total
    _RV1(H_BK_VE_ENC(0x09), 0x06),

    _RV1(L_BK_VE_ENC(0x0A), 0x00),// brightness
    _RV1(H_BK_VE_ENC(0x0A), 0x80),// brightness
    _RV1(L_BK_VE_ENC(0x0B), 0xE3),// burst phase step
    _RV1(H_BK_VE_ENC(0x0B), 0xEF),
    _RV1(L_BK_VE_ENC(0x0C), 0xE6),
    _RV1(H_BK_VE_ENC(0x0C), 0x21),
    _RV1(L_BK_VE_ENC(0x0D), 0x90),// lower stage fraction
    _RV1(H_BK_VE_ENC(0x0D), 0x09),
    _RV1(L_BK_VE_ENC(0x0E), 0x00),// 625 stage fraction
    _RV1(H_BK_VE_ENC(0x0E), 0x00),
    _RV1(L_BK_VE_ENC(0x25), 0xFA),// av st
    _RV1(H_BK_VE_ENC(0x25), 0x00),
    _RV1(L_BK_VE_ENC(0x26), 0x8C),// av end
    _RV1(H_BK_VE_ENC(0x26), 0x06),
    _RV1(L_BK_VE_ENC(0x27), 0x2A),// sync tip level & pad level
    _RV1(H_BK_VE_ENC(0x27), 0x10),
    _RV1(L_BK_VE_ENC(0x28), 0xF0),// sync step & blank level
    _RV1(H_BK_VE_ENC(0x28), 0x32),
    _RV1(L_BK_VE_ENC(0x29), 0x14),// burst amp & burst step
    _RV1(H_BK_VE_ENC(0x29), 0x54),
    _RV1(L_BK_VE_ENC(0x2A), 0x44),//chroma gain
    _RV1(H_BK_VE_ENC(0x2A), 0x60),
    _RV1(L_BK_VE_ENC(0x2D), 0x30),// Y clamp
    _RV1(H_BK_VE_ENC(0x2D), 0x53),
    _RV1(L_BK_VE_ENC(0x2F), 0xF6),
    _RV1(H_BK_VE_ENC(0x2F), 0x00),

    _RV1(L_BK_VE_ENC(0x78), 0x00),// disable MV

    _RV1(L_BK_VE_SRC(0x42), 0xE0),// Frame line number
    _RV1(H_BK_VE_SRC(0x42), 0x01),
    _RV1(L_BK_VE_SRC(0x45), 0x40),// Field Size
    _RV1(H_BK_VE_SRC(0x45), 0x65),
    _END_OF_TBL2_,
};

MS_U8  tVE_ENCODER_PAL_N_TBL[] =
{
    _RV1(L_BK_VE_ENC(0x00), 0x01),// hsync st
    _RV1(H_BK_VE_ENC(0x00), 0x7E),// hsync end
    _RV1(L_BK_VE_ENC(0x01), 0x97),// burst st
    _RV1(H_BK_VE_ENC(0x01), 0xE1),// burst end
    _RV1(L_BK_VE_ENC(0x02), 0x00),
    _RV1(H_BK_VE_ENC(0x02), 0x00),
    _RV1(L_BK_VE_ENC(0x03), 0x07),
    _RV1(H_BK_VE_ENC(0x03), 0x00),
    _RV1(L_BK_VE_ENC(0x04), 0x00),// contrast
    _RV1(H_BK_VE_ENC(0x04), 0x50),// contrast
    _RV1(L_BK_VE_ENC(0x06), 0x02),
    _RV1(L_BK_VE_ENC(0x09), 0xC0),// H total
    _RV1(H_BK_VE_ENC(0x09), 0x06),
    _RV1(L_BK_VE_ENC(0x0A), 0x00),// brightness
    _RV1(H_BK_VE_ENC(0x0A), 0x80),// brightness
    _RV1(L_BK_VE_ENC(0x0B), 0xCB),// burst phase step
    _RV1(H_BK_VE_ENC(0x0B), 0x8A),
    _RV1(L_BK_VE_ENC(0x0C), 0x09),
    _RV1(H_BK_VE_ENC(0x0C), 0x2A),
    _RV1(L_BK_VE_ENC(0x0D), 0x2E),// lower stage fraction
    _RV1(H_BK_VE_ENC(0x0D), 0x05),
    _RV1(L_BK_VE_ENC(0x0E), 0xB2),// 625 stage fraction
    _RV1(H_BK_VE_ENC(0x0E), 0x01),
    _RV1(L_BK_VE_ENC(0x25), 0xFE),// av st
    _RV1(H_BK_VE_ENC(0x25), 0x00),
    _RV1(L_BK_VE_ENC(0x26), 0x96),// av end
    _RV1(H_BK_VE_ENC(0x26), 0x06),
    _RV1(L_BK_VE_ENC(0x27), 0x2A),// sync tip level & pad level
    _RV1(H_BK_VE_ENC(0x27), 0x10),
    _RV1(L_BK_VE_ENC(0x28), 0xF0),// sync step & blank level
    _RV1(H_BK_VE_ENC(0x28), 0x36),
    _RV1(L_BK_VE_ENC(0x29), 0x14),// burst amp & burst step
    _RV1(H_BK_VE_ENC(0x29), 0x54),
    _RV1(L_BK_VE_ENC(0x2A), 0x46),//chroma gain
    _RV1(H_BK_VE_ENC(0x2A), 0x62),
    _RV1(L_BK_VE_ENC(0x2D), 0x30),// Y clamp
    _RV1(H_BK_VE_ENC(0x2D), 0x53),
    _RV1(L_BK_VE_ENC(0x2F), 0xFC),
    _RV1(H_BK_VE_ENC(0x2F), 0x00),

    _RV1(L_BK_VE_ENC(0x78), 0x00),// disable MacroVision

    _RV1(L_BK_VE_SRC(0x42), 0x40),// Frame line number
    _RV1(H_BK_VE_SRC(0x42), 0x02),
    _RV1(L_BK_VE_SRC(0x45), 0x40),// Field Size
    _RV1(H_BK_VE_SRC(0x45), 0x65),
    _END_OF_TBL2_
};

MS_U8  tVE_ENCODER_PAL_NC_TBL[] =
{
    _RV1(L_BK_VE_ENC(0x00), 0x01),// hsync st
    _RV1(H_BK_VE_ENC(0x00), 0x7E),// hsync end
    _RV1(L_BK_VE_ENC(0x01), 0x97),// burst st
    _RV1(H_BK_VE_ENC(0x01), 0xD3),// burst end
    _RV1(L_BK_VE_ENC(0x02), 0x00),
    _RV1(H_BK_VE_ENC(0x02), 0x00),
    _RV1(L_BK_VE_ENC(0x03), 0x07),
    _RV1(H_BK_VE_ENC(0x03), 0x00),
    _RV1(L_BK_VE_ENC(0x04), 0x00),// contrast
    _RV1(H_BK_VE_ENC(0x04), 0x50),// contrast
    _RV1(L_BK_VE_ENC(0x06), 0x02),
    _RV1(L_BK_VE_ENC(0x09), 0xC0),// H total
    _RV1(H_BK_VE_ENC(0x09), 0x06),
    _RV1(L_BK_VE_ENC(0x0A), 0x00),// brightness
    _RV1(H_BK_VE_ENC(0x0A), 0x80),// brightness
    _RV1(L_BK_VE_ENC(0x0B), 0x46),// burst phase step
    _RV1(H_BK_VE_ENC(0x0B), 0x94),
    _RV1(L_BK_VE_ENC(0x0C), 0xF6),
    _RV1(H_BK_VE_ENC(0x0C), 0x21),
    _RV1(L_BK_VE_ENC(0x0D), 0x2E),// lower stage fraction
    _RV1(H_BK_VE_ENC(0x0D), 0x0C),
    _RV1(L_BK_VE_ENC(0x0E), 0xB2),// 625 stage fraction
    _RV1(H_BK_VE_ENC(0x0E), 0x01),
    _RV1(L_BK_VE_ENC(0x25), 0x1C),// av st
    _RV1(H_BK_VE_ENC(0x25), 0x01),
    _RV1(L_BK_VE_ENC(0x26), 0x96),// av end
    _RV1(H_BK_VE_ENC(0x26), 0x06),
    _RV1(L_BK_VE_ENC(0x27), 0x00),// sync tip level & pad level
    _RV1(H_BK_VE_ENC(0x27), 0x10),
    _RV1(L_BK_VE_ENC(0x28), 0xFC),// sync step & blank level
    _RV1(H_BK_VE_ENC(0x28), 0x36),
    _RV1(L_BK_VE_ENC(0x29), 0x14),// burst amp & burst step
    _RV1(H_BK_VE_ENC(0x29), 0x54),
    _RV1(L_BK_VE_ENC(0x2A), 0x46),//chroma gain
    _RV1(H_BK_VE_ENC(0x2A), 0x62),
    _RV1(L_BK_VE_ENC(0x2D), 0x30),// Y clamp
    _RV1(H_BK_VE_ENC(0x2D), 0x53),
    _RV1(L_BK_VE_ENC(0x2F), 0x0C),
    _RV1(H_BK_VE_ENC(0x2F), 0x01),
    _RV1(L_BK_VE_ENC(0x78), 0x00),// disable MV

    _RV1(L_BK_VE_SRC(0x42), 0x40),// Frame line number
    _RV1(H_BK_VE_SRC(0x42), 0x02),
    _RV1(L_BK_VE_SRC(0x45), 0x40),// Field Size
    _RV1(H_BK_VE_SRC(0x45), 0x65),
    _END_OF_TBL2_
};

MS_U8  tVE_ENCODER_PAL_TBL[] =   //for DTV/ATV
{
    _RV1(L_BK_VE_ENC(0x00), 0x01),// hsync st
    _RV1(H_BK_VE_ENC(0x00), 0x7E),// hsync end
    _RV1(L_BK_VE_ENC(0x01), 0x9d),// burst st
    _RV1(H_BK_VE_ENC(0x01), 0xD9),// burst end
    _RV1(L_BK_VE_ENC(0x02), 0x00),
    _RV1(H_BK_VE_ENC(0x02), 0x00),
    _RV1(L_BK_VE_ENC(0x03), 0x07),
    _RV1(H_BK_VE_ENC(0x03), 0x00),
    _RV1(L_BK_VE_ENC(0x04), 0x00),// contrast
    _RV1(H_BK_VE_ENC(0x04), 0x50),// contrast
    _RV1(L_BK_VE_ENC(0x06), 0x02),// H total
    _RV1(H_BK_VE_ENC(0x06), 0x00),
    _RV1(L_BK_VE_ENC(0x09), 0xC0),
    _RV1(H_BK_VE_ENC(0x09), 0x06),
    _RV1(L_BK_VE_ENC(0x0A), 0x00),// brightness
    _RV1(H_BK_VE_ENC(0x0A), 0x80),// brightness
    _RV1(L_BK_VE_ENC(0x0B), 0xCB),// burst phase step
    _RV1(H_BK_VE_ENC(0x0B), 0x8A),
    _RV1(L_BK_VE_ENC(0x0C), 0x09),
    _RV1(H_BK_VE_ENC(0x0C), 0x2A),
    _RV1(L_BK_VE_ENC(0x0D), 0x2E),// lower stage fraction
    _RV1(H_BK_VE_ENC(0x0D), 0x05),
    _RV1(L_BK_VE_ENC(0x0E), 0xB2),// 625 stage fraction
    _RV1(H_BK_VE_ENC(0x0E), 0x01),
    _RV1(L_BK_VE_ENC(0x25), 0x1C),// av st
    _RV1(H_BK_VE_ENC(0x25), 0x01),
    _RV1(L_BK_VE_ENC(0x26), 0x96),// av end
    _RV1(H_BK_VE_ENC(0x26), 0x06),
    _RV1(L_BK_VE_ENC(0x27), 0x00),// sync tip level & pad level
    _RV1(H_BK_VE_ENC(0x27), 0x10),
    _RV1(L_BK_VE_ENC(0x28), 0xFC),// sync step & blank level
    _RV1(H_BK_VE_ENC(0x28), 0x36),
    _RV1(L_BK_VE_ENC(0x29), 0x14),// burst amp & burst step
    _RV1(H_BK_VE_ENC(0x29), 0x54),
    _RV1(L_BK_VE_ENC(0x2A), 0x46),//chroma gain
    _RV1(H_BK_VE_ENC(0x2A), 0x62),
    _RV1(L_BK_VE_ENC(0x2D), 0x30),// Y clamp
    _RV1(H_BK_VE_ENC(0x2D), 0x53),
  //_RV1(L_BK_VE_ENC(0x2E), 0x88), //modify for digital TTX
  //_RV1(H_BK_VE_ENC(0x2E), 0x00),
    _RV1(L_BK_VE_ENC(0x2F), 0x0C),
    _RV1(H_BK_VE_ENC(0x2F), 0x01),


    _RV1(L_BK_VE_ENC(0x78), 0x00),// disable MV

    _RV1(L_BK_VE_SRC(0x42), 0x40),// Frame line number
    _RV1(H_BK_VE_SRC(0x42), 0x02),
    _RV1(L_BK_VE_SRC(0x45), 0x40),// Field Size
    _RV1(H_BK_VE_SRC(0x45), 0x65),
    _END_OF_TBL2_
};

MS_U8 tVE_COEF_NTSC_TBL[] =
{// disable filter
 /* _RV1(L_BK_VE_ENC(0x0F), 0x00),// lfir_coef1
    _RV1(H_BK_VE_ENC(0x0F), 0x00),
    _RV1(L_BK_VE_ENC(0x10), 0x00),// lfir_coef2
    _RV1(H_BK_VE_ENC(0x10), 0x00),
    _RV1(L_BK_VE_ENC(0x11), 0x00),// lfir_coef3
    _RV1(H_BK_VE_ENC(0x11), 0x00),
    _RV1(L_BK_VE_ENC(0x12), 0x00),// lfir_coef4
    _RV1(H_BK_VE_ENC(0x12), 0x00),
    _RV1(L_BK_VE_ENC(0x13), 0x00),// lfir_coef5
    _RV1(H_BK_VE_ENC(0x13), 0x00),
    _RV1(L_BK_VE_ENC(0x14), 0x00),// lfir_coef6
    _RV1(H_BK_VE_ENC(0x14), 0x00),
    _RV1(L_BK_VE_ENC(0x15), 0x00),// lfir_coef7
    _RV1(H_BK_VE_ENC(0x15), 0x00),
    _RV1(L_BK_VE_ENC(0x16), 0x00),// lfir_coef8
    _RV1(H_BK_VE_ENC(0x16), 0x00),
    _RV1(L_BK_VE_ENC(0x17), 0x00),// lfir_coef9
    _RV1(H_BK_VE_ENC(0x17), 0x00),
    _RV1(L_BK_VE_ENC(0x18), 0x00),// lfir_coef10
    _RV1(H_BK_VE_ENC(0x18), 0x00),
    _RV1(L_BK_VE_ENC(0x19), 0x00),// lfir_coef11
    _RV1(H_BK_VE_ENC(0x19), 0x08),
    _RV1(L_BK_VE_ENC(0x1A), 0x00),// cfir_coef1
    _RV1(H_BK_VE_ENC(0x1A), 0x00),
    _RV1(L_BK_VE_ENC(0x1B), 0x00),// cfir_coef2
    _RV1(H_BK_VE_ENC(0x1B), 0x00),
    _RV1(L_BK_VE_ENC(0x1C), 0x00),// cfir_coef3
    _RV1(H_BK_VE_ENC(0x1C), 0x00),
    _RV1(L_BK_VE_ENC(0x1D), 0x00),// cfir_coef4
    _RV1(H_BK_VE_ENC(0x1D), 0x00),
    _RV1(L_BK_VE_ENC(0x1E), 0x00),// cfir_coef5
    _RV1(H_BK_VE_ENC(0x1E), 0x00),
    _RV1(L_BK_VE_ENC(0x1F), 0x00),// cfir_coef6
    _RV1(H_BK_VE_ENC(0x1F), 0x00),
    _RV1(L_BK_VE_ENC(0x20), 0x00),// cfir_coef7
    _RV1(H_BK_VE_ENC(0x20), 0x00),
    _RV1(L_BK_VE_ENC(0x21), 0x00),// cfir_coef7
    _RV1(H_BK_VE_ENC(0x21), 0x00),
    _RV1(L_BK_VE_ENC(0x22), 0x00),// cfir_coef9
    _RV1(H_BK_VE_ENC(0x22), 0x00),
    _RV1(L_BK_VE_ENC(0x23), 0x00),// cfir_coef10
    _RV1(H_BK_VE_ENC(0x23), 0x00),
    _RV1(L_BK_VE_ENC(0x24), 0x00),// cfir_coef11
    _RV1(H_BK_VE_ENC(0x24), 0x04),*/
    _END_OF_TBL2_
};

MS_U8 tVE_COEF_PAL_TBL[] =
{
 /* _RV1(L_BK_VE_ENC(0x0F), 0x00),// lfir_coef1
    _RV1(H_BK_VE_ENC(0x0F), 0x00),
    _RV1(L_BK_VE_ENC(0x10), 0x00),// lfir_coef2
    _RV1(H_BK_VE_ENC(0x10), 0x00),
    _RV1(L_BK_VE_ENC(0x11), 0x00),// lfir_coef3
    _RV1(H_BK_VE_ENC(0x11), 0x00),
    _RV1(L_BK_VE_ENC(0x12), 0x00),// lfir_coef4
    _RV1(H_BK_VE_ENC(0x12), 0x00),
    _RV1(L_BK_VE_ENC(0x13), 0x00),// lfir_coef5
    _RV1(H_BK_VE_ENC(0x13), 0x00),
    _RV1(L_BK_VE_ENC(0x14), 0x00),// lfir_coef6
    _RV1(H_BK_VE_ENC(0x14), 0x00),
    _RV1(L_BK_VE_ENC(0x15), 0x00),// lfir_coef7
    _RV1(H_BK_VE_ENC(0x15), 0x00),
    _RV1(L_BK_VE_ENC(0x16), 0x00),// lfir_coef8
    _RV1(H_BK_VE_ENC(0x16), 0x00),
    _RV1(L_BK_VE_ENC(0x17), 0x00),// lfir_coef9
    _RV1(H_BK_VE_ENC(0x17), 0x00),
    _RV1(L_BK_VE_ENC(0x18), 0x00),// lfir_coef10
    _RV1(H_BK_VE_ENC(0x18), 0x00),
    _RV1(L_BK_VE_ENC(0x19), 0x00),// lfir_coef11
    _RV1(H_BK_VE_ENC(0x19), 0x08),
    _RV1(L_BK_VE_ENC(0x1A), 0x00),// cfir_coef1
    _RV1(H_BK_VE_ENC(0x1A), 0x00),
    _RV1(L_BK_VE_ENC(0x1B), 0x00),// cfir_coef2
    _RV1(H_BK_VE_ENC(0x1B), 0x00),
    _RV1(L_BK_VE_ENC(0x1C), 0x00),// cfir_coef3
    _RV1(H_BK_VE_ENC(0x1C), 0x00),
    _RV1(L_BK_VE_ENC(0x1D), 0x00),// cfir_coef4
    _RV1(H_BK_VE_ENC(0x1D), 0x00),
    _RV1(L_BK_VE_ENC(0x1E), 0x00),// cfir_coef5
    _RV1(H_BK_VE_ENC(0x1E), 0x00),
    _RV1(L_BK_VE_ENC(0x1F), 0x00),// cfir_coef6
    _RV1(H_BK_VE_ENC(0x1F), 0x00),
    _RV1(L_BK_VE_ENC(0x20), 0x00),// cfir_coef7
    _RV1(H_BK_VE_ENC(0x20), 0x00),
    _RV1(L_BK_VE_ENC(0x21), 0x00),// cfir_coef7
    _RV1(H_BK_VE_ENC(0x21), 0x00),
    _RV1(L_BK_VE_ENC(0x22), 0x00),// cfir_coef9
    _RV1(H_BK_VE_ENC(0x22), 0x00),
    _RV1(L_BK_VE_ENC(0x23), 0x00),// cfir_coef10
    _RV1(H_BK_VE_ENC(0x23), 0x00),
    _RV1(L_BK_VE_ENC(0x24), 0x00),// cfir_coef11
    _RV1(H_BK_VE_ENC(0x24), 0x04),*/
    _END_OF_TBL2_
};

MS_U8 tVE_VBI_NTSC_TBL[] =
{
    _RV1(L_BK_VE_ENC(0x2E), 0x00),// VBI mode
    _RV1(H_BK_VE_ENC(0x2E), 0x00),
    _RV1(L_BK_VE_ENC(0x4E), 0x15),// ccvbi_st1
    _RV1(H_BK_VE_ENC(0x4E), 0x00),
    _RV1(L_BK_VE_ENC(0x4F), 0x15),// ccvbi_end1
    _RV1(H_BK_VE_ENC(0x4F), 0x00),
    _RV1(L_BK_VE_ENC(0x50), 0x1C),// ccvbi_st2
    _RV1(H_BK_VE_ENC(0x50), 0x01),
    _RV1(L_BK_VE_ENC(0x51), 0x1C),// ccvbi_end2
    _RV1(H_BK_VE_ENC(0x51), 0x01),
    _RV1(L_BK_VE_ENC(0x56), 0x14),// wssvbi_st1
    _RV1(H_BK_VE_ENC(0x56), 0x00),
    _RV1(L_BK_VE_ENC(0x57), 0x14),// wssvbi_end1
    _RV1(H_BK_VE_ENC(0x57), 0x00),
    _RV1(L_BK_VE_ENC(0x6C), 0x1B),// wssvbi_st2
    _RV1(H_BK_VE_ENC(0x6C), 0x01),
    _RV1(L_BK_VE_ENC(0x6D), 0x1B),// wssvbi_end2
    _RV1(H_BK_VE_ENC(0x6D), 0x01),
    _RV1(L_BK_VE_ENC(0x5C), 0xD6),// cc_phs_step [15:0]
    _RV1(H_BK_VE_ENC(0x5C), 0x1D),
    _RV1(L_BK_VE_ENC(0x5D), 0xC6),// cc_phs_step [31:16]
    _RV1(H_BK_VE_ENC(0x5D), 0x04),
    _RV1(L_BK_VE_ENC(0x60), 0x84),// wws_phs_step [15:0]
    _RV1(H_BK_VE_ENC(0x60), 0x0F),
    _RV1(L_BK_VE_ENC(0x61), 0x3E),// wws_phs_step [31:16]
    _RV1(H_BK_VE_ENC(0x61), 0x04),
    _RV1(L_BK_VE_ENC(0x64), 0x10),// cc_st
    _RV1(H_BK_VE_ENC(0x64), 0x01),
    _RV1(L_BK_VE_ENC(0x66), 0x2E),// wws_st
    _RV1(H_BK_VE_ENC(0x66), 0x01),
    _RV1(L_BK_VE_ENC(0x68), 0x18),// cc_lvl
    _RV1(H_BK_VE_ENC(0x68), 0x01),
    _RV1(L_BK_VE_ENC(0x6A), 0x90),// wws_lvl
    _RV1(H_BK_VE_ENC(0x6A), 0x01),

    _END_OF_TBL2_
};

MS_U8 tVE_VBI_PAL_TBL[] =
{
    // TELETEXT
    _RV1(L_BK_VE_ENC(0x2E), 0x88),// vbi mode

    _RV1(H_BK_VE_ENC(0x2E), 0x00),
    _RV1(L_BK_VE_ENC(0x52), 0x10),// vpsvib_st1
    _RV1(H_BK_VE_ENC(0x52), 0x00),
    _RV1(L_BK_VE_ENC(0x53), 0x10),// vpsvib_end1
    _RV1(H_BK_VE_ENC(0x53), 0x00),
    _RV1(L_BK_VE_ENC(0x54), 0x1C),// vpsvib_st2
    _RV1(H_BK_VE_ENC(0x54), 0x01),
    _RV1(L_BK_VE_ENC(0x55), 0x1B),// vpsvib_end2
    _RV1(H_BK_VE_ENC(0x55), 0x01),
    _RV1(L_BK_VE_ENC(0x56), 0x17),// wssvbi_st1
    _RV1(H_BK_VE_ENC(0x56), 0x00),
    _RV1(L_BK_VE_ENC(0x57), 0x17),// wssvbi_end1
    _RV1(H_BK_VE_ENC(0x57), 0x00),
    _RV1(L_BK_VE_ENC(0x6C), 0x1C),// wssvbi_st2
    _RV1(H_BK_VE_ENC(0x6C), 0x01),
    _RV1(L_BK_VE_ENC(0x6D), 0x1B),// wssvbi_end2
    _RV1(H_BK_VE_ENC(0x6D), 0x01),
    _RV1(L_BK_VE_ENC(0x58), 0x07),// ttvbi_st1
    _RV1(H_BK_VE_ENC(0x58), 0x00),
    _RV1(L_BK_VE_ENC(0x59), 0x16),// ttvbi_end1
    _RV1(H_BK_VE_ENC(0x59), 0x00),
    _RV1(L_BK_VE_ENC(0x5A), 0x3F),// ttvbi_st2
    _RV1(H_BK_VE_ENC(0x5A), 0x01),
    _RV1(L_BK_VE_ENC(0x5B), 0x4F),// ttvbi_end2
    _RV1(H_BK_VE_ENC(0x5B), 0x01),
    _RV1(L_BK_VE_ENC(0x5E), 0xDA),// vps_phs_step[15:0]
    _RV1(H_BK_VE_ENC(0x5E), 0x4B),
    _RV1(L_BK_VE_ENC(0x5F), 0x68),// vps_phs_step[31:16]
    _RV1(H_BK_VE_ENC(0x5F), 0x2F),
    _RV1(L_BK_VE_ENC(0x60), 0xDA),// wws_phs_step[15:0]
    _RV1(H_BK_VE_ENC(0x60), 0x4B),
    _RV1(L_BK_VE_ENC(0x61), 0x68),// wws_phs_step[31:16]
    _RV1(H_BK_VE_ENC(0x61), 0x2F),
    _RV1(L_BK_VE_ENC(0x62), 0x71),// tt_phs_step[15:0]
    _RV1(H_BK_VE_ENC(0x62), 0x1C),
    _RV1(L_BK_VE_ENC(0x63), 0xC7),// tt_phs_step[31:16]
    _RV1(H_BK_VE_ENC(0x63), 0x41),
    _RV1(L_BK_VE_ENC(0x65), 0x53),// vps_st
    _RV1(H_BK_VE_ENC(0x65), 0x01),
    _RV1(L_BK_VE_ENC(0x66), 0x30),// wws_st
    _RV1(H_BK_VE_ENC(0x66), 0x01),
    _RV1(L_BK_VE_ENC(0x67), 0x15),// tt_st
    _RV1(H_BK_VE_ENC(0x67), 0x01),
    _RV1(L_BK_VE_ENC(0x69), 0x90),// vps_lvl
    _RV1(H_BK_VE_ENC(0x69), 0x01),
    _RV1(L_BK_VE_ENC(0x6A), 0x90),// wws_lvl
    _RV1(H_BK_VE_ENC(0x6A), 0x01),
    _RV1(L_BK_VE_ENC(0x6B), 0x90),// tt_lvl
    _RV1(H_BK_VE_ENC(0x6B), 0x01),
    _RV1(H_BK_VE_ENC(0x3D), 0x00),
    _END_OF_TBL2_
};

MS_U8 tVE_CCIROUT_NTSC_TBL[] =
{
    _RV1(L_BK_VE_SRC(0x47), 0x0D),// frame line number
    _RV1(H_BK_VE_SRC(0x47), 0x02),
    _RV1(L_BK_VE_SRC(0x48), 0x01),// F0 blank star
    _RV1(H_BK_VE_SRC(0x48), 0x00),
    _RV1(L_BK_VE_SRC(0x49), 0x17),// F0 blank end
    _RV1(H_BK_VE_SRC(0x49), 0x00),
    _RV1(L_BK_VE_SRC(0x4A), 0x07),// F1 blank start
    _RV1(H_BK_VE_SRC(0x4A), 0x01),
    _RV1(L_BK_VE_SRC(0x4B), 0x1E),// F1 blank end
    _RV1(H_BK_VE_SRC(0x4B), 0x01),
    _RV1(L_BK_VE_SRC(0x4C), 0x04),// F0 start
    _RV1(H_BK_VE_SRC(0x4C), 0x00),
    _RV1(L_BK_VE_SRC(0x4D), 0x0A),// F0 end
    _RV1(H_BK_VE_SRC(0x4D), 0x01),
    _RV1(L_BK_VE_SRC(0x4E), 0x01),// F0 V start
    _RV1(H_BK_VE_SRC(0x4E), 0x00),
    _RV1(L_BK_VE_SRC(0x4F), 0x14),// F0 V end
    _RV1(H_BK_VE_SRC(0x4F), 0x00),
    _RV1(L_BK_VE_SRC(0x50), 0x08),// F1 V start
    _RV1(H_BK_VE_SRC(0x50), 0x01),
    _RV1(L_BK_VE_SRC(0x51), 0x1B),// F1 V end
    _RV1(H_BK_VE_SRC(0x51), 0x01),
    _END_OF_TBL2_,
};

MS_U8 tVE_CCIROUT_PAL_TBL[] =
{
    _RV1(L_BK_VE_SRC(0x47), 0x71),// frame line number
    _RV1(H_BK_VE_SRC(0x47), 0x02),
    _RV1(L_BK_VE_SRC(0x48), 0x01),// F0 blank star
    _RV1(H_BK_VE_SRC(0x48), 0x00),
    _RV1(L_BK_VE_SRC(0x49), 0x19),// F0 blank end
    _RV1(H_BK_VE_SRC(0x49), 0x00),
    _RV1(L_BK_VE_SRC(0x4A), 0x39),// F1 blank start
    _RV1(H_BK_VE_SRC(0x4A), 0x01),
    _RV1(L_BK_VE_SRC(0x4B), 0x52),// F1 blank end
    _RV1(H_BK_VE_SRC(0x4B), 0x01),
    _RV1(L_BK_VE_SRC(0x4C), 0x02),// F0 start
    _RV1(H_BK_VE_SRC(0x4C), 0x00),
    _RV1(L_BK_VE_SRC(0x4D), 0x3A),// F0 end
    _RV1(H_BK_VE_SRC(0x4D), 0x01),
    _RV1(L_BK_VE_SRC(0x4E), 0x07),// F0 V start
    _RV1(H_BK_VE_SRC(0x4E), 0x01),
    _RV1(L_BK_VE_SRC(0x4F), 0x18),// F0 V end
    _RV1(H_BK_VE_SRC(0x4F), 0x00),
    _RV1(L_BK_VE_SRC(0x50), 0x38),// F1 V start
    _RV1(H_BK_VE_SRC(0x50), 0x01),
    _RV1(L_BK_VE_SRC(0x51), 0x51),// F1 V end
    _RV1(H_BK_VE_SRC(0x51), 0x01),
    _END_OF_TBL2_,
};


MS_VE_Out_VideoSYS VE_OUT_VIDEOSTD_TBL[MS_VE_VIDEOSYS_NUM] =
{         //  Reg Tbl                   Coef_TBL         VBI TBL      vtotal_525, bPALSwitch, bPALOut
/*NSTC */    {tVE_ENCODER_NTSC_TBL,     tVE_COEF_NTSC_TBL, tVE_VBI_NTSC_TBL,   0,       0,      0},
/*NSTC_443*/ {tVE_ENCODER_NTSC_443_TBL, tVE_COEF_NTSC_TBL, tVE_VBI_NTSC_TBL,   0,       0,      0},
/*NSTC_J*/   {tVE_ENCODER_NTSC_J_TBL,   tVE_COEF_NTSC_TBL, tVE_VBI_NTSC_TBL,   0,       0,      0},
/*PAL_M*/    {tVE_ENCODER_PAL_M_TBL,    tVE_COEF_PAL_TBL,  tVE_VBI_NTSC_TBL,    0,       1,      1},
/*PAL_N*/    {tVE_ENCODER_PAL_N_TBL,    tVE_COEF_PAL_TBL,  tVE_VBI_PAL_TBL,    1,       1,      1},
/*PAL_NC*/   {tVE_ENCODER_PAL_NC_TBL,   tVE_COEF_PAL_TBL,  tVE_VBI_PAL_TBL,    1,       1,      1},
/*PAL_B*/    {tVE_ENCODER_PAL_TBL,      tVE_COEF_PAL_TBL,  tVE_VBI_PAL_TBL,    1,       1,      1},
/*SECAM*/    {NULL,                     NULL,              NULL,               1,       1,      1},
};



