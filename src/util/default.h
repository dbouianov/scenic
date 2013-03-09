#ifndef _DEFAULT_H_
#define _DEFAULT_H_

#include "common.h"

NAMESPACE_BEGIN

//window parameters
#define DEFAULT_WINDOW_TITLE      "Scenic"
#define DEFAULT_WINDOW_WIDTH      800
#define DEFAULT_WINDOW_HEIGHT     600
#define DEFAULT_WINDOW_X          0
#define DEFAULT_WINDOW_Y          0

//OpenGL parameters
#define DEFAULT_POINT_SIZE        1.0f
#define DEFAULT_LINE_WIDTH        1.0f
#define DEFAULT_CLEAR_DEPTH       1.0f
#define DEFAULT_CLEAR_STENCIL     0
#define DEFAULT_CLEAR_COLOR       0.0f, 0.0f, 0.0f, 1.0f
#define DEFAULT_AMBIENT_COLOR     0.33f, 0.33f, 0.33f, 1.0f
#define DEFAULT_SHININESS         10.0f
#define DEFAULT_STENCIL_VALUE     1
#define DEFAULT_STENCIL_SHADOW    2
#define DEFAULT_STENCIL_MASK      0xffffffff
#define DEFAULT_SHADOW_COLOR      0.0f, 0.0f, 0.0f, 0.5f
#define DEFAULT_BOUNDS_COLOR      0.8f, 0.1f, 0.1f, 1.0f
#define DEFAULT_OFFSET_MIRRORS    -2.0f, -2.0f
#define DEFAULT_OFFSET_SHADOWS    -4.0f, -4.0f
#define DEFAULT_OFFSET_BOUNDS     -4.0f, -4.0f
#define DEFAULT_PIXEL_ALIGNMENT   1

//fog parameters
#define DEFAULT_FOG_COLOR       DEFAULT_CLEAR_COLOR
#define DEFAULT_FOG_DENSITY     0.0625f
#define DEFAULT_FOG_START       0.0f
#define DEFAULT_FOG_END         DEFAULT_CLEAR_DEPTH

//scene parameters
#define DEFAULT_UNIT_SIZE         1.0f
#define DEFAULT_UNIT_HALF_SIZE    0.5f
#define DEFAULT_FIELD_OF_VIEW     40.0f
#define DEFAULT_ASPECT_RATIO      ((float)DEFAULT_WINDOW_WIDTH / DEFAULT_WINDOW_HEIGHT)
#define DEFAULT_NEAR_DISTANCE     0.1f
#define DEFAULT_FAR_DISTANCE      500.0f

//configuration parameters
#define DEFAULT_DATA_FOLDER       "../data"
#define DEFAULT_SKIN_FOLDER       DEFAULT_DATA_FOLDER "/skin00"
#define DEFAULT_SCENE_FOLDER      DEFAULT_DATA_FOLDER "/scene00"
#define DEFAULT_MODEL_FOLDER      DEFAULT_DATA_FOLDER "/model00"
#define DEFAULT_SKYBOX_FOLDER     DEFAULT_DATA_FOLDER "/sky00"
#define DEFAULT_IMAGE_TYPE        "png"
#define DEFAULT_IMAGE_EXT         "." DEFAULT_IMAGE_TYPE
#define DEFAULT_SCENE_IMAGE       "scene" DEFAULT_IMAGE_EXT
#define DEFAULT_FONT_IMAGE        "font"  DEFAULT_IMAGE_EXT
#define DEFAULT_MENU_IMAGE        "menu"  DEFAULT_IMAGE_EXT
#define DEFAULT_MENU_FILE         "menu.lua"
#define DEFAULT_SCENE_FILE        "scene.lua"
#define DEFAULT_TERRAIN_FILE      "terrain.lua"
#define DEFAULT_MIRRORS_FILE      "mirrors.lua"
#define DEFAULT_EFFECTS_FILE      "effects.lua"
#define DEFAULT_LIGHT_FILE        "light.lua"
#define DEFAULT_MODEL_FILE        "model.lua"

//skybox parameters
#define DEFAULT_SKYBOX_SIZE           25.0f
#define DEFAULT_SKYBOX_OVERSIZE       0.0025f
#define DEFAULT_SKYBOX_FOG_DENSITY    0.025f

//camera parameters
#define DEFAULT_CAMERA_X          0.0f
#define DEFAULT_CAMERA_Z          5.0f
#define DEFAULT_CAMERA_Y          1.0f
#define DEFAULT_CAMERA_ANGLE_X    0.0f
#define DEFAULT_CAMERA_ANGLE_Y    0.0f

//flash light parameters
#define DEFAULT_FLASH_LIGHT_DIFFUSE     0.25f, 0.25f, 0.5f
#define DEFAULT_FLASH_LIGHT_SPECULAR    0.5f, 0.5f, 0.5f
#define DEFAULT_FLASH_LIGHT_EXPONENT    64.0f
#define DEFAULT_FLASH_LIGHT_CUTOFF      30.0f

//mouse and keyboard parameters
#define DEFAULT_MOUSE_LOOK_FACTOR       0.08f
#define DEFAULT_MOUSE_SAMPLE_WEIGHT     0.15f
#define DEFAULT_KEYBOARD_LOOK_FACTOR    72.0f
#define DEFAULT_KEYBOARD_MOVE_FACTOR    8.0f
#define DEFAULT_KEYBOARD_ZOOM_FACTOR    16.0f

//cross-hair parameters
#define DEFAULT_CROSSHAIR_SIZE            0.00125f
#define DEFAULT_CROSSHAIR_COLOR           0.1f, 1.0f, 0.1f, 1.0f
#define DEFAULT_CROSSHAIR_CIRCLE_FACTOR   0.6f

//player parameters
#define DEFAULT_JUMP_VELOCITY   5.0f

//environment parameters
#define DEFAULT_SPACE_GRAVITY         0.0f
#define DEFAULT_EARTH_GRAVITY         9.81f
#define DEFAULT_MOON_GRAVITY          1.64f
#define DEFAULT_FRICTION              1.0f
#define DEFAULT_GRAVITY_MIN_SPREAD    5.0f
#define DEFAULT_GRAVITY_MAX_SPREAD    45.0f

//drawing parameters
#define DEFAULT_NUM_SPHERE_SLICES       40
#define DEFAULT_NUM_SPHERE_STACKS       25
#define DEFAULT_NUM_CYLINDER_SLICES     30
#define DEFAULT_NUM_CYLINDER_STACKS     10
#define DEFAULT_NUM_CONE_SLICES         30
#define DEFAULT_NUM_CONE_STACKS         12
#define DEFAULT_NUM_DISK_SLICES         40
#define DEFAULT_NUM_DISK_LOOPS          8
#define DEFAULT_NUM_CIRCLE_SLICES       60
#define DEFAULT_NUM_CROSSHAIR_SLICES    10
#define DEFAULT_CUBE_SIZE_X             16
#define DEFAULT_CUBE_SIZE_Y             16
#define DEFAULT_SQUARE_SIZE_X           16
#define DEFAULT_SQUARE_SIZE_Y           16

//particle system parameters
#define DEFAULT_NUM_PARTICLES         1024
#define DEFAULT_PARTICLE_SIZE         0.01f
#define DEFAULT_PARTICLE_SPREAD       15.0f
#define DEFAULT_PARTICLE_MIN_FADE     0.005f
#define DEFAULT_PARTICLE_MAX_FADE     0.05f
#define DEFAULT_PARTICLE_MAX_LIFE     1.0f

//menu parameters
#define DEFAULT_MENU_SIZE             16
#define DEFAULT_MENU_WIDTH            16
#define DEFAULT_MENU_HEIGHT           16
#define DEFAULT_MENU_ITEM_WIDTH       0.0675f
#define DEFAULT_MENU_ITEM_HEIGHT      0.0045f
#define DEFAULT_MENU_SPACE_FACTOR     2.5f
#define DEFAULT_MENU_BACK_COLOR       0.5f, 0.5f, 0.5f, 0.5f
#define DEFAULT_MENU_ITEM_COLOR       0.1f, 0.8f, 0.1f, 0.5f
#define DEFAULT_MENU_TEXT_COLOR       1.0f, 1.0f, 1.0f, 0.5f
#define DEFAULT_MENU_SELECT_COLOR     0.8f, 0.1f, 0.1f, 0.5f

//font properties
#define DEFAULT_FONT_SIZE     256
#define DEFAULT_FONT_START    32
#define DEFAULT_CHAR_SIZE     16
#define DEFAULT_CHAR_STEP     0.8225f

NAMESPACE_END

#endif //_DEFAULT_H_
