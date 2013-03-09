#include "loader.h"
#include "algebra.h"
#include "groupnode.h"
#include "transform.h"
#include "primitive.h"
#include "material.h"
#include "image.h"
#include "texture.h"
#include "light.h"
#include "skybox.h"
#include "psystem.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

/*#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io_new/bmp_all.hpp>
#include <boost/gil/extension/io_new/png_all.hpp>
#include <boost/gil/extension/io_new/jpeg_all.hpp>
#include <boost/gil/extension/io_new/tiff_all.hpp>

#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::gil;*/

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_LOADER, "Loader",__FILE__);

//lua node data
struct LuaNodeData
{
  SceneNode *pNode;
};

//factory for object creation
static Factory *g_pSceneFactory = NULL;

//factory for texture creation
static Factory *g_pTextureFactory = NULL;

//function to get a tuple of numbers
template<typename T>
void LuaGetTuple(lua_State *L, int arg, T *pData, int size)
{
  luaL_checktype(L, arg, LUA_TTABLE);
  luaL_argcheck (L, luaL_getn(L, arg) == size, arg, "N-tuple expected");
  for (int i = 0; i < size; ++i)
  {
    lua_rawgeti(L, arg, i + 1);
    pData[i] = static_cast<T>(luaL_checknumber(L, -1));
    lua_pop(L, 1);
  }
}

extern "C"
int NewNodeCmd(lua_State *L)
{
  //get node name
  cchar *szName = luaL_checkstring(L, 1);

  DbgMessage("%s", szName);

  //get selectable node indicator, optional
  int selectable = luaL_optint(L, 2, 0);

  //create group node
  GroupNode *pNode = g_pSceneFactory->NewObject<GroupNode>();

  //set node name
  pNode->SetName(szName);

  //set selectable node indicator
  pNode->SetSelectable(selectable != 0);

  //add node to selectable node list if selectable
  g_pSceneFactory->AddSelectNode(pNode);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewTranslationCmd(lua_State *L)
{
  DbgEnter();

  //get translation vector
  Vector deltas;
  LuaGetTuple(L, 1, deltas, DIM_VECTOR - 1);
  deltas[CW] = 0.0f;

  //create translation node
  Translation *pNode = g_pSceneFactory->NewObject<Translation>();

  //set translation parameters
  pNode->SetDeltas(deltas);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewScalingCmd(lua_State *L)
{
  DbgEnter();

  //get scaling vector
  Vector scales;
  LuaGetTuple(L, 1, scales, DIM_VECTOR - 1);
  scales[CW] = 0.0f;

  //create scaling node
  Scaling *pNode = g_pSceneFactory->NewObject<Scaling>();

  //set scaling parameters
  pNode->SetScales(scales);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewRotationCmd(lua_State *L)
{
  DbgEnter();

  //get rotation axis
  Vector axis;
  LuaGetTuple(L, 1, axis, DIM_VECTOR - 1);
  axis[CW] = 0.0f;

  //get rotation angle
  float angle = luaL_checknumber(L, 2);

  //create rotation node
  Rotation *pNode = g_pSceneFactory->NewObject<Rotation>();

  //set rotation parameters
  pNode->SetAxis(axis);
  pNode->SetAngle(angle);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewSphereCmd(lua_State *L)
{
  DbgEnter();

  //create sphere node
  Sphere *pNode = g_pSceneFactory->NewObject<Sphere>();

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewSphereLodCmd(lua_State *L)
{
  DbgEnter();

  //get number of slices
  int slices = luaL_checkint(L, 1);
  luaL_argcheck(L, slices > 0, 1, "Invalid number of slices");

  //get number of stacks
  int stacks = luaL_checkint(L, 2);
  luaL_argcheck(L, stacks > 0, 2, "Invalid number of stacks");

  //create sphere node
  Sphere *pNode = g_pSceneFactory->NewObject<Sphere>();

  //set level of detail
  pNode->SetLod(slices, stacks);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewCylinderCmd(lua_State *L)
{
  DbgEnter();

  //create cylinder node
  Cylinder *pNode = g_pSceneFactory->NewObject<Cylinder>();

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewCylinderLodCmd(lua_State *L)
{
  DbgEnter();

  //get number of slices
  int slices = luaL_checkint(L, 1);
  luaL_argcheck(L, slices > 0, 1, "Invalid number of slices");

  //get number of stacks
  int stacks = luaL_checkint(L, 2);
  luaL_argcheck(L, stacks > 0, 2, "Invalid number of stacks");

  //create cylinder node
  Cylinder *pNode = g_pSceneFactory->NewObject<Cylinder>();

  //set level of detail
  pNode->SetLod(slices, stacks);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewConeCmd(lua_State *L)
{
  DbgEnter();

  //create cone node
  Cone *pNode = g_pSceneFactory->NewObject<Cone>();

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewConeLodCmd(lua_State *L)
{
  DbgEnter();

  //get number of slices
  int slices = luaL_checkint(L, 1);
  luaL_argcheck(L, slices > 0, 1, "Invalid number of slices");

  //get number of stacks
  int stacks = luaL_checkint(L, 2);
  luaL_argcheck(L, stacks > 0, 2, "Invalid number of stacks");

  //create cone node
  Cone *pNode = g_pSceneFactory->NewObject<Cone>();

  //set level of detail
  pNode->SetLod(slices, stacks);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewCubeCmd(lua_State *L)
{
  DbgEnter();

  //create cube node
  Cube *pNode = g_pSceneFactory->NewObject<Cube>();

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewCubeLodCmd(lua_State *L)
{
  DbgEnter();

  //get size in x-direction
  int sizeX = luaL_checkint(L, 1);
  luaL_argcheck(L, sizeX > 0, 1, "Invalid size in x-direction");

  //get size in y-direction
  int sizeY = luaL_checkint(L, 2);
  luaL_argcheck(L, sizeY > 0, 2, "Invalid size in y-direction");

  //create cube node
  Cube *pNode = g_pSceneFactory->NewObject<Cube>();

  //set level of detail
  pNode->SetLod(sizeX, sizeY);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewDiskCmd(lua_State *L)
{
  DbgEnter();

  //create cylinder node
  Disk *pNode = g_pSceneFactory->NewObject<Disk>();

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewDiskLodCmd(lua_State *L)
{
  DbgEnter();

  //get number of slices
  int slices = luaL_checkint(L, 1);
  luaL_argcheck(L, slices > 0, 1, "Invalid number of slices");

  //get number of loops
  int loops = luaL_checkint(L, 2);
  luaL_argcheck(L, loops > 0, 2, "Invalid number of loops");

  //create cylinder node
  Disk *pNode = g_pSceneFactory->NewObject<Disk>();

  //set level of detail
  pNode->SetLod(slices, loops);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewSquareCmd(lua_State *L)
{
  DbgEnter();

  //create square node
  Square *pNode = g_pSceneFactory->NewObject<Square>();

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewSquareLodCmd(lua_State *L)
{
  DbgEnter();

  //get size in x-direction
  int sizeX = luaL_checkint(L, 1);
  luaL_argcheck(L, sizeX > 0, 1, "Invalid size in x-direction");

  //get size in y-direction
  int sizeY = luaL_checkint(L, 2);
  luaL_argcheck(L, sizeY > 0, 2, "Invalid size in y-direction");

  //create square node
  Square *pNode = g_pSceneFactory->NewObject<Square>();

  //set level of detail
  pNode->SetLod(sizeX, sizeY);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewCircleCmd(lua_State *L)
{
  DbgEnter();

  //create cylinder node
  Circle *pNode = g_pSceneFactory->NewObject<Circle>();

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewCircleLodCmd(lua_State *L)
{
  DbgEnter();

  //get number of slices
  int slices = luaL_checkint(L, 1);
  luaL_argcheck(L, slices > 0, 1, "Invalid number of slices");

  //create cylinder node
  Circle *pNode = g_pSceneFactory->NewObject<Circle>();

  //set level of detail
  pNode->SetLod(slices);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = (SceneNode *)pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewMaterialCmd(lua_State *L)
{
  DbgEnter();

  //get diffuse color
  Color diffuse;
  LuaGetTuple(L, 1, diffuse, DIM_COLOR);

  //get specular color
  Color specular;
  LuaGetTuple(L, 2, specular, DIM_COLOR - 1);
  specular[CA] = 1.0f;

  //get shininess
  float shininess = luaL_checknumber(L, 3);

  //create material node
  Material *pNode = g_pSceneFactory->NewObject<Material>();

  //set material properties
  pNode->SetDiffuse(diffuse);
  pNode->SetSpecular(specular);
  pNode->SetShininess(shininess);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewTextureCmd(lua_State *L)
{
  DbgEnter();

  //get image file name
  cchar *szFile = luaL_checkstring(L, 1);

  //get repeat texture indicator
  int repeat = luaL_checkint(L, 2);

  //get mipmap texture indicator
  int mipmap = luaL_checkint(L, 3);

  DbgMessage("Creating texture from %s", szFile);

  //create image
  PngImage image;
  image.Image::Load(szFile);
  luaL_argcheck(L, image.IsValid(), 1, "Invalid image filename");

  //create texture
  Texture *pNode = g_pTextureFactory->NewObject<Texture>();
  if (mipmap != 0)
  {
    pNode->Create(&image, repeat != 0);
  }
  else
  {
    pNode->Create(&image, repeat != 0, false, GL_LINEAR, GL_LINEAR);
  }
  luaL_argcheck(L, pNode->IsValid(), 1, "Unable to create texture");

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewLightCmd(lua_State *L)
{
  DbgEnter();

  //get position
  Vector position;
  LuaGetTuple(L, 1, position, DIM_VECTOR);

  //get direction
  Vector direction;
  LuaGetTuple(L, 2, direction, DIM_VECTOR - 1);
  direction[CW] = 0.0f;

  //get diffuse coefficients
  Vector kd;
  LuaGetTuple(L, 3, kd, DIM_VECTOR - 1);
  kd[CW] = 0.0f;

  //get specular coefficients
  Vector ks;
  LuaGetTuple(L, 4, ks, DIM_VECTOR - 1);
  ks[CW] = 0.0f;

  //get spot exponent
  float exponent = luaL_checknumber(L, 5);

  //get spot cutoff
  float cutoff = luaL_checknumber(L, 6);

  //create light node
  Light *pNode = g_pSceneFactory->NewObject<Light>();

  //set light properties
  pNode->SetPosition(position);
  pNode->SetDirection(direction);
  pNode->SetDiffuseCoeff(kd);
  pNode->SetSpecularCoeff(ks);
  pNode->SetSpotExponent(exponent);
  pNode->SetSpotCutoff(cutoff);

  //enable light
  pNode->SetEnabled(true);

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int NewParticleSystemCmd(lua_State *L)
{
  DbgEnter();

  //get node data
  LuaNodeData *pDataParticle = (LuaNodeData *)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, pDataParticle != NULL, 1, "Node expected");

  //get particle node
  SceneNode *pParticle = pDataParticle->pNode;
  luaL_argcheck(L, pParticle != NULL, 1, "Node expected");

  //get particle color
  Color color;
  LuaGetTuple(L, 2, color, DIM_COLOR);

  //get velocity
  Vector velocity;
  LuaGetTuple(L, 3, velocity, DIM_VECTOR - 1);
  velocity[CW] = 0.0f;

  //get gravity
  Vector gravity;
  LuaGetTuple(L, 4, gravity, DIM_VECTOR - 1);
  gravity[CW] = 0.0f;

  //get number of particles
  int size = luaL_checkint(L, 5);
  luaL_argcheck(L, size > 0, 5, "Invalid number of particles");

  //get size reduction indicator
  int reduceSize = luaL_checkint(L, 6);

  //get alpha reduction indicator
  int reduceAlpha = luaL_checkint(L, 7);

  //get max particle size
  float maxSize = luaL_checknumber(L, 8);
  luaL_argcheck(L, maxSize > 0.0f, 7, "Invalid max particle size");

  //get particle spread value
  float spread = luaL_checknumber(L, 9);

  //get min particle fade rate
  float minFade = luaL_checknumber(L, 10);
  luaL_argcheck(L, minFade > 0.0f, 9, "Invalid min particle fade");

  //get max particle fade rate
  float maxFade = luaL_checknumber(L, 11);
  luaL_argcheck(L, maxFade > 0.0f, 10, "Invalid max particle fade");

  //create particle system node
  ParticleSystem *pNode = g_pSceneFactory->NewObject<ParticleSystem>();

  //set particle system properties
  if (!pNode->Create(pParticle, color, velocity, gravity, size,
    reduceSize != 0, reduceAlpha != 0, maxSize, spread, minFade, maxFade))
  {
    return luaL_error(L, "Unable to create particle system");
  }

  //create user data
  LuaNodeData *pData = (LuaNodeData *)lua_newuserdata(L, sizeof(LuaNodeData));

  //fill user data
  pData->pNode = pNode;
  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

extern "C"
int DeleteObjectCmd(lua_State *L)
{
  DbgEnter();

  //get node data
  LuaNodeData *pData = (LuaNodeData *)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, pData != NULL, 1, "Node expected");

  //delete node using factory
  if (!g_pSceneFactory->DeleteObject(pData->pNode) && g_pTextureFactory)
  {
    //delete node using texture factory
    g_pTextureFactory->DeleteObject(pData->pNode);
  }

  //clear node data
  pData->pNode = NULL;

  return 1;
}

extern "C"
int NodeAddChildCmd(lua_State *L)
{
  DbgEnter();

  //get parent node data
  LuaNodeData *pDataSelf = (LuaNodeData *)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, pDataSelf != NULL, 1, "Node expected");

  //get parent node
  GroupNode *pSelf = dynamic_cast<GroupNode *>(pDataSelf->pNode);
  luaL_argcheck(L, pSelf != NULL, 1, "Group node expected");

  //get child node data
  LuaNodeData *pDataChild = (LuaNodeData *)luaL_checkudata(L, 2, "gr.node");
  luaL_argcheck(L, pDataChild != NULL, 2, "Node expected");

  //add child node
  g_pSceneFactory->AddNode(pSelf, pDataChild->pNode);

  return 0;
}

extern "C"
int NodeGcCmd(lua_State *L)
{
  DbgEnter();

  //get node data
  LuaNodeData *pData = (LuaNodeData *)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, pData != NULL, 1, "Node expected");

  //clear node data without deleting the actual node
  pData->pNode = NULL;

  return 0;
}

//lua node functions
static const luaL_reg g_aLuaNodeFunctions[] =
{
  {"node",        NewNodeCmd           },
  {"translation", NewTranslationCmd    },
  {"scaling",     NewScalingCmd        },
  {"rotation",    NewRotationCmd       },
  {"sphere",      NewSphereCmd         },
  {"sphereLod",   NewSphereLodCmd      },
  {"cylinder",    NewCylinderCmd       },
  {"cylinderLod", NewCylinderLodCmd    },
  {"cone",        NewConeCmd           },
  {"coneLod",     NewConeLodCmd        },
  {"cube",        NewCubeCmd           },
  {"cubeLod",     NewCubeLodCmd        },
  {"disk",        NewDiskCmd           },
  {"diskLod",     NewDiskLodCmd        },
  {"square",      NewSquareCmd         },
  {"squareLod",   NewSquareLodCmd      },
  {"circle",      NewCircleCmd         },
  {"circleLod",   NewCircleLodCmd      },
  {"material",    NewMaterialCmd       },
  {"texture",     NewTextureCmd        },
  {"light",       NewLightCmd          },
  {"psystem",     NewParticleSystemCmd },
  {"delete",      DeleteObjectCmd      },
  {NULL,          NULL                 },
};

//lua node methods
static const luaL_reg g_aLuaNodeMethods[] =
{
  {"__gc",        NodeGcCmd            },
  {"addChild",    NodeAddChildCmd      },
  {NULL,          NULL                 },
};

bool ParseLuaFile(lua_State *L, cchar *szFile,
                  Factory *pSceneFactory, Factory *pTextureFactory)
{
  DbgMessage("Parsing %s", szFile);

  //initialize global factories
  g_pSceneFactory   = pSceneFactory;
  g_pTextureFactory = pTextureFactory ? pTextureFactory : pSceneFactory;

  //load lua libraries
  luaopen_base  (L);
  luaopen_io    (L);
  luaopen_string(L);
  luaopen_math  (L);

  //initialize node metatable
  luaL_newmetatable(L, "gr.node");
  lua_pushstring   (L, "__index");
  lua_pushvalue    (L, -2);
  lua_settable     (L, -3);

  //load node methods
  luaL_openlib(L, 0, g_aLuaNodeMethods, 0);

  //load node functions
  luaL_openlib(L, "gr", g_aLuaNodeFunctions, 0);

  //parse lua script
  if (luaL_loadfile(L, szFile) || lua_pcall(L, 0, 1, 0))
  {
    ShowWarning("%s", lua_tostring(L, -1));
    return false;
  }

  return true;
}

SceneNode *LoadLuaFile(cchar *szFolder, cchar *szFile,
                       Factory &sceneFactory, Factory &textureFactory)
{
  if (!szFolder || !szFile)
  {
    DbgError(STRING(ERR_BAD_INPUT));
    return NULL;
  }

  //form scene file name
  char szLuaFile [MAX_FILE_NAME_SIZE + 1];
  snprintf(szLuaFile, MAX_FILE_NAME_SIZE, "%s/%s", szFolder, szFile);
  szLuaFile[MAX_FILE_NAME_SIZE] = NULL_CHAR;

  //start lua interpreter
  lua_State *L = lua_open();

  if (!ParseLuaFile(L, szLuaFile, &sceneFactory, &textureFactory))
  {
    return NULL;
  }

  //get root node from lua stack
  LuaNodeData *pData = (LuaNodeData *)luaL_checkudata(L, -1, "gr.node");
  if (!pData)
  {
    ShowWarning("Script must return root node");
    return NULL;
  }

  //get root node
  SceneNode *pNode = pData->pNode;

  //close lua interpreter
  lua_close(L);

  //return root node
  return pNode;
}

Light *LoadLuaLight(cchar *szFolder, cchar *szFile, Factory &sceneFactory)
{
  if (!szFolder || !szFile)
  {
    DbgError(STRING(ERR_BAD_INPUT));
    return NULL;
  }

  //form scene file name
  char szLuaFile [MAX_FILE_NAME_SIZE + 1];
  snprintf(szLuaFile, MAX_FILE_NAME_SIZE, "%s/%s", szFolder, szFile);
  szLuaFile[MAX_FILE_NAME_SIZE] = NULL_CHAR;

  //start lua interpreter
  lua_State *L = lua_open();

  if (!ParseLuaFile(L, szLuaFile, &sceneFactory, NULL))
  {
    return NULL;
  }

  //get root node from lua stack
  LuaNodeData *pData = (LuaNodeData *)luaL_checkudata(L, -1, "gr.node");
  if (!pData)
  {
    ShowWarning("Script must return root node");
    return NULL;
  }

  //get light node
  Light *pNode = dynamic_cast<Light *>(pData->pNode);
  if (!pNode)
  {
    ShowWarning("Script must return light node");
    return NULL;
  }

  //close lua interpreter
  lua_close(L);

  //return root node
  return pNode;
}

SceneNode *LoadSkyBox(cchar *szFolder, Factory &sceneFactory, Factory &textureFactory)
{
  if (!szFolder)
  {
    DbgError(STRING(ERR_BAD_INPUT));
    return NULL;
  }

  PngImage image;
  char szFile [MAX_FILE_NAME_SIZE + 1];

  //create skybox
  SkyBox *pSkyBox = sceneFactory.NewObject<SkyBox>();
  Texture *pTexture = NULL;

  //create texture for each skybox face
  for (uint face = 0; face < NUM_CUBE_FACES; ++face)
  {
    //form image file name
    snprintf(szFile, MAX_FILE_NAME_SIZE, "%s/%d" DEFAULT_IMAGE_EXT, szFolder, face);
    szFile[MAX_FILE_NAME_SIZE] = NULL_CHAR;

    //load image
    DbgMessage("Loading skybox face %d from %s", face, szFile);
    if (!image.Image::Load(szFile))
    {
      ShowWarning("Unable to load %s", szFile);
      return NULL;
    }

    //create texture
    pTexture = textureFactory.NewObject<Texture>();
    if (!pTexture->Create(&image, false, false, GL_LINEAR, GL_LINEAR))
    {
      ShowWarning("Unable to create texture from %s", szFile);
      return NULL;
    }

    //set skybox face texture
    pSkyBox->SetTexture(face, pTexture);

    //destroy image
    image.Destroy();
  }

  return pSkyBox;
}

NAMESPACE_END
