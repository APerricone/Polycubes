/*! \file primitives.h
 *	This file contains all declarations of primitive geometric classes.
 *	This classes are the base for calculations of graphics and physics.
 *  \author	Antonino Perricone
 *  \date	Saturday, January 25th, 2003
 */

#ifndef __PRIMITIVES__
#define __PRIMITIVES__

class Color;
class V3D;
class V2D;
class Box3;
class Box2;
class Matrix;
class Quaternion;
struct TV; //tex-pos
struct NV; //nor-pos
struct TNV; //tex-nor-pos
struct CV; //col-pos
struct TCV; //tex-col-pos

#include <math.h>

//! if defined all boolean operation between float include an error
#define USE_EPSILON

//! possible error for boolean operation between float
#define EPSILON 0.001f

/*!	\def FLOATEQUAL(a,b)
 *	It compares 2 float,return true if they are same.<br>
 *	If USE_EPSILON defines, the float value can differ by EPSILON.
 */
/*!	\def FLOATDIFF(a,b)
 *	It compares 2 float,return true if they are different.<br>
 *	If USE_EPSILON defines, the float value must differ by EPSILON.
 */
#ifdef USE_EPSILON
#define FLOATEQUAL(a,b) (fabsf(a-b)<=EPSILON)
#define FLOATDIFF(a,b) (fabsf(a-b)>EPSILON)
#else
#define FLOATEQUAL(a,b) (a==b)
#define FLOATDIFF(a,b) (a!=b)
#endif

//! a color in a variable
class Color
{
public:
	/*! \var unsigned char r
	    Red
	 */
	/*! \var unsigned char g
	    Green
	 */
	/*! \var unsigned char b
	    Blue
	 */
	/*! \var unsigned char a
	    Alpha
	 */
	unsigned char r,g,b,a;
	//!@{
	//!@name Setting operators
	//!	set to transparent black
	Color() { r=g=b=a=0; }
	//! Set to (_r,_g,_b,_a)
	Color(const unsigned char _r,const unsigned char _g,const unsigned char _b,const unsigned char _a) {r=_r; g=_g; b=_b; a=_a; }
	//! Reset to (_r,_g,_b,_a)
	inline Color& Set(const unsigned char _r,const unsigned char _g,const unsigned char _b,const unsigned char _a) {r=_r; g=_g; b=_b; a=_a; return *this;}
	//! Set the color same to c
	inline Color& operator=(const Color &c) { r=c.r; g=c.g; b=c.b; a=c.a; return *this; }
	//!@}
	//!@{
	//!@name boolean operations
	//! Equality
	friend inline bool operator==(const Color &a,const Color &b) { return ((a.r==b.r)&&(a.g==b.g)&&(a.b==b.b)&&(a.a==b.a)); };
	//! Diversity
	friend inline bool operator!=(const Color &a,const Color &b) { return ((a.r!=b.r)||(a.g!=b.g)||(a.b!=b.b)||(a.a!=b.a)); };
	//!@}

	//! Scale operator for a scalar in right
	friend inline Color operator *(const Color &a,const float &b) { return Color((unsigned char)(a.r*b),(unsigned char)(a.g*b),(unsigned char)(a.b*b),(unsigned char)(a.a*b)); }
	//! Scale operator for a scalar in left
	friend inline Color operator *(const float &b,const Color &a) { return Color((unsigned char)(a.r*b),(unsigned char)(a.g*b),(unsigned char)(a.b*b),(unsigned char)(a.a*b)); }
};

//! White
const Color cWhite(255,255,255,255);
//! Black
const Color cBlack(0,0,0,255);
//! 50% Gray
const Color cGray(128,128,128,255);

//! 3D vector
class V3D
{
public:
	/*! \var float x
	    X coordinate
	 */
	/*! \var float y
	    Y coordinate
	 */
	/*! \var float z
	    Z coordinate
	 */
	float x,y,z;
	//! [] opeators to use in loop for
	float &operator[](int i) { return (i==0)? x :(i==1)? y : z;}
	const float operator[](int i) const { return (i==0)? x :(i==1)? y : z;}
	//!@{
	//!@name Setting operators
	//! Set to origin
	V3D() { x=y=z=0.f; }
	//! Set to (_x,_y,_z)
	V3D(const float _x,const float _y,const float _z) { x=_x; y=_y; z=_z; }
	//! Re-set to (_x,_y,_z)
	inline V3D& Set(const float _x,const float _y,const float _z) { x=_x; y=_y; z=_z; return *this; }
	//! Set same as #b
	inline V3D& operator=(const V3D &b) { x=b.x; y=b.y; z=b.z; return *this; }
	//!@}
	//!@{
	//!@name Addition operations
	//! Sum member to member between 3D Vector
	friend inline V3D operator +(const V3D &a,const V3D &b) { return V3D(a.x+b.x,a.y+b.y,a.z+b.z); }
	//! 3D Vector negation
	friend inline V3D operator -(const V3D &a) { return V3D(-a.x,-a.y,-a.z); }
	//! Subtraction member to member between 3D Vector
	friend inline V3D operator -(const V3D &a,const V3D &b) { return V3D(a.x-b.x,a.y-b.y,a.z-b.z); }
	//! Sum member to member between 3D Vector, the result sets on first operator
	inline V3D& operator +=(const V3D &b) { x+=b.x; y+=b.y; z+=b.z; return *this; }
	//! Subtraction member to member between 3D Vector, the result sets on first operator
	inline V3D& operator -=(const V3D &b) { x-=b.x; y-=b.y; z-=b.z; return *this; }
	//!@}
	//!@{
	//!@name Multiplay operations
	//! Scale operator for a scalar in right
	friend inline V3D operator *(const V3D &a,const float &b) { return V3D(a.x*b,a.y*b,a.z*b); }
	//! Scale operator for a scalar in left
	friend inline V3D operator *(const float &a,const V3D &b) { return V3D(a*b.x,a*b.y,a*b.z); }
	//!@}
	//! Dot product between 3D Vector
	friend inline float operator *(const V3D &a,const V3D &b) { return (a.x*b.x+a.y*b.y+a.z*b.z); }
	//! Scale of a 3D Vector
	inline V3D& operator *=(const float &b) { x*=b; y*=b; z*=b; return *this; }
	//!@}
	//!@{
	//!@name Division by scalar
	//! Return the 3D vector scalated, divisioning
	friend inline V3D operator /(const V3D &a,const float &b) { return V3D(a.x/b,a.y/b,a.z/b); }
	//! Scale the 3D vector, divisioning
	inline V3D& operator /=(const float &b) { x/=b; y/=b; z/=b; return *this; }
	//!@}
	//!@{
	//!@name Cross product
	//! Cross product between 3D Vector
	friend inline V3D operator ^(const V3D &a,const V3D &b) { return V3D(
		a.y*b.z-a.z*b.y,
		a.z*b.x-a.x*b.z,
		a.x*b.y-a.y*b.x); };
	//! Cross product between 3D Vector, the result sets on first operator
	inline V3D& operator ^=(const V3D &b) { V3D t=*this; *this=t^b; return *this; }
	//! Product member to member between 3D Vector
	inline V3D MtM(const V3D &b) { return V3D(x*b.x,y*b.y,z*b.z); };
	//!@}
	//!@{
	//!@name boolean operations
	//! Equality
	friend inline bool operator==(const V3D &a,const V3D &b) { return (FLOATEQUAL(a.x,b.x)&&FLOATEQUAL(a.y,b.y)&&FLOATEQUAL(a.z,b.z)); };
	//! Diversity
	friend inline bool operator!=(const V3D &a,const V3D &b) { return (FLOATDIFF(a.x,b.x)||FLOATDIFF(a.y,b.y)||FLOATDIFF(a.z,b.z)); };
	//!@}
	//!@{
	//!@name Heavy operation
	//! return the lenght of vector
	inline float GetLen() const { return sqrtf((*this)*(*this)); };
	//! Set the lenght of vector to 1
	inline float Normalize() { float l=GetLen(); *this/=l; return l; };
	//!@}
};

//! 3D Axes origin
const V3D zero3(0,0,0);
//! 3D X axe
const V3D axesX3(1,0,0);
//! 3D Y axe
const V3D axesY3(0,1,0);
//! 3D Z axes
const V3D axesZ3(0,0,1);

//! 2D vector
class V2D
{
public:
	/*! \var float x
	    X coordinate
	 */
	/*! \var float y
	    Y coordinate
	 */
	float x,y;
	//! [] opeators to use in loop for
	float &operator[](int i) { return (i==0)? x : y;}
	const float operator[](int i) const { return (i==0)? x : y; }
	//!@{
	//!@name Setting operators
	//! Set to origin
	V2D() { x=y=0.f; }
	//! Set to (_x,_y) 
	V2D(const float _x,const float _y) { x=_x; y=_y; }
	//! Re-set to (_x,_y) 
	inline V2D& Set(const float _x,const float _y) { x=_x; y=_y; return *this; }
	//! Set same as #b
	inline V2D& operator=(const V2D &b) { x=b.x; y=b.y; return *this; }
	//!@}
	//!@{
	//!@name Addition operations
	//! Sum member to member between 2D Vector
	friend inline V2D operator +(const V2D &a,const V2D &b) { return V2D(a.x+b.x,a.y+b.y); }
	//! 2D Vector negation
	friend inline V2D operator -(const V2D &a) { return V2D(-a.x,-a.y); }
	//! Subtraction member to member between 2D Vector
	friend inline V2D operator -(const V2D &a,const V2D &b) { return V2D(a.x-b.x,a.y-b.y); }
	//! Sum member to member between 2D Vector, the result sets on first operator
	inline V2D& operator +=(const V2D &b) { x+=b.x; y+=b.y; return *this; }
	//! Subtraction member to member between 2D Vector, the result sets on first operator
	inline V2D& operator -=(const V2D &b) { x-=b.x; y-=b.y; return *this; }
	//!@}
	//!@{
	//!@name Multiplay operations
	//! Scale operator for a scalar in right
	friend inline V2D operator *(const V2D &a,const float &b) { return V2D(a.x*b,a.y*b); }
	//! Scale operator for a scalar in left
	friend inline V2D operator *(const float &a,const V2D &b) { return V2D(a*b.x,a*b.y); }
	//! Return the 3D vector scalated, divisioning
	friend inline V2D operator /(const V2D &a,const float &b) { return V2D(a.x/b,a.y/b); }
	//! Scale of a 2D Vector
	inline V2D& operator *=(const float &b) { x*=b; y*=b; return *this; }
	//! Scale the 2D vector, divisioning
	inline V2D& operator /=(const float &b) { x/=b; y/=b; return *this; }
	//!@}
	//! Dot product between 2D Vector
	friend inline float operator *(const V2D &a,const V2D &b) { return (a.x*b.x+a.y*b.y); }
	//! Product member to member between 2D Vector
	inline V2D MtM(const V2D &b) { return V2D(x*b.x,y*b.y); };
	//!@{
	//!@name boolean operations
	//! Equality
	friend inline bool operator==(const V2D &a,const V2D &b) { return (FLOATEQUAL(a.x,b.x)&&FLOATEQUAL(a.y,b.y)); };
	//! Diversity
	friend inline bool operator!=(const V2D &a,const V2D &b) { return (FLOATDIFF(a.x,b.x)&&FLOATDIFF(a.y,b.y)); };
	//!@}
	//!@{
	//!@name Heavy operation
	//! return the lenght of vector
	inline float GetLen() const { return sqrtf((*this)*(*this)); };
	//! Set the lenght of vector to 1
	inline float Normalize() { float l=GetLen(); *this/=l; return l; };
	//!@}
};

//! 2D Axes origin
const V2D zero2(0,0);
//! 2D X axe
const V2D axesX2(1,0);
//! 2D Y axe
const V2D axesY2(0,1);

//! Managing of a 3D container
class Box3
{
public:
	//! Inferior corner
	V3D inf;
	//! Superior corner
	V3D sup;
	
	//! Construction that sets to invalid the box (#inf>#sup)
	Box3() { inf.Set(1e8,1e8,1e8); sup.Set(-1e8,-1e8,-1e8); }
	//! Construction that sets the box
	Box3(const V3D &_i,const V3D &_s) : inf(_i),sup(_s) {};

	//! Check the validity of box
	//! \return true if the inferior corner is below of superior
	bool IsValid() const { return ((inf.x<=sup.x)&&(inf.y<=sup.y)&&(inf.z<=sup.z)); }
	//! Check if a point is inside the box
	//! \param p point to check
	//! \return true if the point is inside
	bool IsInside(const V3D &p) const { return ((p.x>=inf.x)&&(p.x<=sup.x)&&(p.y>=inf.y)&&(p.y<=sup.y)&&(p.z>=inf.z)&&(p.z<=sup.z)); }

	//! Sets to invalid the box (#inf>#sup)
	void Reset() { inf.Set(1e8,1e8,1e8); sup.Set(-1e8,-1e8,-1e8); }
	//! copy of box
	inline Box3& operator=(const Box3 &b) { inf=b.inf; sup=b.sup; return *this; }
	//! set of a box like a point
	inline Box3& operator=(const V3D &b) { inf=sup=b; return *this; }
	//!  Set of a box
	inline Box3& Set(const V3D &i,const V3D &s) { inf=i; sup=s; return *this; }
	//! Change the box for container the point
	//! \param p point to container
	void Add(const V3D &p)
	{
		if(p.x<inf.x) inf.x=p.x; if(p.x>sup.x) sup.x=p.x;
		if(p.y<inf.y) inf.y=p.y; if(p.y>sup.y) sup.y=p.y;
		if(p.z<inf.z) inf.z=p.z; if(p.z>sup.z) sup.z=p.z;
	}
	//! Change the box for container the box
	//! \param p box to container
	void Add(const Box3 &b) { Add(b.inf); Add(b.sup); }

	//! [] opeators to use in loop for, return all 8 corner in sequency
	V3D operator[](int i) const { return V3D(i%2? sup.x : inf.x, (i/2)%2? sup.y : inf.y, (i/4)%2? sup.z : inf.z); }
};

//! Managing of a 2D container
class Box2
{
public:
	//! Inferior corner
	V2D inf;
	//! Superior corner
	V2D sup;

	//! Construction that sets to invalid the box (#inf>#sup)
	Box2() { inf.Set(1e8,1e8); sup.Set(-1e8,-1e8); }
	//! Construction that sets the box
	Box2(const V2D &_i,const V2D &_s) : inf(_i),sup(_s) {};

	//! Check the validity of box
	//! \return true if the inferior corner is below of superior
	bool IsValid() const { return ((inf.x<=sup.x)&&(inf.y<=sup.y)); }
	//! Check if a point is inside the box
	//! \param p point to check
	//! \return true if the point is inside
	bool IsInside(const V2D &p) const { return ((p.x>=inf.x)&&(p.x<=sup.x)&&(p.y>=inf.y)&&(p.y<=sup.y)); }

	//! Sets to invalid the box (#inf>#sup)
	void Reset() { inf.Set(1e8,1e8); sup.Set(-1e8,-1e8); }
	//! copy of box
	inline Box2& operator=(const Box2 &b) { inf=b.inf; sup=b.sup; return *this; }
	//! set of a box like a point
	inline Box2& operator=(const V2D &b) { inf=sup=b; return *this; }
	//!  Set of a box
	inline Box2& Set(const V2D &i,const V2D &s) { inf=i; sup=s; return *this; }

	//! Change the box for container the point
	//! \param p point to container
	void Add(const V2D &p)
	{
		if(p.x<inf.x) inf.x=p.x; if(p.x>sup.x) sup.x=p.x;
		if(p.y<inf.y) inf.y=p.y; if(p.y>sup.y) sup.y=p.y;
	}

	//! Change the box for container the box
	//! \param p box to container
	void Add(const Box2 &b) { Add(b.inf); Add(b.sup); }

	//! [] opeators to use in loop for, return all 4 corner in sequency
	V2D operator[](int i) const { return V2D(i%2? sup.x : inf.x, i/2? sup.y : inf.y); }
};

//! Return of determinant of a 3x3 matrix
inline float determinant33(float m11,float m12,float m13,float m21,float m22,float m23,float m31,float m32,float m33)
	{ return
		m11*m22*m33+
		m12*m23*m31+
		m13*m21*m32-
		m11*m23*m32-
		m12*m21*m33-
		m13*m22*m31; }

//! Base class for quaternion
//! \sa Quaternion
class QBase
{
public:
	//! Real component
	float s;
	//! Imaginary component
	V3D v;
};

//! Managing of a 4x4 matrix
class Matrix
{
public:
	
	//!@{
	//! elements
	float m11,m12,m13,m14,m21,m22,m23,m24,m31,m32,m33,m34,m41,m42,m43,m44;
	//!@}
	//! return of rows
	float *operator[](int i)
		{ return (i==0)? &m11 : (i==1)? &m21 : (i==2)? &m31 : (i==3)? &m41 : 0; }
	//! return of rows consts
	const float *operator[](int i) const
		{ return (i==0)? &m11 : (i==1)? &m21 : (i==2)? &m31 : (i==3)? &m41 : 0; }
	//! right vector, first row
	V3D &Right() { return *((V3D*)&m11); }
	//! Up vector, second row
	V3D &Up() { return *((V3D*)&m21); }
	//! At vector, third row
	V3D &At() { return *((V3D*)&m31); }
	//! pos vector, fourth row
	V3D &Pos() { return *((V3D*)&m41); }
	//! Construction that sets to identity
	Matrix() { m12=m13=m14=m21=m23=m24=m31=m32=m34=m41=m42=m43=0.f; m11=m22=m33=m44=1.f; }
	//! Construction that sets all values
	Matrix(	const float _11,const float _12,const float _13,const float _14,
			const float _21,const float _22,const float _23,const float _24,
			const float _31,const float _32,const float _33,const float _34,
			const float _41,const float _42,const float _43,const float _44) {
			m11=_11; m12=_12; m13=_13; m14=_14;
			m21=_21; m22=_22; m23=_23; m24=_24;
			m31=_31; m32=_32; m33=_33; m34=_34;
			m41=_41; m42=_42; m43=_43; m44=_44; }
	//! Set of all values
	inline Matrix& Set( const float _11,const float _12,const float _13,const float _14,
						const float _21,const float _22,const float _23,const float _24,
						const float _31,const float _32,const float _33,const float _34,
						const float _41,const float _42,const float _43,const float _44) {
						m11=_11; m12=_12; m13=_13; m14=_14;
						m21=_21; m22=_22; m23=_23; m24=_24;
						m31=_31; m32=_32; m33=_33; m34=_34;
						m41=_41; m42=_42; m43=_43; m44=_44; return *this; }

	//! copy between matrices
	inline Matrix& operator=(const Matrix &b) {
						m11=b.m11; m12=b.m12; m13=b.m13; m14=b.m14;
						m21=b.m21; m22=b.m22; m23=b.m23; m24=b.m24;
						m31=b.m31; m32=b.m32; m33=b.m33; m34=b.m34;
						m41=b.m41; m42=b.m42; m43=b.m43; m44=b.m44; return *this;} //usare memcpy?

	//! Sets to identity
	inline Matrix& Reset() { m12=m13=m14=m21=m23=m24=m31=m32=m34=m41=m42=m43=0.f; m11=m22=m33=m44=1.f; return *this;}

	//! Sum member to member between matrices
	friend inline Matrix operator+(const Matrix &a,const Matrix &b) { return Matrix(
		a.m11+b.m11,a.m12+b.m12,a.m13+b.m13,a.m14+b.m14,
		a.m21+b.m21,a.m22+b.m22,a.m23+b.m23,a.m24+b.m24,
		a.m31+b.m31,a.m32+b.m32,a.m33+b.m33,a.m34+b.m34,
		a.m41+b.m41,a.m42+b.m42,a.m43+b.m43,a.m44+b.m44); }
	//! Subtraction member to member between matrices
	friend inline Matrix operator-(const Matrix &a,const Matrix &b) { return Matrix(
		a.m11-b.m11,a.m12-b.m12,a.m13-b.m13,a.m14-b.m14,
		a.m21-b.m21,a.m22-b.m22,a.m23-b.m23,a.m24-b.m24,
		a.m31-b.m31,a.m32-b.m32,a.m33-b.m33,a.m34-b.m34,
		a.m41-b.m41,a.m42-b.m42,a.m43-b.m43,a.m44-b.m44); }
	//! Scale member to member for a matrix
	friend inline Matrix operator*(const Matrix &a,const float &b) { return Matrix(
		a.m11*b,a.m12*b,a.m13*b,a.m14*b,
		a.m21*b,a.m22*b,a.m23*b,a.m24*b,
		a.m31*b,a.m32*b,a.m33*b,a.m34*b,
		a.m41*b,a.m42*b,a.m43*b,a.m44*b); }
	//! multiplication between matrices
	friend inline Matrix operator*(const Matrix &a,const Matrix &b) { return Matrix(
		a.m11*b.m11+a.m12*b.m21+a.m13*b.m31+a.m14*b.m41,
		a.m11*b.m12+a.m12*b.m22+a.m13*b.m32+a.m14*b.m42,
		a.m11*b.m13+a.m12*b.m23+a.m13*b.m33+a.m14*b.m43,
		a.m11*b.m14+a.m12*b.m24+a.m13*b.m34+a.m14*b.m44,

		a.m21*b.m11+a.m22*b.m21+a.m23*b.m31+a.m24*b.m41,
		a.m21*b.m12+a.m22*b.m22+a.m23*b.m32+a.m24*b.m42,
		a.m21*b.m13+a.m22*b.m23+a.m23*b.m33+a.m24*b.m43,
		a.m21*b.m14+a.m22*b.m24+a.m23*b.m34+a.m24*b.m44,

		a.m31*b.m11+a.m32*b.m21+a.m33*b.m31+a.m34*b.m41,
		a.m31*b.m12+a.m32*b.m22+a.m33*b.m32+a.m34*b.m42,
		a.m31*b.m13+a.m32*b.m23+a.m33*b.m33+a.m34*b.m43,
		a.m31*b.m14+a.m32*b.m24+a.m33*b.m34+a.m34*b.m44,

		a.m41*b.m11+a.m42*b.m21+a.m43*b.m31+a.m44*b.m41,
		a.m41*b.m12+a.m42*b.m22+a.m43*b.m32+a.m44*b.m42,
		a.m41*b.m13+a.m42*b.m23+a.m43*b.m33+a.m44*b.m43,
		a.m41*b.m14+a.m42*b.m24+a.m43*b.m34+a.m44*b.m44); }

	//! return the Transpose matrix
	inline Matrix GetTranspose() const { return Matrix(
		m11,m21,m31,m41,
		m12,m22,m32,m42,
		m13,m23,m33,m43,
		m14,m24,m34,m44); }

	//! Return of determinant 
	inline float determinant() const { return
		m11*m22*m33*m44+
		m12*m23*m34*m41+
		m13*m24*m31*m42+
		m14*m21*m32*m43-
		m11*m24*m33*m42-
		m12*m21*m34*m43-
		m13*m22*m31*m44-
		m14*m23*m32*m41; }

	//! return the invert matrix
	inline Matrix GetInvert() const
	{
		unsigned char i,j,i0,i1,j0,j1;
		Matrix b;
		for(i=0;i<3;i++)
			for(j=0;j<3;j++)
			{
				i0 = (i + 1) % 3;
				i1 = (i + 2) % 3;
				j0 = (j + 1) % 3;
				j1 = (j + 2) % 3;
				b[j][i] =	operator[](i0)[j0]*operator[](i1)[j1]-
							operator[](i0)[j1]*operator[](i1)[j0];
			}

		float d=determinant33
			(m11,m12,m13,
			 m21,m22,m23,
			 m31,m32,m33);
		if(fabsf(d)<EPSILON) return *this;
		d=1/d;
		for(i=0;i<3;i++)
			for(j=0;j<3;j++)
				b[j][i]*=d;
		// COMPUTE INVERSE TRANSLATION
		b[3][0]=-b[0][0]*m41-b[1][0]*m42-b[2][0]*m43;
		b[3][1]=-b[0][1]*m41-b[1][1]*m42-b[2][1]*m43;
		b[3][2]=-b[0][2]*m41-b[1][2]*m42-b[2][2]*m43;

		/*for(i = 0; i < 3; i++) IT DOES NOT NEED SCALE
			dest->m[3][i]=-dest->m[0][i] * m[3][0] - dest->m[1][i] * m[3][1] - dest->m[2][i] * m[3][2];
		*/
		return b;
/*		float d=determinant(); if(d<EPSILON) return *this; NOT GOOD CODE
		d=1/d;
		return Matrix(
			d*determinant33(m22,m23,m24,m32,m33,m34,m42,m43,m44),
			d*determinant33(m21,m23,m24,m31,m33,m34,m41,m43,m44),
			d*determinant33(m21,m22,m24,m31,m32,m34,m41,m42,m44),
			d*determinant33(m21,m22,m23,m31,m32,m33,m41,m42,m43),

			d*determinant33(m12,m13,m14,m32,m33,m34,m42,m43,m44),
			d*determinant33(m11,m13,m14,m31,m33,m34,m41,m43,m44),
			d*determinant33(m11,m12,m14,m31,m32,m34,m41,m42,m44),
			d*determinant33(m11,m12,m13,m31,m32,m33,m41,m42,m43),

			d*determinant33(m12,m13,m14,m22,m23,m24,m42,m43,m44),
			d*determinant33(m11,m13,m14,m21,m23,m24,m41,m43,m44),
			d*determinant33(m11,m12,m14,m21,m22,m24,m41,m42,m44),
			d*determinant33(m11,m12,m13,m21,m22,m23,m41,m42,m43),

			d*determinant33(m12,m13,m14,m22,m23,m24,m32,m33,m34),
			d*determinant33(m11,m13,m14,m21,m23,m24,m31,m33,m34),
			d*determinant33(m11,m12,m14,m21,m22,m24,m31,m32,m34),
			d*determinant33(m11,m12,m13,m21,m22,m23,m31,m32,m33)); */}

	//! Set the position of matrix 
	//! \note you can do Pos()=delta
	inline Matrix& SetPos(const V3D &delta) { Pos()=delta; return *this; }
	//! Move the position of matrix (multiply to left)
	inline Matrix& Translate(const V3D &delta) { Pos()+=delta; return *this; }

	//! Sets like rotation of #alpha on X axe
	inline Matrix& SetXRotate(const float alpha)
	{
		Reset();
		m11=1.f; m44=1.f;
		m22=m33=cosf(alpha);
		m32=sinf(alpha); m23=-m32;
		return *this;
	}

	//! Sets like rotation of #alpha on Y axe
	inline Matrix& SetYRotate(const float alpha)
	{
		Reset();
		m22=1.f; m44=1.f;
		m11=m33=cosf(alpha);
		m13=sinf(alpha); m31=-m13;
		return *this;
	}

	//! Sets like rotation of #alpha on Z axe
	inline Matrix& SetZRotate(const float alpha)
	{
		Reset();
		m33=1.f; m44=1.f;
		m11=m22=cosf(alpha);
		m21=sinf(alpha); m12=-m21;
		return *this;
	}
	//! Sets the matrix like lookat matrix (having At,up and pos)
	inline Matrix& SetLookAt(const V3D &_pos,const V3D &look,const V3D &_up)
	{
		m14=m24=m34=0; m44=1.f;
		Pos()=_pos;
		At()=look-_pos; At().Normalize();
		Right()=At()^_up; Right().Normalize();
		Up()=At()^Right(); Up().Normalize();
		return *this;
	}
	//! Set like rotation of alpha arount axis
	inline Matrix& SetRotate(const V3D &axis,const float alpha)
	{
		m14=m24=m34=0;
		m41=m42=m43=0; m44=1;

		float c=cosf(alpha);
		float i=1-c;
		float s=sinf(alpha); 
		// axis must be normalized
		
		// Rodrigues Rotation Formula
		//http://mathworld.wolfram.com/RodriguesRotationFormula.html
		m11=(axis.x*axis.x)*i+		 c;
		m12=(axis.x*axis.y)*i-axis.z*s;
		m13=(axis.x*axis.z)*i+axis.y*s;

		m21=(axis.y*axis.x)*i+axis.z*s;
		m22=(axis.y*axis.y)*i+		 c;
		m23=(axis.y*axis.z)*i-axis.x*s;

		m31=(axis.z*axis.x)*i-axis.y*s;
		m32=(axis.z*axis.y)*i+axis.x*s;
		m33=(axis.z*axis.z)*i+		 c;
		return *this;
	}
	//! transformation of a point
	inline void Transform(V3D *o,const V3D &v) const
	{
		o->x=m11*v.x+m21*v.y+m31*v.z+m41;
		o->y=m12*v.x+m22*v.y+m32*v.z+m42;
		o->z=m13*v.x+m23*v.y+m33*v.z+m43;
	}
	//! transformation of a normal (no translate, only rotation and scale)
	inline void TransformNormal(V3D *o,const V3D &v) const
	{
		o->x=m11*v.x+m21*v.y+m31*v.z;
		o->y=m12*v.x+m22*v.y+m32*v.z;
		o->z=m13*v.x+m23*v.y+m33*v.z;
	}
	//! Conversion from quaternion
	inline Matrix& operator=(const QBase &q)
	{
		float ss= q.s * q.s ;
		float sx= q.s *q.v.x; sx+=sx;
		float sy= q.s *q.v.y; sy+=sy;
		float sz= q.s *q.v.z; sz+=sz;
		float xx=q.v.x*q.v.x;
		float xy=q.v.x*q.v.y; xy+=xy;
		float xz=q.v.x*q.v.z; xz+=xz;
		float yy=q.v.y*q.v.y;
		float yz=q.v.y*q.v.z; yz+=yz;
		float zz=q.v.z*q.v.z;
		m11=ss+xx-yy-zz; m12=   xy-sz   ; m13=   xz+sy   ;
		m21=   xy+sz   ; m22=ss-xx+yy-zz; m23=   yz-sx   ;
		m31=   xz-sy   ; m32=   yz+sx   ; m33=ss-xx-yy+zz;
		m14=m24=m34=0;
		m41=m42=m43=0; m44=1;
		return *this;
	}
};

//! Managing of quaternion
//! \note QBase defined before for use inside Matrix class
class Quaternion : public QBase
{
public:
	//! Construction that sets to unity
	Quaternion() { v=zero3; s=1; }
	//! Construction that sets
	Quaternion(const float _s,const V3D &_v) { v=_v; s=_s; }
	//! Sets to unity
	inline Quaternion& Reset() { v.x=v.y=v.z=0; s=1; return *this; }
	//! Sets values
	inline Quaternion& Set(const float _s,const V3D &_v) { v=_v; s=_s; return *this; }
	//! Sum between quaternion
	friend inline Quaternion operator +(const Quaternion &a,const Quaternion &b) { return Quaternion(a.s+b.s,a.v+b.v); }
	//! Sum between quaternion, the result sets on first operator
	inline Quaternion& operator +=(const Quaternion &b) { s+=b.s; v+=b.v; return *this; }
	//! Sets like rotation of #alpha around an axe
	//! \sa SetXRotate,SetYRotate, SetZRotate, SetRotateSinCos
	inline Quaternion& SetRotate(const V3D &v,const float alpha) { SetRotateSinCos(v,sinf(alpha/2.f),cosf(alpha/2.f)); return *this; }
	//! Sets like rotation of #alpha around X axes
	//! \sa SetRotate,SetYRotate, SetZRotate, SetXRotateSinCos
	inline Quaternion& SetXRotate(const float alpha) { SetXRotateSinCos(sinf(alpha/2.f),cosf(alpha/2.f)); return *this; }
	//! Sets like rotation of #alpha around Y axes
	//! \sa SetRotate,SetXRotate, SetZRotate, SetYRotateSinCos
	inline Quaternion& SetYRotate(const float alpha) { SetYRotateSinCos(sinf(alpha/2.f),cosf(alpha/2.f)); return *this;}
	//! Sets like rotation of #alpha around Z axes
	//! \sa SetRotate,SetXRotate, SetYRotate, SetZRotateSinCos
	inline Quaternion& SetZRotate(const float alpha) { SetZRotateSinCos(sinf(alpha/2.f),cosf(alpha/2.f)); return *this;}
	//! Sets like rotation around an axe from sin and cosin
	//! \sa SetRotate
	inline Quaternion& SetRotateSinCos(const V3D &_v,const float _s,const float c) { v=_v; v.Normalize(); v*=_s; s=c; return *this; }
	//! Sets like rotation around X axe from sin and cosin
	//! \sa SetXRotate
	inline Quaternion& SetXRotateSinCos(const float _s,const float c) { v.y=v.z=0; v.x=_s; s=c; return *this;}
	//! Sets like rotation around Y axe from sin and cosin
	//! \sa SetZRotate
	inline Quaternion& SetYRotateSinCos(const float _s,const float c) { v.x=v.z=0; v.y=_s; s=c; return *this;}
	//! Sets like rotation around Z axe from sin and cosin
	//! \sa SetYRotate
	inline Quaternion& SetZRotateSinCos(const float _s,const float c) { v.x=v.y=0; v.z=_s; s=c; return *this;}
	//! Product between quaternion
	friend inline Quaternion operator ^(const Quaternion &a,const Quaternion &b)
	{
		return Quaternion(
			a.s*b.s   - a.v.x*b.v.x - a.v.y*b.v.y - a.v.z*b.v.z,V3D(
			a.s*b.v.x + a.v.x*b.s   + a.v.y*b.v.z - a.v.z*b.v.y,
			a.s*b.v.y + a.v.y*b.s   + a.v.z*b.v.x - a.v.x*b.v.z,
			a.s*b.v.z + a.v.z*b.s   + a.v.x*b.v.y - a.v.y*b.v.x));
	}

	//! Scale of a quaternion (useful?)
	inline Quaternion& operator*=(const float _s) { v*=_s; s*=_s; return *this;}
	//! Scale of a quaternion (useful?)
	friend Quaternion operator*(const Quaternion &q,const float _s) { return Quaternion(q.s*_s,q.v*_s); }
	//! Dot product between quaternion
	friend inline float operator *(const Quaternion &a,const Quaternion &b) { return (a.s*b.s+a.v*b.v); }

	//! Conversion to matrix
	//! \note http://www.gamasutra.com/features/19980703/quaternions_01.htm
	Quaternion& operator=(const Matrix &m)
	{
		float	tr, u, q[3];
		int		i, j, k;
		tr = m[0][0] + m[1][1] + m[2][2];
		// check the diagonal
		if (tr > 0.0)
		{
			u = sqrtf(tr + 1.f);
			s=u * 0.5f; //componente scalare
			u = 0.5f / u;
			v.x = (m[2][1] - m[1][2]) * u;
			v.y = (m[0][2] - m[2][0]) * u;
			v.z = (m[1][0] - m[0][1]) * u;
		} else
		{// diagonal is negative
			i = 0;
			if (m[1][1] > m[0][0]) i = 1;
			if (m[2][2] > m[i][i]) i = 2;
			j = (i+1)%3;
			k = (j+1)%3;
			u = sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.f);
			q[i] = u * 0.5f;
			if (u != 0.f) u = 0.5f / u;
			s =    (m[k][j] - m[j][k]) * u;
			q[j] = (m[j][i] + m[i][j]) * u;
			q[k] = (m[k][i] + m[i][k]) * u;
			v.x = q[0];
			v.y = q[1];
			v.z = q[2];
		}
		float f= 1.f/sqrtf(v*v+s*s);
		v *= f;
		s *= f;
		return *this;
	};

	//! Spherical interpolation betweem 2 quaternion
	inline void Slerp(const Quaternion &q1,const Quaternion &q2,float t)
	{
		float	to1[4];
		float	omega, cosom, sinom, scale0, scale1;

		cosom=q1*q2;

		// adjust signs (if necessary)
		if ( cosom <0.0 )
		{
			cosom = -cosom;
			to1[0] = - q2.v.x;
			to1[1] = - q2.v.y;
			to1[2] = - q2.v.z;
			to1[3] = - q2.s;
		} else
		{
			to1[0] = q2.v.x;
			to1[1] = q2.v.y;
			to1[2] = q2.v.z;
			to1[3] = q2.s;
		}
		// calculate coefficients
		if ( cosom < 0.9f )
		{
			// standard case (slerp)
			omega = acosf(cosom);
			sinom = sqrtf(1-cosom*cosom);
			scale0 = sinf((1.f - t) * omega) / sinom;
			scale1 = sinf(t * omega) / sinom;
		} else
		{
			// "from" and "to" quaternions are very close
			//  ... so we can do a linear interpolation
			scale0 = 1.f - t;
			scale1 = t;
		}
		// calculate final values
		v.x = scale0 * q1.v.x + scale1 * to1[0];
		v.y = scale0 * q1.v.y + scale1 * to1[1];
		v.z = scale0 * q1.v.z + scale1 * to1[2];
		s = scale0 * q1.s + scale1 * to1[3];

		if(cosom>=0.9f)
		{
			// Normalizzazione del quaternione se siamo in interpolazione lineare
			float m = 1.f/sqrtf(v*v + s*s);
			v *= m;
			s *= m;
		}
	};

	//! Linear interpolation betweem 2 quaternion
//! Calcola l'interpolazione Lineare tra due quaternioni
	inline void Lerp(const Quaternion &q1,const Quaternion &q2,float t)
	{
		float t1=q1*q2;
		float to1[4];
		// adjust signs (if necessary)
		if (t1<0.0 )
		{
			to1[0] = - q2.v.x;
			to1[1] = - q2.v.y;
			to1[2] = - q2.v.z;
			to1[3] = - q2.s;
		} else
		{
			to1[0] = q2.v.x;
			to1[1] = q2.v.y;
			to1[2] = q2.v.z;
			to1[3] = q2.s;
		}
		t1=1-t;
		v.x=q1.v.x*t1+to1[0]*t;
		v.y=q1.v.y*t1+to1[1]*t;
		v.z=q1.v.z*t1+to1[2]*t;
		 s =q1. s *t1+to1[3]*t;
		float f= 1.f/sqrtf(v*v + s*s);
		v *= f;
		s *= f;
	};

	//! return the conjugate matrix
	inline Quaternion GetConjugate() const { return Quaternion(s,-v); }
	//! rotate the vector with quaternion
	inline void Transform(V3D *o,const V3D &i)
	{
		*o=(v^(v^i-s*i))*2+i;
		//Vertex3D t,t2;
		//t.Cross(v,i);  t.AddScaled(i,-s);
		//t2.Cross(v,t); o->SumScaled(t2,2,i,1);
	}

	//! Quaternion normalitization
	inline float Normalize()
	{
		float l=v*v+s*s;
		if(l!=0)
		{
			float f=1.f/sqrtf(l);
			v *= f;
			s *= f;
		}
		return l;
	}
};

//! Position-Texture structure
struct TV
{
	V3D p; V2D t;
	TV(float x=0,float y=0,float z=0,float u=0,float v=0) { Set(x,y,z,u,v); }
	void Set(float x,float y,float z,float u,float v) { p.Set(x,y,z); t.Set(u,v); }
};
//! Position-Normal structure
struct NV
{
	V3D p; V3D n;
	NV(float x=0,float y=0,float z=0,float nx=0,float ny=0,float nz=0) { Set(x,y,z,nx,ny,nz); }
	void Set(float x,float y,float z,float nx,float ny,float nz) { p.Set(x,y,z); n.Set(nx,ny,nz); }
};
//! Position-Normal-Texture structure
struct TNV
{
	V3D p; V3D n; V2D t;
	TNV(float x=0,float y=0,float z=0,float nx=0,float ny=0,float nz=0,float u=0,float v=0) { Set(x,y,z,nx,ny,nz,u,v); }
	void Set(float x,float y,float z,float nx,float ny,float nz,float u,float v) { p.Set(x,y,z); n.Set(nx,ny,nz); t.Set(u,v); }
};
//! Position-Color structure
struct CV
{
	V3D p; Color c;
	CV(float x=0,float y=0,float z=0,unsigned char r=255,unsigned char g=255,unsigned char b=255,unsigned char a=255) { Set(x,y,z,r,g,b,a); }
	void Set(float x,float y,float z,unsigned char r=255,unsigned char g=255,unsigned char b=255,unsigned char a=255) { p.Set(x,y,z); c.Set(r,g,b,a); }
};
//! Position-Color-texture structure
struct TCV
{
	V3D p; Color c; V2D t;
	TCV(float x=0,float y=0,float z=0,unsigned char r=255,unsigned char g=255,unsigned char b=255,unsigned char a=255,float u=0,float v=0) { Set(x,y,z,r,g,b,a,u,v); }
	void Set(float x,float y,float z,unsigned char r,unsigned char g,unsigned char b,unsigned char a,float u,float v) { p.Set(x,y,z); c.Set(r,g,b,a); t.Set(u,v); }
};


#endif
