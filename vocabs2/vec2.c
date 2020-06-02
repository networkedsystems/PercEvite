#include "vec2.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

double v2_mod(vec2 v)
{
  return sqrt(v.x * v.x + v.y * v.y);
}

vec2 v2_rotate(vec2 v, double angle)
{

  double m = v2_mod(v);
  vec2 vn;
  vn.x = v.x / m;
  vn.y = v.y / m;
  vec2 ret;
  double C = cos(angle);
  double S = sin(angle);

  ret.x = m * (vn.x * C - vn.y * S);
  ret.y = m * (vn.x * S + vn.y * C);

  return ret;
}

vec2 v2_norm(vec2 v)
{
  vec2 n;
  double m = v2_mod(v);
  n.x = v.x / m;
  n.y = v.y / m;
  return n;
}

vec2 v2_add(vec2 v1, vec2 v2)
{
  vec2 res;
  res.x = v1.x + v2.x;
  res.y = v1.y + v2.y;
  return res;
}

vec2 v2_sub(vec2 v1, vec2 v2)
{
  vec2 res;
  res.x = v1.x - v2.x;
  res.y = v1.y - v2.y;
  return res;
}

double v2_dot(vec2 v1, vec2 v2)
{
  return v1.x * v2.x + v1.y * v2.y;
}

vec2 v2_addK(vec2 v, double k)
{
  vec2 res;
  res.x = v.x + k;
  res.y = v.y + k;
  return res;
}

vec2 v2_prodK(vec2 v, double k)
{
  vec2 res;
  res.x = v.x * k;
  res.y = v.y * k;
  return res;
}

double v2_distance(vec2 v1, vec2 v2)
{
  vec2 res = v2_sub(v1, v2);
  return v2_mod(res);
}

vec2 v2_rotateHalfPI(vec2 v, int sign)
{
  vec2 vr;
  vr.x = -sign * v.y;
  vr.y = sign * v.x;
  return vr;
}