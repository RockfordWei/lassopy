OS = $(shell uname)

TARGET = lassopy
OBJS = lassopy.o

#ARCH_X86_64 = -arch x86_64

#OSX_DEPLOY_VERS = 10.5

#SDK = /Developer/SDKs/MacOSX$(OSX_DEPLOY_VERS).sdk

Linux_SONAME = so
#Darwin_SONAME = dylib

#Darwin_CFLAGS += $(ARCH_X86_64) -F/Library/Frameworks -I/usr/local/include -dynamic -DMACHINE_MACOSX=1 -D_PTHREADS -isysroot $(SDK) -mmacosx-version-min=$(OSX_DEPLOY_VERS)
#Darwin_LFLAGS += $(ARCH_X86_64) -L/usr/local/lib -flat_namespace -bundle -F/Library/Frameworks -framework Lasso9 -isysroot $(SDK) -mmacosx-version-min=$(OSX_DEPLOY_VERS)
#Darwin_LIBS += 
#Darwin_LIBSUFFIX = dylib
#Darwin_LN = c++

Linux_CFLAGS += -Wno-multichar -DMACHINE_LINUX=1 -I/usr/include/Lasso9 -I/usr/include/python3.6m
Linux_LFLAGS += -shared -Wl,-Bsymbolic
Linux_LIBS += -llasso9_runtime -lpython3.6m -lstdc++
Linux_LIBSUFFIX = so
Linux_LN = g++

DEBUG = -O3 -DNDEBUG
INCLUDES += -I. 
CXXFLAGS += -fPIC $(DEBUG) $(CFAT) $($(OS)_CFLAGS) -Wno-deprecated -fshort-wchar
LIBS += $($(OS)_LIBS) #-licui18n -licuuc -licudata
LFLAGS += $(LFAT) $($(OS)_LFLAGS) $(LIBS)


all: $(TARGET).$($(OS)_LIBSUFFIX)

$(TARGET).$($(OS)_LIBSUFFIX): $(OBJS)
	$(Linux_LN) $(DEBUG) $(OBJS) -o $@ $(LFLAGS)

clean:
	- rm *.o *.so *.dylib core *~ $(OBJ_DIR)/*.o *.a > /dev/null 2>&1

install:
	- cp *.so /var/lasso/home/LassoModules 