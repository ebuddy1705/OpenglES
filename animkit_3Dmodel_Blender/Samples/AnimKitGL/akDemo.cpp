/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Xavier T.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/

#include "akDemo.h"

#include "akBLoader.h"
#include "akEntity.h"

#include "akMesh.h"
#include "akSkeletonPose.h"
#include "akGeometryDeformer.h"
#include "akAnimationPlayerSet.h"
#include <Debug.h>

#if defined __APPLE__ && defined OPENGL_ES_2_0
#include "Pathes.h"
#endif

akDemo::akDemo() : akDemoBase()
{

}

akDemo::~akDemo()
{
	
}


extern char *blendFile;
void akDemo::init(void)
{
	akBLoader loader(this);
#ifdef QT_BUILD
    DBG("akDemo::init START \n");
    loader.loadFile(blendFile, false, true);
#else
    #if defined __APPLE__ && defined OPENGL_ES_2_0
		char absolutePath[512], bundleDir[512];
		GetResourcePathASCII(bundleDir,512);
		sprintf(absolutePath,"%s/%s",bundleDir,"Blu.blend");
		loader.loadFile(absolutePath, false, true);	
    #else
		loader.loadFile("Blu.blend", false, true);
    #endif
#endif

}


void akDemo::setup(void)
{

    akVector3 trans = akVector3(0.0f, 0.0f, 0.0f);
    akQuat    rot = akQuat::identity();
    akVector3 scale = akVector3(1.0f,1.0f,1.0f);

    rot.rotZ_GLES(0.65f);
    rot.rotX_GLES(-0.2f);
    trans.transY_GLES(-1.0f);
    trans.transX_GLES(0.4f);
    scale.scaleY_GLES(0.3f);


    //        addEntity: Blu
    //        addSubMesh
    //        addSubMesh
    //        addTexture: untitled2
    //        addMesh: Sphere.002
    //        addSkeleton: Armature

    //        addEntity: Plane
    //        addSubMesh
    //        addTexture: untitled
    //        addMesh: Plane

    // Set some animation data the loader cannot detect
    akEntity* square = getEntity("Plane");
#if 1
    if(square){
        square->setPositionAnimated(false);

        if(!square->isPositionAnimated()){
            akTransformState *bluTransform = square->getTransformPtr();
            DBG("scale.x: %f, scale.y: %f, scale.z: %f \n", bluTransform->scale.getX(), bluTransform->scale.getY(), bluTransform->scale.getZ());
            DBG("rot.x: %f, rot.y: %f, rot.z: %f \n", bluTransform->rot.getX(), bluTransform->rot.getY(), bluTransform->rot.getZ());
            DBG("loc.x: %f, loc.y: %f, loc.z: %f \n", bluTransform->loc.getX(), bluTransform->loc.getY(), bluTransform->loc.getZ());

            bluTransform->loc = trans;
            bluTransform->rot = rot;
            bluTransform->scale = scale;
        }

    }
#else
    if(square)
    {
        akAnimationPlayer* play = square->getAnimationPlayers()->getAnimationPlayer(0);
        if(play){
            play->setEnabled(false);
            DBG("Plane  akAnimationPlayer set false \n");
        }
    }
#endif



    // Join the morph action and the rest action together.
    //    addAnimation name: ArmatureAction
    //    addAnimation name: GroundAction
    //    addAnimation name: KeyAction
    //    addAnimation name: Rest
    akAnimationClip* bluc = getAnimation("Rest");
    akAnimationClip* morphc = getAnimation("KeyAction");
    if(bluc && morphc)
    {
        int blucNum = bluc->getNumChannels();
        int morphcNum = morphc->getNumChannels();
        DBG("blucNum: %d, morphcNum: %d \n", blucNum, morphcNum);


        akAnimationChannel* chan = morphc->getChannel("Key 1");
        if(chan)
        {
            morphc->removeChannel(chan);
            bluc->addChannel(chan);
        }
    }


#if 0
    // Joint mask test
    akEntity* blu = getEntity("Blu");
    if(blu)
    {
        int numPlay = blu->getAnimationPlayers()->getNumAnimationPlayers();
        DBG("getNumAnimationPlayers = %d \n", numPlay);// = 1

        akAnimationPlayer* play = blu->getAnimationPlayers()->getAnimationPlayer(0);
        akSkeleton* skel = blu->getSkeleton();
        if(play && skel)
        {
            UTuint32 bid = skel->getIndex("Arm.R");
            if(bid>0)
            {
                play->createJointMask(skel);
                play->setJointMaskWeight(bid, 0.2f);
                DBG("play set Joint OK \n");
            }
        }
    }
#else
    akEntity* blu = getEntity("Blu");
    if(blu)
    {
//        akAnimationPlayer* play = blu->getAnimationPlayers()->getAnimationPlayer(0);
//        play->setEnabled(false);


        if(!blu->isPositionAnimated()){
            akTransformState *bluTransform = blu->getTransformPtr();
            DBG("scale.x: %f, scale.y: %f, scale.z: %f \n", bluTransform->scale.getX(), bluTransform->scale.getY(), bluTransform->scale.getZ());
            DBG("rot.x: %f, rot.y: %f, rot.z: %f \n", bluTransform->rot.getX(), bluTransform->rot.getY(), bluTransform->rot.getZ());
            DBG("loc.x: %f, loc.y: %f, loc.z: %f \n", bluTransform->loc.getX(), bluTransform->loc.getY(), bluTransform->loc.getZ());

            bluTransform->loc = trans;
            bluTransform->rot = rot;
            bluTransform->scale = scale;
        }
    }
#endif

#if 0
    if(m_camera){
        m_camera->m_transform.loc.transX_GLES(0.5f);
    }
#endif
}


