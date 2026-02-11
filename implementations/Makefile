.PHONY: all test test-cpp test-python test-js test-json clean

all: test

test: test-cpp test-python test-js

test-cpp:
	$(MAKE) -C cpp test

test-json:
	$(MAKE) -C cpp test-json

test-python:
	$(MAKE) -C python test

test-js:
	npm --prefix js test

clean:
	$(MAKE) -C cpp clean
	$(MAKE) -C python clean
	# No clean needed for JS usually, but could rm dist if desired
