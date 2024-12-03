.PHONY: clean

dist/pybqn-0.1.0.tar.gz: bqn.c pyproject.toml setup.py
	python3 -m build

install: dist/pybqn-0.1.0.tar.gz
	python3 -m pip install $<

clean:
	rm -rf dist *.egg-info
