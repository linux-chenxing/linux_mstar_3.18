obj-$(CONFIG_MS_SDMMC)   += kdrv_sdmmc.o
kdrv_sdmmc-y =  src/hal_sd_regs.o src/hal_sd_timer.o

ifeq ($(CONFIG_ARCH_INFINITY2),y)
kdrv_sdmmc-y +=  src/hal_card_intr_v5.o
kdrv_sdmmc-y += src/hal_sdmmc_v5.o
kdrv_sdmmc-y += src/hal_sd_platform_iNF2.o
else ifeq ($(CONFIG_ARCH_CLEVELAND),y)
kdrv_sdmmc-y +=  src/hal_card_intr_v5.o
kdrv_sdmmc-y += src/hal_sdmmc_v5.o
kdrv_sdmmc-y += src/hal_sd_platform_C4P.o
else ifeq ($(CONFIG_ARCH_INFINITY3),y)
kdrv_sdmmc-y +=  src/hal_card_intr_v5.o
kdrv_sdmmc-y += src/hal_sdmmc_v5.o
kdrv_sdmmc-y += src/hal_card_platform_iNF3.o
else ifeq ($(CONFIG_ARCH_INFINITY),y)
kdrv_sdmmc-y +=  src/hal_card_intr_v5.o
kdrv_sdmmc-y += src/hal_sdmmc_v5.o
kdrv_sdmmc-y += src/hal_card_platform_iNF.o
endif

kdrv_sdmmc-y += ms_sdmmc_lnx.o
