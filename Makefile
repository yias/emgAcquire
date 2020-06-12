# 	makefile of the emgAcquire package
# 	
#	currently working only for the client
#
#	Developer:  Iason Batzianoulis
#	email:      iasonbatz@gmail.com
#	License:    GNU GPL v3


# define collors for printing to the console
redC := $(shell tput setaf 1)
greenC := $(shell tput setaf 2)
yellowC := $(shell tput setaf 3)
blueC := $(shell tput setaf 4)
boldT := $(shell tput bold)
reset:=$(shell tput sgr0)

# get the current path and export it
EMGACQUIRE_DIR = $(shell pwd)


# set output directory, where the executables will be built
OUTPUT_DIR = ./build
OUTPUT_OBJ_DIRx64 = $(OUTPUT_DIR)/obj/x64
OUTPUT_OBJ_DIRx86 = $(OUTPUT_DIR)/obj/x86
EXP_LIB_OUTPUT_DIRx64 = ./lib/linux/shared/x64
EXP_LIB_OUTPUT_DIRx86 = ./lib/linux/shared/x86
EXP_LIB_OUTPUT_STATIC_DIRx64 = ./lib/linux/static/x64
EXP_LIB_OUTPUT_STATIC_DIRx86 = ./lib/linux/static/x86

# set the compiler and its flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall 
CXX_x86_FLAG = -m32
CXX_x64_FLAG = -m64

# set include directories
INCLUDE_DIR = ./include ${CPP_DEPENDENCIES}/rapidjson/include ./socketStream/include
INCLUDE_PARAMS = $(foreach d, $(INCLUDE_DIR), -I $d)

# set the source directory
SRC_DIR = ./src
SOCKETSTREAM_SRC_DIR = ./socketStream/src
EXAMPLES_DIR = ./client_examples

# set objects to be created
_CLIENT_OBJS = emgAcquire.o acquireFilters.o socketStream.o jsonWrapper.o md5.o
CLIENT_OBJSx64 = $(patsubst %,$(OUTPUT_OBJ_DIRx64)/%,$(_CLIENT_OBJS))
CLIENT_OBJSx86 = $(patsubst %,$(OUTPUT_OBJ_DIRx86)/%,$(_CLIENT_OBJS))



# set the libraries needed for compiling
LIBS = -lpthread



test: makeOutDir clientExample

all: makeOutDir makeObjetsDir makeLibsDir clientExample generateObjectsx64 generateLibx64 generateStaticLibx64 generateObjectsx86 generateLibx86 generateStaticLibx86

libs: makeOutDir makeObjetsDir makeLibsDir generateObjectsx64 generateLibx64 generateStaticLibx64 generateObjectsx86 generateLibx86 generateStaticLibx86

libsx64 : makeObjetsDir generateObjectsx64 generateLibx64 generateStaticLibx64

libsx86: makeObjetsDir generateObjectsx86 generateLibx86 generateStaticLibx86

$(OUTPUT_OBJ_DIRx64)/emgAcquire.o: $(info $(greenC)$(boldT) --> generating emgAcquireClient object x64 $(yellowC)==>$(reset)) $(SRC_DIR)/emgAcquire.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x64_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

$(OUTPUT_OBJ_DIRx64)/acquireFilters.o: $(info $(greenC)$(boldT) --> generating acquireFilters object x64 $(yellowC)==>$(reset)) $(SRC_DIR)/acquireFilters.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x64_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

$(OUTPUT_OBJ_DIRx64)/socketStream.o: $(info $(greenC)$(boldT) --> generating socketStream object x64 $(yellowC)==>$(reset)) $(SOCKETSTREAM_SRC_DIR)/socketStream.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x64_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

$(OUTPUT_OBJ_DIRx64)/jsonWrapper.o: $(info $(greenC)$(boldT) --> generating jsonWrapper object x64 $(yellowC)==>$(reset)) $(SOCKETSTREAM_SRC_DIR)/jsonWrapper.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x64_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

$(OUTPUT_OBJ_DIRx64)/md5.o: $(info $(greenC)$(boldT) --> generating md5 object x64 $(yellowC)==>$(reset)) $(SOCKETSTREAM_SRC_DIR)/md5.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x64_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi


generateObjectsx64: $(info $(greenC)$(boldT) --> generating emgAcquire client objects x64 $(yellowC)==>$(reset)) $(CLIENT_OBJSx64)
	$(CXX) -c -o $^ $(CXXFLAGS) $(CXX_x64_FLAG) $(LIBS)
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

# generate shared libraries
generateLibx64:
	$(info $(greenC)$(boldT) --> generating emgAcquire client shared library x64 $(yellowC)==>$(reset))
	@$(CXX) $(CXXFLAGS) $(CXX_x64_FLAG) \
	-shared -o ${EXP_LIB_OUTPUT_DIRx64}/libemgAcquireClient.so \
	$(CLIENT_OBJSx64) \
	$(INCLUDE_PARAMS) \
	$(LIBS)
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi


$(OUTPUT_OBJ_DIRx86)/emgAcquire.o: $(info $(greenC)$(boldT) --> generating emgAcquireClient object x86 $(yellowC)==>$(reset)) $(SRC_DIR)/emgAcquire.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x86_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

$(OUTPUT_OBJ_DIRx86)/acquireFilters.o: $(info $(greenC)$(boldT) --> generating acquireFilters object x86 $(yellowC)==>$(reset)) $(SRC_DIR)/acquireFilters.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x86_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

$(OUTPUT_OBJ_DIRx86)/socketStream.o: $(info $(greenC)$(boldT) --> generating socketStream object x86 $(yellowC)==>$(reset)) $(SOCKETSTREAM_SRC_DIR)/socketStream.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x86_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

$(OUTPUT_OBJ_DIRx86)/jsonWrapper.o: $(info $(greenC)$(boldT) --> generating jsonWrapper object x86 $(yellowC)==>$(reset)) $(SOCKETSTREAM_SRC_DIR)/jsonWrapper.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x86_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

$(OUTPUT_OBJ_DIRx86)/md5.o: $(info $(greenC)$(boldT) --> generating md5 object x86 $(yellowC)==>$(reset)) $(SOCKETSTREAM_SRC_DIR)/md5.cpp 
	$(CXX) -c -o $@ $< $(INCLUDE_PARAMS) $(CXXFLAGS) $(CXX_x86_FLAG) -fpic
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi


generateObjectsx86: $(info $(greenC)$(boldT) --> generating emgAcquire client objects x86 $(yellowC)==>$(reset)) $(CLIENT_OBJSx86)
	$(CXX) -c -o $^ $(CXXFLAGS) $(CXX_x86_FLAG) $(LIBS)
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

# generate shared libraries
generateLibx86:
	$(info $(greenC)$(boldT) --> generating emgAcquire client shared library x86 $(yellowC)==>$(reset))
	@$(CXX) $(CXXFLAGS) $(CXX_x86_FLAG) \
	-shared -o ${EXP_LIB_OUTPUT_DIRx86}/libemgAcquireClient.so \
	$(CLIENT_OBJSx86) \
	$(INCLUDE_PARAMS) \
	$(LIBS)
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi

# build emgAcquire client
clientExample:
	$(info $(greenC)$(boldT) --> building emgAcquire client example $(yellowC)==>$(reset))
	@$(CXX) $(CXXFLAGS) $(EXAMPLES_DIR)/client_example.cpp \
	$(SRC_DIR)/emgAcquire.cpp $(SRC_DIR)/acquireFilters.cpp $(SOCKETSTREAM_SRC_DIR)/socketStream.cpp $(SOCKETSTREAM_SRC_DIR)/jsonWrapper.cpp $(SOCKETSTREAM_SRC_DIR)/md5.cpp \
	-o ${OUTPUT_DIR}/emgAcquireClient \
	$(INCLUDE_PARAMS) \
	$(LIBS)
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi


# generate static libraries
generateStaticLibx64:
	$(info $(greenC)$(boldT) --> generating emgAcquire client static library x64 $(yellowC)==>$(reset))
	@ar rcs $(EXP_LIB_OUTPUT_STATIC_DIRx64)/libemgAcquireClient.a $(CLIENT_OBJSx64)
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi


generateStaticLibx86:
	$(info $(greenC)$(boldT) --> generating emgAcquire client static library x86 $(yellowC)==>$(reset))
	@ar rcs $(EXP_LIB_OUTPUT_STATIC_DIRx86)/libemgAcquireClient.a $(CLIENT_OBJSx86)
	@if test $$? -eq 0;\
	then tput setaf 4; tput bold; echo " --> OK"; tput sgr0; \
	fi



# echo the path of the cpp dependencies
echoEvnVariable:
	@echo ${CPP_DEPENDENCIES}
	@echo $(PATH)
	@echo $(CLIENT_OBJS)


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


# create a sub-directory for the objects, if it doesn't exist
makeObjetsDir:
	@if test -d ${OUTPUT_OBJ_DIRx64}; \
	then \
		tput setaf 4; tput bold; printf " --> output object folder "${OUTPUT_OBJ_DIRx64}" alread exists"; tput setaf 3; printf " ==>\n";\
		tput sgr0;\
	else \
		tput setaf 2; printf " -->"; tput bold; printf " creating output object folder ${OUTPUT_OBJ_DIRx64} " ; tput setaf 3; printf " ==>\n"; tput sgr0;\
		mkdir -p ${OUTPUT_OBJ_DIRx64}; \
	fi
	@if test -d ${OUTPUT_OBJ_DIRx86}; \
	then \
		tput setaf 4; tput bold; printf " --> output object folder "${OUTPUT_OBJ_DIRx86}" alread exists"; tput setaf 3; printf " ==>\n";\
		tput sgr0;\
	else \
		tput setaf 2; printf " -->"; tput bold; printf " creating output object folder ${OUTPUT_OBJ_DIRx86} " ; tput setaf 3; printf " ==>\n"; tput sgr0;\
		mkdir -p ${OUTPUT_OBJ_DIRx86}; \
	fi


# create a directory for the library, if it doesn't exist
makeLibsDir:
	@if test -d ${EXP_LIB_OUTPUT_DIRx64}; \
	then \
		tput setaf 4; tput bold; printf " --> export libs directory "${EXP_LIB_OUTPUT_DIRx64}" alread exists"; tput setaf 3; printf " ==>\n";\
		tput sgr0;\
	else \
		tput setaf 2; printf " -->"; tput bold; printf " creating export libs directory ${EXP_LIB_OUTPUT_DIRx64} " ; tput setaf 3; printf " ==>\n"; tput sgr0;\
		mkdir -p ${EXP_LIB_OUTPUT_DIRx64}; \
	fi
	@if test -d ${EXP_LIB_OUTPUT_DIRx86}; \
	then \
		tput setaf 4; tput bold; printf " --> export libs directory "${EXP_LIB_OUTPUT_DIRx86}" alread exists"; tput setaf 3; printf " ==>\n";\
		tput sgr0;\
	else \
		tput setaf 2; printf " -->"; tput bold; printf " creating export libs directory ${EXP_LIB_OUTPUT_DIRx86} " ; tput setaf 3; printf " ==>\n"; tput sgr0;\
		mkdir -p ${EXP_LIB_OUTPUT_DIRx86}; \
	fi
	@if test -d ${EXP_LIB_OUTPUT_STATIC_DIRx64}; \
	then \
		tput setaf 4; tput bold; printf " --> export libs directory "${EXP_LIB_OUTPUT_STATIC_DIRx64}" alread exists"; tput setaf 3; printf " ==>\n";\
		tput sgr0;\
	else \
		tput setaf 2; printf " -->"; tput bold; printf " creating export libs directory ${EXP_LIB_OUTPUT_STATIC_DIRx64} " ; tput setaf 3; printf " ==>\n"; tput sgr0;\
		mkdir -p ${EXP_LIB_OUTPUT_STATIC_DIRx64}; \
	fi
	@if test -d ${EXP_LIB_OUTPUT_STATIC_DIRx86}; \
	then \
		tput setaf 4; tput bold; printf " --> export libs directory "${EXP_LIB_OUTPUT_STATIC_DIRx86}" alread exists"; tput setaf 3; printf " ==>\n";\
		tput sgr0;\
	else \
		tput setaf 2; printf " -->"; tput bold; printf " creating export libs directory ${EXP_LIB_OUTPUT_STATIC_DIRx86} " ; tput setaf 3; printf " ==>\n"; tput sgr0;\
		mkdir -p ${EXP_LIB_OUTPUT_STATIC_DIRx86}; \
	fi


.PHONY: clean

clean:
	@tput setaf 2; printf " -->"; tput bold; printf " cleaning up ${OUTPUT_DIR} folder\n"; tput sgr0;
	@rm -r ${OUTPUT_DIR}/*
	@tput setaf 4; printf " -->"; tput bold; printf " output folder ${OUTPUT_DIR} is empty\n"; tput sgr0;\

