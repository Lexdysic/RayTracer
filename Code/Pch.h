#ifdef PCH_H
#  error "Cannot include header more than once."
#endif
#define PCH_H

#define USES_ENGINE_MATH
#define USES_ENGINE_GEOMETRY
#define USES_ENGINE_SYSTEM
#define USES_ENGINE_THREAD
#define USES_ENGINE_COLOR
#define USES_ENGINE_TIME
#define USES_ENGINE_SERVICES_JSON

#include "EngineDeps.h"


#include "Image.h"
#include "Camera.h"
#include "Object.h"
#include "Scene.h"
#include "Renderer.h"
#include "RenderManager.h"
#include "RayTracerApplication.h"
#include "Light.h"