#
# Makefile for MStar CMA Pool device drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include/sys/common
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/ipa_pool
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/miu
# specific options
EXTRA_CFLAGS      +=  -Idrivers/staging/android/ion

# files
obj-$(CONFIG_MSTAR_IPAPOOL)     += ipa_pool.o mdrv_ipa_pool.o ipa_bootargs.o 
obj-$(CONFIG_MSTAR_IPAPOOL)     += pa/
obj-$(CONFIG_MSTAR_IPAPOOL)     += ipa_cma/
 

