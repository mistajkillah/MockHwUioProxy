include ../common.mk

APP := $(OUTPUT)/MockHWEmulator

$(APP): main.c | $(OUTPUT)
	$(CC) $(CFLAGS) -o $@ $<

$(OUTPUT):
	mkdir -p $(OUTPUT)

all: $(APP)

clean:
	rm -f $(APP)
