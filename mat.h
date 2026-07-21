#ifndef MAT
#define MAT

#include "vec.h"

struct Mat4 {
  float mat[4][4];

  Vec4 operator*(const Vec4 &v) const {
    return Vec4(
        v.x * mat[0][0] + v.y * mat[0][1] + v.z * mat[0][2] + v.w * mat[0][3],
        v.x * mat[1][0] + v.y * mat[1][1] + v.z * mat[1][2] + v.w * mat[1][3],
        v.x * mat[2][0] + v.y * mat[2][1] + v.z * mat[2][2] + v.w * mat[2][3],
        v.x * mat[3][0] + v.y * mat[3][1] + v.z * mat[3][2] + v.w * mat[3][3]);
  }
};

#endif