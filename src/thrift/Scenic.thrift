#!/usr/local/bin/thrift --gen cpp

namespace cpp scenic.server

struct Point {
  1:required double x;
  2:required double y;
  3:required double z;
}

struct Color {
  1:required double r;
  2:required double g;
  3:required double b;
}

struct Scene {
  1:required string name;
  2:optional Color  ambient;
}

struct Camera {
  1:required Point  pos;
  2:optional Point  lookAt;
  3:optional double angleX;
  4:optional double angleY;
}

enum ImageType {
  JPEG = 1,
  PNG  = 2,
  BMP  = 3,
  TIFF = 4,
}

struct Image {
  1:optional ImageType type;
  2:optional i32 quality;
}

typedef i32 SceneNodeId

service Scenic {
  void Ping();

  binary GetSceneImage(1:Scene  scene,
                       2:Camera camera,
                       3:Image  image);

  SceneNodeId GetSceneNode(1:Scene  scene,
                           2:Camera camera,
                           3:i32 x,
                           4:i32 y);
}
