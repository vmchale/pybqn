all: bqn.so

install: bqn.so
	cp $^ $$(python3 -m site --user-site)

bqn.o: bqnmodule.c
	gcc -fPIC -O2 -c $< -I /usr/local/include/python3.11 -o $@

bqn.so: bqn.o
	gcc -shared $^ -o $@ -lcbqn

clean:
	rm -rf *.o *.so
