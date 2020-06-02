#include "drone.h"
#include <stdlib.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
#include "vec2.h"
#define MAX_ANGLE 0.01

static uint32_t ids = 0;

double generateGaussian(double mean, double stdDev)
{
	static double spare;
	static bool hasSpare = false;

	if (hasSpare)
	{
		hasSpare = false;
		return spare * stdDev + mean;
	}
	else
	{
		double u, v, s;
		do
		{
			u = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
			v = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
			s = u * u + v * v;
		} while (s >= 1.0 || s == 0.0);
		s = sqrt(-2.0 * log(s) / s);
		spare = v * s;
		hasSpare = true;
		return mean + stdDev * u * s;
	}
}

Obstacle compute_obstacle(Drone *d1, Drone *d2)
{
	// Minkowski addition
	double r = d1->size + d2->size;

	//Computing tangent lines to circle passing through the point self.position
	double dx = d1->position.x - d2->position.x;
	double a = dx * dx - r * r;
	double b = 2 * dx * (d1->position.y - d2->position.y);
	double c = (d2->position.y - d1->position.y) * (d2->position.y - d1->position.y) - r * r;
	double Delta = b * b - 4 * a * c;

	//Angular coefficient
	double m1 = (-b + sqrt(Delta)) / (2 * a);
	double m2 = (-b - sqrt(Delta)) / (2 * a);
	//Intersection with y axis
	double q1 = d1->position.y - m1 * d1->position.x;
	double q2 = d1->position.y - m2 * d1->position.x;

	//(xt1,yt1) - first tangent point.
	double a1 = 1 + m1 * m1;
	double b1 = 2 * m1 * q1 - 2 * d2->position.x - m1 * 2 * d2->position.y;

	double xt1 = (-b1) / (2 * a1);
	double yt1 = m1 * xt1 + q1;

	//(xt2,yt2) - Second tangent point
	double a2 = 1 + m2 * m2;
	double b2 = 2 * m2 * q2 - 2 * d2->position.x - m2 * 2 * d2->position.y;

	double xt2 = (-b2) / (2 * a2);
	double yt2 = m2 * xt2 + q2;

	//Construct obstacle
	Obstacle o;
	o.position = d2->position;
	o.radius = r;
	o.T1.x = xt1;
	o.T1.y = yt1;
	o.T2.x = xt2;
	o.T2.y = yt2;
	// printf("T1.x= %.3f,T1.y= %.3f\n",o.T1.x,o.T1.y);
	return o;
}

barycoords barycentric(vec2 A, vec2 B, vec2 C, vec2 P)
{
	barycoords bc;
	bc.gamma = ((A.y - B.y) * P.x + (B.x - A.x) * P.y + A.x * B.y - B.x * A.y) /
			   ((A.y - B.y) * C.x + (B.x - A.x) * C.y + A.x * B.y - B.x * A.y);
	bc.beta = ((A.y - C.y) * P.x + (C.x - A.x) * P.y + A.x * C.y - C.x * A.y) /
			  ((A.y - C.y) * B.x + (C.x - A.x) * B.y + A.x * C.y - C.x * A.y);
	bc.alpha = 1 - bc.beta - bc.gamma;

	return bc;
}

Drone DR_newDrone(double x, double y, double vx, double vy, double size)
{
	Drone d;
	d.id = ids;
	ids++;
	d.position.x = x;
	d.position.y = y;
	d.speed.x = vx;
	d.speed.y = vy;
	d._speed_mod = v2_mod(d.speed);
	d.waypoints = malloc(2 * sizeof(vec2));
	d.wp_len = 2;
	d.curr_wp = 0;
	d.waypoints[0].x = x;
	d.waypoints[0].y = y;
	d.size = size;
	return d;
}

void DR_move(Drone *d, double dt)
{

	if (v2_distance(d->position, d->waypoints[d->curr_wp]) < d->size)
	{
		// printf("D%d Reached: %.3f,%.3f\n", d->id, d->waypoints[d->curr_wp].x, d->waypoints[d->curr_wp].y);
		DR_pop_waypoint(d);
	}

	DR_goto(d, d->waypoints[d->curr_wp]);

	vec2 dP = v2_prodK(d->speed, dt);
	d->position = v2_add(d->position, dP);
}

void DR_goto(Drone *d, vec2 waypoint)
{

	vec2 dir = v2_norm(d->speed);

	vec2 dirp = v2_sub(waypoint, d->position);

	dirp = v2_norm(dirp);

	double C = v2_dot(dir, dirp);

	double angle = 0;

	if (C < -0.9999)
	{
		//angle = M_PI;
		d->speed.x = -d->speed.x;
		d->speed.y = -d->speed.y;
	}
	else if (C >= -0.9999 && C < 0.9999)
	{
		angle = -acos(C);
		d->speed = v2_rotate(d->speed, angle);
	}
}

bool DR_collision(Drone *d1, Drone *d2)
{
	Obstacle o = compute_obstacle(d1, d2);

	vec2 dif = v2_sub(d1->speed, d2->speed);
	if (v2_mod(dif) > 1.9 * d1->_speed_mod)
	{
		return true;
	}
	vec2 ds = v2_add(dif, d1->position);

	barycoords bc = barycentric(d1->position, o.T2, o.T1, ds);

	if (bc.alpha > 0 && bc.beta > 0 && bc.gamma > 0)
	{
		return true;
	}

	return false;
}

void DR_avoid(Drone *d, Drone *d2, double error)
{
	Drone dx = *d2;
	if (error > 0)
	{
		vec2 pos_error;
		pos_error.x = generateGaussian(0, 5);
		pos_error = v2_rotate(pos_error, 2 * M_PI * rand() / RAND_MAX);

		dx.position = v2_add(dx.position, pos_error);
	}

	if (DR_collision(d, &dx))
	{

		vec2 dir = v2_norm(d->speed);
		double theta = atan2(dir.y, dir.x);
		vec2 p2rel = v2_sub(dx.position, d->position);
		double thetaP2 = atan2(p2rel.y, p2rel.x);
		if (fabs(thetaP2) > fabs(theta))
		{
			vec2 escape = v2_rotateHalfPI(d->speed, -1);
			escape = v2_norm(escape);
			escape = v2_prodK(escape, 4 * (d->size + dx.size));
			escape = v2_add(escape, d->position);
			DR_push_waypoint(d, escape);
		}
	}
}

void DR_stopAndWait(Drone *d, Drone *d2, double error)
{
	Drone dx = *d2;
	if (error > 0)
	{
		vec2 pos_error;
		pos_error.x = generateGaussian(0, error);
		pos_error = v2_rotate(pos_error, 2 * M_PI * rand() / RAND_MAX);

		dx.position = v2_add(dx.position, pos_error);
	}

	if (DR_collision(d, &dx))
	{

		if (dx.id > d->id)
		{

			d->speed.x = 0;
			d->speed.y = 0;
		}
	}
	else
	{
		d->speed.x = d->_speed_mod;
		d->speed.y = 0;
	}
}

void DR_push_waypoint(Drone *d, vec2 wp)
{

	if (d->curr_wp == (d->wp_len - 1))
	{
		d->wp_len = (d->wp_len + (d->wp_len >> 1));
		d->waypoints = realloc(d->waypoints, (d->wp_len + (d->wp_len >> 1)) * sizeof(vec2));
	}
	d->curr_wp += 1;
	d->waypoints[d->curr_wp] = wp;
}
void DR_pop_waypoint(Drone *d)
{
	d->curr_wp = (d->curr_wp > 0) ? d->curr_wp - 1 : 0;
}

void DR_freeDrone(Drone *d)
{
	free(d->waypoints);
	free(d);
}
