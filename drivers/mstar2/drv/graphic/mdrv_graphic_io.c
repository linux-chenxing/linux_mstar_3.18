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
// @file   mdrv_graphic_io.c
// @brief  Graphic Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/pfn.h>
#include <linux/delay.h>
#include <linux/compat.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/uaccess.h>

//drver header files
#include "mdrv_mstypes.h"
#include "chip_int.h"
#include "mst_devid.h"
#include "mdrv_graphic.h"
#include "mdrv_graphic_io.h"
#include "mdrv_system.h"
#include "mhal_graphic.h"
#include "mhal_graphic_reg.h"

//ini parser.h
#include "iniparser.h"

//=============================================================================
/* GOP Definition */
#define BPX                 32    /* bits per pixel */
#define PANEL_H_START       112
#define XRES                1920
#define YRES                1080
#define V_XRES              XRES
//#define V_YRES              YRES*2 /*  x2 for double buffer */
#define V_YRES              YRES /*  x2 for single buffer */
#define VIDEOMEMSIZE        (V_XRES * V_YRES * (BPX >> 3))  /* frame buffer size */
#define ALIGN_UP(_val_,_align_) ((_val_ + _align_ - 1) & (~(_align_ - 1)))

#define MIU0_BUS_OFFSET 	ARM_MIU0_BUS_BASE
#define MIU1_BUS_OFFSET   ARM_MIU1_BUS_BASE
#define MIU2_BUS_OFFSET   ARM_MIU2_BUS_BASE
#define MIU1_INTERVAL 		(ARM_MIU1_BUS_BASE - ARM_MIU0_BUS_BASE)

static struct fb_ops mstar_fb_ops =
{
    .owner = THIS_MODULE,
    .fb_open = mstar_fb_open,
    .fb_release = mstar_fb_release,
    .fb_mmap = _mstar_fb_mmap,
    .fb_set_par = mstar_fb_set_par,
    .fb_check_var = mstar_fb_check_var,
    .fb_blank = mstar_fb_blank,
    .fb_pan_display = mstar_fb_pan_display,
    .fb_setcolreg = mstar_fb_setcolreg,
    .fb_fillrect = mstar_fb_fillrect,
    .fb_copyarea = mstar_fb_copyarea,
    .fb_imageblit = mstar_fb_imageblit,
    .fb_destroy = mstar_fb_destroy,
    .fb_ioctl = mstar_fb_ioctl,
};

//-------------------------------------------------------------------------------------------------
//  MstarFB sturct and function
//-------------------------------------------------------------------------------------------------
typedef struct
{
    MS_U8 u8GopIdx;
    MS_U8 u8GwinId;
    E_DRV_GOP_SEL_TYPE eMiuSel;
    MI_FB_DstDisplayplane_e eGopDst;
    MI_FB_OutputColorSpace_e eOutputColorSpace;
    MS_PHYADDR phyAddr;
    //GWIN info
    DRV_GWIN_INFO stGwinInfo;
    //Stretch Win Info
    DRV_STRETCHWIN_INFO stStretchWinInfo;
    //Global Alpha
    MI_FB_GlobalAlpha_t stAlpha;
    //Color Key
    MI_FB_ColorKey_t stColorKey;
    //Whether shown
    MS_BOOL bShown;
    //reference count
    unsigned int ref_count;
}HwLayerInfo_t;

typedef struct
{
    MS_U8 u8GopIdx;
    MS_U8 u8GwinId;
    E_DRV_GOP_SEL_TYPE eMiuSel;
    MI_FB_DstDisplayplane_e eGopDst;
    MI_FB_OutputColorSpace_e eOutputColorSpace;
    MS_PHYADDR phyAddr;
    //GWIN info
    DRV_GWIN_INFO stGwinInfo;
    //Cursor Info
    MS_U32 u32HotSpotX;
    MS_U32 u32HotSpotY;
    MS_U16 u16IconWidth;
    MS_U16 u16IconHeight;
    MI_FB_GlobalAlpha_t stAlpha;
    MI_FB_ColorKey_t stColorKey;
    //Whether shown
    MS_BOOL bShown;
}HwCursorInfo_t;

//static const char* config_file = "/data/fbdev.ini";
static const char* config_file = "/var/sda1/fbdev.ini";
static HwLayerInfo_t* fbHwlayerInfos = NULL;
static HwCursorInfo_t* fbHwCursorInfo = NULL;
static struct fb_fix_screeninfo* mstar_fb_fix_infos  = NULL;
static struct fb_var_screeninfo* mstar_fb_var_infos = NULL;
static int numFbHwlayer = 0;
static int first_fb_node = 0;

void _fb_gwin_update(struct fb_var_screeninfo *fbvar,  struct fb_info *pinfo)
{
    DRV_GWIN_INFO WinInfo;
    MS_U32 bytes_per_pixel;
    int fbIdx = pinfo->node - first_fb_node;
    MS_U8 u8GopIdx = fbHwlayerInfos[fbIdx].u8GopIdx;
    MS_U8 u8GwinIdx = fbHwlayerInfos[fbIdx].u8GwinId;

    if(fbvar->bits_per_pixel == 1)
       bytes_per_pixel = 1;
    else
       bytes_per_pixel = fbvar->bits_per_pixel/8;

    if(pinfo->fix.smem_start >= MIU1_INTERVAL)
       WinInfo.u32Addr = pinfo->fix.smem_start - MIU1_INTERVAL;
    else
       WinInfo.u32Addr = pinfo->fix.smem_start;

    if(fbvar->xoffset || fbvar->yoffset)
    {
        WinInfo.u32Addr += (fbvar->xoffset + fbvar->xres_virtual*fbvar->yoffset)* bytes_per_pixel;
    }

    //WinInfo.clrType = E_DRV_GOP_COLOR_ABGR8888;
    WinInfo.clrType = fbHwlayerInfos[fbIdx].stGwinInfo.clrType;
    WinInfo.u16HStart = 0;
    WinInfo.u16HEnd = fbvar->xres;
    WinInfo.u16VStart = 0;
    WinInfo.u16VEnd = fbvar->yres;
    WinInfo.u16Pitch = fbvar->xres_virtual;
    _MDrv_GOP_SetGwinInfo(u8GopIdx,u8GwinIdx,WinInfo);
    _MDrv_GRAPHIC_UpdateReg(u8GopIdx);
}

static int mstar_fb_set_par(struct fb_info *pinfo)
{
    struct fb_var_screeninfo *var = &pinfo->var;
    MS_U32 bits_per_pixel;

    switch (var->bits_per_pixel)
    {
       case 32:
       case 16:
            pinfo->fix.visual = FB_VISUAL_TRUECOLOR;
            bits_per_pixel = var->bits_per_pixel;
            break;
       case 1:
            pinfo->fix.visual = FB_VISUAL_MONO01;
            bits_per_pixel = 8;
            break;
       default:
            pinfo->fix.visual = FB_VISUAL_PSEUDOCOLOR;
            bits_per_pixel = 8;
            break;
    }

    pinfo->fix.line_length = (var->xres_virtual * bits_per_pixel) / 8;
    pinfo->fix.xpanstep = GOP_WordUnit/(bits_per_pixel >>3);
    /* activate this new configuration */

   return 0;
}

static int mstar_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
    MS_U32 line_length,bits_per_pixel;
    EN_DRV_GOPColorType ColorFmt;

    /*
     *  FB_VMODE_CONUPDATE and FB_VMODE_SMOOTH_XPAN are equal!
     *  as FB_VMODE_SMOOTH_XPAN is only used internally
     */
     if (var->vmode & FB_VMODE_CONUPDATE)
     {
         var->vmode |= FB_VMODE_YWRAP;
         var->xoffset = info->var.xoffset;
         var->yoffset = info->var.yoffset;
     }
     /*
      *  Some very basic checks
      */

     if (!var->xres)
         var->xres = info->var.xres;
     if (!var->yres)
         var->yres = info->var.xres;
     if (var->xres > var->xres_virtual)
         var->xres_virtual = var->xres;
     if (var->yres > var->yres_virtual)
         var->yres_virtual = var->yres;
     if (var->bits_per_pixel <= 1)
     {
         var->bits_per_pixel = 1;
         bits_per_pixel = 8;
     }else if (var->bits_per_pixel <= 8)
     {
         var->bits_per_pixel = 8;
         bits_per_pixel = 8;
     }else if (var->bits_per_pixel <= 16)
     {
         var->bits_per_pixel = 16;
         bits_per_pixel = 16;
     }else if (var->bits_per_pixel <= 32)
     {
         var->bits_per_pixel = 32;
         bits_per_pixel = 32;
     }else
         return -EINVAL;

     if (var->xres_virtual < var->xoffset + var->xres)
         var->xres_virtual = var->xoffset + var->xres;
     if (var->yres_virtual < var->yoffset + var->yres)
         var->yres_virtual = var->yoffset + var->yres;

     /*
      *  Memory limit
      */
     line_length = get_line_length(var->xres_virtual,bits_per_pixel);
     if(line_length * var->yres_virtual > VIDEOMEMSIZE)
        return -ENOMEM;

     /*
      * Now that we checked it we alter var. The reason being is that the video
      * mode passed in might not work but slight changes to it might make it
      * work. This way we let the user know what is acceptable.
      */
      ColorFmt = get_color_fmt(var);
      if(ColorFmt == E_DRV_GOP_COLOR_INVALID)
         return -EINVAL;

      return 0;
}

static int mstar_fb_blank(int blank, struct fb_info *info)
{
    return 0;
}

static int mstar_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *pinfo)
{
     int fbIdx = pinfo->node - first_fb_node;
     MS_U8 u8GopId = fbHwlayerInfos[fbIdx].u8GopIdx;
     MS_U8 u8GwinId = fbHwlayerInfos[fbIdx].u8GwinId;
     MS_U16 u16DstX = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16Xpos;
     MS_U16 u16DstY = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16Ypos;
     MS_U16 u16Width = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16DstWidth;
     MS_U16 u16Height = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16DstHeight;
     MS_BOOL bShown =fbHwlayerInfos[fbIdx].bShown;
    //update stretch win
    if( (pinfo->var.xres != var->xres) || (pinfo->var.yres != var->yres)) {
        _fb_strewin_update(var, u8GopId, u16DstX, u16DstY, u16Width, u16Height);
    }
    //update gwin
    //if(0 == strncmp(current->comm,"recovery",strlen(current->comm)))
    {
        _fb_gwin_update(var,pinfo);
    }
    //enable gwin
    if(bShown == FALSE)
    {
        _fb_gwin_enable(u8GopId,u8GwinId,TRUE);
        fbHwlayerInfos[fbIdx].bShown = TRUE;
    }
    //update  mstar_fb_var
    memcpy(&(pinfo->var), var, sizeof(struct fb_var_screeninfo));
    //update StretchWin srcWidth srcHeight
    fbHwlayerInfos[fbIdx].stStretchWinInfo.u16SrcWidth = pinfo->var.xres;
    fbHwlayerInfos[fbIdx].stStretchWinInfo.u16SrcHeight = pinfo->var.yres;
    return 0;
}


static int mstar_fb_setcolreg(unsigned regno, unsigned red, unsigned green,
                              unsigned blue, unsigned transp, struct fb_info *info)
{
    /* grayscale works only partially under directcolor */
    if(info->var.grayscale)
    {
        /* grayscale = 0.30*R + 0.59*G + 0.11*B */
        red = green = blue = (red * 77 + green * 151 + blue * 28) >> 8;
    }

    if(info->fix.visual == FB_VISUAL_TRUECOLOR || info->fix.visual == FB_VISUAL_DIRECTCOLOR)
    {
        MS_U32 v;

        if(regno >= 16)
            return -EINVAL;

        v = (red << info->var.red.offset) | (green << info->var.green.offset) | (blue << info->var.blue.offset) | (transp << info->var.transp.offset);
            ((U32*)(info->pseudo_palette))[regno] = v;
    }
    return 0;
}

static void mstar_fb_fillrect(struct fb_info *p, const struct fb_fillrect *rect)
{
#ifdef CONFIG_FB_VIRTUAL
        sys_fillrect(p, rect);
#endif
}

static void mstar_fb_copyarea(struct fb_info *p, const struct fb_copyarea *area)
{
#ifdef CONFIG_FB_VIRTUAL
        sys_copyarea(p, area);
#endif
}

static void mstar_fb_imageblit(struct fb_info *p, const struct fb_image *image)
{
#ifdef CONFIG_FB_VIRTUAL
        sys_imageblit(p, image);
#endif
}

static void mstar_fb_destroy(struct fb_info *info)
{
    printk("\33[0;31m  fb%d mstar_fb_destroy was invoked!\33[m \n",info->node);
    if(info->screen_base) {
      iounmap(info->screen_base);
      info->screen_base = NULL;
    }
    framebuffer_release(info);
}

static MS_U32 get_line_length(int xres_virtual, int bpp)
{
    MS_U32 length;

    length = xres_virtual * bpp;
    length = (length + 31) & ~31;
    length >>= 3;

    return (length);
}

static EN_DRV_GOPColorType get_color_fmt(struct fb_var_screeninfo *var)
{
    EN_DRV_GOPColorType ColorFmt = E_DRV_GOP_COLOR_INVALID;

    switch (var->bits_per_pixel) {
        case 1:
        case 8:
            var->red.offset = 0;
            var->red.length = 8;
            var->green.offset = 0;
            var->green.length = 8;
            var->blue.offset = 0;
            var->blue.length = 8;
            var->transp.offset = 0;
            var->transp.length = 0;
            ColorFmt = E_DRV_GOP_COLOR_I8;
            break;
        case 16:
            if ( (var->transp.length) && (var->transp.offset == 15))
            {
                var->blue.offset = 0;
                var->blue.length = 5;
                var->green.offset = 5;
                var->green.length = 5;
                var->red.offset = 10;
                var->red.length = 5;
                var->transp.offset = 15;
                var->transp.length = 1;
                ColorFmt = E_DRV_GOP_COLOR_ARGB1555;
            }else if ((var->transp.length) && (var->transp.offset == 12))
            {
                var->blue.offset = 0;
                var->blue.length = 4;
                var->green.offset = 4;
                var->green.length = 4;
                var->red.offset = 8;
                var->red.length = 4;
                var->transp.offset = 12;
                var->transp.length = 4;
                ColorFmt = E_DRV_GOP_COLOR_ARGB4444;
            }else if ( (var->transp.length) && (var->transp.offset == 0))
            {
                var->transp.offset = 0;
                var->transp.length = 1;
                var->blue.offset = 1;
                var->blue.length = 5;
                var->green.offset = 6;
                var->green.length = 5;
                var->red.offset = 11;
                var->red.length = 5;
                ColorFmt = E_DRV_GOP_COLOR_RGBA5551;
            }else
            {
                /* RGB 565 */
                var->blue.offset = 0;
                var->blue.length = 5;
                var->green.offset = 5;
                var->green.length = 6;
                var->red.offset = 11;
                var->red.length = 5;
                var->transp.offset = 0;
                var->transp.length = 0;
                ColorFmt = E_DRV_GOP_COLOR_RGB565;
            }
            break;
            case 32:/* ARGB 8888 */
                if ((var->transp.length) && (var->red.offset == 16))
                {
                    var->blue.offset = 0;
                    var->blue.length = 8;
                    var->green.offset = 8;
                    var->green.length = 8;
                    var->red.offset = 16;
                    var->red.length = 8;
                    var->transp.offset = 24;
                    var->transp.length = 8;
                    ColorFmt = E_DRV_GOP_COLOR_ARGB8888;
                }else
                {
                    var->red.offset = 0;
                    var->red.length = 8;
                    var->green.offset = 8;
                    var->green.length = 8;
                    var->blue.offset = 16;
                    var->blue.length = 8;
                    var->transp.offset = 24;
                    var->transp.length = 8;
                    ColorFmt = E_DRV_GOP_COLOR_ABGR8888;
                }
                break;
      }

      var->red.msb_right = 0;
      var->green.msb_right = 0;
      var->blue.msb_right = 0;
      var->transp.msb_right = 0;
      return ColorFmt;
}

static void _fb_buf_init(struct fb_info *pinfo, unsigned long pa, int fbSeq)
{
    E_DRV_GOP_SEL_TYPE miuSel = fbHwlayerInfos[fbSeq].eMiuSel;
    pinfo->fix.smem_start = pa;
    //pinfo->fix.smem_len = pinfo->var.xres * pinfo->var.yres * (pinfo->var.bits_per_pixel >> 3)*2/*double buffer*/;
    if (miuSel==E_DRV_GOP_SEL_MIU0) {
        pinfo->screen_base =
            (char __iomem *)ioremap(pa + MIU0_BUS_OFFSET, pinfo->fix.smem_len);
    } else if (miuSel==E_DRV_GOP_SEL_MIU1) {
        pinfo->screen_base =
            (char __iomem *)ioremap(pa + MIU1_BUS_OFFSET, pinfo->fix.smem_len);
    } else if (miuSel==E_DRV_GOP_SEL_MIU2) {
        pinfo->screen_base =
            (char __iomem *)ioremap(pa + MIU2_BUS_OFFSET, pinfo->fix.smem_len);
    }
    memset(pinfo->screen_base, 0x0, pinfo->fix.smem_len);
}

static void _fb_gwin_init(int fbseq)
{
    MS_U8 u8GopIdx = fbHwlayerInfos[fbseq].u8GopIdx;
    MS_U8 u8GwinIdx = fbHwlayerInfos[fbseq].u8GwinId;
    MI_FB_OutputColorSpace_e eOutputColorSpace
        = fbHwlayerInfos[fbseq].eOutputColorSpace;
    E_DRV_GOP_SEL_TYPE eMiuSel = fbHwlayerInfos[fbseq].eMiuSel;
    EN_DRV_GOP_DST_TYPE eGopDst = (EN_DRV_GOP_DST_TYPE)fbHwlayerInfos[fbseq].eGopDst;
    _MDrv_GRAPHIC_Init(u8GopIdx);
    _MDrv_GRAPHIC_SetHMirror(u8GopIdx,FALSE);
    _MDrv_GRAPHIC_SetVMirror(u8GopIdx,FALSE);
    _MDrv_GRAPHIC_OutputColor_EX(u8GopIdx, (EN_DRV_GOP_OUTPUT_COLOR)(eOutputColorSpace));
    _MDrv_GRAPHIC_MIUSel(u8GopIdx, eMiuSel);
    _MDrv_GRAPHIC_SetGOPDst(u8GopIdx, eGopDst, FALSE);
    //do not set Colorkey default
    _MDrv_GRAPHIC_EnableTransClr_EX(u8GopIdx,GOPTRANSCLR_FMT0, FALSE);
    _MDrv_GRAPHIC_SetBlending(u8GopIdx, u8GwinIdx, TRUE, 0xFF);
    _fb_gwin_enable(u8GopIdx,u8GwinIdx, FALSE);
}

static void _fb_gwin_enable(MS_U8 u8GopIdx,MS_U8 u8GwinIdx ,MS_BOOL bEnable)
{
    _MDrv_GRAPHIC_GWIN_Enable(u8GopIdx, u8GwinIdx, bEnable);
    _MDrv_GRAPHIC_UpdateReg(u8GopIdx);
}

static void _fb_strewin_update(struct fb_var_screeninfo *var,
    MS_U8 u8GopId, MS_U16 dstX, MS_U16 dstY, MS_U16 u16DstWidth, MS_U16 u16DstHeight)
{
    _MDrv_GRAPHIC_SetStretchWin(u8GopId,dstX,dstY,var->xres,var->yres);
    _MDrv_GRAPHIC_SetHScale(u8GopId,TRUE,var->xres,u16DstWidth);
    _MDrv_GRAPHIC_SetVScale(u8GopId,TRUE,var->yres,u16DstHeight);
}
//-------------------------------------------------------------------------------------------------
// Parse Configuration file
//-------------------------------------------------------------------------------------------------
static int readConfile(char** data)
{
    struct file* filp = NULL;
    mm_segment_t fs;
    loff_t size = 0;
    ssize_t ret = 0;
    int result = 0;
    filp = filp_open(config_file, O_RDONLY, 0644);
    if (IS_ERR_OR_NULL(filp))
    {
        printk(KERN_ERR "\33[0;36m read /config/fbdev.ini failed!\n");
        return -1;
    }
    do
    {
        fs = get_fs();
        set_fs(KERNEL_DS);
        size = filp->f_op->llseek(filp, 0, SEEK_END);
        *data = (char*)kzalloc(size + 1, GFP_KERNEL);
        if (IS_ERR_OR_NULL(*data)) {
            printk(KERN_ERR "Can not allocate buffer for conf file!\n");
            result = -1;
            break;
        }
        filp->f_op->llseek(filp, 0, SEEK_SET);
        ret = vfs_read(filp, *data, size, &filp->f_pos);
        if (ret != size) {
            printk(KERN_ERR "fs_read return ret=%zu, But real size=%lld\n",ret,size);
            kfree(*data);
            result = -1;
            break;
        }
    }while(0);
    set_fs(fs);
    filp_close(filp, NULL);
    return result;
}

static void set_fb_bitfield(MI_FB_ColorFmt_e eColorFmt , struct fb_var_screeninfo *var)
{
    switch (eColorFmt)
    {
        case E_MI_FB_COLOR_FMT_RGB565:
        {
            var->blue.offset = 0;
            var->blue.length = 5;
            var->green.offset = 5;
            var->green.length = 6;
            var->red.offset = 11;
            var->red.length = 5;
            var->transp.offset = 0;
            var->transp.length = 0;
        }
        break;
        case E_MI_FB_COLOR_FMT_ARGB4444:
        {
            var->blue.offset = 0;
            var->blue.length = 4;
            var->green.offset = 4;
            var->green.length= 4;
            var->red.offset = 8;
            var->red.length = 4;
            var->transp.offset = 12;
            var->transp.length = 4;
        }
        break;
        case E_MI_FB_COLOR_FMT_ARGB8888:
        {
            var->red.offset = 16;
            var->red.length = 8;
            var->green.offset = 8;
            var->green.length = 8;
            var->blue.offset = 0;
            var->blue.length = 8;
            var->transp.offset = 24;
            var->transp.length = 8;
        }
        break;
        case E_MI_FB_COLOR_FMT_ARGB1555:
        {
            var->blue.offset = 0;
            var->blue.length = 5;
            var->green.offset = 5;
            var->green.length = 5;
            var->red.offset = 10;
            var->red.length = 5;
            var->transp.offset = 15;
            var->transp.length = 1;
        }
        break;
        case E_MI_FB_COLOR_FMT_YUV422:
        default:
        {
            printk("\33[0;36m  set_fb_bitfield colorFmt=%d \33[m \n",eColorFmt);
        }
        break;
    }
}
static void parse_hwcursorInfo(IniSectionNode *fbCursorSection)
{
    const char* gopId = NULL;
    const char* gwinId = NULL;
    const char* gopDst = NULL;
    const char* gwinFmt = NULL;
    const char* outputColorSpace = NULL;
    u8 u8GopDst,u8GwinFmt,u8OutputColor;

    gopId = get_key_value(fbCursorSection, "FB_HWLAYER_ID");
    kstrtou8(gopId, 10, &(fbHwCursorInfo->u8GopIdx));

    gwinId = get_key_value(fbCursorSection, "FB_HWWIN_ID");
    kstrtou8(gwinId, 10, &(fbHwCursorInfo->u8GwinId));

    gopDst = get_key_value(fbCursorSection, "FB_HWLAYER_DST");
    kstrtou8(gopDst,10, &u8GopDst);
    fbHwCursorInfo->eGopDst = (MI_FB_DstDisplayplane_e)u8GopDst;

    gwinFmt = get_key_value(fbCursorSection, "FB_HWWIN_FORMAT");
    kstrtou8(gwinFmt,10, &u8GwinFmt);
    fbHwCursorInfo->stGwinInfo.clrType = (EN_DRV_GOPColorType)u8GwinFmt;

    outputColorSpace = get_key_value(fbCursorSection, "FB_HWLAYER_OUTPUTCOLOR");
    kstrtou8(outputColorSpace,10, &u8OutputColor);
    fbHwCursorInfo->eOutputColorSpace = (MI_FB_OutputColorSpace_e)u8OutputColor;

}

static void parse_hwLayerInfo(IniSectionNode *hwLayerInfo, int seq)
{
    const char* gopId = NULL;
    const char* gwinId = NULL;
    const char* gopDst = NULL;
    const char* gwinFmt = NULL;
    const char* outputColorSpace = NULL;
    const char* fbWidth = NULL;
    const char* fbHeight = NULL;
    const char* timingWidth = NULL;
    const char* timingHeight = NULL;
    u8 u8GopDst,u8GwinFmt,u8OutputColor;
    u16 u16FbWidth,u16FbHeight;
    u16 u16TimingWidth,u16TimingHeight;
    MS_U16 bpx = 4;
    const char* strId = "MStar FB";

    gopId = get_key_value(hwLayerInfo, "FB_HWLAYER_ID");
    kstrtou8(gopId, 10, &(fbHwlayerInfos[seq].u8GopIdx));

    gwinId = get_key_value(hwLayerInfo, "FB_HWWIN_ID");
    kstrtou8(gwinId, 10, &(fbHwlayerInfos[seq].u8GwinId));

    gopDst = get_key_value(hwLayerInfo, "FB_HWLAYER_DST");
    kstrtou8(gopDst, 10, &u8GopDst);
    fbHwlayerInfos[seq].eGopDst = (MI_FB_DstDisplayplane_e)u8GopDst;

    gwinFmt = get_key_value(hwLayerInfo, "FB_HWWIN_FORMAT");
    kstrtou8(gwinFmt, 10, &u8GwinFmt);
    fbHwlayerInfos[seq].stGwinInfo.clrType = (EN_DRV_GOPColorType)u8GwinFmt;

    outputColorSpace = get_key_value(hwLayerInfo, "FB_HWLAYER_OUTPUTCOLOR");
    kstrtou8(outputColorSpace, 10, &u8OutputColor);
    fbHwlayerInfos[seq].eOutputColorSpace = (MI_FB_OutputColorSpace_e)u8OutputColor;

    fbWidth = get_key_value(hwLayerInfo, "FB_WIDTH");
    kstrtou16(fbWidth, 10, &u16FbWidth);
    fbHwlayerInfos[seq].stGwinInfo.u16HStart = 0;
    fbHwlayerInfos[seq].stGwinInfo.u16HEnd = ALIGN_UP(u16FbWidth, 16);

    fbHeight = get_key_value(hwLayerInfo, "FB_HEIGHT");
    kstrtou16(fbHeight, 10, &u16FbHeight);
    fbHwlayerInfos[seq].stGwinInfo.u16VEnd= u16FbHeight;
    fbHwlayerInfos[seq].stGwinInfo.u16Pitch = ALIGN_UP(u16FbWidth, 16);

    //Stretch Window Info
    fbHwlayerInfos[seq].stStretchWinInfo.u16Xpos = 0;
    fbHwlayerInfos[seq].stStretchWinInfo.u16Ypos = 0;
    fbHwlayerInfos[seq].stStretchWinInfo.u16SrcWidth = ALIGN_UP(u16FbWidth, 16);
    fbHwlayerInfos[seq].stStretchWinInfo.u16SrcHeight= u16FbHeight;
    timingWidth = get_key_value(hwLayerInfo, "FB_TIMMING_WIDTH");
    kstrtou16(timingWidth, 10, &u16TimingWidth);
    fbHwlayerInfos[seq].stStretchWinInfo.u16DstWidth = u16TimingWidth;
    timingHeight = get_key_value(hwLayerInfo, "FB_TIMMING_HEIGHT");
    kstrtou16(timingHeight, 10, &u16TimingHeight);
    fbHwlayerInfos[seq].stStretchWinInfo.u16DstHeight = u16TimingHeight;
    //TODO :Hard code for temp,only for test!!
    fbHwlayerInfos[seq].phyAddr = _fb_phy_addr;
    fbHwlayerInfos[seq].eMiuSel = E_DRV_GOP_SEL_MIU0;
    fbHwlayerInfos[seq].bShown = FALSE;
    bpx =  _MDrv_GRAPHIC_GetBpp((MI_FB_ColorFmt_e)u8GwinFmt);
    //Disable colorkey default
    fbHwlayerInfos[seq].stColorKey.bKeyEnable = FALSE;
    //Enalbe alpha blend default
    fbHwlayerInfos[seq].stAlpha.bAlphaEnable = TRUE;
    fbHwlayerInfos[seq].stAlpha.bAlphaChannel = FALSE;
    //init mstar_fb_fix_infos
    memcpy(mstar_fb_fix_infos[seq].id,strId,strlen(strId));
    mstar_fb_fix_infos[seq].type = FB_TYPE_PACKED_PIXELS;
    mstar_fb_fix_infos[seq].visual = FB_VISUAL_TRUECOLOR;
    mstar_fb_fix_infos[seq].xpanstep = (GOP_WordUnit/bpx);
    mstar_fb_fix_infos[seq].ypanstep = 1;
    mstar_fb_fix_infos[seq].ywrapstep = 1;
    mstar_fb_fix_infos[seq].line_length =
            fbHwlayerInfos[seq].stGwinInfo.u16Pitch * bpx;
    /**
      *mmio_start and mmio_len was used in fb_mmap
      *If fb_mmap has been implement by vendor,it'll
      *invoke vender implement.the mstar_fb_mmap is
      *the implementation of MStar, it's not necessary to
      *init mmio_start and mmio_len. So hardcode it.
      */
    mstar_fb_fix_infos[seq].mmio_start = 0x08000000;
    mstar_fb_fix_infos[seq].mmio_len = 0x10000000;
    mstar_fb_fix_infos[seq].accel = FB_ACCEL_NONE;
    //TODO: hardcode temp. double buffer. (1920x1080x4x2)
    mstar_fb_fix_infos[seq].smem_start = fbHwlayerInfos[seq].phyAddr;
    mstar_fb_fix_infos[seq].smem_len = 0x7E9000;
    //init mstar_fb_var_infos
    mstar_fb_var_infos[seq].xres = ALIGN_UP(u16FbWidth, 16);
    mstar_fb_var_infos[seq].yres = u16FbHeight;
    mstar_fb_var_infos[seq].xres_virtual = mstar_fb_var_infos[seq].xres;
    //double buffer
    mstar_fb_var_infos[seq].yres_virtual = u16FbHeight << 1;
    mstar_fb_var_infos[seq].bits_per_pixel = bpx << 3;
    mstar_fb_var_infos[seq].activate = FB_ACTIVATE_NOW;
    set_fb_bitfield((MI_FB_ColorFmt_e)u8GwinFmt, &mstar_fb_var_infos[seq]);
    //set width, heigth as maxium
    mstar_fb_var_infos[seq].width = -1;
    mstar_fb_var_infos[seq].height = -1;
    mstar_fb_var_infos[seq].grayscale = 0;
    /*timing useless? use the vfb default */
    mstar_fb_var_infos[seq].pixclock
        = 100000000000LLU / (6 *  1920 * 1080);
    mstar_fb_var_infos[seq].left_margin = 64;
    mstar_fb_var_infos[seq].right_margin = 64;
    mstar_fb_var_infos[seq].upper_margin = 32;
    mstar_fb_var_infos[seq].lower_margin = 32;
    mstar_fb_var_infos[seq].hsync_len  =64;
    mstar_fb_var_infos[seq].vsync_len = 2;
    mstar_fb_var_infos[seq].vmode  = FB_VMODE_NONINTERLACED;
}
//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------

static int __init mstar_fb_probe(struct platform_device *dev)
{
    struct fb_info *pinfo = NULL;
    struct fb_info *pFirstFbInfo = NULL;
    struct fb_info *pCurrentFbInfo = NULL;
    int retval = 0;
    IniSectionNode* root = NULL;
    IniSectionNode* fbCursorSection = NULL;
    IniSectionNode* fbDevSection = NULL;
    char* contents = NULL;
    size_t sectionNum = 0;
    int i = 0;
    int j = 0;
    int ret = 0;
    if (NULL == dev)
    {
        printk("ERROR: in mstar_fb_prob: dev is NULL pointer \r\n");
        return -ENOTTY;
    }
    ret = readConfile(&contents);
    if (ret < 0) {
        printk(KERN_ERR "ERROR: in mstar_fb_prob: read fbdev.ini failed\n");
        return -1;
    }
    if (alloc_and_init_ini_tree(&root, contents)) {
        retval= -1;
        goto out;
    }
    dump_ini(root);
    fbCursorSection = get_section(root, "FB_CURSOR");
    if (fbCursorSection) {
        fbHwCursorInfo = (HwCursorInfo_t*)kzalloc(sizeof(HwCursorInfo_t), GFP_KERNEL);
        if (IS_ERR_OR_NULL(fbHwCursorInfo)) {
            printk(KERN_ERR "allocate memroy for HwCursorInfo_t failed!\n");
            retval = -1;
            goto out;
        }
        parse_hwcursorInfo(fbCursorSection);
    }
    sectionNum = get_section_num(root);
    numFbHwlayer = sectionNum;
    if (fbCursorSection) {
        numFbHwlayer = numFbHwlayer-1;
    }
    printk("\33[0;36m  Mstar frame buffer device,numFbHwlayer:%d,sectionNum:%d \33[m \n",numFbHwlayer,sectionNum);
    if (numFbHwlayer > 0) {
        fbHwlayerInfos = (HwLayerInfo_t*)kzalloc(numFbHwlayer* sizeof(HwLayerInfo_t),GFP_KERNEL);
        if (IS_ERR_OR_NULL(fbHwlayerInfos)) {
            printk(KERN_ERR "allocate memory for HwLayerInfo_t failed!\n");
            retval = -1;
            goto out;
        }
        mstar_fb_fix_infos = (struct fb_fix_screeninfo*)kzalloc(
            numFbHwlayer * sizeof(struct fb_fix_screeninfo), GFP_KERNEL);
        if (IS_ERR_OR_NULL(mstar_fb_fix_infos)) {
            printk(KERN_ERR "allocate memory for mstar_fb_fix_infos failed!\n");
            retval = -1;
            goto out;
        }
        mstar_fb_var_infos = (struct fb_var_screeninfo*)kzalloc(
            numFbHwlayer*sizeof(struct fb_var_screeninfo), GFP_KERNEL);
        if (IS_ERR_OR_NULL(mstar_fb_var_infos)) {
            printk(KERN_ERR "allocate memory for mstar_fb_var_infos failed!\n");
            retval = -1;
            goto out;
        }
        fbDevSection = get_section(root, "FB_DEVICE");
        for (i = 0; i < numFbHwlayer; i++) {
            parse_hwLayerInfo(fbDevSection, i);
            fbDevSection = fbDevSection->next;
        }
        //resister framebufferInfo and init GOP && GWIN
        for (i = 0; i <  numFbHwlayer; i++) {
            pinfo = framebuffer_alloc(0, &dev->dev);
            if (!pinfo) {
                printk(KERN_ERR "framebuffer_alloc:%d failed!\n", i);
                retval = -1;
                break;
            }
            if (0 == i) {
                pFirstFbInfo = pinfo;
            }
            pinfo->fbops = &mstar_fb_ops;
            pinfo->var = mstar_fb_var_infos[i];
            pinfo->fix = mstar_fb_fix_infos[i];
            pinfo->pseudo_palette = NULL;
            pinfo->par = NULL;
            pinfo->flags = FBINFO_FLAG_DEFAULT;
        }
        //If framebuffer_alloc fail, release fb_info already allocated
        if (i < numFbHwlayer) {
            for (j = 0; j < i; j++) {
                pCurrentFbInfo = pFirstFbInfo + j;
                framebuffer_release(pCurrentFbInfo);
            }
            pFirstFbInfo = NULL;
            pCurrentFbInfo = NULL;
            goto out;
        }
        //fb_alloc_cmap for all fb_info
        for (i = 0; i < numFbHwlayer; i++) {
            pCurrentFbInfo = pFirstFbInfo + i;
            retval  = fb_alloc_cmap(&pCurrentFbInfo->cmap, 256, 0);
            if (retval < 0) {
                printk(KERN_ERR "fb_alloc_cmap:%d failed!\n", i);
                break;
            }
        }
        if (i < numFbHwlayer) {
            //release cmap and fb_info already been alocated
            for (j = 0; j < i; j++) {
                pCurrentFbInfo = pFirstFbInfo + j;
                fb_dealloc_cmap(&pCurrentFbInfo->cmap);
            }
            for (j=0; j < numFbHwlayer; j++) {
                pCurrentFbInfo = pFirstFbInfo + j;
                framebuffer_release(pCurrentFbInfo);
            }
            pFirstFbInfo = NULL;
            pCurrentFbInfo = NULL;
            goto out;
        }
        //register framebuffer
        for (i = 0; i < numFbHwlayer; i++) {
            pCurrentFbInfo = pFirstFbInfo + i;
            retval = register_framebuffer(pCurrentFbInfo);
            if (retval < 0) {
                printk(KERN_ERR "register_framebuffer:%d failed!\n", i);
                break;
            }
        }
        if ( i < numFbHwlayer) {
            //unregister framebuffer already registerd
            for (j = 0; j < i; j++) {
                pCurrentFbInfo = pFirstFbInfo + j;
                unregister_framebuffer(pCurrentFbInfo);
            }
            //release cmap and fb_info
            for (j = 0; j < numFbHwlayer; j++) {
                pCurrentFbInfo = pFirstFbInfo + j;
                fb_dealloc_cmap(&pCurrentFbInfo->cmap);
                framebuffer_release(pCurrentFbInfo);
            }
            pFirstFbInfo = NULL;
            pCurrentFbInfo = NULL;
            goto out;
        }
        //save pFirstFbInfo as private data
        platform_set_drvdata(dev, pFirstFbInfo);
        first_fb_node = pFirstFbInfo->node;
        printk("\33[0;36m  fb%d: Mstar frame buffer device \33[m \n",pFirstFbInfo->node);
    }
    out:
        if (retval < 0) {
            if (fbHwCursorInfo) {
                kfree(fbHwCursorInfo);
                fbHwCursorInfo = NULL;
            }
            if (fbHwlayerInfos) {
                kfree(fbHwlayerInfos);
                fbHwlayerInfos = NULL;
            }
        }
        if (mstar_fb_fix_infos) {
                kfree(mstar_fb_fix_infos);
                mstar_fb_fix_infos = NULL;
            }
        if (mstar_fb_var_infos) {
            kfree(mstar_fb_var_infos);
            mstar_fb_var_infos = NULL;
        }
        release_ini_tree(root);
        kfree(contents);
    return retval;
}

static int mstar_fb_remove(struct platform_device *dev)
{
    struct fb_info *pinfo;
    int i=0;
    pinfo = 0;
    if (NULL == dev)
    {
        printk("ERROR: mstar_fb_remove: dev is NULL pointer \n");
        return -ENOTTY;
    }
    for (i = 0;  i < numFbHwlayer; i++) {
        _fb_gwin_enable(fbHwlayerInfos[i].u8GopIdx, fbHwlayerInfos[i].u8GwinId, FALSE);
        fbHwlayerInfos[i].bShown  =FALSE;
    }
    pinfo = platform_get_drvdata(dev);
    for (i = 0;  i < numFbHwlayer; i++) {
        pinfo = pinfo + i;
        unregister_framebuffer(pinfo);
        framebuffer_release(pinfo);
    }
    if (fbHwCursorInfo) {
        kfree(fbHwCursorInfo);
        fbHwCursorInfo = NULL;
    }
    if (fbHwlayerInfos) {
        kfree(fbHwlayerInfos);
       fbHwlayerInfos = NULL;
    }
    if (mstar_fb_fix_infos) {
        kfree(mstar_fb_fix_infos);
        mstar_fb_fix_infos = NULL;
    }
    if (mstar_fb_var_infos) {
        kfree(mstar_fb_var_infos);
        mstar_fb_var_infos = NULL;
    }
    return 0;
}

static void mstar_fb_platform_release(struct device *device)
{
    if (NULL == device)
    {
        printk("ERROR: in mstar_fb_platform_release, \
                device is NULL pointer !\r\n");
    }
    else
    {
        printk("in mstar_fb_platform_release, module unload!\n");
    }
}

/*device .name and driver .name must be the same, then it will call
       probe function */
static struct platform_driver Mstar_fb_driver =
{
    .probe  = mstar_fb_probe,    //initiailize
    .remove = mstar_fb_remove,   /*it free(mem),
                                   release framebuffer, free irq etc. */
    .driver =
    {
        .name = "Mstar-fb",
    },
};

static u64 mstar_fb_device_lcd_dmamask = 0xffffffffUL;

static struct platform_device Mstar_fb_device =
{
    .name = "Mstar-fb",
    .id = 0,
    .dev =
    {
        .release = mstar_fb_platform_release,
        .dma_mask = &mstar_fb_device_lcd_dmamask,
        .coherent_dma_mask = 0xffffffffUL
    }
};

static int __init mstar_fb_init(void)
{
    int ret = 0;

    ret = platform_driver_register(&Mstar_fb_driver);

    if (!ret)
    {
        /*register driver sucess
          register device*/
        ret = platform_device_register(&Mstar_fb_device);
        if(ret)    /*if register device fail, then unregister the driver.*/
        {
            platform_driver_unregister(&Mstar_fb_driver);
        }
    }
    return ret;
}

static void __exit mstar_fb_exit(void)
{
    platform_device_unregister(&Mstar_fb_device);
    platform_driver_unregister(&Mstar_fb_driver);
}

static int mstar_fb_open(struct fb_info *info, int user)
{
    int fbIdx = info->node - first_fb_node;
    if (!fbHwlayerInfos[fbIdx].ref_count) {
        if (0 != info->fix.smem_start) {
            printk("\33[0;36m mstar_fb_open fb:%d info->fix.smem_start = 0x%lx \33[m \n",info->node,info->fix.smem_start);
            _fb_buf_init(info, info->fix.smem_start, fbIdx);
            _fb_gwin_init(fbIdx);
            _fb_strewin_update(&info->var,fbHwlayerInfos[fbIdx].u8GopIdx, 0, 0,
                fbHwlayerInfos[fbIdx].stStretchWinInfo.u16DstWidth,fbHwlayerInfos[fbIdx].stStretchWinInfo.u16DstHeight);
             _fb_gwin_update(&info->var, info);
       }
   }
   fbHwlayerInfos[fbIdx].ref_count++;
   return 0;
}
static int mstar_fb_release(struct fb_info *info, int user)
{
    int fbIdx = info->node - first_fb_node;
    if (!fbHwlayerInfos[fbIdx].ref_count)
		return -EINVAL;
    if (fbHwlayerInfos[fbIdx].ref_count ==1) {
        if (info->screen_base != NULL) {
            iounmap(info->screen_base);
            info->screen_base = NULL;
        }
    }
    fbHwlayerInfos[fbIdx].ref_count--;
    return 0;
}

static int _mstar_fb_mmap(struct fb_info *pinfo, struct vm_area_struct *vma)
{
      size_t size;
      int fbIdx = pinfo->node - first_fb_node;
      E_DRV_GOP_SEL_TYPE emiuSel =  fbHwlayerInfos[fbIdx].eMiuSel;
      size = 0;
      if (NULL == pinfo)
      {
         printk("ERROR: mstar_fb_mmap, pinfo is NULL pointer !\n");
         return -ENOTTY;
      }
      if (NULL == vma)
      {
          printk("ERROR: mstar_fb_mmap, vma is NULL pointer !\n");
          return -ENOTTY;
      }
      if (0 == pinfo->fix.smem_start)
      {
          printk("ERROR: mstar_fb_mmap, physical addr is NULL pointer !\n");
          return -ENOTTY;
      }

      size = vma->vm_end - vma->vm_start;
      if (emiuSel == E_DRV_GOP_SEL_MIU0) {
            vma->vm_pgoff = (pinfo->fix.smem_start + MIU0_BUS_OFFSET) >> PAGE_SHIFT;
      } else if (emiuSel == E_DRV_GOP_SEL_MIU1) {
            vma->vm_pgoff = (pinfo->fix.smem_start + MIU1_BUS_OFFSET) >> PAGE_SHIFT;
      }
      printk(
      "\33[0;36m 2 mstar_fb_mmap vma->vm_start=%x\n vma->vm_end=%x\n vma->vm_pgoff =%x \33[m \n",
      (unsigned int) vma->vm_start, (unsigned int)vma->vm_end ,
      (unsigned int)vma->vm_pgoff);

       vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
      // Remap-pfn-range will mark the range VM_IO and VM_RESERVED

      if (remap_pfn_range(vma, vma->vm_start,
          vma->vm_pgoff, size, vma->vm_page_prot))
      return -EAGAIN;

      return 0;
}

static MS_U32 convert1555To8888(MS_U8 u8Red, MS_U8 u8Green, MS_U8 u8Blue)
{
    MS_U32 fullclr = 0;
    MS_U16 clr = (0x1 << 15) | ((u8Red & 0x1f) << 10)
        | ((u8Green&0x1f) << 5) | (u8Blue & 0x1f);
    //amplify
    fullclr = ((clr>>10)&0x1f)<<3; //R Component
    fullclr |= ((clr>>12)&0x7);
    fullclr = fullclr << 8;
    fullclr |= ((clr>>5)&0x1f)<<3;      // G Component
    fullclr |= ((clr>>7)&0x7);
    fullclr = fullclr << 8;
    fullclr |= (clr&0x1f)<<3;           // B
    fullclr |= ((clr>>2)&0x7);
    return fullclr;
}
static MS_U32 convert565To8888(MS_U8 u8Red, MS_U8 u8Green, MS_U8 u8Blue)
{
    MS_U32 fullclr = 0;
    MS_U16 clr = ((u8Red & 0x1f) << 11) | ((u8Green & 0x3f)<<5)
        | (u8Blue & 0x1f);
    //amplify to ARGB8888
    fullclr = ((clr >> 11)&0x1f) << 3; //R
    fullclr |= ((clr >> 13)&0x7);
    fullclr = fullclr << 8;
    fullclr |= ((clr >> 5)&0x3f) << 2; //G
    fullclr |= ((clr >> 9) &0x3);
    fullclr = fullclr << 8;
    fullclr |= (clr&0x1f) << 3; //B
    fullclr |= ((clr>>2)&0x7);
    return fullclr;
}
static MS_U32 convert4444To8888(MS_U8 u8Red, MS_U8 u8Green, MS_U8 u8Blue)
{
    MS_U32 fullclr = 0;
    MS_U16 clr = ((u8Red & 0xf) << 8) | ((u8Green & 0xf) << 4)
        |((u8Blue & 0xf) << 4);
    //amplify to ARGB8888
    fullclr = ((clr >> 8) & 0xf) << 4; //R
    fullclr |= ((clr >> 8) & 0xf);
    fullclr = fullclr << 8;
    fullclr |= ((clr >> 4) & 0xf) << 4; //G
    fullclr |= ((clr >> 4) & 0xf);
    fullclr = fullclr << 8;
    fullclr |= (clr & 0xf) << 4; //B
    fullclr |= (clr & 0xf);
    return fullclr;
}
static void convertClrKeyByFmt(MI_FB_ColorKey_t* clrKeyInfo,
    EN_DRV_GOPColorType clrFmt)
{
    MS_U8 red = clrKeyInfo->u8Red;
    MS_U8 green = clrKeyInfo->u8Green;
    MS_U8 blue = clrKeyInfo->u8Blue;
    switch (clrFmt)
    {
        case E_MI_FB_COLOR_FMT_RGB565:
        {
            clrKeyInfo->u8Red = (red >> 3)&(0x1f);
            clrKeyInfo->u8Green = (green >> 2)&(0x3f);
            clrKeyInfo->u8Blue = (blue >> 3)&(0x1f);
        }
        break;
        case E_MI_FB_COLOR_FMT_ARGB4444:
        {
            clrKeyInfo->u8Red = (red >> 4)&0xf;
            clrKeyInfo->u8Green = (green >> 4)&0xf;
            clrKeyInfo->u8Blue = (blue>>4)&0xf;
       }
       break;
       case E_MI_FB_COLOR_FMT_ARGB1555:
       {
            clrKeyInfo->u8Red = (red>>3) & 0x1f;
            clrKeyInfo->u8Green= (green >>3) & 0x1f;
            clrKeyInfo->u8Blue = (blue >>3) &0x1f;
       }
       break;
       default:
       {
            if (clrFmt != E_DRV_GOP_COLOR_ARGB8888) {
                printk("\33[0;31m convertColorKeyByFmt invalid colorfmt is %d\33[m\n",clrFmt);
            }
       }
       break;
    }
}
static int mstar_fb_ioctl(struct fb_info *pinfo, MS_U32 u32Cmd, unsigned long u32Arg)
{
    int retval = 0;
    unsigned int dir;
    int fbIdx = pinfo->node - first_fb_node;
    //TODO get current timing via utopia function
    MS_U16 curTimingWidth = 1920;
    MS_U16 curTimingHeight = 1080;
    MS_U8 gopId = fbHwlayerInfos[fbIdx].u8GopIdx;
    MS_U8 gwinId = fbHwlayerInfos[fbIdx].u8GwinId;
    EN_DRV_GOPColorType currentFmt = fbHwlayerInfos[fbIdx].stGwinInfo.clrType;
    MS_U16 stretchWinXpos = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16Xpos;
    MS_U16 stretchWinYpos = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16Ypos;
    MS_U16 stretchWinSrcWidth = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16SrcWidth;
    MS_U16 stretchWinSrcHeight = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16SrcHeight;
    MS_U16 stretchWinDstWidth = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16DstWidth;
    MS_U16 stretchWinDstHeight = fbHwlayerInfos[fbIdx].stStretchWinInfo.u16DstHeight;

    union
    {
        MI_FB_Rectangle_t dispRegion;
        MI_FB_GlobalAlpha_t alpahInfo;
        MI_FB_ColorKey_t colorKeyInfo;
        MI_FB_DisplayLayerAttr_t dispLayerAttr;
        MI_FB_CursorAttr_t hwcursorAttr;
        MS_BOOL bShown;
    }data;
    if (_IOC_TYPE(u32Cmd) != FB_IOC_MAGIC) {
        return -ENOTTY;
    }
    if (_IOC_SIZE(u32Cmd) > sizeof(data)) {
        return  -EINVAL;
    }
    dir = _IOC_DIR(u32Cmd);
    if (dir & _IOC_WRITE) {
        if (copy_from_user(&data, (void __user*)u32Arg,_IOC_SIZE(u32Cmd)))
            return -EFAULT;
    }
    switch (u32Cmd) {
        case FBIOGET_SCREEN_LOCATION:
        {
            data.dispRegion.u16Xpos = stretchWinXpos;
            data.dispRegion.u16Ypos = stretchWinYpos;
            data.dispRegion.u16Width = stretchWinDstWidth;
            data.dispRegion.u16Height= stretchWinDstHeight;
            retval  = copy_to_user((MI_FB_Rectangle_t __user*)u32Arg,
                &data.dispRegion, sizeof(MI_FB_Rectangle_t));
        }
        break;
        case FBIOSET_SCREEN_LOCATION:
        {
            MS_BOOL xPosEqual =
                (data.dispRegion.u16Xpos == stretchWinXpos);
            MS_BOOL yPosEqual =
                (data.dispRegion.u16Ypos == stretchWinYpos);
            MS_BOOL widthEqual =
                (data.dispRegion.u16Width == stretchWinDstWidth);
            MS_BOOL heightEqual =
                (data.dispRegion.u16Height== stretchWinDstHeight);
            if (xPosEqual && yPosEqual
                && widthEqual && heightEqual) {
                return retval;
            }
            if ((data.dispRegion.u16Width < stretchWinSrcWidth)
                 || (data.dispRegion.u16Height < stretchWinSrcHeight)) {
                printk("\33[0;31m  fb%d FBIOSET_SCREEN_LOCATION parameter is invalid srcSize[%d,%d] is larger than destSize[%d,%d]\33[m \n",
                    pinfo->node,stretchWinSrcWidth,stretchWinSrcHeight, data.dispRegion.u16Width, data.dispRegion.u16Height);
                return -EINVAL;
            }
            //TODO: get current timming via utopia function
            if (data.dispRegion.u16Xpos + data.dispRegion.u16Width > curTimingWidth) {
                printk(
                "\33[0;31m fb%d FBIOSET_SCREEN_LOCATION param is invalid the sum of xpos and width: %d should not  larger than current timming width: %d \33[m \n",
                    pinfo->node,data.dispRegion.u16Xpos + data.dispRegion.u16Width,curTimingWidth);
                return -EINVAL;
            }
            if (data.dispRegion.u16Ypos + data.dispRegion.u16Height > curTimingHeight) {
                printk(
                "\33[0;31m fb%d FBIOSET_SCREEN_LOCATION param is invalid the sum of ypos and height: %d should not  larger than current timming height: %d \33[m \n",
                    pinfo->node,data.dispRegion.u16Ypos + data.dispRegion.u16Height,curTimingHeight);
                return -EINVAL;
            }
            _fb_strewin_update(&pinfo->var,gopId,
                data.dispRegion.u16Xpos,data.dispRegion.u16Ypos,data.dispRegion.u16Width,data.dispRegion.u16Height);
             _MDrv_GRAPHIC_UpdateReg(gopId);
             //Store stretchwindow position
             fbHwlayerInfos[fbIdx].stStretchWinInfo.u16Xpos = data.dispRegion.u16Xpos;
             fbHwlayerInfos[fbIdx].stStretchWinInfo.u16Ypos = data.dispRegion.u16Ypos;
             fbHwlayerInfos[fbIdx].stStretchWinInfo.u16DstWidth = data.dispRegion.u16Width;
             fbHwlayerInfos[fbIdx].stStretchWinInfo.u16DstHeight  = data.dispRegion.u16Height;
        }
        break;
        case FBIOGET_SHOW:
        {
            retval =
                __put_user(fbHwlayerInfos[fbIdx].bShown, (MS_BOOL __user*)u32Arg);
        }
        break;
        case FBIOSET_SHOW:
        {
            MS_BOOL bNeedShow = data.bShown;
            if (bNeedShow != fbHwlayerInfos[fbIdx].bShown) {
                _fb_gwin_enable(gopId,gwinId,bNeedShow);
                fbHwlayerInfos[fbIdx].bShown = bNeedShow;
            }
        }
        break;
        case FBIOGET_GLOBAL_ALPHA:
        {
            retval  = copy_to_user((MI_FB_GlobalAlpha_t __user*)u32Arg,
                &fbHwlayerInfos[fbIdx].stAlpha, sizeof(MI_FB_GlobalAlpha_t));
        }
        break;
        case FBIOSET_GLOBAL_ALPHA:
        {
            if (!memcpy(&fbHwlayerInfos[fbIdx].stAlpha,
                &data.alpahInfo, sizeof(MI_FB_GlobalAlpha_t))){
                return retval;
            }
            if (!data.alpahInfo.bAlphaEnable) {
                _MDrv_GRAPHIC_SetBlending(gopId, gwinId, FALSE, 0xFF);
            } else {
                if (currentFmt == E_DRV_GOP_COLOR_ARGB1555) {
                    _MDrv_Graphic_SetAlpha0(gopId, data.alpahInfo.u8Alpha0);
                    _MDrv_Graphic_SetAlpha1(gopId, data.alpahInfo.u8Alpha1);
                }
                if (data.alpahInfo.bAlphaChannel) {
                     _MDrv_GRAPHIC_SetBlending(gopId, gwinId,
                        TRUE, data.alpahInfo.u8GlobalAlpha);
                    //do not support multialpha
                    if (_MDrv_Graphic_IsSupportMultiAlpha(gopId)){
                        _MDrv_Graphic_EnableMultiAlpha(gopId, TRUE);
                    }
                }else {
                    if (fbHwlayerInfos[fbIdx].stAlpha.bAlphaChannel) {
                         if (_MDrv_Graphic_IsSupportMultiAlpha(gopId)){
                            _MDrv_Graphic_EnableMultiAlpha(gopId, FALSE);
                        }
                    }
                    _MDrv_GRAPHIC_SetBlending(gopId, gwinId, TRUE, 0xFF);
                }
            }
            _MDrv_GRAPHIC_UpdateReg(gopId);
            memcpy(&fbHwlayerInfos[fbIdx].stAlpha, &data.alpahInfo, sizeof(MI_FB_GlobalAlpha_t));
        }
        break;
        case FBIOGET_COLORKEY:
        {
            retval = copy_to_user((MI_FB_ColorKey_t __user*)u32Arg,
                &fbHwlayerInfos[fbIdx].stColorKey, sizeof(MI_FB_ColorKey_t));
        }
        break;
        case FBIOSET_COLORKEY:
        {
            MS_U32 fullclr = 0;
            MI_FB_ColorKey_t reqClrKeyInfo;
            if (!memcmp(&fbHwlayerInfos[fbIdx].stColorKey,
                &data.colorKeyInfo, sizeof(MI_FB_ColorKey_t)) ){
                return retval;
            }
            if ((currentFmt == E_DRV_GOP_COLOR_YUV422) ||
                 (currentFmt == E_DRV_GOP_COLOR_INVALID) ) {
                printk("\33[0;31m fb%d can not set colorkey prop for clrFmt:%d \33[m \n",pinfo->node,currentFmt);
                return  -EINVAL;
            }
            memcpy(&reqClrKeyInfo, &data.colorKeyInfo, sizeof(MI_FB_ColorKey_t));
            if (!data.colorKeyInfo.bKeyEnable) {
                _MDrv_GRAPHIC_EnableTransClr_EX(gopId,GOPTRANSCLR_FMT0, FALSE);
            } else {
                //convert request colorkey from RGB888 to current fmt format
                convertClrKeyByFmt(&reqClrKeyInfo, currentFmt);
                _MDrv_GRAPHIC_EnableTransClr_EX(gopId, GOPTRANSCLR_FMT0, TRUE);
                switch (currentFmt) {
                    case E_DRV_GOP_COLOR_RGB565:
                    {
                        fullclr = convert565To8888(reqClrKeyInfo.u8Red,
                            reqClrKeyInfo.u8Green, reqClrKeyInfo.u8Blue);
                    }
                    break;
                    case E_DRV_GOP_COLOR_ARGB4444:
                    {
                        fullclr = convert4444To8888(reqClrKeyInfo.u8Red,
                            reqClrKeyInfo.u8Green, reqClrKeyInfo.u8Blue);
                    }
                    break;
                    case E_DRV_GOP_COLOR_ARGB1555:
                    {
                        fullclr = convert1555To8888(reqClrKeyInfo.u8Red,
                            reqClrKeyInfo.u8Green, reqClrKeyInfo.u8Blue);
                    }
                    break;
                    case E_DRV_GOP_COLOR_ARGB8888:
                    {
                        fullclr = (0xff<<24) | (reqClrKeyInfo.u8Red<<16)
                            | (reqClrKeyInfo.u8Green<<8) | (reqClrKeyInfo.u8Blue);
                    }
                    break;
                    default:
                        return  -EINVAL;
                    }
                    _MDrv_Graphic_SetTransClr_8888(gopId,fullclr,0);
            }
            _MDrv_GRAPHIC_UpdateReg(gopId);
            memcpy(&fbHwlayerInfos[fbIdx].stColorKey, &data.colorKeyInfo, sizeof(MI_FB_ColorKey_t));
        }
        break;
        case FBIOGET_DISPLAYLAYER_ATTRIBUTES:
        {
            data.dispLayerAttr.u32Xpos = stretchWinXpos;
            data.dispLayerAttr.u32YPos = stretchWinYpos;
            data.dispLayerAttr.u32dstWidth = stretchWinDstWidth;
            data.dispLayerAttr.u32dstHeight = stretchWinDstHeight;
            data.dispLayerAttr.u32DisplayWidth = stretchWinSrcWidth;
            data.dispLayerAttr.u32DisplayHeight = stretchWinSrcHeight;
            data.dispLayerAttr.u32ScreenWidth = curTimingWidth;
            data.dispLayerAttr.u32ScreenHeight = curTimingHeight;
            data.dispLayerAttr.eFbColorFmt = (MI_FB_ColorFmt_e)currentFmt;
            data.dispLayerAttr.eFbOutputColorSpace = fbHwlayerInfos[fbIdx].eOutputColorSpace;
            data.dispLayerAttr.eFbDestDisplayPlane = fbHwlayerInfos[fbIdx].eGopDst;
            _MDrv_Graphic_GetGwinNewAlphaModeEnable(gopId, gwinId, &data.dispLayerAttr.bPreMul);
            data.dispLayerAttr.u32SetAttrMask = 0x0;
            retval = copy_to_user((MI_FB_DisplayLayerAttr_t __user*)u32Arg,
                &data.dispLayerAttr, sizeof(MI_FB_DisplayLayerAttr_t));
        }
        break;
        case FBIOSET_DISPLAYLAYER_ATTRIBUTES:
        {
            MS_U16 dispXpos = stretchWinXpos;
            MS_U16 dispYpos = stretchWinYpos;
            MS_U16 dispWidth = stretchWinDstWidth;
            MS_U16 dispHeight = stretchWinDstHeight;
            MS_U16 fbWidth = pinfo->var.xres;
            MS_U16 fbHeight = pinfo->var.yres;
            MS_U16 timingWidth = curTimingWidth;
            MS_U16 timingHeight = curTimingHeight;
            EN_DRV_GOPColorType colorFmt = currentFmt;
            DRV_GWIN_INFO gwinInfo = fbHwlayerInfos[fbIdx].stGwinInfo;
            DRV_STRETCHWIN_INFO stretchWinInfo = fbHwlayerInfos[fbIdx].stStretchWinInfo;
            MS_BOOL bCurPremul  = FALSE;
            MS_U8 dispMask = E_MI_FB_DISPLAYLAYER_ATTR_MASK_DISP_POS |
                              E_MI_FB_DISPLAYLAYER_ATTR_MASK_DISP_SIZE |
                              E_MI_FB_DISPLAYLAYER_ATTR_MASK_BUFFER_SIZE |
                              E_MI_FB_DISPLAYLAYER_ATTR_MASK_COLOR_FMB;
            if (data.dispLayerAttr.u32SetAttrMask &
                E_MI_FB_DISPLAYLAYER_ATTR_MASK_DISP_POS) {
                dispXpos = data.dispLayerAttr.u32Xpos;
                dispYpos = data.dispLayerAttr.u32YPos;
            }
            if (data.dispLayerAttr.u32SetAttrMask &
                E_MI_FB_DISPLAYLAYER_ATTR_MASK_DISP_SIZE) {
                dispWidth = data.dispLayerAttr.u32dstWidth;
                dispHeight = data.dispLayerAttr.u32dstHeight;
            }
            if (data.dispLayerAttr.u32SetAttrMask &
                E_MI_FB_DISPLAYLAYER_ATTR_MASK_BUFFER_SIZE) {
                fbWidth = data.dispLayerAttr.u32DisplayWidth;
                fbHeight = data.dispLayerAttr.u32DisplayHeight;
            }
            if (data.dispLayerAttr.u32SetAttrMask &
                E_MI_FB_DISPLAYLAYER_ATTR_MASK_SCREEN_SIZE) {
                timingWidth = data.dispLayerAttr.u32ScreenWidth;
                timingHeight = data.dispLayerAttr.u32ScreenHeight;
            }
            if (data.dispLayerAttr.u32SetAttrMask &
                E_MI_FB_DISPLAYLAYER_ATTR_MASK_COLOR_FMB) {
                colorFmt = data.dispLayerAttr.eFbColorFmt;
                if (colorFmt == E_DRV_GOP_COLOR_INVALID)
                    return -EINVAL;
            }
            if (data.dispLayerAttr.u32SetAttrMask &
                E_MI_FB_DISPLAYLAYER_ATTR_MASK_DST_DISP) {
                if ((data.dispLayerAttr.eFbDestDisplayPlane != E_MI_FB_DST_OP0) &&
                    (data.dispLayerAttr.eFbDestDisplayPlane != E_MI_FB_DST_OP1))
                    return -EINVAL;
            }
            if (data.dispLayerAttr.u32SetAttrMask & dispMask) {
                if (dispWidth < fbWidth || dispHeight < fbHeight) {
                    return -EINVAL;
                }
                if (((dispXpos + dispWidth) > timingWidth)||
                    ((dispYpos + dispHeight) > timingHeight)) {
                    return -EINVAL;
                }
                if ((fbWidth > pinfo->var.xres_virtual) ||
                    (fbHeight > pinfo->var.yres_virtual)) {
                    return -EINVAL;
                }
                //update GwinInfo
                gwinInfo.u16HEnd = fbWidth;
                gwinInfo.u16VEnd = fbHeight;
                gwinInfo.clrType = colorFmt;
                if (!memcmp(&gwinInfo,&fbHwlayerInfos[fbIdx].stGwinInfo,sizeof(DRV_GWIN_INFO))) {
                    _MDrv_GOP_SetGwinInfo(gopId,gwinId,gwinInfo);
                    //store gwininfo
                    memcpy(&fbHwlayerInfos[fbIdx].stGwinInfo, &gwinInfo, sizeof(DRV_GWIN_INFO));
                }
                //Upate stretchwindow info
                stretchWinInfo.u16Xpos = dispXpos;
                stretchWinInfo.u16Ypos = dispYpos;
                stretchWinInfo.u16SrcWidth = fbWidth;
                stretchWinInfo.u16SrcHeight = fbHeight;
                stretchWinInfo.u16DstWidth = dispWidth;
                stretchWinInfo.u16DstHeight = dispHeight;
                if (!memcmp(&stretchWinInfo,
                    &fbHwlayerInfos[fbIdx].stStretchWinInfo, sizeof(DRV_STRETCHWIN_INFO))) {
                        _MDrv_GRAPHIC_SetStretchWin(gopId, dispXpos, dispYpos, fbWidth, fbHeight);
                        _MDrv_GRAPHIC_SetHScale(gopId, TRUE, fbWidth, dispWidth);
                        _MDrv_GRAPHIC_SetVScale(gopId, TRUE, fbHeight, dispHeight);
                        memcpy(&fbHwlayerInfos[fbIdx].stStretchWinInfo, &stretchWinInfo,
                            sizeof(DRV_STRETCHWIN_INFO));
                }
                //store visual resolution
                pinfo->var.xres = fbWidth;
                pinfo->var.yres = fbHeight;
            }
            if (data.dispLayerAttr.u32SetAttrMask
                & E_MI_FB_DISPLAYLAYER_ATTR_MASK_PREMUL) {
                 _MDrv_Graphic_GetGwinNewAlphaModeEnable(gopId, gwinId, &bCurPremul);
                 if (data.dispLayerAttr.bPreMul != bCurPremul)
                    _MDrv_Graphic_SetNewAlphaMode(gopId, gwinId, data.dispLayerAttr.bPreMul);
            }
            if (data.dispLayerAttr.u32SetAttrMask
                & E_MI_FB_DISPLAYLAYER_ATTR_MASK_OUTPUT_COLORSPACE) {
                if (fbHwlayerInfos[fbIdx].eOutputColorSpace !=
                        data.dispLayerAttr.eFbOutputColorSpace) {
                    _MDrv_GRAPHIC_OutputColor_EX(gopId, data.dispLayerAttr.eFbOutputColorSpace);
                    fbHwlayerInfos[fbIdx].eOutputColorSpace =  data.dispLayerAttr.eFbOutputColorSpace;
                }
            }
            if (data.dispLayerAttr.u32SetAttrMask
                & E_MI_FB_DISPLAYLAYER_ATTR_MASK_DST_DISP) {
                 if (fbHwlayerInfos[fbIdx].eGopDst !=
                        data.dispLayerAttr.eFbDestDisplayPlane) {
                    _MDrv_GRAPHIC_SetGOPDst(gopId, data.dispLayerAttr.eFbDestDisplayPlane, FALSE);
                    //TODO:calculate pipe_delay and set it!
                    fbHwlayerInfos[fbIdx].eGopDst =  data.dispLayerAttr.eFbDestDisplayPlane;
                 }
            }
            if (data.dispLayerAttr.u32SetAttrMask
                & E_MI_FB_DISPLAYLAYER_ATTR_MASK_SCREEN_SIZE) {
                if (timingWidth != curTimingWidth ||
                    timingHeight != curTimingHeight) {
                    //Set hstart by currentTiming and calculate piepdelay
                    
                }
            }
            _MDrv_GRAPHIC_UpdateReg(gopId);
        }
        break;
        case FBIOGET_CURSOR_ATTRIBUTE:
        {
        }
        break;
        case FBIOSET_CURSOR_ATTRIBUTE:
        {
        }
        break;
    }
    return retval;
}

#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
#else
module_init(mstar_fb_init);
module_exit(mstar_fb_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("GRAPHIC ioctrl driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
