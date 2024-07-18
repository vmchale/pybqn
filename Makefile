UNAME:=$(shell uname)

PY_VER := 3.12

ifeq ($(UNAME),Darwin)
	CFLAGS := -I /Library/Frameworks/Python.framework/Versions/$(PY_VER)/include/python$(PY_VER) \
		  -I /Library/Frameworks/Python.framework/Versions/$(PY_VER)/lib/python$(PY_VER)/site-packages/numpy/_core/include
else
	CFLAGS := -I "$$(python3 -m site --user-site)/numpy/_core/include" \
		  -I /usr/local/include/python$(PY_VER)
endif

ifeq ($(UNAME),Darwin)
	LDFLAGS := -rpath /usr/local/lib /Library/Frameworks/Python.framework/Versions/$(PY_VER)/Python
endif

all: bqn.so

install: bqn.so
	cp $^ $$(python3 -m site --user-site)

bqn.o: bqnmodule.c
	$(CC) -fPIC -O2 -c $< $(CFLAGS) -o $@

bqn.so: bqn.o
	$(CC) -shared $^ -o $@ -lcbqn $(LDFLAGS)

clean:
	rm -rf *.o *.so
