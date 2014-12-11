#include "Create.h"
#include <stdio.h>
#include <unistd.h>
#include <libplayerc/playerc.h>

Create::Create() {
  for (int i = 0; i < 4; ++i)
    norm[i] = 0;

  // Create a client and connect it to the server.
  printf("creating client...\n");
  client = playerc_client_create(NULL, "localhost", 6665);
  printf("connecting client...\n");
  if (0 != playerc_client_connect(client))
    quit("Could not connect to client\n");
  // Create and subscribe to position2d, ir, and opaque devices.
  printf("creating position2d...\n");
  position2d = playerc_position2d_create(client, 0);
  printf("subscribing position2d...\n");
  if (playerc_position2d_subscribe(position2d, PLAYER_OPEN_MODE))
    quit("Could not subscribe to position2d driver\n");
  printf("creating ir...\n");
  ir = playerc_ir_create(client, 0);
  printf("subscribing ir...\n");
  if (playerc_ir_subscribe(ir, PLAYER_OPEN_MODE))
    quit("Could not subscribe to ir driver\n");
  printf("creating opaque...\n");
  opaque = playerc_opaque_create(client, 0);
  printf("subscribing opaque...\n");
  if (playerc_opaque_subscribe(opaque, PLAYER_OPEN_MODE))
    quit("Could not subscribe to opaque driver\n");
  printf("Done subscribing to devices.\n");
}

void Create::lights(int middle_power, int left_color, int left_intensity) {
  uint8_t d[8];
  player_opaque_data_t data;

  if (opaque != NULL) {
    d[0] = 2;
    d[1] = 1;
    d[2] = middle_power;    // middle light
    d[3] = 0;
    d[4] = 0;
    d[5] = 0;
    d[6] = left_color;      // left light color (0 = green, 255 = orange)
    d[7] = left_intensity;  // left light intensity (0 to 255)
    data.data_count = 8;
    data.data = &(d[0]);
    playerc_opaque_cmd(opaque, &data);
  }
}

void Create::blink_led() {          // every 400ms
  static int t = 0;
  if (t < 20)
    lights(255, 0, 0);      // middle light on 200ms
  else
    lights(0, 0, 0);        // middle light off 200ms
  ++t;
  if (t >= 40)
    t = 0;
}

void Create::motor_raw(double forward_speed, double angular_speed) {  
  if (playerc_position2d_set_cmd_vel(position2d, forward_speed, 0, angular_speed, 1))
    quit("move command failed: playerc_position2d_set_cmd_vel");
}

void Create::move(double forward_speed) {
  if (playerc_position2d_set_cmd_vel(position2d, forward_speed, 0, 0, 1))
    quit("move command failed: playerc_position2d_set_cmd_vel");
}

void Create::turn(double forward_speed, double angular_speed) {
  if (playerc_position2d_set_cmd_vel(position2d, forward_speed, 0, angular_speed, 1))
    quit("move command failed: playerc_position2d_set_cmd_vel");
  usleep(1000000);  // turn uninterupted for 1 second, then straighten out
  if (playerc_position2d_set_cmd_vel(position2d, forward_speed, 0, 0, 1))
    quit("move command failed: playerc_position2d_set_cmd_vel");
}

void Create::calibrate_ir() {
  // turn in place (+360 degrees, then -360 degrees)
  printf("begin calibration\n");
  ir_values readings;
  float ir_avg[4] = {0, 0, 0, 0};
  int i = 0;
  //clock_t t = clock();
  readings = read_ir_raw();
  ir_avg[0] += readings.left;
  ir_avg[1] += readings.fleft;
  ir_avg[2] += readings.fright;
  ir_avg[3] += readings.right;
  motor_raw(0,-1.57);   // turn clockwise at 1.57 radians (90 degrees) per second
  for (; i < 44; ++i, usleep(10000)) {    
    readings = read_ir_raw();
    ir_avg[0] += readings.left;
    ir_avg[1] += readings.fleft;
    ir_avg[2] += readings.fright;
    ir_avg[3] += readings.right;
  }
  motor_raw(0, 0); usleep(500000);   // pause between turns
  //t = clock();
  motor_raw(0,1.57);   // turn counterclockwise at 1.57 radians (90 degrees) per second
  for (i = 0; i < 44; ++i, usleep(10000)) {    
    readings = read_ir_raw();
    ir_avg[0] += readings.left;
    ir_avg[1] += readings.fleft;
    ir_avg[2] += readings.fright;
    ir_avg[3] += readings.right;
  }
  motor_raw(0, 0);     // Stop turning
  for (int i = 0; i < 4; ++i)
    ir_avg[i] /= 88;
  printf("Averages:\n\tir[0] = %f\n\tir[1] = %f\n\tir[2] = %f\n\tir[3] = %f\n",
         ir_avg[0], ir_avg[1], ir_avg[2], ir_avg[3]);
  
  // Determine normalization coefficients
  // (assume calibration was done on butcher paper - normalize to IR reflectivity value of 1100)
  const float norm_val = 1100;
  for (int i = 0; i < 4; ++i)
    norm[i] = norm_val / ir_avg[i];
}

Create::ir_values Create::read_ir_raw() {
  ir_values rtn;
  playerc_client_read(client);
  //if(playerc_client_read_nonblock(client) != 1)
  //  quit("Failed to read IR sensors. Shutting down...\n");
  rtn.left = (unsigned int) ir->data.ranges[12];
  rtn.fleft = (unsigned int) ir->data.ranges[13];
  rtn.fright = (unsigned int) ir->data.ranges[14];
  rtn.right = (unsigned int) ir->data.ranges[15];
  return rtn;
}

Create::ir_values Create::read_ir() {       // returnes calibrated values
  ir_values rtn;
  playerc_client_read(client);
  rtn.left = (unsigned int) (ir->data.ranges[12] * norm[0]);
  rtn.fleft = (unsigned int) (ir->data.ranges[13] * norm[1]);
  rtn.fright = (unsigned int) (ir->data.ranges[14] * norm[2]);
  rtn.right = (unsigned int) (ir->data.ranges[15] * norm[3]);
  return rtn;
}

void Create::shutdown() {
  playerc_position2d_unsubscribe(position2d);
  playerc_position2d_destroy(position2d);
  playerc_ir_unsubscribe(ir);
  playerc_ir_destroy(ir);
  playerc_opaque_unsubscribe(opaque);
  playerc_opaque_destroy(opaque);
  playerc_client_disconnect(client);
  playerc_client_destroy(client);
}

void Create::quit(const char *msg) {
  printf("%s", msg);
  shutdown();
  exit(1);
}

