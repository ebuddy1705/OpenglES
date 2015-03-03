CPPSRCS+=   ./GLES20/Math/Vector.cpp \
            ./GLES20/Math/Transform.cpp \
            ./GLES20/Math/Random.cpp \
            ./GLES20/Math/Matrix.cpp \
            ./GLES20/Math/neonmath/neon_matrix_impl.cpp \
            ./GLES20/Math/vfpmath/utility_impl.cpp \
            ./GLES20/Math/vfpmath/matrix_impl.cpp
CSRCS+= 

		
HDRS+= $(wildcard ./GLES20/Math/*.h)
HDRS+= $(wildcard ./GLES20/Math/neonmath/*.h)
HDRS+= $(wildcard ./GLES20/Math/vfpmath/*.h)
