#pragma once

struct Vector3
{
	float x;
	float y;
	float z;
};


inline Vector3 operator+(Vector3 v, float s)
{
	v.x += s;
	v.y += s;
	v.z += s;

	return v;
}
inline Vector3 operator-(Vector3 v, float s)
{
	v.x -= s;
	v.y -= s;
	v.z -= s;

	return v;
}
inline Vector3 operator*(Vector3 v, float s)
{
	v.x *= s;
	v.y *= s;
	v.z *= s;

	return v;
}
inline Vector3 operator/(Vector3 v, float s)
{
	v.x /= s;
	v.y /= s;
	v.z /= s;

	return v;
}
inline Vector3& operator+=(Vector3& v, float s)
{

	v.x += s;
	v.y += s;
	v.z += s;

	return v;
}
inline Vector3& operator-=(Vector3& v, float s)
{

	v.x -= s;
	v.y -= s;
	v.z -= s;

	return v;
}
inline Vector3& operator*=(Vector3& v, float s)
{

	v.x *= s;
	v.y *= s;
	v.z *= s;

	return v;
}
inline Vector3& operator/=(Vector3& v, float s)
{

	v.x /= s;
	v.y /= s;
	v.z /= s;

	return v;
}

inline Vector3 operator+(float s,Vector3 v)
{
	v.x += s;
	v.y += s;
	v.z += s;

	return v;
}
inline Vector3 operator*(float s,Vector3 v)
{
	v.x *= s;
	v.y *= s;
	v.z *= s;

	return v;
}

inline Vector3 operator+(Vector3 a, const Vector3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return a;

}
inline Vector3 operator-(Vector3 a, const Vector3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;

	return a;
}
inline Vector3 operator*(Vector3 a, const Vector3& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;

	return a;
}
inline Vector3 operator/(Vector3 a, const Vector3& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;

	return a;
}
inline Vector3& operator+=(Vector3& a, const Vector3& b)
{

	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return a;
}
inline Vector3& operator-=(Vector3& a, const Vector3& b)
{

	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;

	return a;
}
inline Vector3& operator*=(Vector3& a, const Vector3& b)
{

	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;

	return a;
}
inline Vector3& operator/=(Vector3& a, const Vector3& b)
{

	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;

	return a;
}