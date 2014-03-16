//==================================================================================================
//
// File:	Renderer.h
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// A renderer samples into a scene for color and applies the color onto a buffer. A renderer can
// render all or part of the scene.
//=================================================================================================
#ifndef RENDERER_H
#define RENDERER_H

namespace RT
{

class RenderManager;

struct Block
{
	uint x;
	uint y;
	uint width;
	uint height;
};



//==================================================================================================
//
// Represents the rendering of the scene to the buffer
//
//==================================================================================================

class Renderer : public Thread
{
	
public:
	Renderer (Scene & scene, Camera & camera, CImage & backbuffer, RenderManager & manager);
    //Renderer (Renderer && rhs);

	void SetSamplesPerPixel (uint spp);
	inline bool IsDone () const { return mbDone; }

private: // Thread
	virtual void ThreadEnter();

private: // Internal Private

	Color SamplePixel (const float64 & u, const float64 & v);
	Color SampleScene (const Ray3 & ray, uint32 recursiveDepth = 0);

	void Setup (const Block & block);
	void Render();
	void CopyToBackbuffer();
	void Cleanup();

	Block	mBlock;
	bool	mbDone;		// Flag to that will be set when this renderer is finished with its work

	uint    mSpp;					// Total samples per pixel
	uint	mSamplesStratifiedSide; // Number of samples per side of a pixel to be stratified
	uint	mSamplesRandom;			// Number of samples which are left over from stratified
	//v2	mPixelSize; 			// Size of a pixel (Camera Space)
	//v2    mSubPixelSize;			// Size of each sub-pixel area for stratified sampling (Camera Space)
	float64 mPixelWidth;
	float64 mPixelHeight;
	float64 mSubPixelWidth;
	float64 mSubPixelHeight;


	RenderManager & mManager;

	CImage          mBuffer;     // The temporary buffer while rendering
	CImage &        mBackbuffer; // The output bitmap where this renderer will be drawing to
	Scene           mScene;		 // The input scene of objects
	Camera &        mCamera;	 // The camera this renderer will fetch primary rays from
	Random          mRand;
};

}; // namespace RT

#endif //RENDERER_H