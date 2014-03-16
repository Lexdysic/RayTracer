//==================================================================================================
//
// File:	Light.h
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// Defines different types of lights which are used in scenes
//=================================================================================================
#ifndef LIGHT_H
#define LIGHT_H

namespace RT
{

//=============================================================================
//
// Interface for all lights
//
//=============================================================================
class Light
{
public:
	Light(const Color & color);
	//! Function to get a ray to a point
	virtual v3 GetRay(const p3 & point) const = 0;
	//! Returns the color of this light
	inline const Color & GetColor() const { return mColor; }

protected:
	Color	mColor;
};

//=============================================================================
//
// A simple light which only exists at a point
//
//=============================================================================
class PointLight : public Light
{
public:
	PointLight( const p3 & pos, const Color & color );

	virtual v3 GetRay(const p3 & point) const;

private:
	p3	mPos;
};



} // namespace RT

#endif //LIGHT_H