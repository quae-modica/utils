CC = g++
OPTIONS = -Wall -Wextra -ggdb
INCLUDES = -lrt -pthread -Iinc/

test: src/Test.cpp
	${CC} ${OPTIONS} ${INCLUDES} $?

log.o: src/Log.cpp
	${CC} ${OPTIONS} ${INCLUDES} -c $? -o $@

time.o: src/Time.cpp
	${CC} ${OPTIONS} ${INCLUDES} -c $? -o $@

properties.o: src/Properties.cpp
	${CC} ${OPTIONS} ${INCLUDES} -c $? -o $@

socket.o: src/Socket.cpp
	${CC} ${OPTIONS} ${INCLUDES} -c $? -o $@

library: log.o time.o properties.o socket.o
	ar rcs libutil.a $?

clean:
	rm -rf *.o a.out