EXECS=parallel
MPICC?=mpic++
NUMPROCS=5
INPUT=sample_input1

all: ${EXECS}

parallel: parallel.cpp
	${MPICC} -o parallel parallel.cpp

test:
	./test.sh ${NUMPROCS}

.PHONY: clean
clean:
	rm -f ${EXECS}
