#ifndef _LOADER_H_
#define _LOADER_H_

#include "scenenode.h"
#include "factory.h"

NAMESPACE_BEGIN

enum LoaderFlags
{
  LOADER_FLAGS_NONE         = 0x00000000,
  LOADER_FLAGS_ALL          = 0xffffffff,
};

//forward declarations
class Light;

//load objects from a lua file
SceneNode *LoadLuaFile(cchar *szFolder,
                       cchar *szFile,
                       OUT Factory &sceneFactory,
                       OUT Factory &textureFactory);

//load light from a lua file
Light *LoadLuaLight(cchar *szFolder,
                    cchar *szFile,
                    OUT Factory &sceneFactory);

//load skybox from a skybox folder
SceneNode *LoadSkyBox(cchar *szFolder,
                      OUT Factory &sceneFactory,
                      OUT Factory &textureFactory);

NAMESPACE_END

#endif //_LOADER_H_
