zreladdr-y   := $(shell /bin/bash -c 'printf "0x%08x" $$[$(CONFIG_MEMORY_START_ADDRESS) + 0x80000]')
params_phys-y   := 0x40000100
initrd_phys-y   := 0x40800000

