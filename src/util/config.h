#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "common.h"
#include "default.h"
#include "point.h"

NAMESPACE_BEGIN

//configuration field names
#define CONFIG_FIELD_NAME_WIDTH       "width"
#define CONFIG_FIELD_NAME_HEIGHT      "height"
#define CONFIG_FIELD_NAME_FULLSCREEN  "fullscreen"
#define CONFIG_FIELD_NAME_X           "x"
#define CONFIG_FIELD_NAME_Y           "y"
#define CONFIG_FIELD_NAME_Z           "z"
#define CONFIG_FIELD_NAME_ANGLE_X     "angle_x"
#define CONFIG_FIELD_NAME_ANGLE_Y     "angle_y"
#define CONFIG_FIELD_NAME_SKIN        "skin"
#define CONFIG_FIELD_NAME_SCENE       "scene"
#define CONFIG_FIELD_NAME_MODEL       "model"
#define CONFIG_FIELD_NAME_LIGHT       "light"
#define CONFIG_FIELD_NAME_SKYBOX      "skybox"
#define CONFIG_FIELD_NAME_IMAGE       "image"

//configuration flags
enum ConfigFlags
{
  CONFIG_FLAGS_NONE         = 0x00000000,
  CONFIG_FLAGS_ALL          = 0xffffffff,
  CONFIG_FLAGS_FULLSCREEN   = 0x00000001,
};

//configuration
struct Config
{
  uint width;
  uint height;
  ulong flags;
  PointUnion(pos);
  float angleX;
  float angleY;
  char szConfigFile  [MAX_FILE_NAME_SIZE + 1];
  char szSceneFolder [MAX_FILE_NAME_SIZE + 1];
  char szSkyFolder   [MAX_FILE_NAME_SIZE + 1];
  char szSkinFolder  [MAX_FILE_NAME_SIZE + 1];
  char szModelFolder [MAX_FILE_NAME_SIZE + 1];
  char szSceneImage  [MAX_FILE_NAME_SIZE + 1];
};

//zero-initialize a configuration
void ConfigZero(OUT Config *pConfig);

//initialize a configuration
void ConfigInit(OUT Config *pConfig);

//update configuration with parsed data
bool ConfigParse(OUT Config *pConfig, int argc, char *argv[]);

//update configuration with parsed data
bool ConfigParseField(OUT Config *pConfig, cchar *szField, cchar *szValue);

//load configuration from a file
bool ConfigLoad(OUT Config *pConfig, cchar *szFile);

//save configuration to a file
bool ConfigSave(Config *pConfig, cchar *szFile);

//output configuration to a stream
void ConfigPrint(Config *pConfig, OUT FILE *pStream);

//output configuration usage to a stream
void ConfigPrintUsage(OUT FILE *pStream);

NAMESPACE_END

#endif //_CONFIG_H_
