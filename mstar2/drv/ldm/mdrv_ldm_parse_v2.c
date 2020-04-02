///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 Mstar Semiconductor, Inc.
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
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_ldm_parse.c
// @brief  LDM  Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#if (defined (CONFIG_HAS_LD_DMA_MODE))
#include <linux/types.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/string.h>
#include "mdrv_mstypes.h"
#include <linux/ctype.h>
#include <linux/slab.h>

#include "mdrv_ldm_io.h"
#include "mdrv_ldm_common.h"
#include "mdrv_ldm_init.h"
#include "mdrv_ldm_interface.h"
#include "mdrv_ldm_parse.h"
#include "mdrv_ldm_dma.h"
#include "mdrv_mspi.h"



//=============================================================================
// Local Defines
//=============================================================================

#define LDM_BUF_SIZE         5500  // number of LDM.ini member length
#define LDM_BIN_CHECK_POSITION 4    //according bin file

#define SIZEARRAY(a)    (sizeof(a) / sizeof((a)[0]))

#define PQ_GET_INT(str, value)\
    n = MDrv_LD_GetInteger(str, value);\
    LD_INFO("%s = %ld \n",str,n);\
    stDrvLdPQInfo##str = n;

#define PQ_GET_BOOL(str, value)\
    n = MDrv_LD_GetBoolean(str, value);\
    LD_INFO("%s = %ld \n",str,n);\
    stDrvLdPQInfo##str = n;

#define assert(e)
#define _DRVLDPARSE_C_

#if !defined LINE_BUFFERSIZE
#define LINE_BUFFERSIZE  256
#endif



//--------------------------------------------------------------------------------------------------
//  functions declaration
//--------------------------------------------------------------------------------------------------
//static int _mod_ldm_open (struct inode *inode, struct file *filp);

//=============================================================================
// Local Variables: Device handler
//=============================================================================


static char * g_BufferAddr = NULL;
static unsigned int g_BufferLen;
static unsigned int g_BufferPos;
static unsigned int g_BeginPos;

extern ST_DRV_LD_DMA_INFO  stDrvLdMDAInfo;
extern MSPI_config  stDrvLdMspiInfo;


void MDrv_LD_InitBuff(char *BufferAddr,unsigned int BufLen)
{
    if(NULL == BufferAddr)
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return ;
    }
    g_BufferAddr = BufferAddr;
    g_BufferLen = BufLen;
    g_BufferPos = 0;
}

int MDrv_LD_Gets(char * s, int nchar)
{
    int n;
    if ((NULL == s)||(NULL == g_BufferAddr))
    {
        return -1;
    }

    n = 0;
    while(g_BufferPos < g_BufferLen)
    {
        s[n] = g_BufferAddr[g_BufferPos++];
        if (s[n] == '\n')
        {
            s[n+1] = '\0';
            return n;
        }
        n++;
        if (n>=nchar-1)
        {
            s[n] = '\0';
            return n;
        }
    }

    if (n)
    {
        s[n] = '\0';
        return n;
    }

    return -1;
}

void MDrv_LD_seek(unsigned int pos)
{
    g_BufferPos = pos;
}

int MDrv_LD_Strnicmp(const char *s1, const char *s2, size_t n)
{
    char c1, c2;

    if((NULL == s1)||(NULL == s2))
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return -1;
    }

    while (n-- != 0 && (*s1 || *s2))
    {
        c1 = *(char *)s1++;
        if ('a' <= c1 && c1 <= 'z')
            c1 += ('A' - 'a');
        c2 = *(char *)s2++;
        if ('a' <= c2 && c2 <= 'z')
            c2 += ('A' - 'a');
        if (c1 != c2)
        return c1 - c2;
    }

    return 0;
}

char MDrv_LD_StringToUpperCase(char ch)
{
    return (ch<0x7F)? toupper(ch): ch;
}

unsigned int MDrv_LD_ParseHexU32(char * str)
{
    unsigned int i, ret = 0;

    CHECK_POINTER_I(str);

    if (str[0] == '0' && MDrv_LD_StringToUpperCase(str[1]) == 'X')
        str += 2;
    for (i = 0; i < 8 && (*str != 0); i++, str++) //note: atmost 8*4=32bits
    {
        char ch = MDrv_LD_StringToUpperCase(*str);
        if ('0' <= ch && ch <= '9')
            ret = (ret << 4) | (ch-'0');
        else if ('A' <= ch && ch <= 'F')
            ret = (ret << 4) | (10+ch-'A');
        else
            break;
    }

    return ret;
}


static char* MDrv_LD_SkipLeading(const char *str)
{
    if(NULL == str)
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return NULL;
    }

    while (*str != '\0' && *str <= ' ')
        str++;

    return (char *)str;
}

static char* MDrv_LD_SkipTrailing(const char *str, const char *base)
{
    if((NULL == str) || (NULL == base))
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return NULL;
    }

    while (str > base && *(str-1) <= ' ')
        str--;

    return (char *)str;
}

static char* MDrv_LD_StripTrailing(char *str)
{
    char *ptr = MDrv_LD_SkipTrailing(strchr(str, '\0'), str);
    if(NULL == ptr)
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return NULL;
    }

    *ptr='\0';
    return str;
}

static int MDrv_LD_GetBeginString(const char *Key)
{
    char *sp, *ep;
    int len, num;
    char LocalBuffer[LINE_BUFFERSIZE];

    /* Move through file 1 line at a time until a section is matched or EOF.  */
    CHECK_POINTER_I(Key);
    len = strlen(Key);
    if (len > 0)
    {
        do
        {
            num = MDrv_LD_Gets(LocalBuffer, LINE_BUFFERSIZE);
            //LD_INFO("MDrv_LD_GetBeginString num:%d  \n", num);
            if (-1 == num)
                return -1;
            sp = MDrv_LD_SkipLeading(LocalBuffer);
            ep = strchr(sp, ']');
        }while (*sp != '[' || ep == NULL || ((int)(ep-sp-1) != len || MDrv_LD_Strnicmp(sp+1,Key,len) != 0));
        g_BeginPos = g_BufferPos;
    }
    else
        return -1;

    return 0;
}


static int MDrv_LD_GetKeyString(const char *Key, char *Buffer, int BufferSize)
{
    char *sp, *ep;
    int len, idx, isstring;
    char LocalBuffer[LINE_BUFFERSIZE];

    CHECK_POINTER_I(g_BufferAddr);
    CHECK_POINTER_I(Key);
    CHECK_POINTER_I(Buffer);

    // find the entry. Stop searching upon [end] area.
    len = (int)strlen(Key);
    idx = -1;
    do
    {
        if (-1 == MDrv_LD_Gets(LocalBuffer,LINE_BUFFERSIZE))
            return -1;
        sp = MDrv_LD_SkipLeading(LocalBuffer);
        ep = strchr(sp, '='); /* Parse out the equal sign */
        if (ep == NULL)
            ep = strchr(sp, ':');
    }while (*sp == ';' || *sp == '#' || ep == NULL || ((int)(MDrv_LD_SkipTrailing(ep,sp)-sp) != len || MDrv_LD_Strnicmp(sp,Key,len) != 0));


    /* Copy up to BufferSize chars to buffer */
    sp = MDrv_LD_SkipLeading(ep + 1);
    isstring = 0;
    for (ep = sp; *ep != '\0' && ((*ep != ';' && *ep != '#') || isstring); ep++)
    {
        if (*ep == '"')
        {
            if (*(ep + 1) == '"')
            ep++;                 /* skip "" (both quotes) */
            else
            isstring = !isstring; /* single quote, toggle isstring */
        }
        else if (*ep == '\\' && *(ep + 1) == '"')
        {
            ep++;                   /* skip \" (both quotes */
        }
    }
    assert(ep != NULL && (*ep == '\0' || *ep == ';' || *ep == '#'));
    *ep = '\0';                 /* terminate at a comment */
    MDrv_LD_StripTrailing(sp);

    strncpy(Buffer,sp,BufferSize);
    Buffer[BufferSize-1]='\0';

    return 0;
}

/** MDrv_LD_GetString()
* \param Section     the name of the section to search for
* \param Key         the name of the entry to find the value of
* \param DefValue    default string in the event of a failed read
* \param Buffer      a pointer to the buffer to copy into
* \param BufferSize  the maximum number of characters to copy
*
* \return            the number of characters copied into the supplied buffer
*/
int MDrv_LD_GetString(const char *Key, const char *DefValue, char *Buffer, int BufferSize)
{
    int ok = 0;

    if (Buffer == NULL || DefValue == NULL || BufferSize <= 0 || Key == NULL)
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return -1;
    }

    if (g_BufferAddr != NULL)
    {
        ok = MDrv_LD_GetKeyString(Key, Buffer, BufferSize);
        MDrv_LD_seek(g_BeginPos);
    }

    LD_INFO("MDrv_LD_GetString ok:%d  \n", ok);
    if (-1 == ok)
    {
        strncpy(Buffer, DefValue, BufferSize);
        Buffer[BufferSize-1]='\0';
    }

    return strlen(Buffer);
}

int MDrv_LD_GetArray(const char *Key,MS_U8*pu8array)
{
    char buff[64];
    char* tempbuffer = NULL;
    int len = 0;
    unsigned char u8Number =0;
    char puTmp[5]={0};
    if(NULL == Key)
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return -1;
    }

    len = MDrv_LD_GetString(Key, "", buff, SIZEARRAY(buff));
    tempbuffer = buff;

    if (len == 0)
        return -1;

    LD_INFO("BUFF str len:%d  buff = %s \n",len, buff);

    while ((tempbuffer=strstr(tempbuffer,"0x"))!=NULL)
    {
        memcpy(puTmp,tempbuffer,sizeof(puTmp)-1);
        u8Number=simple_strtoul(puTmp,NULL,16);
        //LD_INFO("!!0x%x \n",(unsigned int)u8Number);
        *(pu8array++)=u8Number;
        tempbuffer+=2;
    }
    return 0;
}

int MDrv_LD_GetArrayu16(const char *Key,MS_U16*pu16array)
{
    char buff[64];
    char* tempbuffer = NULL;
    int len = 0;
    MS_U16 u16Number = 0;
    char puTmp[7]={0};
    if(NULL == Key)
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return -1;
    }

    len = MDrv_LD_GetString(Key, "", buff, SIZEARRAY(buff));
    tempbuffer = buff;

    if (len == 0)
        return -1;

    LD_INFO("BUFF str len:%d  buff = %s \n",len, buff);

    while ((tempbuffer=strstr(tempbuffer,"0x"))!=NULL)
    {
        memcpy(puTmp,tempbuffer,sizeof(puTmp)-1);
        u16Number=simple_strtoul(puTmp,NULL,16);
        //LD_INFO("!!0x%x \n",(unsigned int)u8Number);
        *(pu16array++)=u16Number;
        tempbuffer+=2;
    }
    return 0;
}


/** MDrv_LD_GetInteger()
* \param Section     the name of the section to search for
* \param Key         the name of the entry to find the value of
* \param DefValue    the default value in the event of a failed read
*
* \return            the value located at Key
*/
long MDrv_LD_GetInteger(const char *Key, long DefValue)
{
    char buff[64];
    int len = 0;

    if(NULL == Key)
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return DefValue;
    }

    len = MDrv_LD_GetString(Key, "", buff, SIZEARRAY(buff));

    if (len == 0)
        return DefValue;

    LD_INFO("BUFF str len:%d  buff = %s \n",len, buff);
    if (buff[0] == '0' && MDrv_LD_StringToUpperCase(buff[1]) == 'X')
        return MDrv_LD_ParseHexU32(buff);
    else
        return simple_strtol(buff,NULL,10);
}

int MDrv_LD_GetBoolean(const char *Key, int DefValue)
{
    char buff[64];
    int len = 0;
    int ret = 0;

    if(NULL == Key)
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    len = MDrv_LD_GetString(Key, "", buff, SIZEARRAY(buff));

    if (len == 0)
    {
        return DefValue;
    }

    if((buff[0] == 'y') || (buff[0] == 'Y') || (buff[0] == '1') || (buff[0] == 't') || (buff[0] == 'T'))
    {
        ret = 1 ;
    }
    else if((buff[0] == 'n') || (buff[0] == 'N') || (buff[0] == '0') || (buff[0] == 'f') || (buff[0] == 'F'))
    {
        ret = 0 ;
    }
    else
    {
        ret = DefValue ;
    }

    return ret;
}

static void MDrv_LD_GetIniData(void)
{
    int n = 0;
    int i = 0;
    //begin
    n = MDrv_LD_GetBeginString("begin");
    LD_INFO("MDrv_LD_GetBeginString = %d \n",n);
    //panel
    stDrvLdInfo.u16PanelWidth = MDrv_LD_GetInteger("u16PanelWidth", 0x00);
    LD_INFO("u16PanelWidth = %d \n",stDrvLdInfo.u16PanelWidth);
    stDrvLdInfo.u16PanelHeight = MDrv_LD_GetInteger("u16PanelHeight", 0x00);
    LD_INFO("u16PanelHeight = %d \n",stDrvLdInfo.u16PanelHeight);
    stDrvLdInfo.u32BaseAddr = MDrv_LD_GetInteger("u32BaseAddr", 0x00);
    LD_INFO("u32BaseAddr = 0x%x \n",(MS_U32)(stDrvLdInfo.u32BaseAddr));
    //pq
    stDrvLdPQInfo.enLEDType = MDrv_LD_GetInteger("eLEDType", 0x00);
    LD_INFO("eLEDType = %d \n",stDrvLdPQInfo.enLEDType);
    stDrvLdPQInfo.u8LDFWidth = MDrv_LD_GetInteger("u8LDFWidth", 0x00);
    LD_INFO("u8LDFWidth = %d \n",stDrvLdPQInfo.u8LDFWidth);
    stDrvLdPQInfo.u8LDFHeight = MDrv_LD_GetInteger("u8LDFHeight", 0x00);
    LD_INFO("u8LDFHeight = %d \n",stDrvLdPQInfo.u8LDFHeight);
    stDrvLdPQInfo.u8LEDWidth = MDrv_LD_GetInteger("u8LEDWidth", 0x00);
    LD_INFO("u8LEDWidth = %d \n",stDrvLdPQInfo.u8LEDWidth);
    stDrvLdPQInfo.u8LEDHeight = MDrv_LD_GetInteger("u8LEDHeight", 0x00);
    LD_INFO("u8LEDHeight = %d \n",stDrvLdPQInfo.u8LEDHeight);
    stDrvLdPQInfo.u8LSFWidth = MDrv_LD_GetInteger("u8LSFWidth", 0x00);
    LD_INFO("u8LSFWidth = %d \n",stDrvLdPQInfo.u8LSFWidth);
    stDrvLdPQInfo.u8LSFHeight = MDrv_LD_GetInteger("u8LSFHeight", 0x00);
    LD_INFO("u8LSFHeight = %d \n",stDrvLdPQInfo.u8LSFHeight);
    stDrvLdPQInfo.bCompensationEn = MDrv_LD_GetBoolean("bCompEn", 0);    //FALSE
    LD_INFO("bCompEn = %d \n",stDrvLdPQInfo.bCompensationEn);
    stDrvLdPQInfo.bEdge2DEn = MDrv_LD_GetBoolean("bEdge2DEn", 0);    //TRUE
    LD_INFO("bEdge2DEn = %d \n",stDrvLdPQInfo.bEdge2DEn);
    stDrvLdPQInfo.bLSFEncodingEn = MDrv_LD_GetBoolean("bLSFEncodingEn", 0);    //TRUE
    LD_INFO("bLSFEncodingEn = %d \n",stDrvLdPQInfo.bLSFEncodingEn);
    stDrvLdPQInfo.bCompLowGainEn = MDrv_LD_GetBoolean("bCompLowGainEn", 0);    //TRUE
    LD_INFO("bCompLowGainEn = %d \n",stDrvLdPQInfo.bCompLowGainEn);
    stDrvLdPQInfo.bSaturationEn = MDrv_LD_GetBoolean("bSaturationEn", 0);    //TRUE
    LD_INFO("bSaturationEn = %d \n",stDrvLdPQInfo.bSaturationEn);
    stDrvLdPQInfo.bSFLinearEn = MDrv_LD_GetBoolean("bSFLinearEn", 0);    //TRUE
    LD_INFO("bSFLinearEn = %d \n",stDrvLdPQInfo.bSFLinearEn);
    stDrvLdPQInfo.u32LsfInitialPhase_H = MDrv_LD_GetInteger("u32LsfInitialPhase_HL", 0x80000);
    LD_INFO("u32LsfInitialPhase_H = 0x%x \n",stDrvLdPQInfo.u32LsfInitialPhase_H);
    stDrvLdPQInfo.u32LsfInitialPhase_V = MDrv_LD_GetInteger("u32LsfInitialPhase_HR", 0x80000);
    LD_INFO("u32LsfInitialPhase_V = 0x%x \n",stDrvLdPQInfo.u32LsfInitialPhase_V);
    stDrvLdPQInfo.u8DCMaxAlpha = MDrv_LD_GetInteger("u8DCMaxAlpha", 0x8);
    LD_INFO("u8DCMaxAlpha = %d \n",stDrvLdPQInfo.u8DCMaxAlpha);
    stDrvLdPQInfo.u8CompenGain = MDrv_LD_GetInteger("u8CompensationBlend", 0x8);
    LD_INFO("u8CompenGain = %d \n",stDrvLdPQInfo.u8CompenGain);
    //pq sw
    stDrvLdPQInfo.u8GDStrength = MDrv_LD_GetInteger("u8GDStrength", 0xFF);
    LD_INFO("u8GDStrength = %d \n",stDrvLdPQInfo.u8GDStrength);
    stDrvLdPQInfo.u8LDStrength = MDrv_LD_GetInteger("u8LDStrength", 0xFF);
    LD_INFO("u8LDStrength = %d \n",stDrvLdPQInfo.u8LDStrength);
    stDrvLdPQInfo.u8TFStrengthUp = MDrv_LD_GetInteger("u8TFStrengthUp", 0xF6);
    LD_INFO("u8TFStrengthUp = %d \n",stDrvLdPQInfo.u8TFStrengthUp);
    stDrvLdPQInfo.u8TFStrengthDn = MDrv_LD_GetInteger("u8TFStrengthDn", 0xF6);
    LD_INFO("u8TFStrengthDn = %d \n",stDrvLdPQInfo.u8TFStrengthDn);
    stDrvLdPQInfo.u8TFLowThreshold = MDrv_LD_GetInteger("u8TFLowThreshold", 0x14);
    LD_INFO("u8TFLowThreshold = %d \n",stDrvLdPQInfo.u8TFLowThreshold);
    stDrvLdPQInfo.u8TFHightThreshold = MDrv_LD_GetInteger("u8TFHightThreshold", 0x36);
    LD_INFO("u8TFHightThreshold = %d \n",stDrvLdPQInfo.u8TFHightThreshold);
    stDrvLdPQInfo.u8SFStrength = MDrv_LD_GetInteger("u8SFStrength", 0xFF);
    LD_INFO("u8SFStrength = %d \n",stDrvLdPQInfo.u8SFStrength);
    stDrvLdPQInfo.u8SFStrength2 = MDrv_LD_GetInteger("u8SFStrength2", 0xFF);
    LD_INFO("u8SFStrength2 = %d \n",stDrvLdPQInfo.u8SFStrength2);
    stDrvLdPQInfo.u8SFStrength3 = MDrv_LD_GetInteger("u8SFStrength3", 0xFF);
    LD_INFO("u8SFStrength3 = %d \n",stDrvLdPQInfo.u8SFStrength3);
    stDrvLdPQInfo.u8SFStrength4 = MDrv_LD_GetInteger("u8SFStrength4", 0xFF);
    LD_INFO("u8SFStrength4 = %d \n",stDrvLdPQInfo.u8SFStrength4);
    stDrvLdPQInfo.u8SFStrength5 = MDrv_LD_GetInteger("u8SFStrength5", 0xFF);
    LD_INFO("u8SFStrength5 = %d \n",stDrvLdPQInfo.u8SFStrength5);
    stDrvLdPQInfo.u8InGamma = MDrv_LD_GetInteger("u8InGamma", 0x00);
    LD_INFO("u8InGamma = %d \n",stDrvLdPQInfo.u8InGamma);
    stDrvLdPQInfo.u8OutGamma = MDrv_LD_GetInteger("u8OutGamma", 0x00);
    LD_INFO("u8OutGamma = %d \n",stDrvLdPQInfo.u8OutGamma);
    stDrvLdPQInfo.u8BLMinValue = MDrv_LD_GetInteger("u8BLMinValue", 0xFF);
    LD_INFO("u8BLMinValue = %d \n",stDrvLdPQInfo.u8BLMinValue);
    stDrvLdPQInfo.u8NRStrength = MDrv_LD_GetInteger("u8NRStrength", 0x30);
    LD_INFO("u8NRStrength = %d \n",stDrvLdPQInfo.u8NRStrength);
    //misc
    stDrvLdMiscInfo.u32MarqueeDelay = MDrv_LD_GetInteger("u32MarqueeDelay", 0xFFF);
    LD_INFO("u32MarqueeDelay = %d \n",stDrvLdMiscInfo.u32MarqueeDelay);
    stDrvLdMiscInfo.bDbgEn = MDrv_LD_GetBoolean("bDbgEn", 0);    //FALSE
    LD_INFO("bDbgEn = %d \n",stDrvLdMiscInfo.bDbgEn);
    stDrvLdMiscInfo.u8DebugLevel = MDrv_LD_GetInteger("u8DebugLevel", 1);
    LD_INFO("u8DebugLevel = %d \n",stDrvLdMiscInfo.u8DebugLevel);
    stDrvLdMiscInfo.bLDEn = MDrv_LD_GetBoolean("bLDEn", 0);
    LD_INFO("bLDEn = %d \n",stDrvLdMiscInfo.bLDEn);
    stDrvLdMiscInfo.u8SPIBits = MDrv_LD_GetInteger("u8SPIBits", 0);
    LD_INFO("u8SPIBits = %d \n",stDrvLdMiscInfo.u8SPIBits);
    stDrvLdMiscInfo.u8ClkHz= MDrv_LD_GetInteger("u8ClkHz", 60);
    LD_INFO("u8ClkHz = %d \n",stDrvLdMiscInfo.u8ClkHz);
    stDrvLdMiscInfo.u8MirrorPanel = MDrv_LD_GetInteger("u8MirrorPanel", 0);
    LD_INFO("u8MirrorPanel = %d \n",stDrvLdMiscInfo.u8MirrorPanel);

    //MSPI
    stDrvLdMspiInfo.eChannel = MDrv_LD_GetInteger("u8MspiChanel", 0xFFF);
    LD_INFO("u8MspiChanel = %d \n",stDrvLdMspiInfo.eChannel);
    stDrvLdMspiInfo.eMSPIMode = MDrv_LD_GetInteger("u8MspiMode", 0);    //FALSE
    LD_INFO("u8MspiMode = %d \n",stDrvLdMspiInfo.eMSPIMode);

    stDrvLdMspiInfo.tMSPI_DCConfig.u8TrStart = MDrv_LD_GetInteger("u8TrStart", 0);
    LD_INFO("u8TrStart = %d \n",stDrvLdMspiInfo.tMSPI_DCConfig.u8TrStart);

    stDrvLdMspiInfo.tMSPI_DCConfig.u8TrEnd = MDrv_LD_GetInteger("u8TrEnd", 0);
    LD_INFO("u8TrEnd = %d \n",stDrvLdMspiInfo.tMSPI_DCConfig.u8TrEnd);

    stDrvLdMspiInfo.tMSPI_DCConfig.u8TB = MDrv_LD_GetInteger("u8TB", 0);
    LD_INFO("u8TB = %d \n",stDrvLdMspiInfo.tMSPI_DCConfig.u8TB);

    stDrvLdMspiInfo.tMSPI_DCConfig.u8TRW = MDrv_LD_GetInteger("u8TRW", 0);
    LD_INFO("u8TRW = %d \n",stDrvLdMspiInfo.tMSPI_DCConfig.u8TRW);

    stDrvLdMspiInfo.tMSPI_ClockConfig.u32Clock = MDrv_LD_GetInteger("u32MspiClk", 0);
    LD_INFO("u32Clock = %d \n",stDrvLdMspiInfo.tMSPI_ClockConfig.u32Clock);

    stDrvLdMspiInfo.tMSPI_ClockConfig.BClkPolarity = MDrv_LD_GetInteger("BClkPolarity", 0);
    LD_INFO("BClkPolarity = %d \n",stDrvLdMspiInfo.tMSPI_ClockConfig.BClkPolarity);

    stDrvLdMspiInfo.tMSPI_ClockConfig.BClkPhase = MDrv_LD_GetInteger("BClkPhase", 0);
    LD_INFO("BClkPhase = %d \n",stDrvLdMspiInfo.tMSPI_ClockConfig.BClkPhase);

    stDrvLdMspiInfo.tMSPI_ClockConfig.u32MAXClk = MDrv_LD_GetInteger("u32MAXClk", 0);
    LD_INFO("u32MAXClk = %d \n",stDrvLdMspiInfo.tMSPI_ClockConfig.u32MAXClk);

    stDrvLdMspiInfo.u8MspiBuffSizes = MDrv_LD_GetInteger("u8MspiBuffSizes", 1);
    LD_INFO("u8MspiBuffSizes = %d \n",stDrvLdMspiInfo.u8MspiBuffSizes);

    MDrv_LD_GetArray("u8WBitConfig[8]", stDrvLdMspiInfo.tMSPI_FrameConfig.u8WBitConfig);
    MDrv_LD_GetArray("u8RBitConfig[8]", stDrvLdMspiInfo.tMSPI_FrameConfig.u8RBitConfig);    //FALSE
    for(i=0;i<8;i++)
    {
        LD_INFO("u8WBitConfig[%d] = %d ",i,stDrvLdMspiInfo.tMSPI_FrameConfig.u8WBitConfig[i]);
        LD_INFO("u8RBitConfig[%d] = %d \n",i,stDrvLdMspiInfo.tMSPI_FrameConfig.u8RBitConfig[i]);
    }


    //LD_DMA
    stDrvLdMDAInfo.eLDMAchanel = MDrv_LD_GetInteger("u8LDMAchanel", 1);
    LD_INFO("eLDMAchanel = %d \n",stDrvLdMDAInfo.eLDMAchanel);

    stDrvLdMDAInfo.eLDMATrimode = MDrv_LD_GetInteger("u8LDMATrimode", 0);
    LD_INFO("eLDMATrimode = %d \n",stDrvLdMDAInfo.eLDMATrimode);

    stDrvLdMDAInfo.eLDMACheckSumMode = MDrv_LD_GetInteger("u8LDMACheckSumMode", 0);
    LD_INFO("u8LDMACheckSumMode = %d \n",stDrvLdMDAInfo.eLDMACheckSumMode);

    stDrvLdMDAInfo.u8cmdlength = MDrv_LD_GetInteger("u8cmdlength", 1);
    LD_INFO("u8cmdlength = %d \n",stDrvLdMDAInfo.u8cmdlength);

    stDrvLdMDAInfo.u8BLWidth = MDrv_LD_GetInteger("u8LEDWidth", 0);
    LD_INFO("u8BLWidth = %d \n",stDrvLdMDAInfo.u8BLWidth);

    stDrvLdMDAInfo.u8BLHeight = MDrv_LD_GetInteger("u8LEDHeight", 0);
    LD_INFO("u8BLHeight = %d \n",stDrvLdMDAInfo.u8BLHeight);

    stDrvLdMDAInfo.u16LedNum = MDrv_LD_GetInteger("u16LedNum", 0);
    LD_INFO("u16LedNum = %d \n",stDrvLdMDAInfo.u16LedNum);

    stDrvLdMDAInfo.u8DataPackMode = MDrv_LD_GetInteger("u8DataPackMode", 0);
    LD_INFO("u8DataPackMode = %d \n",stDrvLdMDAInfo.u8DataPackMode);

    stDrvLdMDAInfo.u8DataInvert = MDrv_LD_GetInteger("u8DataInvert", 0);
    LD_INFO("u8DataInvert = %d \n",stDrvLdMDAInfo.u8DataInvert);

    MDrv_LD_GetArrayu16("u16MspiHead[8]", stDrvLdMDAInfo.u16MspiHead);
    for(i=0;i<8;i++)
    {
        LD_INFO("u16MspiHead[%d] = %d ",i,stDrvLdMDAInfo.u16MspiHead[i]);
    }
    MDrv_LD_GetArrayu16("u16DMADelay[4]", stDrvLdMDAInfo.u16DMADelay);
    for(i=0;i<4;i++)
    {
        LD_INFO("u16DMADelay[%d] = %d ",i,stDrvLdMDAInfo.u16DMADelay[i]);
    }
}

MS_BOOL MDrv_LD_ParseIni(char *con_file ,char *con_fileUpdate)
{
    char *buf;
    long n = 0;
    mm_segment_t old_fs;
    struct file *filp;

    CHECK_POINTER_I(con_file);
    CHECK_POINTER_I(con_fileUpdate);
    LD_INFO(" customer path:%s, customer u_path:%s  \n", con_file, con_fileUpdate);
    //Used USB update LDM.ini
    filp = filp_open(con_fileUpdate, O_RDONLY, 0);
    if(IS_ERR(filp))
    {
        LD_INFO("--------------  load U files fail  filp:%p  \n", filp);
        filp = filp_open(con_file, O_RDONLY , 0);

        if (IS_ERR(filp))
        {
            printk(KERN_ERR"-------------- error! MDrv_LD_ParseIni config fail filp:%p  \n", filp);
            return FALSE;
        }
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    buf = kzalloc(LDM_BUF_SIZE+1, GFP_KERNEL);
    if(!buf)
    {
        printk(KERN_ERR"-------------- error! MDrv_LD_ParseIni kzalloc fail buf:%p  \n", buf);
        return FALSE;
    }

    if (filp->f_op && filp->f_op->read)
    {
        n = filp->f_op->read(filp, buf, LDM_BUF_SIZE, &filp->f_pos);
    }

    if (n < 0)
    {
        set_fs(old_fs);
        filp_close(filp, NULL);
        kfree(buf);
        return FALSE;
    }

    MDrv_LD_InitBuff(buf,LDM_BUF_SIZE);
    MDrv_LD_GetIniData();

    set_fs(old_fs);
    filp_close(filp, NULL);
    kfree(buf);

    return TRUE;
}

MS_BOOL MDrv_LD_CheckData(char *buff, int buff_len)
{
    MS_U16 u16Checked = 0;
    MS_U32 u32Counter = 0;
    MS_U64 u64Sum = 0;

    if(NULL == buff)
    {
        printk(KERN_ERR"error! %s:%d, parametre error Pointer is null \n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    //before checked data
    for(; u32Counter<LDM_BIN_CHECK_POSITION; u32Counter++)
    {
        u64Sum += *(buff+u32Counter);
    }

    u16Checked = *(buff+LDM_BIN_CHECK_POSITION) + ((*(buff+LDM_BIN_CHECK_POSITION+1))<<8);

    //after checked to 0xbuff_len
    for(u32Counter = LDM_BIN_CHECK_POSITION+2; u32Counter<buff_len; u32Counter++)
    {
        u64Sum += *(buff+u32Counter);
    }

    LD_INFO(" buff_len:%d, u16Checked:0x%x, u64Sum:0x%lx  \n", buff_len, u16Checked,(unsigned long)u64Sum);
    if(u16Checked != (u64Sum&0xFFFF))
    {
        return FALSE;
    }

    return TRUE ;
}

MS_BOOL MDrv_LD_ParseBin(char *buff, int buff_len, char *con_file, char *con_fileUpdate)
{
    int n = 0;
    mm_segment_t old_fs;
    struct file *filp;
    MS_BOOL bValid = FALSE;

    CHECK_POINTER_I(buff);
    CHECK_POINTER_I(con_file);
    CHECK_POINTER_I(con_fileUpdate);
    LD_INFO(" customer path:%s, customer u_path:%s  \n", con_file, con_fileUpdate);
    //Used USB update LDM.ini
    filp = filp_open(con_fileUpdate, O_RDONLY, 0);
    if(IS_ERR(filp))
    {
        LD_INFO("--------------  load U filse fail  filp:%p  \n", filp);
        filp = filp_open(con_file, O_RDONLY , 0);

        if (IS_ERR(filp))
        {
            printk(KERN_ERR"-------------- error! MDrv_LD_ParseBin config fail filp:%p  \n", filp);
            return FALSE;
        }
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    if (filp->f_op && filp->f_op->read)
    {
        n = filp->f_op->read(filp, buff, buff_len, &filp->f_pos);
    }

    bValid = MDrv_LD_CheckData(buff, n);

    if ((n < 0) || (FALSE == bValid))
    {
        set_fs(old_fs);
        filp_close(filp, NULL);
        return FALSE;
    }

    set_fs(old_fs);
    filp_close(filp, NULL);

    return TRUE ;
}

#endif
