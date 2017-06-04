all:
	$(MAKE) -C server all
	$(MAKE) -C client all
	ln -s server/server Server
	ln -s client/client Client

.PHONY: clean

clean:
	rm -f Server
	rm -f Client
	$(MAKE) -C server clean
	$(MAKE) -C client clean
