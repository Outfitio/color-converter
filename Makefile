SAMPNAME = ColorConverter

OUT_DIR = $(SAMPNAME)/bin
OBJ_DIR = $(SAMPNAME)/obj
SOURCE = $(SAMPNAME)/src

COMMON_OBJS = \
	$(OBJ_DIR)/$(SAMPNAME).o \
	$(OBJ_DIR)/PDFLInitCommon.o \
	$(OBJ_DIR)/PDFLInitHFT.o \
	$(OBJ_DIR)/InitializeLibrary.o \
	$(OBJ_DIR)/APDFLDoc.o

.NOTPARALLEL:	dummy

OS_ARCH = linux_amd64
BUILD_64_BIT = true

VENDOR = apdfl
INCLUDE = $(VENDOR)/CPlusPlus/Include/Headers
COMMON = $(VENDOR)/CPlusPlus/Sample_Source/_Common
BINARIES = $(VENDOR)/CPlusPlus/Binaries

LIBRARIES = $(OUT_DIR)/Binaries
RESOURCES = $(OUT_DIR)/Resources
RELEASE = $(OUT_DIR)/$(SAMPNAME)

PDFL_PATH = $(LIBRARIES)

CC = /usr/bin/gcc
CXX = /usr/bin/g++

ifeq ($(STAGE), debug)
    DEBUG=-DDEBUG -D_DEBUG
else
    DEBUG=-DNDEBUG
endif

ifeq ($(BUILD_64_BIT), true)
    ARCH_FLAGS = -m64
else
    ARCH_FLAGS = -m32
endif

CCFLAGS  = $(ARCH_FLAGS) -g $(PDF_FDIR_DEF) -DNO_PRAGMA_ONCE -DUNIX_PLATFORM=1 -DUNIX_ENV=1 -DPRODUCT=\"HFTLibrary.h\" $(DEBUG) -D_REENTRANT -Wno-multichar -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT

CXXFLAGS = $(CCFLAGS)

LDFLAGS = $(ARCH_FLAGS) -L$(PDFL_PATH)
LIBS = -lDL180pdfl -lDL180CoolType -lDL180AGM -lDL180BIB -lDL180ACE -lDL180ARE \
	   -lDL180BIBUtils -lDL180JP2K -lDL180AdobeXMP -lDL180AXE8SharedExpat \
	   -licuuc -licudata -lpthread


CPPFLAGS = -I. -I$(INCLUDE) -I$(COMMON)
CFLAGS = $(CCFLAGS)

default: $(OBJ_DIR) $(OUT_DIR) $(LIBRARIES) $(RESOURCES) $(RELEASE)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(LIBRARIES):
	rsync -ruPL $(BINARIES)/* $@

$(RESOURCES):
	rsync -ruPL $(VENDOR)/Resources/* $@

$(RELEASE): $(COMMON_OBJS) $(OTHER_OBJS)
	$(CXX) -o $@ $(COMMON_OBJS) $(OTHER_OBJS) $(LDFLAGS) $(LIBS) $(EXTRA_LIBS)

$(OBJ_DIR)/PDFLInitCommon.o: $(VENDOR)/CPlusPlus/Include/Source/PDFLInitCommon.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/PDFLInitHFT.o: $(VENDOR)/CPlusPlus/Include/Source/PDFLInitHFT.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/InitializeLibrary.o: $(COMMON)/InitializeLibrary.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/APDFLDoc.o: $(COMMON)/APDFLDoc.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR) $(OUT_DIR)

$(OBJ_DIR)/$(SAMPNAME).o: $(SOURCE)/main.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -std=gnu++11 -c $< -o $@