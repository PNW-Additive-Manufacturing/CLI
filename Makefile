CURRENT_DIR := $(CURDIR)

output: cJSON cTable program.o access.o ams.o
	gcc program.o access.o ams.o libs/cTable/bin/src/table.c.o libs/cTable/bin/src/vector.c.o libs/cTable/bin/src/string_util.c.o libs/cTable/bin/src/string_builder.c.o libs/cJSON/build/cygcjson-1.dll -lcurl -o cli.exe

program.o: program.c
	gcc -c program.c

access.o: access.c
	gcc -c access.c

ams.o: ams.c
	gcc -c ams.c

# Compile libraries

cTable:
	cd ./libs/cTable && make

cJSON:
	cd ./libs/cJSON && mkdir -p build && cd build && cmake .. -DENABLE_CJSON_TEST=Off && make install && cp $(CURRENT_DIR)/libs/cJSON/build/cygcjson-1.dll $(CURRENT_DIR)/cygcjson-1.dll 
# This requires that the generated cygcjson-1.dll is put into the root directory - we should change this.

# Functions

clean:
	rm -f ./ams.o && rm -f ./access.o && rm -f ./program.o && rm -f ./cygcjson-1.dll && rm -f ./cli.exe && rm -rf ./out/ && cd ./libs/cJSON && make clean