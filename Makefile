all:
	$(MAKE) -C server all
	$(MAKE) -C client all
	ln -fs server/server Server
	ln -fs client/client Client

.PHONY: clean

clean:
	rm -f Server
	rm -f Client
	$(MAKE) -C server clean
	$(MAKE) -C client clean
