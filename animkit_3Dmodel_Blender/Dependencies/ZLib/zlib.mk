CPPSRCS+=
CSRCS+= ./Dependencies/ZLib/adler32.c \
        ./Dependencies/ZLib/compress.c \
        ./Dependencies/ZLib/crc32.c \
        ./Dependencies/ZLib/deflate.c \
        ./Dependencies/ZLib/gzio.c \
        ./Dependencies/ZLib/infback.c \
        ./Dependencies/ZLib/inffast.c \
        ./Dependencies/ZLib/inflate.c \
        ./Dependencies/ZLib/inftrees.c \
        ./Dependencies/ZLib/trees.c \
        ./Dependencies/ZLib/uncompr.c \
        ./Dependencies/ZLib/zutil.c

HDRS+= $(wildcard ./Dependencies/ZLib/*.h)
		
