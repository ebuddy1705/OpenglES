# Straight forward Makefile to compile all examples in a row

INCDIR=-I./Common
LIBS=-lGLESv2 -lEGL -lm -lX11 -lpng -lpthread



# new add
CFLAGS = -DUSE_EGL_X11  -DDEBUG -D_DEBUG -D_GNU_SOURCE -fPIC -O3 -fno-strict-aliasing -fno-optimize-sibling-calls -Wall -g	

LDFLAGS = -L/usr/lib \
		  -L/usr/lib64 \
		  -L/usr/local/lib
			
INCDIR += -I/usr/include \
		  -I/usr/local/include
		  						
LIBS+= -lstdc++ 
	
CC=gcc
CXX=g++
	
	
	

COMMONSRC=./Common/esShader.c    \
          ./Common/esTransform.c \
          ./Common/esShapes.c    \
          ./Common/esUtil.c
COMMONHDR-=esUtil.h

CH02SRC=./Chapter_2/Hello_Triangle/Hello_Triangle.c
CH08SRC=./Chapter_8/Simple_VertexShader/Simple_VertexShader.c
CH09SRC1=./Chapter_9/Simple_Texture2D/Simple_Texture2D.c
CH09SRC2=./Chapter_9/MipMap2D/MipMap2D.c
CH09SRC3=./Chapter_9/Simple_TextureCubemap/Simple_TextureCubemap.c
CH09SRC4=./Chapter_9/TextureWrap/TextureWrap.c
CH10SRC=./Chapter_10/MultiTexture/MultiTexture.c
CH11SRC=./Chapter_11/Multisample/Multisample.c
CH11SRC2=./Chapter_11/Stencil_Test/Stencil_Test.c
CH13SRC2=./Chapter_13/ParticleSystem/ParticleSystem.c

CH16SRC1=./Chapter_16/triangles_fan/triangles_fan.c
CH16SRC2=./Chapter_16/triangles_strip/triangles_strip.c
CH16SRC3=./Chapter_16/yuv_render/yuv_render.c


default: all

all: ./Chapter_2/Hello_Triangle/CH02_HelloTriangle \
     ./Chapter_8/Simple_VertexShader/CH08_SimpleVertexShader \
     ./Chapter_9/Simple_Texture2D/CH09_SimpleTexture2D \
     ./Chapter_9/MipMap2D/CH09_MipMap2D \
     ./Chapter_9/Simple_TextureCubemap/CH09_TextureCubemap \
     ./Chapter_9/TextureWrap/CH09_TextureWrap \
     ./Chapter_10/MultiTexture/CH10_MultiTexture \
     ./Chapter_11/Multisample/CH11_Multisample \
     ./Chapter_11/Stencil_Test/CH11_Stencil_Test \
     ./Chapter_13/ParticleSystem/CH13_ParticleSystem \
     ./Chapter_16/triangles_fan/CH16_triangles_fan \
	 ./Chapter_16/triangles_strip/CH16_triangles_strip
     
clean:
	find . -name "CH??_*" | xargs rm -f

./Chapter_2/Hello_Triangle/CH02_HelloTriangle: ${COMMONSRC} ${CH02SRC}
	gcc ${COMMONSRC} ${CH02SRC} -o $@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS} 
./Chapter_8/Simple_VertexShader/CH08_SimpleVertexShader: ${COMMONSRC} ${CH08SRC}
	gcc ${COMMONSRC} ${CH08SRC} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
./Chapter_9/Simple_Texture2D/CH09_SimpleTexture2D: ${COMMONSRC}  ${CH09SRC1}
	gcc ${COMMONSRC} ${CH09SRC1} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
./Chapter_9/MipMap2D/CH09_MipMap2D: ${COMMONSRC} ${CH09SRC2}
	gcc ${COMMONSRC} ${CH09SRC2} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
./Chapter_9/Simple_TextureCubemap/CH09_TextureCubemap: ${COMMONSRC} ${CH09SRC3}
	gcc ${COMMONSRC} ${CH09SRC3} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
./Chapter_9/TextureWrap/CH09_TextureWrap: ${COMMONSRC} ${CH09SRC4}
	gcc ${COMMONSRC} ${CH09SRC4} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
./Chapter_10/MultiTexture/CH10_MultiTexture: ${COMMONSRC} ${CH10SRC}
	gcc ${COMMONSRC} ${CH10SRC} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
./Chapter_11/Multisample/CH11_Multisample: ${COMMONSRC} ${CH11SRC}
	gcc ${COMMONSRC} ${CH11SRC} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
./Chapter_11/Stencil_Test/CH11_Stencil_Test: ${COMMONSRC} ${CH11SRC2}
	gcc ${COMMONSRC} ${CH11SRC2} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
./Chapter_13/Noise3D/CH13_Noise3D: ${COMMONSRC} ${CH13SRC1}
	gcc ${COMMONSRC} ${CH13SRC1} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
./Chapter_13/ParticleSystem/CH13_ParticleSystem: ${COMMONSRC} ${CH13SRC2}
	gcc ${COMMONSRC} ${CH13SRC2} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}	
./Chapter_16/triangles_fan/CH16_triangles_fan: ${COMMONSRC} ${CH16SRC1}
	gcc ${COMMONSRC} ${CH16SRC1} -o ./$@ ${INCDIR} $(CFLAGS) $(LDFLAGS)  ${LIBS}	
./Chapter_16/triangles_strip/CH16_triangles_strip: ${COMMONSRC} ${CH16SRC2}
	gcc ${COMMONSRC} ${CH16SRC2} -o ./$@ $(CFLAGS) $(LDFLAGS) ${INCDIR} ${LIBS}
	
	
	
	
	
	
