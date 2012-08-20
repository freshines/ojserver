PROGRAM = server
OUTPUT_PATH = ../output
CC = g++
#CFLAGS = -Wall -O
CFLAGS = -O
INCLUDE = -I ../include
LIB = -lmysqlclient
SOURCES = $(wildcard *.c *.cpp)
CPP_OBJ = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))

all : output

$(PROGRAM) : $(CPP_OBJ)
	$(CC) -o $@ $^ $(LIB)
	
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)
 
output : $(PROGRAM)
	@if [ ! -d $(OUTPUT_PATH)/bin ]; then \
		mkdir -p $(OUTPUT_PATH)/bin; \
	fi
	cp $(PROGRAM) $(OUTPUT_PATH)/bin
	cp -r ../conf $(OUTPUT_PATH)
	
.PHONY : clean
clean:
	rm -rf $(CPP_OBJ)
	@if [ -d $(OUTPUT_PATH) ]; then \
		rm -rf $(OUTPUT_PATH); \
	fi 
