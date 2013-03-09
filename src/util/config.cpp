#include "config.h"
#include <stdio.h>
#include <errno.h>

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_CONFIG, "Config",__FILE__);

void ConfigZero(Config *pConfig)
{
  MemZero(pConfig, sizeof(Config));
}

void ConfigInit(Config *pConfig)
{
  pConfig->width  = DEFAULT_WINDOW_WIDTH;
  pConfig->height = DEFAULT_WINDOW_HEIGHT;
  pConfig->flags  = CONFIG_FLAGS_NONE;
  PointInit(pConfig->pos, DEFAULT_CAMERA_X, DEFAULT_CAMERA_Y, DEFAULT_CAMERA_Z);
  pConfig->angleX = DEFAULT_CAMERA_ANGLE_X;
  pConfig->angleY = DEFAULT_CAMERA_ANGLE_Y;
  pConfig->szConfigFile[0] = NULL_CHAR;
  strcpy(pConfig->szSceneFolder, DEFAULT_SCENE_FOLDER);
  strcpy(pConfig->szSkyFolder,   DEFAULT_SKYBOX_FOLDER);
  strcpy(pConfig->szSkinFolder,  DEFAULT_SKIN_FOLDER);
  strcpy(pConfig->szModelFolder, DEFAULT_MODEL_FOLDER);
  strcpy(pConfig->szSceneImage,  DEFAULT_SCENE_IMAGE);
}

bool ConfigParse(Config *pConfig, int argc, char *argv[])
{
  for (int i = 1; i < argc; i += 2)
  {
    if (i + 1 < argc)
    {
      ConfigParseField(pConfig, argv[i], argv[i + 1]);
    }
  }
  return true;
}

bool ConfigParseField(Config *pConfig, cchar *szField, cchar *szValue)
{
  if (strcmp(szField, EMPTY_STRING) && strcmp(szValue, EMPTY_STRING))
  {
    if (!strcmp(szField, CONFIG_FIELD_NAME_SCENE))
    {
      strncpy(pConfig->szSceneFolder, szValue, MAX_FILE_NAME_SIZE);
      pConfig->szSceneFolder[MAX_FILE_NAME_SIZE] = NULL_CHAR;
      return true;
    }
    else if (!strcmp(szField, CONFIG_FIELD_NAME_SKYBOX))
    {
      strncpy(pConfig->szSkyFolder, szValue, MAX_FILE_NAME_SIZE);
      pConfig->szSkyFolder[MAX_FILE_NAME_SIZE] = NULL_CHAR;
      return true;
    }
    else if (!strcmp(szField, CONFIG_FIELD_NAME_SKIN))
    {
      strncpy(pConfig->szSkinFolder, szValue, MAX_FILE_NAME_SIZE);
      pConfig->szSkinFolder[MAX_FILE_NAME_SIZE] = NULL_CHAR;
      return true;
    }
    else if (!strcmp(szField, CONFIG_FIELD_NAME_MODEL))
    {
      strncpy(pConfig->szModelFolder, szValue, MAX_FILE_NAME_SIZE);
      pConfig->szModelFolder[MAX_FILE_NAME_SIZE] = NULL_CHAR;
      return true;
    }
    else if (!strcmp(szField, CONFIG_FIELD_NAME_IMAGE))
    {
      strncpy(pConfig->szSceneImage, szValue, MAX_FILE_NAME_SIZE);
      pConfig->szSceneImage[MAX_FILE_NAME_SIZE] = NULL_CHAR;
      return true;
    }
    else
    {
      errno = 0;
      int value = atoi(szValue);
      if (!errno)
      {
        if (!strcmp(szField, CONFIG_FIELD_NAME_WIDTH))
        {
          pConfig->width = value;
          return true;
        }
        else if (!strcmp(szField, CONFIG_FIELD_NAME_HEIGHT))
        {
          pConfig->height = value;
          return true;
        }
        else if (!strcmp(szField, CONFIG_FIELD_NAME_X))
        {
          pConfig->posX = value;
          return true;
        }
        else if (!strcmp(szField, CONFIG_FIELD_NAME_Y))
        {
          pConfig->posY = value;
          return true;
        }
        else if (!strcmp(szField, CONFIG_FIELD_NAME_Z))
        {
          pConfig->posZ = value;
          return true;
        }
        else if (!strcmp(szField, CONFIG_FIELD_NAME_ANGLE_X))
        {
          pConfig->angleX = value;
          return true;
        }
        else if (!strcmp(szField, CONFIG_FIELD_NAME_ANGLE_Y))
        {
          pConfig->angleY = value;
          return true;
        }
        else if (!strcmp(szField, CONFIG_FIELD_NAME_FULLSCREEN))
        {
          if (value != 0)
          {
            pConfig->flags |= CONFIG_FLAGS_FULLSCREEN;
          }
          else
          {
            pConfig->flags &= ~CONFIG_FLAGS_FULLSCREEN;
          }
        }
      }
    }
  }
  return false;
}

bool ConfigLoad(Config *pConfig, cchar *szFile)
{
  FILE *pFile = fopen(szFile, "r");
  if (pFile)
  {
    enum { TEXT_SIZE = 32 };
    char szField[TEXT_SIZE + 1];
    char szValue[TEXT_SIZE + 1];
    while (!feof(pFile))
    {
      if (2 == fscanf(pFile, "%s%s", szField, szValue))
      {
        ConfigParseField(pConfig, szField, szValue);
      }
    }
    fclose(pFile);
    return true;
  }
  DbgError("Unable to open file %s for reading!", szFile);
  return false;
}

bool ConfigSave(Config *pConfig, cchar *szFile)
{
  FILE *pFile = fopen(szFile, "w");
  if (pFile)
  {
    ConfigPrint(pConfig, pFile);
    fclose(pFile);
    return true;
  }
  DbgError("Unable to open file %s for writing!", szFile);
  return false;
}

void ConfigPrint(Config *pConfig, FILE *pStream)
{
  fprintf(pStream, CONFIG_FIELD_NAME_WIDTH   " %u\n", pConfig->width);
  fprintf(pStream, CONFIG_FIELD_NAME_HEIGHT  " %u\n", pConfig->height);
  fprintf(pStream, CONFIG_FIELD_NAME_SCENE   " %s\n", pConfig->szSceneFolder);
  fprintf(pStream, CONFIG_FIELD_NAME_SKYBOX  " %s\n", pConfig->szSkyFolder);
  fprintf(pStream, CONFIG_FIELD_NAME_SKIN    " %s\n", pConfig->szSkinFolder);
  fprintf(pStream, CONFIG_FIELD_NAME_MODEL   " %s\n", pConfig->szModelFolder);
  fprintf(pStream, CONFIG_FIELD_NAME_IMAGE   " %s\n", pConfig->szSceneImage);
  fprintf(pStream, CONFIG_FIELD_NAME_X       " %d\n", static_cast<int>(pConfig->posX));
  fprintf(pStream, CONFIG_FIELD_NAME_Y       " %d\n", static_cast<int>(pConfig->posY));
  fprintf(pStream, CONFIG_FIELD_NAME_Z       " %d\n", static_cast<int>(pConfig->posZ));
  fprintf(pStream, CONFIG_FIELD_NAME_ANGLE_X " %d\n", static_cast<int>(pConfig->angleX));
  fprintf(pStream, CONFIG_FIELD_NAME_ANGLE_Y " %d\n", static_cast<int>(pConfig->angleY));
  fprintf(pStream, CONFIG_FIELD_NAME_FULLSCREEN " %u\n",
    pConfig->flags & CONFIG_FLAGS_FULLSCREEN ? 1 : 0);
}

void ConfigPrintUsage(FILE *pStream)
{
  fprintf(pStream, "[" CONFIG_FIELD_NAME_WIDTH   "\t<pixels>]\n");
  fprintf(pStream, "[" CONFIG_FIELD_NAME_HEIGHT  "\t<pixels>]\n");
  fprintf(pStream, "[" CONFIG_FIELD_NAME_X       "\t<meters>]\n");
  fprintf(pStream, "[" CONFIG_FIELD_NAME_Y       "\t<meters>]\n");
  fprintf(pStream, "[" CONFIG_FIELD_NAME_Z       "\t<meters>]\n");
  fprintf(pStream, "[" CONFIG_FIELD_NAME_ANGLE_X "\t<degrees>]\n");
  fprintf(pStream, "[" CONFIG_FIELD_NAME_ANGLE_Y "\t<degrees>]\n");
  fprintf(pStream, "[" CONFIG_FIELD_NAME_SCENE   "\t<folder>]\n");
  fprintf(pStream, "[" CONFIG_FIELD_NAME_SKYBOX  "\t<folder>]\n");
  fprintf(pStream, "[" CONFIG_FIELD_NAME_IMAGE   "\t<file>]\n");
}

NAMESPACE_END
