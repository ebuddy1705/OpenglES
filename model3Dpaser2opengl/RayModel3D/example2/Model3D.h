#ifndef MODEL3D_H
#define MODEL3D_H

#ifdef __cplusplus
extern "C" {
#endif




#include "./myObj/cube/cube.h"
#include "./myObj/starship/starship.h"
#include "./myObj/mushroom/mushroom.h"








typedef enum{
    CUBE_OBJ,
    STARSHIP_OBJ,
    MUSHROOM_OBJ
}myObj;


#ifdef __cplusplus
}//extern "C" {
#endif

#endif //MODEL3D_H
