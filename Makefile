all: bqn.so

install: bqn.so
	cp $^ $$(python3 -m site --user-site)

bqn.o: bqnmodule.c
	$(CC) -fPIC -O2 -c $< \
		-I "$$(python3 -m site --user-site)/numpy/core/include" \
		-I /usr/local/include/python3.12 -o $@

bqn.so: bqn.o
	$(CC) -shared $^ -o $@ -lcbqn
	strip $@

clean:
	rm -rf *.o *.so
