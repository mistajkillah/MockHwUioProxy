include ../common.mk

APP := $(OUTPUT)/ExampleApp

$(APP): main.c | $(OUTPUT)
	$(CC) $(CFLAGS) -o $@ $<

$(OUTPUT):
	mkdir -p $(OUTPUT)

all: $(APP)

clean:
	rm -f $(APP)
