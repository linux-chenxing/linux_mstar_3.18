EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/fusion

obj-$(CONFIG_MSTAR_FUSION) += fusion.o

fusion-y := call.o entries.o fifo.o fusiondev.o fusionee.o list.o property.o reactor.o ref.o skirmish.o shmpool.o
$(obj)/call.o: include/linux/fusion.h
include/linux/fusion.h: FORCE
	@cp mstar2/drv/fusion/include/fusion.h  include/linux/fusion.h -f

