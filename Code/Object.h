//==================================================================================================
//
// File:	Object.h
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// Defines different type of objects which belong to a scene
//=================================================================================================
#ifndef OBJECT_H
#define OBJECT_H

namespace RT
{


struct Result
{
	float32	time;
	Point3	point;
	Vector3	normal;
};

enum EMaterialType
{
	MATERIAL_TYPE_DIFFUSE,
	MATERIAL_TYPE_REFLECT,
	MATERIAL_TYPE_REFRACT,
};

struct Material
{
	EMaterialType type;
	Color         diffuse;
	Color         emissive;
};


class Object;


Object * ParseObject (const Json::CValue & json);



//==================================================================================================
// Objects - base class for all scene objects
//==================================================================================================
class Object
{
public:
	Object(const Material & material);

	virtual bool     Intersect(Result & out, const Ray3 & ray) const = 0;
	virtual Object * Clone() const = 0;

	const Material & GetMaterial() const { return mMaterial; }

protected:
	Material	mMaterial;

};



//==================================================================================================
// Sphere
//==================================================================================================
class Sphere : public Object
{
public:
	Sphere(const Point3 & pos, float32 radius, const Material & material);

	virtual bool Intersect(Result & out, const Ray3 & ray) const;
	virtual Sphere * Clone() const;

private:
	Sphere3		mSphere;
};



//==================================================================================================
// Ellipsoid
//==================================================================================================
class Ellipsoid : public Object
{
public:
	Ellipsoid(const Point3 & pos, const Vector3 & radii, const Material & material);
	Ellipsoid(const Point3 & pos, const Vector3 & u, const Vector3 & v, const Vector3 & w, const Material & material);
	Ellipsoid(const Ellipsoid & e);

	virtual bool Intersect(Result & out, const Ray3 & ray) const;
	virtual Ellipsoid * Clone() const;

private:
	Point3   mCenter;
	Matrix33 mObjectToWorldIT;
	Matrix33 mObjectToWorldI;
	Matrix33 mObjectToWorldT;
	Matrix33 mObjectToWorld;
};



//==================================================================================================
// Aabb
//==================================================================================================
class Aabb : public Object
{
public: 
	Aabb(const Point3 & min, const Point3 & max, const Material & material);

	virtual bool Intersect(Result & out, const Ray3 & ray) const;
	virtual Aabb * Clone() const;

private:
	Aabb3 mAabb;
};

} // namespace RT

#endif //OBJECT_H