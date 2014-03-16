//==================================================================================================
//
// File:	Camera.h
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// Defines a camera which is a view into the ray traced scene.
//=================================================================================================
#ifndef CAMERA_H
#define CAMERA_H

namespace RT
{

//==================================================================================================
//
// Class which represents the virtual camera in a raytraced scene
//==================================================================================================
class Camera
{
public:
	
	void Setup (
        const Point3 & eye,
        const Point3 & at,
        const Vector3 & up,
        float32 distance,
        float32 width,
        float32 aspect
    );
    void Setup (const Json::CValue & json, float32 aspect);
	inline Ray3 GetRay (float64 u, float64 v) const;
	inline Ray3 GetRay (const Vector2 & uv) const;
	inline Point3 GetEye () const { return mEye; }

private:
	Point3  mEye;		// Point where rays originate
	Vector3 mRight;		// Vector which extends the half length of the screen to the right
	Vector3 mDown;		// Vector which extends the half length of the screen toward the bottom
	Point3  mCenter;	// Point at the very center of the screen
};



//=============================================================================
Ray3 Camera::GetRay (float64 u, float64 v) const
{
	return GetRay(Vector2((float32)u, (float32)v));
}

//=============================================================================
Ray3 Camera::GetRay (const Vector2 & uv) const
{
	ASSERT(-1.0f <= uv.x && uv.x <= 1.0f );
	ASSERT(-1.0f <= uv.y && uv.y <= 1.0f );

	const Vector3 direction = (mCenter + uv.x * mRight + uv.y * mDown) - mEye;
	return Ray3(mEye, Normalize(direction));
}

}

#endif //CAMERA_H