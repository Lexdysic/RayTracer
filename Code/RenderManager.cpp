//==================================================================================================
//
// File:	Renderer.cpp
// Author:	Jason Jackson
// Date:	December 15, 2008
//
// A renderer samples into a scene for color and applies the color onto a buffer. A renderer can
// render all or part of the scene.
//
//=================================================================================================

#include "Pch.h"

namespace RT
{

//=============================================================================
RenderManager::RenderManager (Scene & scene, Camera & camera, CImage & backbuffer) :
	mScene(scene),
	mCamera(camera),
	mBackbuffer(backbuffer),
	mSpp(10)
{


}

//=============================================================================
RenderManager::~RenderManager ()
{
	for (auto & renderer : mRenderers)
		renderer.ThreadStop();
}

//=============================================================================
void RenderManager::Start ()
{
	// Blocks
	{
		const uint w = mBackbuffer.GetWidth();
		const uint h = mBackbuffer.GetHeight();

		mBlocks.reserve(h * 2);
		for (uint i = h; i-- > 0; )
		{
			Block block1 = { 0, i, w/2, 1 };
			Block block2 = { w/2, i, w/2, 1 };
			mBlocks.push_back(block1);
			mBlocks.push_back(block2);
		}

		mTotalBlocks = mBlocks.size();
	}

	// Renderers
	{
		const uint numLogicProc = ThreadLogicalProcessorCount();
		const uint numRenderers = Max<sint>(1, numLogicProc - 1);

		for (uint i = 0; i < numRenderers; ++i)
			mRenderers.push_back(Renderer(mScene, mCamera, mBackbuffer, *this));
	}

	for( RendererList::iterator it = mRenderers.begin(); it != mRenderers.end(); ++it )
	{
		it->SetSamplesPerPixel(mSpp);
		it->ThreadStart();
	}
}

//=============================================================================
bool RenderManager::IsDone ()
{
	return mCompletedBlocks == mTotalBlocks;
}

//=============================================================================
bool RenderManager::GetBlock (Block & out) 
{
    mLockRenderers.Enter();

	if (mBlocks.empty())
		return false;

	out = mBlocks.back();
	mBlocks.pop_back();
    
    mLockRenderers.Leave();

	return true;
}

//=============================================================================
void RenderManager::CompleteBlock ()
{
    mCompletedBlocks++;
}

//=============================================================================
float32 RenderManager::GetProgress ()
{
	return mCompletedBlocks / float32(mTotalBlocks);
}

}// namespace RT