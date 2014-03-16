//=================================================================================================
//
// File:	Scene.h
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// Defines a scene which can be ray traced
//=================================================================================================
#ifndef SCENE_H
#define SCENE_H

namespace RT
{

class Object;
class Light;

//==================================================================================================
//
// Represents all the objects and properties of the raytraced environments
//==================================================================================================
class Scene
{
public:
	Scene ();
	Scene (const Scene & scene);
	~Scene ();

	//! Adds an object to the scene, the scene takes over this object
	inline void AddObject (const Object * pObj) { mpObjects.push_back(pObj); }
	//! Adds a light to the scene, the scene takes over this object
	inline void AddLight (const Light * pLight) { mpLights.push_back(pLight); }

	//! Sets the color to be used when no object is hit
	inline void SetBackgroundColor (const Color & color) { mBackground = color; }

	inline Color GetBackgroundColor () const { return mBackground; }
	
	bool FindObject (const Object *& pBestObjectOut, Result & bestResultsOut, const Ray3 & ray) const;


	// Data
	std::vector<const Object *>	mpObjects;	//!< List of objects in the scene
	std::vector<const Light *>	mpLights;	//!< List of lights in the scene
	Color 						mBackground;	//!< The color to be used when no object is intersected
};

} // namespace RT


bool ParsePoint3 (const Json::CValue & json, Point3 * out);
bool ParseVector2 (const Json::CValue & json, Vector2 * out);
bool ParseVector3 (const Json::CValue & json, Vector3 * out);
bool ParseColor (const Json::CValue & json, Color * out);
bool ParseMaterial (const Json::CValue & json, RT::Material * out);


#endif //SCENE_H