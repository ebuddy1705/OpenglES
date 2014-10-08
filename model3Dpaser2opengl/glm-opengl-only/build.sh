cd ./glm-0.3.1/examples
gcc gltb.c smooth.c -o smooth -lGL -lGLU -lglut -lglm -lX11 -lm -lpng -ljpeg -lz

smooth model3d.obj  (include model3d.obj, model3d.mtl and image texture in png format)
