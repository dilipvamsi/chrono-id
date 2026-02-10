VENV = venv
PYTHON = $(VENV)/bin/python3
PIP = $(VENV)/bin/pip

.PHONY: test setup clean

setup: $(VENV)/bin/activate

$(VENV)/bin/activate:
	python3 -m venv $(VENV)
	$(PIP) install coverage

test: setup
	export PYTHONPATH=$$(pwd)/src; \
	$(VENV)/bin/coverage run -m unittest discover tests; \
	$(VENV)/bin/coverage report -m

clean:
	rm -rf $(VENV)
	rm -rf .coverage
	find . -type d -name "__pycache__" -exec rm -rf {} +
