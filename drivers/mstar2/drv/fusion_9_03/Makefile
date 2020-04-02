EXTRA_CFLAGS      +=  -Idrivers/mstar2/include 
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/fusion_9_03
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/fusion_9_03/single

obj-$(CONFIG_MSTAR_FUSION_9_03) += fusion_9_03.o
fusion_9_03-y := single/fusioncore_impl.o call.o debug.o entries.o fifo.o fusiondev.o fusionee.o list.o property.o reactor.o ref.o skirmish.o shmpool.o hash.o

$(obj)/call.o: include/linux/fusion.h
include/linux/fusion.h: FORCE
	@cp mstar2/drv/fusion_9_03/include/fusion.h 	include/linux/fusion.h -f