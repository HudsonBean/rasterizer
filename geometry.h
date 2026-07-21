#pragma once
#ifndef GEOMETRY
#define GEOMETRY

#include "vec.h"

// Decides if point p is to the left or right of the line starting at point a
// and facing towards point b.
float orient2D(Vec2 a, Vec2 b, Vec2 p) {
  // Define our line segments
  const Vec2 AB = b - a;
  const Vec2 AP = p - a;

  // Compute the cross product to see if point p
  // lies on the left or right of AB
  return AB.cross2D(AP);
}

#endif