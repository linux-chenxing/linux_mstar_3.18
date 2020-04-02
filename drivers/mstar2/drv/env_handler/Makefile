#
# Makefile for MStar Env handler device drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/env_handler

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_ENABLE_ENV_HANDLER)	+= env-handler.o
env-handler-objs			+= env_handler.o
