#ifndef __DRV_AUDIO_DEBUG_H__
#define __DRV_AUDIO_DEBUG_H__
//------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------

#if 0
#define TRACE_LEVEL_TAG        1
#define ERROR_LEVEL_TAG        1
#define DEBUG_LEVEL_TAG        1
#define IRQ_LEVEL_TAG          1
#define MHAL_LEVEL_TAG         1
#else
#define TRACE_LEVEL_TAG        0
#define ERROR_LEVEL_TAG        1
#define DEBUG_LEVEL_TAG        0
#define IRQ_LEVEL_TAG          0
#define MHAL_LEVEL_TAG         0
#endif


#define TRACE_LEVEL          "[AUDIO TRACE]"
#define ERROR_LEVEL          "[AUDIO ERROR]"
#define DEBUG_LEVEL          "[AUDIO DEBUG]"
#define IRQ_LEVEL            "[AUDIO IRQ]"
#define MHAL_LEVEL           "[AUDIO MHAL]"


#define LOG_MSG 					1
#if LOG_MSG
#define AUD_PRINTF(level ,fmt, arg...)		if (level##_TAG) printk(KERN_ERR level fmt, ##arg);
#else
#define AUD_PRINTF(level ,fmt, arg...)
#endif

#endif  /* __DRV_AUDIO_DEBUG_H__ */

