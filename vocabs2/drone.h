#ifndef DRONE_H
#define DRONE_H
#include "vec2.h"
#include <stdint.h>
#include <stdbool.h>

//Struct to keep barycentric coordinates
typedef struct
{
  double alpha;
  double beta;
  double gamma;
} barycoords;

typedef struct
{
  //radius
  double radius;
  //center
  vec2 position;
  //tangent points
  vec2 T1;
  vec2 T2;
} Obstacle;

typedef struct
{
  uint32_t id;       //Unique ID
  vec2 position;     // current position
  vec2 speed;        //current speed
  double _speed_mod; //speed module
  /*I might consider making the flight plan a separate object*/
  vec2 *waypoints;      //flight plan (array of waypoints that rescales automagically when adding new waypoints)
  unsigned int wp_len;  //max flight plan length
  unsigned int curr_wp; //Index of the current waypoint
  double size;          //Physical size of the drone
} Drone;

//Initialize a new drone.
Drone DR_newDrone(double x, double y, double vx, double vy, double size);
//Free memory used by a drone
void DR_freeDrone(Drone *d);
//move the drone by speed x time delta
void DR_move(Drone *d, double dt);
//Steer towards next waypoint (It doesn't move the drone!!)
void DR_goto(Drone *d, vec2 waypoint);
//Are the drones on a collision route?
bool DR_collision(Drone *d1, Drone *d2);
//Compute avoidance maneuver and add escape waypoint
void DR_avoid(Drone *d, Drone *d2, double error);
//Instead of computing an avoidance maneuver waits until no collision is imminent
void DR_stopAndWait(Drone *d, Drone *d2, double error);
//Compute avoidance for a list of drones
void DR_avoidMany(Drone *d, Drone *drones, uint32_t ndrones, double error);
//Waypoints are stacked (LIFO) push adds a waypoint on top of the stack
//Increase the waypoint array size if needed
void DR_push_waypoint(Drone *d, vec2 wp);
//pop removes the top waypoints (but it does not shrink the waypoint array)
void DR_pop_waypoint(Drone *d);
//Communication part

#endif
