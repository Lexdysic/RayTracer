//==================================================================================================
//
// File:	Light.cpp
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// Implements how each type of light in the scene emits light
//=================================================================================================

#include "Pch.h"

namespace RT
{

Light::Light( const Color & color )
: mColor( color )
{
}


PointLight::PointLight(const Point3 & pos, const Color & color)
: mPos( pos )
, Light( color )
{
}

Vector3 PointLight::GetRay(const Point3 & point) const
{
	return mPos - point;
}

} // namespace RT