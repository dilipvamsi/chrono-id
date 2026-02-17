.PHONY: gen clean

# Path to the Python implementation source
PY_SRC = ../implementations/python/src

# Generate the cross-platform test cases
gen:
	export PYTHONPATH=$(PY_SRC):$$PYTHONPATH; python3 gen_tests.py

# Clean up any temporary files (none currently, but good practice)
clean:
	@echo "Nothing to clean in scripts."
