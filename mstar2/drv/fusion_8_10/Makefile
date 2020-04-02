EXTRA_CFLAGS      +=  -Idrivers/mstar2/include 
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/fusion_8_10
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/fusion_8_10/single

obj-$(CONFIG_MSTAR_FUSION_8_10) += fusion_8_10.o

fusion_8_10-y := single/fusioncore_impl.o call.o debug.o entries.o fifo.o fusiondev.o fusionee.o list.o property.o reactor.o ref.o skirmish.o shmpool.o hash.o

$(obj)/call.o: include/linux/fusion.h
include/linux/fusion.h: FORCE
	@cp mstar2/drv/fusion_8_10/include/fusion.h 	include/linux/fusion.h -f
