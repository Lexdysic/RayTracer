//==================================================================================================
//
// File:	Scene.cpp
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// Defines what a ray traced scene looks like. The scene can be sampled for color given a ray.
//
//=================================================================================================

#include "Pch.h"

namespace RT
{

//=============================================================================
Scene::Scene () :
	mBackground(0.0f, 0.0f, 0.0f)
{
}

//=============================================================================
Scene::Scene (const Scene & scene) :
	mBackground(scene.mBackground)
{
	for( uint32 i = 0; i < scene.mpObjects.size(); ++i )
	{
		const Object* pObject = scene.mpObjects[i]->Clone();
		mpObjects.push_back(pObject);
	}
}

//=============================================================================
Scene::~Scene ()
{
	for( uint32 i = 0; i < mpObjects.size(); ++i )
		delete mpObjects[i];
}

//=============================================================================
bool Scene::FindObject (const Object *& pBestObject, Result & bestResult, const Ray3 & ray) const 
{
	pBestObject = NULL;
	bestResult.time = std::numeric_limits<float32>::infinity();

	for( uint32 i = 0; i < mpObjects.size(); ++i )
	{
		const Object* pObject = mpObjects[i];

		Result result;
		if( pObject->Intersect(result, ray) )
		{
			if( result.time < bestResult.time )
			{
				bestResult	= result;
				pBestObject = pObject;
			}
		}
	}

	return pBestObject != NULL;
}

}// namespace RT


//=============================================================================
template <uint N, typename T>
static bool ParseNumberSequence (const Json::CValue & json, T * out)
{
    using namespace Json;

    if (json.GetType() != EType::Array)
        return false;

    const ArrayType & array = *json.As<ArrayType>();
    if (array.Count() != N)
        return false;

    for (uint i = 0; i < N; ++i)
    {
        if (array[i].GetType() != EType::Number)
            return false;

        (*out)[i] = float32(*array[i].As<NumberType>());
    }

    return true;
}

//=============================================================================
bool ParsePoint3 (const Json::CValue & json, Point3 * out)
{
    return ParseNumberSequence<3>(json, out);
}

//=============================================================================
bool ParseVector2 (const Json::CValue & json, Vector2 * out)
{
    return ParseNumberSequence<2>(json, out);
}

//=============================================================================
bool ParseVector3 (const Json::CValue & json, Vector3 * out)
{
    return ParseNumberSequence<3>(json, out);;
}

//=============================================================================
bool ParseColor (const Json::CValue & value, Color * out)
{
    using namespace Json;

    if (value.GetType() != EType::Array)
        return false;

    const ArrayType & array = *value.As<ArrayType>();
    if (array.Count() != 3)
        return false;

    for (uint i = 0; i < 3; ++i)
    {
        if (array[i].GetType() != EType::Number)
            return false;
    }

    out->r = float32(*array[0].As<NumberType>());
    out->g = float32(*array[1].As<NumberType>());
    out->b = float32(*array[2].As<NumberType>());
    out->a = 1.0f;

    return true;
}


//=============================================================================
bool ParseMaterial (const Json::CValue & json, RT::Material * out)
{
    ASSERT(out);

    using namespace Json;

    // Type
    const CValue & jsonType = json["type"];
    if (jsonType.GetType() != EType::String)
        return false;

    const StringType & type = *jsonType.As<StringType>();
    if      (type == "diffuse") out->type = RT::MATERIAL_TYPE_DIFFUSE;
    else if (type == "reflect") out->type = RT::MATERIAL_TYPE_REFLECT;
    else if (type == "refract") out->type = RT::MATERIAL_TYPE_REFRACT;
    else return false;

    if (!ParseColor(json["diffuse"], &out->diffuse))
        return false;

    if (!ParseColor(json["emissive"], &out->emissive))
        return false;

    return true;
}