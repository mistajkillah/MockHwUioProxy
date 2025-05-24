# Only include common.mk here in the wrapper phase (not during Kbuild processing)
COMMON := ../common.mk
ifneq ("$(wildcard $(COMMON))","")
include $(COMMON)
endif

obj-m := MockHwUioProxyDriver.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Normal user-facing build target
all: $(OUTPUT)
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	cp MockHwUioProxyDriver.ko $(OUTPUT)/MockHwUioProxyDriver.ko
	cp install_driver.sh $(OUTPUT)/install_driver.sh
	chmod +x $(OUTPUT)/install_driver.sh

# Only available when building outside the kernel tree
$(OUTPUT):
	mkdir -p $(OUTPUT)

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(OUTPUT)/MockHwUioProxyDriver.ko $(OUTPUT)/install_driver.sh
