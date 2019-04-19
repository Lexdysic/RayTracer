//==================================================================================================
//
// File:	Camera.cpp
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// Defines a camera which is a view into a ray traced scene
//
//=================================================================================================

#include "Pch.h"

namespace RT
{

void Camera::Setup (
    const Point3 & eye,
    const Point3 & at,
    const Vector3 & up,
    float32 distance,
    float32 width,
    float32 aspect
) {
	const Vector3 & view = Normalize(at - eye);
	const Vector3 & upN  = Normalize(up);

	mEye	= eye;
	mCenter	= eye + view * distance;
	mRight	= Cross(view, upN);
	mDown	= -Cross(view, mRight);

	mRight *= (width * 0.5f * aspect);
	mDown  *= (width * 0.5f);
	
}

void Camera::Setup (
    const Json::CValue & json,
    float32 aspect
) {
    using namespace Json;

    const auto * settings = json.As<ObjectType>();
    if (!settings)
        return;

    const auto & eyeValue      = json[{"eye"}];
    const auto & lookatValue   = json[{"lookat"}];
    const auto & upValue       = json[{"up"}];
    const auto & distanceValue = json[{"distance"}];
    const auto & widthValue    = json[{"width"}];
    if (
        eyeValue == null ||
        lookatValue == null ||
        upValue == null ||
        distanceValue == null ||
        widthValue == null
    ) {
        return;
    }

    Vector3 eye, at, up;
    if (!ParseVector3(eyeValue, &eye))
        return;

    if (!ParseVector3(lookatValue, &at))
        return;

    if (!ParseVector3(upValue, &up))
        return;

    const float32 distance = float32(*distanceValue.As<NumberType>());
    const float32 width = float32(*widthValue.As<NumberType>());

    Setup(eye, at, up, distance, width, aspect);
}

} //namespace RT