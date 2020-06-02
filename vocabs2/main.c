#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "vec2.h"
#include "drone.h"
#include "comms.h"
//#include "video.h"
#include <omp.h>
omp_lock_t writelock;

time_t t;
uint64_t iterations = 10000;

double dt = 1E-3; //seconds

double error = -1.0;

double rates[] = {
    1E-2,
    2.5E-2,
    5E-2,
    7.5E-2,
    1E-1,
    2.5E-1,
    5E-1,
    7.5E-1,
    1.0,
    1.5,
    2.0,
    2.5,
    3.0,
    3.5,
    4.0,
    4.5,
    5.0,
    5.5,
    6.0,
    6.5,
    7.0,
    7.5,
    8.0,
    8.5,
    9.0,
    9.5,
    10.0,
    11.0,
    12.0,
    13.0,
    14.0,
    15.0,
    16.0}; //msg/s

double speed = 20.0;
uint64_t len_rates = sizeof(rates) / sizeof(double);
int si = 0;
double rate = 1.0;
char prob = 'A';
RFsystem sys;
double l = 1000.0;
int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    prob = argv[1][0];
  }
  if (argc > 2)
  {
    error = atof(argv[2]);
  }
  if (argc > 3)
  {
    l = 1000.0 * atof(argv[3]);
  }
  if (argc > 4)
  {
    speed = atof(argv[4]);
  }
  printf("Error: %.3f\n", error);
  switch (prob)
  {
  case 'E':
    printf("Wi-Fi beacons\n");
    sys = WI_FI;
    break;
  case 'C':
    printf("ADS-B\n");
    sys = ADS_B;
    break;
  default:
    sys = NO_LOSS;
    printf("No loss\n");
  }
  printf("Loss: %.3f\n", l);
  printf("Speed: %.3f\n", speed);
  double collisions[len_rates];
  for (uint64_t k = 0; k < len_rates; k++)
  {
    collisions[k] = 0.0;
  }
  omp_init_lock(&writelock);
  t = time(NULL);
  srand(t);

  //speed is in m/s

  vec2 p1 = {500.0, 500.0};
  vec2 p2 = {1000.0, 1000.0};
  vec2 p3 = {000.0, 1000.0};
  // vec2 p4 = {0.0, 0.0};

  printf("Rate:\tPcrash:\n");

#pragma omp parallel
  {

    for (uint64_t i = 0; i < len_rates; i++)
    {

#pragma omp for
      for (uint64_t it = 0; it < iterations; it++)
      {

        Drone d1 = DR_newDrone(0.0, 0.0, speed, 0.0, 1);
        Drone d2 = DR_newDrone(1000.0, 0.0, speed, 0.0, 1);
        // Drone d3 = DR_newDrone(500.0, 999.0, 20.0, 0.0, 1);
        DR_push_waypoint(&d1, p2);
        DR_push_waypoint(&d2, p3);
        // DR_push_waypoint(&d3, p4);
        DR_push_waypoint(&d1, p1);
        DR_push_waypoint(&d2, p1);
        // DR_push_waypoint(&d3, p1);

        rate = rates[i];

        bool running = true;
        double timer = 0;

        while (running)
        {

          if (timer >= 1 / rate)
          {

            if (COM_broadcast(d1.position, d2.position, sys, l))
            {

              DR_avoid(&d2, &d1, error);
              DR_avoid(&d1, &d2, error);
              // DR_avoid(&d3, &d1, error);
              // DR_avoid(&d1, &d3, error);
              // DR_avoid(&d2, &d3, error);
              // DR_avoid(&d3, &d2, error);
              // DR_stopAndWait(&d1, &d2, error);
              // DR_stopAndWait(&d2, &d1, error);
            }
            timer = 0;
          }

          DR_move(&d1, dt);
          DR_move(&d2, dt);
          // DR_move(&d3, dt);
          if (d1.waypoints[d1.curr_wp].x == 0 && d1.waypoints[d1.curr_wp].y == 0)
          {
            running = false;
          }
          if (d2.waypoints[d2.curr_wp].x == 0 && d2.waypoints[d2.curr_wp].y == 0)
          {
            running = false;
          }
          // if (d3.waypoints[d3.curr_wp].x == 0 && d3.waypoints[d3.curr_wp].y == 0)
          // {
          //   running = false;
          // }
          if (v2_distance(d1.position, d2.position) < (d1.size + d2.size)) // ||
                                                                           //v2_distance(d1.position, d3.position) < (d1.size + d3.size)))// ||
                                                                           //v2_distance(d3.position, d2.position) < (d3.size + d2.size))
          {
            omp_set_lock(&writelock);
            collisions[i] += 1;
            omp_unset_lock(&writelock);
            running = false;
          }

          timer += dt;
        }

      } //iterations
      if (collisions[i] == 0.0)
      {
        printf("%.3f\n", rates[i]);
        break;
      }
    } //rates

  } //openmp

  FILE *results = fopen("results_speed_loss_avoid.txt", "a");
  fprintf(results, "Error: %.3f\n", error);
  fprintf(results, "Loss: %.3f\n", l);
  fprintf(results, "Speed: %.3f\n", speed);
  switch (prob)
  {
  case 'E':
    fprintf(results, "Wi-Fi beacons\n");
    break;
  case 'C':
    fprintf(results, "ADS-B\n");
    break;
  default:
    fprintf(results, "No loss\n");
  }

  for (uint64_t k = 0; k < len_rates; k++)
  {
    printf("%.3f\t%.6f\n", rates[k], collisions[k] / iterations);
    fprintf(results, "%.3f\t%.10f\n", rates[k], collisions[k] / iterations);
  }
  fclose(results);

  return 0;
}
