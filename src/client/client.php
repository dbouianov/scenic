#!/usr/bin/php -f
<?php
$GLOBALS['THRIFT_ROOT'] = '/usr/share/php/Thrift';
require_once $GLOBALS['THRIFT_ROOT'].'/Thrift.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/TSocket.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/TTransport.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/TBufferedTransport.php';
require_once $GLOBALS['THRIFT_ROOT'].'/transport/TFramedTransport.php';
require_once $GLOBALS['THRIFT_ROOT'].'/protocol/TBinaryProtocol.php';
require_once $GLOBALS['THRIFT_ROOT'].'/packages/Scenic/Scenic.php';

try {
  $socket    = new TSocket('127.0.0.1', 9090);
  //$transport = new TBufferedTransport($socket, 1024, 1024);
  $transport = new TFramedTransport($socket);
  //$protocol  = new TBinaryProtocol($transport);
  $protocol  = new TBinaryProtocolAccelerated($transport);
  $client    = new ScenicClient($protocol);

  $transport->open();

  $scene = new Scene();
  $scene->name = "scene00";
  $scene->ambient = new Color();
  $scene->ambient->r = 0.5;
  $scene->ambient->g = 0.5;
  $scene->ambient->b = 0.5;

  $camera = new Camera();
  $camera->pos = new Point();
  $camera->pos->x = 0.0;
  $camera->pos->y = 1.5;
  $camera->pos->z = 5.0;
  $camera->angleX = 10.0;
  $camera->angleY = 0.0;

  $image = new Image();
  $image->type = ImageType::BMP;

  //echo $client->GetSceneImage($scene, $camera, $image);
  echo $client->GetSceneNode($scene, $camera, 400, 300);

  $transport->close();
}
catch (TException $x) {
  echo 'Thrift Exception: '.$x->getMessage()."\n";
}
catch (Exception $x) {
  echo 'Exception: '.$x->getMessage()."\n";
}
