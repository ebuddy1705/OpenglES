
APPNAME=AppAnimkit
IMAGESDIR= ./data

INCDIR= -I./Source \
		-I./Dependencies/Utils \
		-I./Dependencies/vectormath \
		-I./Dependencies/FileTools/FileFormats/Blend \
		-I./Dependencies/FileTools/File \
		-I./GLES20/Include \
		-I./GLES20/Renderer \
		-I./GLES20/Renderer/Resource \
		-I./GLES20/Renderer/Core/GraphicsDevice \
		-I./GLES20/Renderer/Core/UI \
		-I./GLES20/Math \
		-I./GLES20/Math/neonmath \
		-I./GLES20/Math/vfpmath \
		-I./Dependencies/ZLib \
		-I./Samples \
		-I./Samples/AnimKitGL \
		-I./Samples/BlenderLoader \
		-I./GLES20/Utility/DeviceType \
		-I./GLES20/Utility/MemoryManager \
		-I./GLES20/Utility/Pathes \
		-I./GLES20/Utility/Khronos
		
-include ./Source/source.mk
-include ./Dependencies/ZLib/zlib.mk
-include ./Dependencies/Utils/utils.mk
-include ./Dependencies/FileTools/FileFormats/Blend/blend.mk
-include ./Dependencies/FileTools/File/file.mk
-include ./Samples/BlenderLoader/blenderloader.mk
-include ./Samples/AnimKitGL/animkitgl.mk
-include ./Samples/samples.mk
-include ./GLES20/Math/math.mk
-include ./GLES20/Renderer/renderer.mk
-include ./GLES20/Utility/Pathes/pathes.mk
-include ./GLES20/Utility/MemoryManager/memorymanager.mk
-include ./GLES20/Utility/Khronos/khronos.mk



CPPSRCS+= ./GLES20/main.cpp
CSRCS+=


ifeq ('$(ARCH)', 'arm')
BINDIR=./build/bin-arm
OBJSDIR:=./build/objs-arm
else
BINDIR=./build/bin
OBJSDIR:=./build/objs
endif

LIBS=-lGLESv2 -lEGL -lm -lz -lpng 

OBJS:= $(patsubst %.cpp, $(OBJSDIR)/%.o, $(CPPSRCS))
OBJS+= $(patsubst %.c, $(OBJSDIR)/%.o, $(CSRCS))

ifeq ('$(ARCH)', 'arm')
	CFLAGS += -DLINUX -DEGL_API_FB -DFREESCALE=1 -DDEBUG -D_DEBUG -D_GNU_SOURCE  -mfloat-abi=softfp -mfpu=neon -march=armv7-a -fPIC -O3 -fno-strict-aliasing -fno-optimize-sibling-calls  -g 
	CFLAGS += -DQT_BUILD -DOPENGL_ES_2_0 -DFBT_USE_GZ_FILE=1
	
	LDFLAGS += -L$(ROOTFS)/usr/lib -mfloat-abi=softfp -mfpu=neon -march=armv7-a
	INCDIR += -I$(ROOTFS)/usr/include -I$(ROOTFS)/usr/include/freetype2
	LIBS+= -lGAL 
	INSTALLDIR:= $(ROOTFS)/usr/sbin
	INSTALL_IMAGES_DIR:= $(ROOTFS)/opt/carmeter	
	CC=$(CROSS_COMPILE)gcc
	CXX=$(CROSS_COMPILE)g++
else
	CFLAGS += -DUSE_EGL_X11 -DDEBUG -D_DEBUG -D_GNU_SOURCE -fPIC -O3 -fno-strict-aliasing -fno-optimize-sibling-calls -Wall -g
	CFLAGS += -DQT_BUILD -DOPENGL_ES_2_0 -DFBT_USE_GZ_FILE=1

	INCDIR += -I/usr/include -I/usr/include/freetype2
	INSTALLDIR:= /usr/sbin
	INSTALL_IMAGES_DIR:= /opt/carmeter
	LIBS+= -lX11
	CC=gcc
	CXX=g++
endif



# -include rules.mk
default: all
all: $(BINDIR)/$(APPNAME)

clean: 
	@echo "rm -rf $(BINDIR)/$(APPNAME) $(OBJS)"
	@rm -rf $(BINDIR)/$(APPNAME) $(OBJS)
cleanall:
	@echo "rm -rf ./build/bin* ./build/objs*"
	@rm -rf ./build/bin* ./build/objs*

$(OBJSDIR)/%.o: ./%.c $(HDRS)
	@echo " [CC]   $@"
	@mkdir -p $(shell dirname $@)
	@$(CC) -c $< -o $@ $(CFLAGS) ${INCDIR}

$(OBJSDIR)/%.o: ./%.cpp $(HDRS)
	@echo " [CXX]  $@"
	@mkdir -p $(shell dirname $@)
	@$(CXX) -c $< -o $@ $(CFLAGS) ${INCDIR} -std=gnu++0x
	
$(BINDIR)/$(APPNAME) : $(OBJS)
	@echo " [LINK] $@"
	@mkdir -p $(shell dirname $@)
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS) ${LIBS}
	
install: all
	cp -f $(BINDIR)/$(APPNAME) $(INSTALLDIR)
	mkdir -p $(INSTALL_IMAGES_DIR)
	cp -rf $(IMAGESDIR) $(INSTALL_IMAGES_DIR)
	
doc:
	doxygen carmeter.doxygen
		
save: cleanall
	@mkdir -p ../history/$(shell date +'%Y-%m-%d')
	@tar zcf  ../history/$(shell date +'%Y-%m-%d')/animkit-$(shell date +'%Y-%m-%d-%H-%M-%S').tar.gz ../$(shell basename $(PWD))

ifneq ('$(DEST)', '')
copyto:	
		cp -rf ./common ./controller ./model ./view ./widgets  ./*.cpp  ./data  ./Makefile ./rules.mk $(DEST)/
else
copyto:
		@echo "set DEST for destination directory"
endif
ifneq ('$(SOURCE)', '')
copyfrom:
	cp -rf $(SOURCE)/common $(SOURCE)/controller $(SOURCE)/model $(SOURCE)/view $(SOURCE)/widgets $(SOURCE)/*.cpp $(SOURCE)/data $(SOURCE)/rules.mk $(SOURCE)/Makefile ./
else
copyfrom:
	@echo "set SOURCE for source directory"
endif

run:
	@$(BINDIR)/$(APPNAME)

