CPPSRCS+=  ./Source/akAnimationChannel.cpp \
            ./Source/akAnimationClip.cpp \
            ./Source/akAnimationCurve.cpp \
            ./Source/akAnimationPlayer.cpp \
            ./Source/akAnimationPlayerSet.cpp \
            ./Source/akBufferInfo.cpp \
            ./Source/akDualQuat.cpp \
            ./Source/akGeometryDeformer.cpp \
            ./Source/akMathUtils.cpp \
            ./Source/akMesh.cpp \
            ./Source/akMorphTarget.cpp \
            ./Source/akPose.cpp \
            ./Source/akPoseBlender.cpp \
            ./Source/akSkeleton.cpp \
            ./Source/akSkeletonPose.cpp \
            ./Source/akTransitionBlender.cpp \
            ./Source/akVertexGroup.cpp \
            ./Source/btAlignedAllocator.cpp
CSRCS+= 

HDRS+= $(wildcard ./Source/*.h)
