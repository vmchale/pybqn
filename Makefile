PY_VER := 3.12

all: bqn.so

install: bqn.so
	cp $^ $$(python3 -m site --user-site)

bqn.o: bqnmodule.c
	$(CC) -fPIC -O2 -c $< \
		-I /Library/Frameworks/Python.framework/Versions/$(PY_VER)/include/python$(PY_VER) \
		-I /Library/Frameworks/Python.framework/Versions/$(PY_VER)/lib/python$(PY_VER)/site-packages/numpy/core/include -o $@

bqn.so: bqn.o
	$(CC) -shared $^ -o $@ -lcbqn -rpath /usr/local/lib /Library/Frameworks/Python.framework/Versions/$(PY_VER)/Python

clean:
	rm -rf *.o *.so
