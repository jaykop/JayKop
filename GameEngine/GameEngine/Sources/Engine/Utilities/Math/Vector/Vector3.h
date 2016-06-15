#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include <iostream>

template <typename Type>
class Matrix4x4;

template <typename Type>
class Vector4;

template <typename Type>
class Vector3
{
public:

	//Vector3(void);
	Vector3(Type _x = 0, Type _y = 0, Type _z = 0);
	Vector3(const Vector3<Type>& copy);
	Vector3(const Vector4<Type>& copy);
	~Vector3(void);

	Vector3& operator=(const Vector3& rhs);
	Vector3& operator-(void);
	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator+=(const Type& constant);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator-=(const Type& constant);
	Vector3& operator*=(const Type& constant);

	Vector3 operator+(const Type& constant) const;
	Vector3 operator+(const Vector3& rhs) const;
	
	Vector3 operator-(const Type& constant) const;
	Vector3 operator-(const Vector3& rhs) const;

	Vector3 operator*(const Type& constant) const;
	Vector3 operator/(const Type& constant) const;

	Type Length(void) const;
	Type DotProduct(const Vector3& rhs);
	Vector3<Type> CrossProduct(const Vector3& rhs);
	Vector3<Type> Normalize(void);
	Vector3<Type> Reflection(Vector3 rhs);

	Vector3<Type> Absolute(void);
	Vector3<Type> Rotation(float angle, const Vector3& pivot);
	Vector3<Type> Rotation(float angle);
	float IncludedAngle(const Vector3& other);

	template <typename Type>
	friend Vector3<Type> operator+(Type constant, const Vector3<Type>& rhs);

	template <typename Type>
	friend Vector3<Type> operator*(Type constant, const Vector3<Type>& rhs);

	template <typename Type>
	friend std::ostream& operator<<(std::ostream& os, const Vector3<Type>& constents);

	Type x, y, z;

private:
	
};

#include "Vector3.cpp"

#endif // _VECTOR3_H_