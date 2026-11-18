SHELL := /bin/bash
.PHONY: check test replay-k6 pdf
check:
	sha256sum -c SHA256SUMS
	python3 -I tests/test_verify.py
	python3 -I tests/check_manifest.py
	python3 -I validation/check_counting.py
test:
	ASAN_OPTIONS=detect_leaks=0 bash tests/run_tests.sh
replay-k6:
	bash validation/replay_k6.sh
pdf:
	pdflatex -interaction=nonstopmode -halt-on-error manuscript.tex
	pdflatex -interaction=nonstopmode -halt-on-error manuscript.tex
