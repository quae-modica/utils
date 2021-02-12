CC = g++
OPTIONS = -Wall -Wextra -Werror -ggdb
INCLUDES = -lrt -pthread -Iinc/ -Lobj/

# make obj directory
$(shell mkdir -p obj)

# 
util-test: libutil.so obj/main.o
	${CC} ${OPTIONS} ${INCLUDES} obj/main.o -o $@

libutil.so: $(patsubst src/%.cpp,obj/%.o,$(wildcard src/*cpp))
	${CC} -shared ${OPTIONS} ${INCLUDES} -o $@

# build any source file into an object file
obj/main.o: src/main.cc
	${CC} ${OPTIONS} ${INCLUDES} -c $< -o $@
obj/%.o: src/%.cpp
	${CC} ${OPTIONS} ${INCLUDES} -c $< -o $@

clean:
	rm -rf obj util-test libutil.so

.PHONY: clean
