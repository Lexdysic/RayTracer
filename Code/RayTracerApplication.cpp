
#include <iomanip>
#include <iostream>
#include "Pch.h"

const uint WIDTH  = 512;
const uint HEIGHT = 384;
const Vector3  WALL_EXTENTS(120.0f, 100.0f, 100.0f);



//=============================================================================
// ToTime
//=============================================================================
struct ToTime 
{
	ToTime(Time::Delta t) : 
		time(t) 
	{
	}

	Time::Delta time;
};

//=============================================================================
std::ostream & operator<< (std::ostream & out, const ToTime & t)
{
	uint s = FloatToUint(t.time.GetSeconds());
	
	const uint SEC_IN_MIN  = 60;
	const uint SEC_IN_HOUR = 60 * SEC_IN_MIN;

	const char * LABEL_H  = "h";
	const char * LABEL_M  = "m";
	const char * LABEL_S  = "s";
	const char * LABEL_MS = "ms";

	const uint h  = s / SEC_IN_HOUR; s -= h * SEC_IN_HOUR;
	const uint m  = s / SEC_IN_MIN;  s -= m * SEC_IN_MIN;

	out << std::setfill('0');

	if (h)
	{
		out << h << LABEL_H << " ";
		out << std::setw(2) << m << LABEL_M;
	}
	else if (m)
	{
		out << m << LABEL_M << " ";
		out << std::setw(2) << s << LABEL_S;
	}
	else
	{
		out << s << LABEL_S;
	}

	return out;
}

//===========================================================================
void find_replace (char * str, char what, char with) {
    for (char * it = str; *it != '\0'; ++it) {
        if (*it == what)
            *it = with;
    }
}


//=============================================================================
// Application
//=============================================================================

//=============================================================================
Application::Application() :
	mBackbuffer(WIDTH, HEIGHT),
	mRenderManager(mScene, mCamera, mBackbuffer)
{
    //if (!TrySceneFromFile())
    {
	    mRenderManager.SetSamplesPerPixel(100);
	    SceneCreateWalls();
	    SceneCreateSpheres();
	    SceneCreateReddit();
    }
}

//=============================================================================
Application::~Application()
{
}

//=============================================================================
void Application::Run() 
{
	const Time::Point startTick = Time::GetRealTime();

	mRenderManager.Start();

	while (!mRenderManager.IsDone())
	{
		const float32     currProgress = mRenderManager.GetProgress();
		const Time::Point endTick      = Time::GetRealTime();

		std::cout 
            << '['
			<< ToTime(endTick - startTick)
            << ']'
			<< std::setfill(' ')
			<< std::fixed 
			<< std::setprecision(2)
			<< std::setw(6)
			<< currProgress * 100.0f
			<< "%  "
				
			<< std::endl;

        mRenderManager.WaitForProgress();
	}
    
    char filename[64];
    strncpy_s(filename, "Image - " __DATE__ " - " __TIME__ ".tga", 64);
    find_replace(filename, ':', '_');

    mBackbuffer.Save(filename);
}

//=============================================================================
bool Application::TrySceneFromFile ()
{
    using namespace Json;

    CDocument doc;
    doc.Parse("scene.json");

    if (!doc.IsValid())
        return false;

    const CValue & root = doc.GetValue();
    if (root.GetType() != EType::Object)
        return false;
    
    // Settings
    breakable_scope
    {
        const CValue & settings = root[{"settings"}];
        if (settings == null)
            break;

        // Size
        breakable_scope
        {
            const CValue & jsonSize = settings[{"size"}];
            if (jsonSize == null)
                break;

            Vector2 size;
            if (!ParseVector2(jsonSize, &size))
                break;

            mBackbuffer.Resize(
                FloatToUint(float32(size.x)),
                FloatToUint(float32(size.y))
            );
        }

        // Samples
        breakable_scope
        {
            const CValue & samplesValue = settings[{"samples"}];
            if (samplesValue == null)
                break;

            const auto * samples = samplesValue.As<NumberType>();
            if (!samples)
                break;

            const uint spp = FloatToUint(*samples);
            mRenderManager.SetSamplesPerPixel(spp);
        }

        // Background
        mScene.SetBackgroundColor(Color::Black);
        breakable_scope
        {
            const CValue & jsonBg = settings[{"background"}];
            if (jsonBg == null)
                break;
            
            Color color;
            if (!ParseColor(jsonBg, &color))
                break;

            mScene.SetBackgroundColor(color);
        }
    }
    
    // Camera
    breakable_scope
    {
        const CValue & jsonCamera = root[{"camera"}];
        if (jsonCamera == null)
            return false;
         
        const float32 aspect = mBackbuffer.GetWidth() / float32(mBackbuffer.GetHeight());
        mCamera.Setup(jsonCamera, aspect);
    }

    //Objects
    breakable_scope
    {
        const CValue & jsonObjects = root[{"objects"}];
        if (jsonObjects == null)
            break;

        if (jsonObjects.GetType() != EType::Array)
            break;

        for (const CValue & jsonObject : *jsonObjects.As<ArrayType>())
        {
            if (jsonObject.GetType() != EType::Object)
                continue;

            RT::Object * object = RT::ParseObject(jsonObject);
            if (!object)
                continue;

            mScene.AddObject(object);
        }
    }

    return true;
}

//=============================================================================
void Application::SceneCreateWalls()
{
	using namespace RT;

	const Vector3 mask100(1, 0, 0);
	const Vector3 mask010(0, 1, 0);
	const Vector3 mask001(0, 0, 1);
	const Vector3 mask011(0, 1, 1);
	const Vector3 mask101(1, 0, 1);
	const Vector3 mask110(1, 1, 0);
	const Aabb3   dims(Point3::Zero, WALL_EXTENTS);

	mScene.SetBackgroundColor(Color(0.1f, 0.1f, 0.1f));

	// Left
	{
		Material mat;
		mat.type     = MATERIAL_TYPE_DIFFUSE;
		mat.diffuse  = Color(0.25f, 0.75f, 0.25f);
		mat.emissive = Color(0.0f, 0.0f, 0.0f);

		Object * pObject = new RT::Aabb(
			-WALL_EXTENTS * 1.1f * mask100 - WALL_EXTENTS * mask011, 
			-WALL_EXTENTS * 1.0f * mask100 + WALL_EXTENTS * mask011, 
			mat
		);
		mScene.AddObject(pObject);
	}

	// Right
	{
		Material mat;
		mat.type     = MATERIAL_TYPE_DIFFUSE;
		mat.diffuse  = Color(0.75f, 0.25f, 0.25f);
		mat.emissive = Color(0.0f, 0.0f, 0.0f);

		Object * pObject = new RT::Aabb(
			WALL_EXTENTS * 1.0f * mask100 - WALL_EXTENTS * mask011, 
			WALL_EXTENTS * 1.1f * mask100 + WALL_EXTENTS * mask011, 
			mat
		);
		mScene.AddObject(pObject);
	}

	// Back
	{
		Material mat;
		mat.type     = MATERIAL_TYPE_DIFFUSE;
		mat.diffuse  = Color(0.75f, 0.75f, 0.75f);
		mat.emissive = Color(0.0f, 0.0f, 0.0f);

		Object * pObject = new RT::Aabb(
			WALL_EXTENTS * 1.0f * mask010 - WALL_EXTENTS * mask101, 
			WALL_EXTENTS * 1.1f * mask010 + WALL_EXTENTS * mask101, 
			mat
		);
		mScene.AddObject(pObject);
	}

	// Bottom
	{
		Material mat;
		mat.type     = MATERIAL_TYPE_DIFFUSE;
		mat.diffuse  = Color(0.75f, 0.75f, 0.75f);
		mat.emissive = Color(0.0f, 0.0f, 0.0f);

		Object * pObject = new RT::Aabb(
			-WALL_EXTENTS * 1.1f * mask001 - WALL_EXTENTS * mask110, 
			-WALL_EXTENTS * 1.0f * mask001 + WALL_EXTENTS * mask110, 
			mat
		);
		mScene.AddObject(pObject);
	}
	
	// Top
	{
		Material mat;
		mat.type     = MATERIAL_TYPE_DIFFUSE;
		mat.diffuse  = Color(0.75f, 0.75f, 0.75f);
		mat.emissive = Color(0.0f, 0.0f, 0.0f);

		Object * pObject = new RT::Aabb(
			WALL_EXTENTS * 1.0f * mask001 - WALL_EXTENTS * mask110, 
			WALL_EXTENTS * 1.1f * mask001 + WALL_EXTENTS * mask110, 
			mat
		);
		mScene.AddObject(pObject);
	}

	// Light
	{
		const float32 radius = 40.0f;
		Material mat;
		mat.type     = MATERIAL_TYPE_DIFFUSE;
		mat.diffuse  = Color(0.0f, 0.0f, 0.0f);
		mat.emissive = Color(10.0f, 10.0f, 10.0f);

		Object * pObject = new RT::Aabb(
			Point3(-radius, -radius, WALL_EXTENTS.y - 0.1f),
			Point3(+radius, +radius, WALL_EXTENTS.y - 0.0f),
			mat
		);
		mScene.AddObject(pObject);
	}

	// Camera
	{
		const float32 aspect = mBackbuffer.GetWidth() / (float32)mBackbuffer.GetHeight();
		mCamera.Setup(
            Vector3(0.0f, -2.5f * WALL_EXTENTS.y, WALL_EXTENTS.z * 0.2f),
            Vector3::Zero,
            Vector3::UnitZ,
            1.0f,
            1.0f,
            aspect);
	}

}

//=============================================================================
void Application::SceneCreateReddit()
{
	using namespace RT;

	const Material MAT_WHITE = {
		MATERIAL_TYPE_DIFFUSE,
		Color(1.0f, 1.0f, 1.0f),
		Color(0.0f, 0.0f, 0.0f)
	};

	const Material MAT_ORANGE = {
		MATERIAL_TYPE_DIFFUSE,
		Color(1.0f, 0.7f, 0.0f),
		Color(0.0f, 0.0f, 0.0f)
	};

	const Material MAT_BLACK = {
		MATERIAL_TYPE_DIFFUSE,
		Color(0.0f, 0.0f, 0.0f),
		Color(0.0f, 0.0f, 0.0f)
	};

	const float32 BODY_HEIGHT = 55;
	const float32 BODY_WIDTH  = 31;
	const float32 BODY_Z      = BODY_HEIGHT - 5;
	const float32 BODY_THICK  = 10;

	const float32 HEAD_ASPECT = 1.6f;
	const float32 HEAD_HEIGHT = 37;
	const float32 HEAD_WIDTH  = HEAD_HEIGHT * HEAD_ASPECT;
	const float32 HEAD_Z      = BODY_HEIGHT * 2 - 5;
	const float32 HEAD_THICK  = 25;

	const float32 EYE_Z       = HEAD_Z + 7;
	const float32 EYE_X       = 19;
	const float32 EYE_Y	      = -HEAD_THICK;
	const float32 EYE_RADIUS  = 8;

	const float32 FEET_WIDTH  = 18;
	const float32 FEET_HEIGHT = 11;
	const float32 FEET_Z      = 0;
	const float32 FEET_X      = 25;
	const float32 FEET_THICK  = 10;

	const float32 ARM_HEIGHT  = 28;
	const float32 ARM_WIDTH   = 20;
	const float32 ARM_X       = BODY_WIDTH - 8;
	const float32 ARM_Z       = BODY_Z - 5.5f;
	const float32 ARM_THICK   = BODY_THICK * 0.5f;

	const float32 EAR_RADIUS  = 13;
	const float32 EAR_Z       = HEAD_Z + HEAD_HEIGHT * 0.49f;
	const float32 EAR_X	      = HEAD_WIDTH * 0.87f;
	const float32 EAR_Y	      = 0.0f;
	const float32 EAR_THICK   = 5.0f;

	const float32 ANTENNA_BALL_RADIUS  = 10;
	const Vector3  ANTENNA_BALL_POS(38.5f, -30.0f, -WALL_EXTENTS.z + HEAD_Z + HEAD_HEIGHT + 16.0f);
	const Vector3  ANTENNA_BEND_POS(10.0f, -15.0f, -WALL_EXTENTS.z + HEAD_Z + HEAD_HEIGHT + 25.0f);
	const Vector3  ANTENNA_HEAD_TOP(0.0f, 0.0f, -WALL_EXTENTS.z + HEAD_Z + HEAD_HEIGHT);
	const float32 ANTENNA_RADIUS = 2.0f;

	const float32 MOUTH_RADIUS = 15.0f;
	const float32 MOUTH_THICK  = 1.0f;
	const float32 MOUTH_Y = -HEAD_THICK + MOUTH_RADIUS - 10.0f;
	const float32 MOUTH_Z = HEAD_Z - HEAD_HEIGHT * 0.5f;
	const Radian MOUTH_TILT(Degree(70));


	// Antenna
	{
		// Line 1
		{
			const Vector3 N = (ANTENNA_BEND_POS - ANTENNA_BALL_POS);
			const Point3 P = (ANTENNA_BEND_POS + ANTENNA_BALL_POS) * 0.5f;

			Vector3 U, V, W;
			BuildBasis(N, U, V, W);

			Object * pObject = new RT::Ellipsoid(
				P,
				N * 0.5f, V * ANTENNA_RADIUS, W * ANTENNA_RADIUS,
				MAT_BLACK
			);
			
			mScene.AddObject(pObject);	
		}

		// Line 2
		{
			const Vector3 N = (ANTENNA_BEND_POS - ANTENNA_HEAD_TOP);
			const Point3 P = (ANTENNA_BEND_POS + ANTENNA_HEAD_TOP) * 0.5f;

			Vector3 U, V, W;
			BuildBasis(N, U, V, W);

			Object * pObject = new RT::Ellipsoid(
				P,
				N * 0.5f, V * ANTENNA_RADIUS, W * ANTENNA_RADIUS,
				MAT_BLACK
			);
			
			mScene.AddObject(pObject);	
		}

		// Ball
		{
			Object * pObject = new RT::Sphere(
				ANTENNA_BALL_POS,
				ANTENNA_BALL_RADIUS,
				MAT_WHITE
			);
			
			mScene.AddObject(pObject);
		}
	}

	// Head
	{
		Object * pObject = new RT::Ellipsoid(
			Point3(0.0f, 0.0f, -WALL_EXTENTS.z + HEAD_Z),
			Vector3(HEAD_HEIGHT * HEAD_ASPECT, HEAD_THICK, HEAD_HEIGHT),
			MAT_WHITE
		);
		
		mScene.AddObject(pObject);
	}
	
	// Left Ear
	{
		Object * pObject = new RT::Ellipsoid(
			Point3(-EAR_X, EAR_Y, -WALL_EXTENTS.z + EAR_Z),
			Vector3(EAR_RADIUS, EAR_THICK, EAR_RADIUS),
			MAT_WHITE
		);
		
		mScene.AddObject(pObject);
	}

	// Right Ear
	{
		Object * pObject = new RT::Ellipsoid(
			Point3(EAR_X, EAR_Y, -WALL_EXTENTS.z + EAR_Z),
			Vector3(EAR_RADIUS, EAR_THICK, EAR_RADIUS),
			MAT_WHITE
		);
		
		mScene.AddObject(pObject);
	}

	// Left Eye
	{
		Object * pObject = new RT::Sphere(
			Point3(-EYE_X, EYE_Y, -WALL_EXTENTS.z + EYE_Z),
			EYE_RADIUS,
			MAT_ORANGE
		);
		
		mScene.AddObject(pObject);
	}

	// Right Eye
	{
		Object * pObject = new RT::Sphere(
			Point3(EYE_X, EYE_Y, -WALL_EXTENTS.z + EYE_Z),
			EYE_RADIUS,
			MAT_ORANGE
		);
		
		mScene.AddObject(pObject);
	}

	// Mouth
	{
		const Vector3 N(0.0f, -Cos(MOUTH_TILT), Sin(MOUTH_TILT));

		Vector3 U, V, W;
		BuildBasis(N, U, V, W);

		Object * pObject = new RT::Ellipsoid(
			Point3(0.0f, MOUTH_Y, -WALL_EXTENTS.z + MOUTH_Z),
			U * MOUTH_THICK, V * MOUTH_RADIUS, W * MOUTH_RADIUS, 
			MAT_BLACK
		);
		
		mScene.AddObject(pObject);
	}

	// Body
	{
		Object * pObject = new RT::Ellipsoid(
			Point3(0.0f, 0.0f, -WALL_EXTENTS.z + BODY_Z),
			Vector3(BODY_WIDTH, BODY_THICK, BODY_HEIGHT),
			MAT_WHITE
		);
		
		mScene.AddObject(pObject);
	}

	// Left Arm
	{
		Object * pObject = new RT::Ellipsoid(
			Point3(-ARM_X, 0.0f, -WALL_EXTENTS.z + ARM_Z),
			Vector3(ARM_WIDTH, ARM_THICK, ARM_HEIGHT),
			MAT_WHITE
		);
		
		mScene.AddObject(pObject);
	}

	// Right Arm
	{
		Object * pObject = new RT::Ellipsoid(
			Point3(ARM_X, 0.0f, -WALL_EXTENTS.z + ARM_Z),
			Vector3(ARM_WIDTH, ARM_THICK, ARM_HEIGHT),
			MAT_WHITE
		);
		
		mScene.AddObject(pObject);
	}

	// Left Foot
	{
		Object * pObject = new RT::Ellipsoid(
			Point3(-FEET_X, 0.0f, -WALL_EXTENTS.z + FEET_Z),
			Vector3(FEET_WIDTH, FEET_THICK, FEET_HEIGHT),
			MAT_WHITE
		);
		
		mScene.AddObject(pObject);
	}

	// Right Foot
	{
		Object * pObject = new RT::Ellipsoid(
			Point3(FEET_X, 0.0f, -WALL_EXTENTS.z + FEET_Z),
			Vector3(FEET_WIDTH, FEET_THICK, FEET_HEIGHT),
			MAT_WHITE
		);
		
		mScene.AddObject(pObject);
	}

}

//=============================================================================
void Application::SceneCreateSpheres()
{
	
	const float32 BALL_SIZE = 40.0f;

	using namespace RT;


	// Mirror
	{
		Material mat;
		mat.type     = MATERIAL_TYPE_REFLECT;
		mat.diffuse  = Color(1.0f, 1.0f, 1.0f);
		mat.emissive = Color(0.0f, 0.0f, 0.0f);

		Object * pObject = new RT::Sphere(
			Point3(-1.2f * BALL_SIZE, -40.0f, -WALL_EXTENTS.z + BALL_SIZE), 
			BALL_SIZE, 
			mat
		);
		mScene.AddObject(pObject);
	}

	// Glass
	{
		Material mat;
		mat.type 	 = MATERIAL_TYPE_REFRACT;
		mat.diffuse  = Color(1.0f, 1.0f, 1.0f);
		mat.emissive = Color(0.0f, 0.0f, 0.0f);

		Object * pObject = new RT::Sphere(
			Point3(1.5f * BALL_SIZE, 0.0f, -WALL_EXTENTS.z + BALL_SIZE), 
			BALL_SIZE, 
			mat
		);
		mScene.AddObject(pObject);
	}

}