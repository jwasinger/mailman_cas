# Tamito KAJIYAMA <19 December 2000>

all:

PYTHON=	python
OPTS=	

test:
	$(PYTHON) ./regrtest.py $(OPTS)

.PHONY:	output
output:
	test -d output || mkdir output
	$(RM) output/*
	$(PYTHON) ./regrtest.py -g $(OPTS)

clean:
	rm -f *~ *.pyc
