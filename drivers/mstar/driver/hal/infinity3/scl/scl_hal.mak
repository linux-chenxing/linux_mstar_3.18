#-------------------------------------------------------------------------------
#	Description of some variables owned by the library
#-------------------------------------------------------------------------------
# Library module (lib) or Binary module (bin)
PROCESS = lib
PATH_C +=\
  $(PATH_scl_hal)/src/hal\
  $(PATH_scl_hal)/src/drv\
  $(PATH_scl_hal)/src/drv/rtk\
  $(PATH_scl_hal)/src/pq

PATH_H +=\
	$(PATH_scl_hal)/inc/pq \
  $(PATH_scl_hal)/inc/hal\
  $(PATH_scl_hal)/inc/drv\
  $(PATH_scl_hal)/inc/drv/rtk\
  $(PATH_scl)/inc/rtk\
  $(PATH_scl)/inc/pq\
  $(PATH_scl)/pub\
  $(PATH_scl)/pub/rtk\
  $(PATH_cam_os_wrapper)/pub


#-------------------------------------------------------------------------------
#	List of source files of the library or executable to generate
#-------------------------------------------------------------------------------
SRC_C_LIST =\
  hal_scl.c\
  hal_utility.c \
  hal_scl_pnl.c\
  hal_scl_irq.c\
  hal_scl_hvsp.c\
  hal_scl_dma.c\
  hal_scl_cmdq.c \
  hal_scl_pq_qualitymap_main.c \
  hal_scl_pq.c \
  hal_scl_vip.c \
	drv_scl.c\
  drv_scl_irq.c \
  drv_scl_pnl.c \
  drv_scl_cmdq.c \
  drv_scl_dma.c \
  drv_scl_hvsp.c \
  drv_scl_pnl_m.c \
  drv_scl_hvsp_m.c \
  drv_scl_dma_m.c \
  drv_scl_hvsp_io_wrapper.c \
  drv_scl_dma_io_wrapper.c \
  drv_scl_pnl_io_wrapper.c \
  drv_scl_multiinst_m.c \
  drv_scl_vip.c \
  drv_scl_vip_m.c \
  drv_scl_vip_io_wrapper.c

#------------------------------------------------------------
# List of source files of verification to generate
#------------------------------------------------------------
ifeq ($(call FIND_COMPILER_OPTION, __I_SW__), TRUE)
    SRC_C_LIST += scl_test.c
endif

