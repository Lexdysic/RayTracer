#include "Pch.h"

namespace RT
{

//=============================================================================
// Helpers
//=============================================================================



//=============================================================================
// Functions
//=============================================================================

//=============================================================================
Object * ParseObject (const Json::CValue & json)
{
    using namespace Json;

    Material material;
    if (!ParseMaterial(json["material"], &material))
        return null;

    const CValue & jsonShape = json["shape"];

    const CValue & jsonType = jsonShape["type"];
    if (jsonType.GetType() != EType::String)
        return null;

    const StringType & type = *jsonType.As<StringType>();
    if (type == "sphere")
    {
        ASSERT(false);
    }
    else if (type == "ellipsoid")
    {
        ASSERT(false);
    }
    else if (type == "aabb")
    {
        Point3 min, max;
        if (!ParsePoint3(jsonShape["min"], &min))
            return null;

        if (!ParsePoint3(jsonShape["max"], &max))
            return null;

        return new RT::Aabb(min, max, material);
    }
    

    return null;
}



//=============================================================================
// Object
//=============================================================================

//=============================================================================
Object::Object(const Material & material) :
    mMaterial(material)
{
}



//=============================================================================
// Sphere
//=============================================================================

//=============================================================================
Sphere::Sphere(const p3 & pos, float32 radius, const Material & material) :
    Object(material),
    mSphere(pos, radius)
{

}

//=============================================================================
Sphere * Sphere::Clone() const
{
	return new Sphere(mSphere.center, mSphere.radius, mMaterial);
}

//=============================================================================
bool Sphere::Intersect( Result & out, const Ray3 & ray ) const
{
	IntersectInfo3 info;
	if( ::Intersect(info, ray, mSphere) )
	{
		out.time	= info.time;
		out.point	= info.point;
		out.normal	= Normalize(info.normal);

		return true;
	}

	return false;
}

//=============================================================================
// Ellipsoid
//=============================================================================

//=============================================================================
Ellipsoid::Ellipsoid(const p3 & pos, const v3 & u, const v3 & v, const v3 & w, const Material & material) :
	Object(material),
	mCenter(pos),
	mObjectToWorld(
		u.x, v.x, w.x,
		u.y, v.y, w.y,
		u.z, v.z, w.z
	)
{
	assert(Dot(u, v) < 1e-4);
	assert(Dot(v, w) < 1e-4);
	assert(Dot(w, u) < 1e-4);

	mObjectToWorldI  = Inverse(mObjectToWorld);
	mObjectToWorldT  = Transpose(mObjectToWorld);
	mObjectToWorldIT = Transpose(mObjectToWorldI);
}

//=============================================================================
Ellipsoid::Ellipsoid(const p3 & pos, const v3 & radii, const Material & material) :
	Object(material),
	mCenter(pos),
	mObjectToWorld(
		radii.x, 0.0f,    0.0f,
		0.0f,    radii.y, 0.0f,
		0.0f,    0.0f,    radii.z
	)
{
	mObjectToWorldI  = Inverse(mObjectToWorld);
	mObjectToWorldT  = Transpose(mObjectToWorld);
	mObjectToWorldIT = Transpose(mObjectToWorldI);
}

//=============================================================================
Ellipsoid::Ellipsoid(const Ellipsoid & e) :
	Object(e.mMaterial),
	mCenter(e.mCenter),
	mObjectToWorld(e.mObjectToWorld),
	mObjectToWorldI(e.mObjectToWorldI),
	mObjectToWorldT(e.mObjectToWorldT),
	mObjectToWorldIT(e.mObjectToWorldIT)
{
}

//=============================================================================
bool Ellipsoid::Intersect(Result & out, const Ray3 & ray) const
{
	const Ray3 r(
		mObjectToWorldI * (ray.origin - v3(mCenter)), 
		mObjectToWorldI * ray.direction
	);
	
	IntersectInfo3 info;
	if( ::Intersect(info, r, Sphere3::Unit) )
	{
		if ( info.time > 0.0f )
		{
			out.point  = ray.origin + ray.direction * info.time;//mObjectToWorld * info.point;
			out.normal = mObjectToWorldIT * info.normal;
			out.time   = info.time;

			return true;
		}
	}

	return false;
}

//=============================================================================
Ellipsoid * Ellipsoid::Clone() const
{
	return new Ellipsoid(*this);
}


//=============================================================================
// Aabb
//=============================================================================

//=============================================================================
Aabb::Aabb(const p3 & min, const p3 & max, const Material & material) :
	mAabb(min, max),
	Object(material)
{

}

//=============================================================================
Aabb * Aabb::Clone() const
{
	return new Aabb(mAabb.min, mAabb.max, mMaterial);
}

//=============================================================================
bool Aabb::Intersect(Result & out, const Ray3 & ray) const
{
	IntersectInfo3 info;
	if( ::Intersect(info, ray, mAabb) && info.time > 0.0f )
	{
		out.time	= info.time;
		out.point	= info.point;
		out.normal	= info.normal;

		return true;
	}

	return false;
}

} // namespace RT