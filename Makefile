EXECS=convolute
MPICC?=mpic++
NUMPROCS=5
INPUT=sample_input1

all: ${EXECS}

convolute: convolute.cpp
	${MPICC} -o convolute convolute.cpp

test:
	./test.sh ${NUMPROCS}

.PHONY: clean
clean:
	rm -f ${EXECS}
