CPPSRCS+=   ./GLES20/Renderer/Core/GraphicsDevice/Shader.cpp \
            ./GLES20/Renderer/Resource/ResourceFile.cpp \
            ./GLES20/Renderer/Core/UI/DisplayText.cpp \
            ./GLES20/Renderer/Core/UI/DisplayTextAPI.cpp \
            ./GLES20/Renderer/piper_p.cpp \
            ./GLES20/Renderer/piperGL20.cpp \
            ./GLES20/Renderer/piperGL11.cpp \
            ./GLES20/Renderer/shaderData.cpp
            
CSRCS+= 

		
HDRS+= $(wildcard ./GLES20/Renderer/*.h)
HDRS+= $(wildcard ./GLES20/Renderer/Core/UI/*.h)
HDRS+= $(wildcard ./GLES20/Renderer/Core/GraphicsDevice/*.h)
