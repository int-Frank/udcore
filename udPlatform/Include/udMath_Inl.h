
#include "udPlatform.h"

#include <float.h>
#include <math.h>

#if defined(near)
# undef near
#endif
#if defined(far)
# undef far
#endif
#if defined(max)
# undef max
#endif
#if defined(min)
# undef min
#endif

UDFORCE_INLINE float udPow(float f, float n) { return powf(f, n); }
UDFORCE_INLINE double udPow(double d, double n) { return pow(d, n); }
UDFORCE_INLINE float udRSqrt(float f) { return 1.f/sqrtf(f); }
UDFORCE_INLINE double udRSqrt(double d) { return 1.0/sqrt(d); }
UDFORCE_INLINE float udSqrt(float f) { return sqrtf(f); }
UDFORCE_INLINE double udSqrt(double d) { return sqrt(d); }
UDFORCE_INLINE float udSin(float f) { return sinf(f); }
UDFORCE_INLINE double udSin(double d) { return sin(d); }
UDFORCE_INLINE float udCos(float f) { return cosf(f); }
UDFORCE_INLINE double udCos(double d) { return cos(d); }
UDFORCE_INLINE float udTan(float f) { return tanf(f); }
UDFORCE_INLINE double udTan(double d) { return tan(d); }
UDFORCE_INLINE float udACos(float f) { return acosf(f); }
UDFORCE_INLINE double udACos(double d) { return acos(d); }
UDFORCE_INLINE float udATan(float f) { return atanf(f); }
UDFORCE_INLINE double udATan(double d) { return atan(d); }

template <typename T> udVector2<T> abs(const udVector2<T> &v) { udVector2<T> r = { v.x<T(0)?-v.x:v.x, v.y<T(0)?-v.y:v.y }; return r; }
template <typename T> udVector3<T> abs(const udVector3<T> &v) { udVector3<T> r = { v.x<T(0)?-v.x:v.x, v.y<T(0)?-v.y:v.y, v.z<T(0)?-v.z:v.z }; return r; }
template <typename T> udVector4<T> abs(const udVector4<T> &v) { udVector4<T> r = { v.x<T(0)?-v.x:v.x, v.y<T(0)?-v.y:v.y, v.z<T(0)?-v.z:v.z, v.w<T(0)?-v.w:v.w }; return r; }

template <typename T> udVector2<T> min(const udVector2<T> &v1, const udVector2<T> &v2) { udVector2<T> r = { v1.x<v2.x?v1.x:v2.x, v1.y<v2.y?v1.y:v2.y }; return r; }
template <typename T> udVector3<T> min(const udVector3<T> &v1, const udVector3<T> &v2) { udVector3<T> r = { v1.x<v2.x?v1.x:v2.x, v1.y<v2.y?v1.y:v2.y, v1.z<v2.z?v1.z:v2.z }; return r; }
template <typename T> udVector4<T> min(const udVector4<T> &v1, const udVector4<T> &v2) { udVector4<T> r = { v1.x<v2.x?v1.x:v2.x, v1.y<v2.y?v1.y:v2.y, v1.z<v2.z?v1.z:v2.z, v1.w<v2.w?v1.w:v2.w }; return r; }
template <typename T> udVector2<T> max(const udVector2<T> &v1, const udVector2<T> &v2) { udVector2<T> r = { v1.x>v2.x?v1.x:v2.x, v1.y>v2.y?v1.y:v2.y }; return r; }
template <typename T> udVector3<T> max(const udVector3<T> &v1, const udVector3<T> &v2) { udVector3<T> r = { v1.x>v2.x?v1.x:v2.x, v1.y>v2.y?v1.y:v2.y, v1.z>v2.z?v1.z:v2.z }; return r; }
template <typename T> udVector4<T> max(const udVector4<T> &v1, const udVector4<T> &v2) { udVector4<T> r = { v1.x>v2.x?v1.x:v2.x, v1.y>v2.y?v1.y:v2.y, v1.z>v2.z?v1.z:v2.z, v1.w>v2.w?v1.w:v2.w }; return r; }

template <typename T> T dot2(const udVector2<T> &v1, const udVector2<T> &v2) { return v1.x*v2.x + v1.y*v2.y; }
template <typename T> T dot2(const udVector3<T> &v1, const udVector3<T> &v2) { return v1.x*v2.x + v1.y*v2.y; }
template <typename T> T dot2(const udVector4<T> &v1, const udVector4<T> &v2) { return v1.x*v2.x + v1.y*v2.y; }
template <typename T> T dot3(const udVector3<T> &v1, const udVector3<T> &v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
template <typename T> T dot3(const udVector4<T> &v1, const udVector4<T> &v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
template <typename T> T dot4(const udVector4<T> &v1, const udVector4<T> &v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w; }
template <typename T> T doth(const udVector3<T> &v3, const udVector4<T> &v4) { return v3.x*v4.x + v3.y*v4.y + v3.z*v4.z + v4.w; }

template <typename T> T magSq2(const udVector2<T> &v) { return v.x*v.x + v.y*v.y; }
template <typename T> T magSq2(const udVector3<T> &v) { return v.x*v.x + v.y*v.y; }
template <typename T> T magSq2(const udVector4<T> &v) { return v.x*v.x + v.y*v.y; }
template <typename T> T magSq3(const udVector3<T> &v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
template <typename T> T magSq3(const udVector4<T> &v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
template <typename T> T magSq4(const udVector4<T> &v) { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }

template <typename T> T mag2(const udVector2<T> &v) { return udSqrt(v.x*v.x + v.y*v.y); }
template <typename T> T mag2(const udVector3<T> &v) { return udSqrt(v.x*v.x + v.y*v.y); }
template <typename T> T mag2(const udVector4<T> &v) { return udSqrt(v.x*v.x + v.y*v.y); }
template <typename T> T mag3(const udVector3<T> &v) { return udSqrt(v.x*v.x + v.y*v.y + v.z*v.z); }
template <typename T> T mag3(const udVector4<T> &v) { return udSqrt(v.x*v.x + v.y*v.y + v.z*v.z); }
template <typename T> T mag4(const udVector4<T> &v) { return udSqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w); }

template <typename T> T cross2(const udVector2<T> &v1, const udVector2<T> &v2) { return v1.x*v2.y - v1.y*v2.x; }
template <typename T> T cross2(const udVector3<T> &v1, const udVector3<T> &v2) { return v1.x*v2.y - v1.y*v2.x; }
template <typename T> T cross2(const udVector4<T> &v1, const udVector4<T> &v2) { return v1.x*v2.y - v1.y*v2.x; }

template <typename T> udVector3<T> cross3(const udVector3<T> &v1, const udVector3<T> &v2) { udVector3<T> r = { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x }; return r; }
template <typename T> udVector3<T> cross3(const udVector4<T> &v1, const udVector4<T> &v2) { udVector3<T> r = { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x }; return r; }

template <typename T> udVector2<T> normalize2(const udVector2<T> &v) { T s = udRSqrt(v.x*v.x + v.y*v.y); udVector2<T> r = { v.x*s, v.y*s }; return r; }
template <typename T> udVector3<T> normalize2(const udVector3<T> &v) { T s = udRSqrt(v.x*v.x + v.y*v.y); udVector3<T> r = { v.x*s, v.y*s, v.z }; return r; }
template <typename T> udVector4<T> normalize2(const udVector4<T> &v) { T s = udRSqrt(v.x*v.x + v.y*v.y); udVector4<T> r = { v.x*s, v.y*s, v.z, v.w }; return r; }
template <typename T> udVector3<T> normalize3(const udVector3<T> &v) { T s = udRSqrt(v.x*v.x + v.y*v.y + v.z*v.z); udVector3<T> r = { v.x*s, v.y*s, v.z*s }; return r; }
template <typename T> udVector4<T> normalize3(const udVector4<T> &v) { T s = udRSqrt(v.x*v.x + v.y*v.y + v.z*v.z); udVector4<T> r = { v.x*s, v.y*s, v.z*s, v.w }; return r; }
template <typename T> udVector4<T> normalize4(const udVector4<T> &v) { T s = udRSqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w); udVector4<T> r = { v.x*s, v.y*s, v.z*s, v.w*s }; return r; }


// many kinds of mul...
template <typename T>
udMatrix4x4<T> mul(const udMatrix4x4<T> &m, T f)
{
  udMatrix4x4<T> r = { m.a[0]*f, m.a[1]*f, m.a[2]*f, m.a[3]*f,
                       m.a[4]*f, m.a[5]*f, m.a[6]*f, m.a[7]*f,
                       m.a[8]*f, m.a[9]*f, m.a[10]*f,m.a[11]*f,
                       m.a[12]*f,m.a[13]*f,m.a[14]*f,m.a[15]*f };
  return r;
}
template <typename T>
udVector2<T> mul(const udMatrix4x4<T> &m, const udVector2<T> &v)
{
  udVector2<T> r;
  r.x = m.m._00*v.x + m.m._01*v.y + m.m._03;
  r.y = m.m._10*v.x + m.m._11*v.y + m.m._13;
  return r;
}
template <typename T>
udVector3<T> mul(const udMatrix4x4<T> &m, const udVector3<T> &v)
{
  udVector3<T> r;
  r.x = m.m._00*v.x + m.m._01*v.y + m.m._02*v.z + m.m._03;
  r.y = m.m._10*v.x + m.m._11*v.y + m.m._12*v.z + m.m._13;
  r.z = m.m._20*v.x + m.m._21*v.y + m.m._22*v.z + m.m._23;
  return r;
}
template <typename T>
udVector4<T> mul(const udMatrix4x4<T> &m, const udVector4<T> &v)
{
  udVector4<T> r;
  r.x = m.m._00*v.x + m.m._01*v.y + m.m._02*v.z + m.m._03*v.w;
  r.y = m.m._10*v.x + m.m._11*v.y + m.m._12*v.z + m.m._13*v.w;
  r.z = m.m._20*v.x + m.m._21*v.y + m.m._22*v.z + m.m._23*v.w;
  r.w = m.m._30*v.x + m.m._31*v.y + m.m._32*v.z + m.m._33*v.w;
  return r;
}
template <typename T>
udMatrix4x4<T> mul(const udMatrix4x4<T> &m1, const udMatrix4x4<T> &m2)
{
  udMatrix4x4<T> r;
  r.m._00 = m1.m._00*m2.m._00 + m1.m._01*m2.m._10 + m1.m._02*m2.m._20 + m1.m._03*m2.m._30;
  r.m._01 = m1.m._00*m2.m._01 + m1.m._01*m2.m._11 + m1.m._02*m2.m._21 + m1.m._03*m2.m._31;
  r.m._02 = m1.m._00*m2.m._02 + m1.m._01*m2.m._12 + m1.m._02*m2.m._22 + m1.m._03*m2.m._32;
  r.m._03 = m1.m._00*m2.m._03 + m1.m._01*m2.m._13 + m1.m._02*m2.m._23 + m1.m._03*m2.m._33;
  r.m._10 = m1.m._10*m2.m._00 + m1.m._11*m2.m._10 + m1.m._12*m2.m._20 + m1.m._13*m2.m._30;
  r.m._11 = m1.m._10*m2.m._01 + m1.m._11*m2.m._11 + m1.m._12*m2.m._21 + m1.m._13*m2.m._31;
  r.m._12 = m1.m._10*m2.m._02 + m1.m._11*m2.m._12 + m1.m._12*m2.m._22 + m1.m._13*m2.m._32;
  r.m._13 = m1.m._10*m2.m._03 + m1.m._11*m2.m._13 + m1.m._12*m2.m._23 + m1.m._13*m2.m._33;
  r.m._20 = m1.m._20*m2.m._00 + m1.m._21*m2.m._10 + m1.m._22*m2.m._20 + m1.m._23*m2.m._30;
  r.m._21 = m1.m._20*m2.m._01 + m1.m._21*m2.m._11 + m1.m._22*m2.m._21 + m1.m._23*m2.m._31;
  r.m._22 = m1.m._20*m2.m._02 + m1.m._21*m2.m._12 + m1.m._22*m2.m._22 + m1.m._23*m2.m._32;
  r.m._23 = m1.m._20*m2.m._03 + m1.m._21*m2.m._13 + m1.m._22*m2.m._23 + m1.m._23*m2.m._33;
  r.m._30 = m1.m._30*m2.m._00 + m1.m._31*m2.m._10 + m1.m._32*m2.m._20 + m1.m._33*m2.m._30;
  r.m._31 = m1.m._30*m2.m._01 + m1.m._31*m2.m._11 + m1.m._32*m2.m._21 + m1.m._33*m2.m._31;
  r.m._32 = m1.m._30*m2.m._02 + m1.m._31*m2.m._12 + m1.m._32*m2.m._22 + m1.m._33*m2.m._32;
  r.m._33 = m1.m._30*m2.m._03 + m1.m._31*m2.m._13 + m1.m._32*m2.m._23 + m1.m._33*m2.m._33;
  return r;
}

template <typename T>
udVector2<T> lerp(const udVector2<T> &v1, const udVector2<T> &v2, T t)
{
  udVector2<T> r;
  T invT = T(1)-t;
  r.x = v1.x*invT + v2.x*t;
  r.y = v1.y*invT + v2.y*t;
  return r;
}
template <typename T>
udVector3<T> lerp(const udVector3<T> &v1, const udVector3<T> &v2, T t)
{
  udVector3<T> r;
  T invT = T(1)-t;
  r.x = v1.x*invT + v2.x*t;
  r.y = v1.y*invT + v2.y*t;
  r.y = v1.z*invT + v2.z*t;
  return r;
}
template <typename T>
udVector4<T> lerp(const udVector4<T> &v1, const udVector4<T> &v2, T t)
{
  udVector4<T> r;
  T invT = T(1)-t;
  r.x = v1.x*invT + v2.x*t;
  r.y = v1.y*invT + v2.y*t;
  r.y = v1.z*invT + v2.z*t;
  r.w = v1.w*invT + v2.w*t;
  return r;
}
template <typename T>
udMatrix4x4<T> lerp(const udMatrix4x4<T> &m1, const udMatrix4x4<T> &m2, T t)
{
  return (T(1)-t)*m1 + t*m2;
}
template <typename T>
udQuaternion<T> lerp(const udQuaternion<T> &q1, const udQuaternion<T> &q2, T t)
{
  udQuaternion<T> r;
  T invT = T(1)-t;
  r.x = q1.x*invT + q2.x*t;
  r.y = q1.y*invT + q2.y*t;
  r.y = q1.z*invT + q2.z*t;
  r.w = q1.w*invT + q2.w*t;
  return r;
}

template <typename T>
udQuaternion<T> slerp(const udQuaternion<T> &q1, const udQuaternion<T> &q2, T t)
{
  udQuaternion<T> r;
  UDASSERT(false, "TODO");
  return r;
}

template <typename T>
udMatrix4x4<T> transpose(const udMatrix4x4<T> &m)
{
  udMatrix4x4<T> r = {
    m.a[0], m.a[4], m.a[8], m.a[12],
    m.a[1], m.a[5], m.a[9], m.a[13],
    m.a[2], m.a[6], m.a[10], m.a[14],
    m.a[3], m.a[7], m.a[11], m.a[15]
  };
  return r;
}

template <typename T>
T determinant(const udMatrix4x4<T> &m)
{
  return m.m._03*m.m._12*m.m._21*m.m._30 - m.m._02*m.m._13*m.m._21*m.m._30 - m.m._03*m.m._11*m.m._22*m.m._30 + m.m._01*m.m._13*m.m._22*m.m._30 +
         m.m._02*m.m._11*m.m._23*m.m._30 - m.m._01*m.m._12*m.m._23*m.m._30 - m.m._03*m.m._12*m.m._20*m.m._31 + m.m._02*m.m._13*m.m._20*m.m._31 +
         m.m._03*m.m._10*m.m._22*m.m._31 - m.m._00*m.m._13*m.m._22*m.m._31 - m.m._02*m.m._10*m.m._23*m.m._31 + m.m._00*m.m._12*m.m._23*m.m._31 +
         m.m._03*m.m._11*m.m._20*m.m._32 - m.m._01*m.m._13*m.m._20*m.m._32 - m.m._03*m.m._10*m.m._21*m.m._32 + m.m._00*m.m._13*m.m._21*m.m._32 +
         m.m._01*m.m._10*m.m._23*m.m._32 - m.m._00*m.m._11*m.m._23*m.m._32 - m.m._02*m.m._11*m.m._20*m.m._33 + m.m._01*m.m._12*m.m._20*m.m._33 +
         m.m._02*m.m._10*m.m._21*m.m._33 - m.m._00*m.m._12*m.m._21*m.m._33 - m.m._01*m.m._10*m.m._22*m.m._33 + m.m._00*m.m._11*m.m._22*m.m._33;
}

template <typename T>
udMatrix4x4<T> inverse(const udMatrix4x4<T> &m)
{
  udMatrix4x4<T> r;
  r.m._00 = m.m._12*m.m._23*m.m._31 - m.m._13*m.m._22*m.m._31 + m.m._13*m.m._21*m.m._32 - m.m._11*m.m._23*m.m._32 - m.m._12*m.m._21*m.m._33 + m.m._11*m.m._22*m.m._33;
  r.m._01 = m.m._03*m.m._22*m.m._31 - m.m._02*m.m._23*m.m._31 - m.m._03*m.m._21*m.m._32 + m.m._01*m.m._23*m.m._32 + m.m._02*m.m._21*m.m._33 - m.m._01*m.m._22*m.m._33;
  r.m._02 = m.m._02*m.m._13*m.m._31 - m.m._03*m.m._12*m.m._31 + m.m._03*m.m._11*m.m._32 - m.m._01*m.m._13*m.m._32 - m.m._02*m.m._11*m.m._33 + m.m._01*m.m._12*m.m._33;
  r.m._03 = m.m._03*m.m._12*m.m._21 - m.m._02*m.m._13*m.m._21 - m.m._03*m.m._11*m.m._22 + m.m._01*m.m._13*m.m._22 + m.m._02*m.m._11*m.m._23 - m.m._01*m.m._12*m.m._23;
  r.m._10 = m.m._13*m.m._22*m.m._30 - m.m._12*m.m._23*m.m._30 - m.m._13*m.m._20*m.m._32 + m.m._10*m.m._23*m.m._32 + m.m._12*m.m._20*m.m._33 - m.m._10*m.m._22*m.m._33;
  r.m._11 = m.m._02*m.m._23*m.m._30 - m.m._03*m.m._22*m.m._30 + m.m._03*m.m._20*m.m._32 - m.m._00*m.m._23*m.m._32 - m.m._02*m.m._20*m.m._33 + m.m._00*m.m._22*m.m._33;
  r.m._12 = m.m._03*m.m._12*m.m._30 - m.m._02*m.m._13*m.m._30 - m.m._03*m.m._10*m.m._32 + m.m._00*m.m._13*m.m._32 + m.m._02*m.m._10*m.m._33 - m.m._00*m.m._12*m.m._33;
  r.m._13 = m.m._02*m.m._13*m.m._20 - m.m._03*m.m._12*m.m._20 + m.m._03*m.m._10*m.m._22 - m.m._00*m.m._13*m.m._22 - m.m._02*m.m._10*m.m._23 + m.m._00*m.m._12*m.m._23;
  r.m._20 = m.m._11*m.m._23*m.m._30 - m.m._13*m.m._21*m.m._30 + m.m._13*m.m._20*m.m._31 - m.m._10*m.m._23*m.m._31 - m.m._11*m.m._20*m.m._33 + m.m._10*m.m._21*m.m._33;
  r.m._21 = m.m._03*m.m._21*m.m._30 - m.m._01*m.m._23*m.m._30 - m.m._03*m.m._20*m.m._31 + m.m._00*m.m._23*m.m._31 + m.m._01*m.m._20*m.m._33 - m.m._00*m.m._21*m.m._33;
  r.m._22 = m.m._01*m.m._13*m.m._30 - m.m._03*m.m._11*m.m._30 + m.m._03*m.m._10*m.m._31 - m.m._00*m.m._13*m.m._31 - m.m._01*m.m._10*m.m._33 + m.m._00*m.m._11*m.m._33;
  r.m._23 = m.m._03*m.m._11*m.m._20 - m.m._01*m.m._13*m.m._20 - m.m._03*m.m._10*m.m._21 + m.m._00*m.m._13*m.m._21 + m.m._01*m.m._10*m.m._23 - m.m._00*m.m._11*m.m._23;
  r.m._30 = m.m._12*m.m._21*m.m._30 - m.m._11*m.m._22*m.m._30 - m.m._12*m.m._20*m.m._31 + m.m._10*m.m._22*m.m._31 + m.m._11*m.m._20*m.m._32 - m.m._10*m.m._21*m.m._32;
  r.m._31 = m.m._01*m.m._22*m.m._30 - m.m._02*m.m._21*m.m._30 + m.m._02*m.m._20*m.m._31 - m.m._00*m.m._22*m.m._31 - m.m._01*m.m._20*m.m._32 + m.m._00*m.m._21*m.m._32;
  r.m._32 = m.m._02*m.m._11*m.m._30 - m.m._01*m.m._12*m.m._30 - m.m._02*m.m._10*m.m._31 + m.m._00*m.m._12*m.m._31 + m.m._01*m.m._10*m.m._32 - m.m._00*m.m._11*m.m._32;
  r.m._33 = m.m._01*m.m._12*m.m._20 - m.m._02*m.m._11*m.m._20 + m.m._02*m.m._10*m.m._21 - m.m._00*m.m._12*m.m._21 - m.m._01*m.m._10*m.m._22 + m.m._00*m.m._11*m.m._22;
  return r*(T(1)/determinant(r));
}
template <typename T>
udQuaternion<T> inverse(const udQuaternion<T> &q)
{
  udQuaternion<T> r;
  UDASSERT(false, "TODO");
  return r;
}

template <typename T>
udQuaternion<T> conjugate(const udQuaternion<T> &q)
{
  udQuaternion<T> r;
  UDASSERT(false, "TODO");
  return r;
}

// udMatrix4x4 members
template <typename T>
udMatrix4x4<T>& udMatrix4x4<T>::transpose()
{
  *this = ::transpose(*this);
  return *this;
}

template <typename T>
T udMatrix4x4<T>::determinant()
{
  return ::determinant(*this);
}

template <typename T>
udMatrix4x4<T>& udMatrix4x4<T>::inverse()
{
  *this = ::inverse(*this);
  return *this;
}

// udQuaternion members
template <typename T>
udQuaternion<T>& udQuaternion<T>::inverse()
{
  *this = ::inverse(*this);
  return *this;
}

template <typename T>
udQuaternion<T>& udQuaternion<T>::conjugate()
{
  *this = ::conjugate(*this);
  return *this;
}

template <typename T>
udVector3<T> udQuaternion<T>::apply(const udVector3<T> &v)
{
  udVector3<T> r;
  UDASSERT(false, "TODO");
  return r;
}

// udMatrix4x4 initialisers
template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::identity()
{
  udMatrix4x4<T> r = { T(1),T(0),T(0),T(0),
                       T(0),T(1),T(0),T(0),
                       T(0),T(0),T(1),T(0),
                       T(0),T(0),T(0),T(1) };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::create(const T m[16])
{
  udMatrix4x4<T> r = { m[0], m[1], m[2], m[3],
                       m[4], m[5], m[6], m[7],
                       m[8], m[9], m[10],m[11],
                       m[12],m[13],m[14],m[15] };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::create(T _00, T _10, T _20, T _30, T _01, T _11, T _21, T _31, T _02, T _12, T _22, T _32, T _03, T _13, T _23, T _33)
{
  udMatrix4x4<T> r = { _00, _10, _20, _30,  // NOTE: remember, this looks a bit funny because we store columns (axiis) contiguous!
                       _01, _11, _21, _31,
                       _02, _12, _22, _32,
                       _03, _13, _23, _33 };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::create(const udVector4<T> &xColumn, const udVector4<T> &yColumn, const udVector4<T> &zColumn, const udVector4<T> &wColumn)
{
  udMatrix4x4<T> r = { xColumn.x, xColumn.y, xColumn.z, xColumn.w,
                       yColumn.x, yColumn.y, yColumn.z, yColumn.w,
                       zColumn.x, zColumn.y, zColumn.z, zColumn.w,
                       wColumn.x, wColumn.y, wColumn.z, wColumn.w };
  return r;
}

template <typename T>
template <typename U> // OMG, nested templates... I didn't even know this was a thing!
udMatrix4x4<T> udMatrix4x4<T>::create(const udMatrix4x4<U> &_m)
{
  udMatrix4x4<T> r = { T(_m.m._00), T(_m.m._10), T(_m.m._20), T(_m.m._30),
                       T(_m.m._01), T(_m.m._11), T(_m.m._21), T(_m.m._31),
                       T(_m.m._02), T(_m.m._12), T(_m.m._22), T(_m.m._32),
                       T(_m.m._03), T(_m.m._13), T(_m.m._23), T(_m.m._33) };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::rotationX(T rad, const udVector3<T> &t)
{
  T c = udCos(rad);
  T s = udSin(rad);
  udMatrix4x4<T> r = { T(1),T(0),T(0),T(0),
                       T(0),  c ,  s ,T(0),
                       T(0), -s ,  c ,T(0),
                       t.x, t.y, t.z, T(1) };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::rotationY(T rad, const udVector3<T> &t)
{
  T c = udCos(rad);
  T s = udSin(rad);
  udMatrix4x4<T> r = {   c ,T(0), -s,T(0),
                       T(0),T(1),T(0),T(0),
                         s ,T(0),  c ,T(0),
                       t.x, t.y, t.z, T(1) };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::rotationZ(T rad, const udVector3<T> &t)
{
  T c = udCos(rad);
  T s = udSin(rad);
  udMatrix4x4<T> r = {   c ,  s ,T(0),T(0),
                        -s ,  c ,T(0),T(0),
                       T(0),T(0),T(1),T(0),
                       t.x, t.y, t.z, T(1) };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::rotationAxis(const udVector3<T> &axis, T rad, const udVector3<T> &t)
{
  T c = cos(rad);
  T s = sin(rad);

  udVector3<T> n = normalize3(axis);
  udVector3<T> a = (T(1) - c) * axis;

  udMatrix4x4<T> r = {
    a.x*n.x + c,     a.x*n.y + s*n.z, a.x*n.z - s*n.y, T(0),
    a.y*n.x - s*n.z, a.y*n.y + c,     a.y*n.z + s*n.x, T(0),
    a.z*n.x + s*n.y, a.z*n.y - s*n.x, a.z*n.z + c,     T(0),
    t.x,             t.y,             t.z,             T(1) };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::rotationPYR(const udVector3<T> &pyr, const udVector3<T> &t)
{
  udMatrix4x4<T> r = translation(t);
  UDASSERT(false, "TODO");
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::rotationQ(const udQuaternion<T> &q, const udVector3<T> &t)
{
  udMatrix4x4<T> r = translation(t);
  UDASSERT(false, "TODO");
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::translation(const udVector3<T> &t)
{
  udMatrix4x4<T> r = { T(1),T(0),T(0),T(0),
                       T(0),T(1),T(0),T(0),
                       T(0),T(0),T(1),T(0),
                       t.x, t.y, t.z, T(1) };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::perspective(T fovY, T aspectRatio, T near, T far)
{
  float fov = udTan(fovY / T(2));
  udMatrix4x4<T> r = { T(1)/(aspectRatio*fov), T(0),         T(0),                            T(0),
                       T(0),                   T(0),         (far + near) / (far - near),     T(1),
                       T(0),                   T(1)/fov,     T(0),                            T(0),
                       T(0),                   T(0),         -(T(2)*far*near) / (far - near), T(0) };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::ortho(T left, T right, T bottom, T top, T near, T far)
{
  udMatrix4x4<T> r = { T(2) / (right - left),            T(0),                             T(0),                         T(0),
                       T(0),                             T(0),                             T(2) / (far - near),          T(0),
                       T(0),                             T(2) / (top - bottom),            T(0),                         T(0),
                       -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), T(1) };
  return r;
}

template <typename T>
udMatrix4x4<T> udMatrix4x4<T>::orthoForScreeen(T width, T height, T near, T far)
{
  udMatrix4x4<T> r = { T(2) / width, T(0),           T(0),                         T(0),
                       T(0),         -T(2) / height, T(0),                         T(0),
                       T(0),         T(0),           T(2) / (far - near),          T(0),
                       T(-1),        T(1),          -(far + near) / (far - near), T(1) };
  return r;
}


template <typename T>
udQuaternion<T> udQuaternion<T>::create(const udVector3<T> &axis, T rad)
{
  udQuaternion<T> r;
  UDASSERT(false, "TODO");
  return r;
}