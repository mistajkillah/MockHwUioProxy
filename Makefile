all:
	$(MAKE) -C MockHwUioProxyDriver
	$(MAKE) -C ExampleApp
	$(MAKE) -C MockHWEmulator

clean:
	$(MAKE) -C MockHwUioProxyDriver clean
	$(MAKE) -C ExampleApp clean
	$(MAKE) -C MockHWEmulator clean
	rm -rf output
