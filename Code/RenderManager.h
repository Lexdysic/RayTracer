//==================================================================================================
//
// File:	Renderer.h
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// A renderer samples into a scene for color and applies the color onto a buffer. A renderer can
// render all or part of the scene.
//=================================================================================================
#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <atomic>

namespace RT
{


//==================================================================================================
//
// 
//
//==================================================================================================

class RenderManager
{
	friend class Renderer;
public:
	RenderManager(Scene & scene, Camera & camera, CImage & backbuffer);
	~RenderManager();

	void Start();
	bool IsDone();
	float32 GetProgress();
	void SetSamplesPerPixel(uint32 spp) { mSpp = spp; }

protected:

	bool GetBlock (Block & out);
    void CompleteBlock ();

private:
	typedef std::vector<Renderer> RendererList;
	typedef std::vector<Block>    BlockList;

    CriticalSection   mLockRenderers;
	RendererList 	  mRenderers;
	BlockList	      mBlocks;
	Scene & 		  mScene;
	Camera & 	      mCamera;
	CImage &          mBackbuffer;
    std::atomic<uint> mCompletedBlocks;
	uint32               mTotalBlocks;
	uint32               mSpp;

};

}; // namespace RT

#endif //RENDERMANAGER_H