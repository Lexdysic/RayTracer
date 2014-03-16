
class Application : public System::CWindowNotify
{
public:
	Application();
	~Application();

	void Run();
    
private:

	// Data
	RT::CImage        mBackbuffer;
	System::IWindow * mWindow;
	uint              mFrameTime;
	uint              mAveFrameTime;
	RT::Camera		  mCamera;
	RT::Scene		  mScene;
	RT::RenderManager mRenderManager;

	// Helpers
	void SceneCreateReddit();
	void SceneCreateSpheres();
	void SceneCreateWalls();
    bool TrySceneFromFile();
};