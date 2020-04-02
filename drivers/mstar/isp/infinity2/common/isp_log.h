#ifndef ISP_LOG_H
#define ISP_LOG_H
#include <isp_porting_cfg.h>

#if _OS_SEL_ == _LINUX_KERNEL_
#include <printk.h>
//#define isp_pr_debug(args...)    pr_debug(args)
//#define isp_pr_info(args...)     pr_info(args)
//#define isp_pr_err(args...)      pr_err(args)
#define print_kmsg(args...)      pr_info(args)
#elif _OS_SEL_ == _RTK_
#define pr_debug(args...)    UartSendTrace(args)
#define pr_info(args...)     UartSendTrace(args)
#define pr_error(args...)    UartSendTrace(args)

#endif

#endif
