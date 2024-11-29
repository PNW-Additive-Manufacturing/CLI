output: cJSON cTable program.o cJSON
	gcc program.o libs/ctable/bin/src/table.c.o libs/ctable/bin/src/vector.c.o libs/ctable/bin/src/string_util.c.o libs/ctable/bin/src/string_builder.c.o libs/cJSON/build/cygcjson-1.dll -o program.exe

program.o: program.c
	gcc -c program.c

# Compile libraries

cTable:
	cd ./libs/ctable && make


cJSON: 
	cd ./libs/cJSON && mkdir -p build && cd build && cmake .. -DENABLE_CJSON_TEST=Off && make install
# This requires that the generated cygcjson-1.dll is put into the root directory - we should change this.

# Functions

clean:
	cd ./libs/cJSON && make clean