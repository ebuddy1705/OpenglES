	
.PHONY: all, clean

TARGET	:= MainApp


INCDIR=
LIBS=-lGLESv2 -lEGL -lm -lz -lpng -l3ds
BINDIR=bin
APPNAME=MainApp
IMAGESDIR=images
PROJ_DIR=/carmeter/src

COMMONSRC=
          
COMMONHDR=

APPSOURCE=
           
APPSOURCEC=      

ifeq ('$(ARCH)', 'arm')
	CFLAGS = -DLINUX -DEGL_API_FB -DFREESCALE=1 -DDEBUG -D_DEBUG -D_GNU_SOURCE -DBUILDTARGET -mfloat-abi=softfp -mfpu=neon -fPIC -O3 -fno-strict-aliasing -fno-optimize-sibling-calls -Wall -g
	LDFLAGS = -L$(ROOTFS)/usr/lib 
	INCDIR += -I$(ROOTFS)/usr/include 			
	LIBS+= -lGAL -lstdc++
	INSTALLDIR:= $(ROOTFS)/usr/sbin
	INSTALL_IMAGES_DIR:= $(ROOTFS)/opt/centic
	CC=$(CROSS_COMPILE)gcc
	CXX=$(CROSS_COMPILE)g++
else
	CFLAGS = -DUSE_EGL_X11  -DDEBUG -D_DEBUG -D_GNU_SOURCE -fPIC -O3 -fno-strict-aliasing -fno-optimize-sibling-calls -Wall -g
	LDFLAGS = -L./lib
	INCDIR += -I./include	
	INSTALLDIR:= /usr/sbin
	IMAGESDIR:= /opt/centic

	LIBS+= -lX11 -lstdc++
	CC=gcc
	CXX=g++
endif



#-----------------------------------------------

CPP_SRCS	:= ./Cmodel3ds.cpp 
           
C_SRCS		:= 


				
C_OBJS		:= $(subst .c,.o,$(C_SRCS))
CPP_OBJS	:= $(subst .cpp,.o,$(CPP_SRCS))
OBJS		:= $(C_OBJS) $(CPP_OBJS) 


#-----------------------------------------------
all: $(TARGET)
	
%.o: %.cpp
	$(CXX) -c $(CFLAGS) ${INCDIR} $(LDFLAGS) ${LIBS} $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) ${INCDIR} $(LDFLAGS) ${LIBS} $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $^ $(CFLAGS) ${INCDIR} $(LDFLAGS) ${LIBS} -o $@
	chmod a+x $@
		
install: all
	cp -f $(TARGET) ../bin

clean:
	rm -f *.o
	rm -f $(TARGET)	




	
