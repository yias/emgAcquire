# 	makefile of the emgAcquire package
# 	
#	currently working only for the client
#
#	Developer:  Iason Batzianoulis
#	email:      iasonbatz@gmail.com
#	License:    MIT


# define collors for printing to the console
redC := $(shell tput setaf 1)
greenC := $(shell tput setaf 2)
yellowC := $(shell tput setaf 3)
blueC := $(shell tput setaf 4)
boldT := $(shell tput bold)
reset:=$(shell tput sgr0)

# set output directory, where the executables will be built
OUTPUT_DIR = ./build

# set the compiler and its flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall 

# set include directories
INCLUDE_DIR = ./include ${CPP_DEPENDENCIES}/rapidjson/include ./socketStream/include
INCLUDE_PARAMS = $(foreach d, $(INCLUDE_DIR), -I $d)

# set the source directory
SRC_DIR = ./src
SOCKETSTREAM_SRC_DIR = ./socketStream/src
EXAMPLES_DIR = ./client_examples

# set the libraries needed for compiling
LIBS = -lpthread


test: makeOutDir clientExample

all: makeOutDir clientExample

# build a socketStream server for listening to clients and retrieve the signals of a client
clientExample:
	$(info $(greenC)$(boldT) --> building emgAcquire client example $(yellowC)==>$(reset))
	@$(CXX) $(CXXFLAGS) $(EXAMPLES_DIR)/tmp_client_example.cpp \
	$(SRC_DIR)/emgAcquire.cpp $(SRC_DIR)/acquireFilters.cpp $(SOCKETSTREAM_SRC_DIR)/socketStream.cpp $(SOCKETSTREAM_SRC_DIR)/jsonWrapper.cpp $(SOCKETSTREAM_SRC_DIR)/md5.cpp \
	-o ${OUTPUT_DIR}/emgAcquireClient.exe \
	$(INCLUDE_PARAMS) \
	$(LIBS)
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi


# echo the path of the cpp dependencies
echoEvnVariable:
	@echo ${CPP_DEPENDENCIES}
	@echo $(INCLUDE_PARAMS)


# create a directory for the builts, if it doesn't exist
makeOutDir:
	@if test -d ${OUTPUT_DIR}; \
	then \
		tput setaf 4; tput bold; printf " --> output folder "${OUTPUT_DIR}" alread exists"; tput setaf 3; printf " ==>\n";\
		tput sgr0;\
	else \
		tput setaf 2; printf " -->"; tput bold; printf " creating output folder ${OUTPUT_DIR} " ; tput setaf 3; printf " ==>\n"; tput sgr0;\
		mkdir -p ${OUTPUT_DIR}; \
	fi


.PHONY: clean

clean:
	@tput setaf 2; printf " -->"; tput bold; printf " cleaning up ${OUTPUT_DIR} folder\n"; tput sgr0;
	@rm ${OUTPUT_DIR}/*
	@tput setaf 4; printf " -->"; tput bold; printf " output folder ${OUTPUT_DIR} is empty\n"; tput sgr0;\

