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

const float32 EPSILON = 0.001f;

//=============================================================================
Vector3 SampleInHemisphere(const Random & rand) 
{
	const float32 u1  = 2.0f * Math::Pi * rand.GetFloat32();
	const float32 u2  = rand.GetFloat32();
	const float32 u2s = Sqrt(u2);

	return Vector3(Sqrt(1.0f - u2), Cos(Radian(u1)) * u2s, Sin(Radian(u1)) * u2s);
}

//=============================================================================
Vector3 UniformSampleInHemisphere(const Random & rand) 
{
	const float32 u1 = rand.GetFloat32();
	const float32 u2 = rand.GetFloat32();

	const float32 r = Sqrt(1.0f - Sq(u1));
	const Radian phi(2.0f * Math::Pi * u2);

	return Vector3(r * Cos(phi), r * Sin(phi), u1);
}

//=============================================================================
Vector3 CosineSampleInHemisphere(const Random & rand) 
{
	const float32 u1 = rand.GetFloat32();
	const float32 u2 = rand.GetFloat32();

	const float32 r = Sqrt(u1);
	const Radian theta(2.0f * Math::Pi * u2);

	const float32 x = r * Cos(theta);
	const float32 y = r * Sin(theta);
	const float32 z = Sqrt(Max(0.0f, 1.0f - Sq(x) - Sq(y)));

	return Vector3(x, y, z);
}

//=============================================================================
Renderer::Renderer(Scene & scene, Camera & camera, CImage & backbuffer, RenderManager & manager) :
	mScene(scene),
	mCamera(camera),
	mBackbuffer(backbuffer),
	mManager(manager),
	mbDone(false)
{
    mPixelWidth  = 2.0 / mBackbuffer.GetWidth();
	mPixelHeight = 2.0 / mBackbuffer.GetHeight();
}

//=============================================================================
void Renderer::SetSamplesPerPixel (uint spp)
{
	mSpp                   = spp;
	mSamplesStratifiedSide = FloatToUint(Floor(Sqrt(float32(mSpp))));
	mSamplesRandom         = mSpp - Sq(mSamplesStratifiedSide);

	mSubPixelWidth  = mPixelWidth  / float64(mSamplesStratifiedSide);
	mSubPixelHeight = mPixelHeight / float64(mSamplesStratifiedSide);
}

//=============================================================================
void Renderer::Setup (const Block & block)
{
	mBlock = block;
	mBuffer = CImage(mBlock.width, mBlock.height);
}

//=============================================================================
void Renderer::Cleanup()
{
    mManager.CompleteBlock();
}

//=============================================================================
// (x, y) = pixel coordinates of the pixel. (u, v) = camera coordinates of the pixel.
// (j, k) = sub-pixel coordinates
void Renderer::Render()
{
	const float64 left = -1.0 + mPixelWidth * (float64)mBlock.x;
	const float64 top  = -1.0 + mPixelHeight * (float64)mBlock.y;

	for (uint y = 0; y < mBlock.height; ++y )
	{
		const float64 v = top + mPixelHeight * y;
		for (uint x = 0; x < mBlock.width; ++x)
		{
			const float64 u = left + mPixelWidth * x;

			const Color color = SamplePixel(u, v);
			mBuffer.SetPixel(x, y, color);
		}
	}
}

//=============================================================================
Color Renderer::SamplePixel (const float64 & u, const float64 & v)
{
	mRand.Seed(
		FloatToUint(
            Floor(
			    float32(u * mBlock.width * 23.04124233 + v * mBlock.height * mBlock.width * 33676.53234236)
		    )
        )
	);


	Color color(0, 0, 0);

	// Stratified sampling
	{
		for (uint n = 0; n < mSamplesStratifiedSide; ++n)
		{
			const float64 k = v + mSubPixelHeight * n;
			for (uint m = 0; m < mSamplesStratifiedSide; ++m)
			{
				const float64 j = u + mSubPixelWidth * m;

				const float64 rj    = mRand.GetFloat64();
				const float64 rk    = mRand.GetFloat64();

				const float64 randJ = mSubPixelWidth  * rj;
				const float64 randK = mSubPixelHeight * rk;

				const Ray3 ray = mCamera.GetRay(j + randJ, k + randK);
				color += SampleScene(ray);
			}
		}
	}

	// The rest of the sames are just randomly selected
	// within the area of the entire pixel
	for (uint i = 0; i < mSamplesRandom; ++i)
	{
		const float64 randU = mPixelWidth  * mRand.GetFloat64();
		const float64 randV = mPixelHeight * mRand.GetFloat64();
		const Ray3 ray = mCamera.GetRay(u + randU, v + randV);
		color += (SampleScene(ray));

	}

	return color / (float32)mSpp;
}

//=============================================================================
static bool ComputeTransmission(
	const Vector3 & I, 
	const Vector3 & N, 
	const float32 nI,
	const float32 nT,
	Vector3 & T, 
	float32 & coeffT,
	Vector3 & R, 
	float32 & coeffR
) {
	//assert(Normalized(I));
	//assert(Normalized(N));

	// Snell's Law
	const float32 n      = nI / nT;
	const float32 cosI   = Dot(I, N);
	const float32 cosTSq = 1.0f - Sq(n) * (1.0f - Sq(cosI));

	R = I - ((2.0f * cosI) * N); // There is something wrong here
	//assert(Normalized(R));

	// Total internal reflection 
	if (cosTSq < 0.0f)
	{
		coeffR = 1.0f;
		coeffT = 0.0f;
		return true;
	}
	else
	{
		const float32 cosThT = Sqrt(cosTSq);

		T = n * I - (n * cosI - cosThT) * N;
		//assert(Normalized(T));

		// Fresnel Equation (Schlick's Approximation)
		const float32 c  = 1.0f - (nI <= nT ? cosI : Dot(T, N)); 
		const float32 R0 = Sq(nI - nT) / Sq(nI + nT);
		coeffR       = R0 + (1.0f - R0) * c*c*c*c*c;
		coeffT       = 1.0f - coeffR;
		return false;
	}
}

//=============================================================================
Color Renderer::SampleScene (const Ray3 & ray, unsigned recursiveDepth)
{
	const Object * pObject = null;
	Result bestResult;
	if (!mScene.FindObject(pObject, bestResult, ray))
		return mScene.GetBackgroundColor();

	const Material & mat = pObject->GetMaterial();

	Color f = mat.diffuse;
	const float32 p = Max(f.r, f.g, f.b);

	if (recursiveDepth > 5)
	{
		if (mRand.GetFloat32() >= p || recursiveDepth > 16)
			return mat.emissive;
		f /= p;
	}

	//assert(Normalized(ray.direction));
	//assert(Normalized(bestResult.normal));

	const Point3 &  P = bestResult.point;
	const Vector3 & D = ray.direction;
	const Vector3 & N = bestResult.normal;

	switch (mat.type)
	{
		default:
		case MATERIAL_TYPE_DIFFUSE:
		{
			Vector3 U, V, W;
			BuildBasis(N, U, V, W);

			const Vector3 uvw = SampleInHemisphere(mRand);
			//assert(!Equal(uvw, Vector3::Zero));
			//assert(Normalized(uvw));

			const Vector3 newDirection = Normalize(uvw.x * U + uvw.y * V + uvw.z * W);
			//assert(Normalized(newDirection));

			const Ray3 newRay(P + N * EPSILON, newDirection);

			const Color reflected = SampleScene(newRay, recursiveDepth + 1);

			return mat.emissive + f * reflected;
		}

		case MATERIAL_TYPE_REFLECT:
		{
			const Vector3 newDirection =  Normalize(D - N * 2.0f * Dot(N, D));
			//assert(Normalized(newDirection));
			const Ray3 newRay(P + N * EPSILON, newDirection);
			const Color reflected = SampleScene(newRay, recursiveDepth + 1);
			return mat.emissive + f * reflected;
		}

		case MATERIAL_TYPE_REFRACT:
		{
			const float32 NdotD = Dot(N, D);
			const bool bInto = NdotD < 0.0f;
			const Vector3 M = bInto ? N : -N; // Incident normal vector

			// Index of refraction
			float32 nI = 1.0f;
			float32 nT = 1.5f;
			if (!bInto)
				std::swap(nI, nT);

			float32 reflCoeff, transCoeff;
			Vector3  reflDir, transDir;
			const bool bTIR = ComputeTransmission(
				-D, M, 
				nI, nT, 
				transDir, transCoeff, 
				reflDir, reflCoeff
			);

			const Ray3 reflRay(P + 0.01f * M, -reflDir);
			const Ray3 transRay(P - 0.01f * M, -transDir);
			if (bTIR)
				return mat.emissive + f * SampleScene(reflRay, recursiveDepth + 1); 

			if (recursiveDepth <= 2)
            {
				return SampleScene(transRay, recursiveDepth + 1) * transCoeff + SampleScene(reflRay, recursiveDepth + 1) * reflCoeff;
            }
			else 
			{
				const float32 Pr = 0.25f + 0.5f * reflCoeff; // [0, 1] -> [0.25, 0.75]
				const float32 RP = reflCoeff / Pr;
				const float32 TP = transCoeff / (1 - Pr);
				if(mRand.GetFloat32() < Pr)
					return SampleScene(reflRay, recursiveDepth + 1) * RP;
				else
					return SampleScene(transRay, recursiveDepth) * TP;
			}
		}
	}
}

//=============================================================================
void Renderer::CopyToBackbuffer()
{
    for (uint y = 0; y < mBlock.height; ++y)
    {
        for (uint x = 0; x < mBlock.width; ++x)
        {
            mBackbuffer.SetPixel(
                mBlock.x + x,
                mBlock.y + y,
                mBuffer.GetPixel(x, y)
            );
        }
    }
}

//=============================================================================
void Renderer::ThreadEnter()
{
    
    Block block;
	while (mManager.GetBlock(block))
	{
		Setup(block);
		Render();
		CopyToBackbuffer();
		Cleanup();

	}

	mbDone = true;
}

}// namespace RT