#include <linux/module.h>
#include <linux/init.h>
#include <drv_isrcb.h>

EXPORT_SYMBOL(ISRCB_RegisterCallback);
EXPORT_SYMBOL(ISRCB_UnRegisterCallback);
EXPORT_SYMBOL(ISRCB_Proc);

extern int __init ISRCB_EarlyInit(void);
early_initcall(ISRCB_EarlyInit);

MODULE_LICENSE("GPL");
