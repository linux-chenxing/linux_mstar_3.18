///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016-2017 MStar Semiconductor, Inc.
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
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_graphic_io.h
// @brief  Graphic Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MSTAR_FB_GRAPHIC_H
#define MSTAR_FB_GRAPHIC_H


/* ========================================================================= */
/* prototype of function */
static int mstar_fb_set_par(struct fb_info *pinfo);
static int mstar_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);
static int mstar_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info);
static int mstar_fb_blank(int blank, struct fb_info *info);
static int mstar_fb_setcolreg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp, struct fb_info *info);
static int _mstar_fb_mmap(struct fb_info *pinfo, struct vm_area_struct *vma);
static int mstar_fb_open(struct fb_info *info, int user);
static int mstar_fb_release(struct fb_info *info, int user);
static void mstar_fb_destroy(struct fb_info *info);
static int mstar_fb_ioctl(struct fb_info *pinfo, MS_U32 u32Cmd, unsigned long u32Arg);
static void mstar_fb_fillrect(struct fb_info *p, const struct fb_fillrect *rect);
static void mstar_fb_copyarea(struct fb_info *p, const struct fb_copyarea *area);
static void mstar_fb_imageblit(struct fb_info *p, const struct fb_image *image);
static MS_U32 get_line_length(int xres_virtual, int bpp);
static void _fb_strewin_update(struct fb_var_screeninfo *var, MS_U8 u8GopId,
    MS_U16 dstX, MS_U16 dstY, MS_U16 u16DstWidth, MS_U16 u16DstHeight);
static void _fb_gwin_enable(MS_U8 u8GopIdx, MS_U8 u8GwinId, MS_BOOL bEnable);
static void _fb_buf_init(struct fb_info *pinfo, unsigned long pa, int fbseq);
static EN_DRV_GOPColorType get_color_fmt(struct fb_var_screeninfo *var);
#endif
