#include "FvSoundTest.h"

#define ROTATION_SPEED 10
#define MOVEMENT_SPEED 500

void DeviceListener::onButtonPress(BetaGUI::Button *ref, Ogre::uchar type)
{
	String textValue;
	Real floatValue;
	if(type == 1) // button down
	{
		if(ref == dfUp01)
		{
			textValue = doppler->getValue();
			floatValue = Ogre::StringConverter::parseReal(textValue);
			floatValue += 0.01;
			textValue = Ogre::StringConverter::toString(floatValue);
			doppler->setValue(textValue);
			FvSoundManager::getSingleton().SetDopplerFactor(floatValue);
		}
		else if(ref == dfUp10)
		{
			textValue = doppler->getValue();
			floatValue = Ogre::StringConverter::parseReal(textValue);
			floatValue += 0.1;
			textValue = Ogre::StringConverter::toString(floatValue);
			doppler->setValue(textValue);
			FvSoundManager::getSingleton().SetDopplerFactor(floatValue);
		}
		else if(ref == dfDn01)
		{
			textValue = doppler->getValue();
			floatValue = Ogre::StringConverter::parseReal(textValue);
			floatValue -= 0.01;
			if(floatValue < 0) floatValue = 0;
			textValue = Ogre::StringConverter::toString(floatValue);
			doppler->setValue(textValue);
			FvSoundManager::getSingleton().SetDopplerFactor(floatValue);
		}
		else if(ref == dfDn10)
		{
			textValue = doppler->getValue();
			floatValue = Ogre::StringConverter::parseReal(textValue);
			floatValue -= 0.1;
			if(floatValue < 0) floatValue = 0;
			textValue = Ogre::StringConverter::toString(floatValue);
			doppler->setValue(textValue);
			FvSoundManager::getSingleton().SetDopplerFactor(floatValue);
		}
		else if(ref == ssUp01)
		{
			textValue = speed->getValue();
			floatValue = Ogre::StringConverter::parseReal(textValue);
			floatValue += 1;
			textValue = Ogre::StringConverter::toString(floatValue);
			speed->setValue(textValue);
			FvSoundManager::getSingleton().SetSpeedOfSound(floatValue);
		}
		else if(ref == ssUp10)
		{
			textValue = speed->getValue();
			floatValue = Ogre::StringConverter::parseReal(textValue);
			floatValue += 10;
			textValue = Ogre::StringConverter::toString(floatValue);
			speed->setValue(textValue);
			FvSoundManager::getSingleton().SetSpeedOfSound(floatValue);
		}
		else if(ref == ssDn01)
		{
			textValue = speed->getValue();
			floatValue = Ogre::StringConverter::parseReal(textValue);
			floatValue -= 1;
			if(floatValue < 0) floatValue = 0;
			textValue = Ogre::StringConverter::toString(floatValue);
			speed->setValue(textValue);
			FvSoundManager::getSingleton().SetSpeedOfSound(floatValue);
		}
		else if(ref == ssDn10)
		{
			textValue = speed->getValue();
			floatValue = Ogre::StringConverter::parseReal(textValue);
			floatValue -= 10;
			if(floatValue < 0) floatValue = 0;
			textValue = Ogre::StringConverter::toString(floatValue);
			speed->setValue(textValue);
			FvSoundManager::getSingleton().SetSpeedOfSound(floatValue);
		}
	}
}

DeviceListener::DeviceListener(RenderWindow* win, Camera* cam, SceneManager *m_pkSceneMgr)
{
	// Populate the m_pkCamera and scene manager containers
	mSceneMgr = m_pkSceneMgr;
	m_pkCamera = cam;
	mWindow = win;

	// Get the m_pkCamera m_pkNode
	mPitchNode = m_pkCamera->getParentSceneNode();
	mCamNode = mPitchNode->getParentSceneNode();

	// continue rendering
	mContinue = true;

	// Initialize the direction vector
	mDirection = Vector3::ZERO;
	yaw = 0;
	pitch = 0;

	// Initialize OIS
	OIS::ParamList params;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	params.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));

	mInputManager = OIS::InputManager::createInputSystem(params);

	//if(mInputManager->numMice() > 0)
	//{
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
		mMouse->setEventCallback(this);
	//}
	//if(mInputManager->numKeyboards() > 0)
	//{
		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
		mKeyboard->setEventCallback(this);
	//}

	windowResized(mWindow);
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	carNode = mSceneMgr->getSceneNode("CarNode");

	m_pkSoundManager = FvSoundManager::getSingletonPtr();

	/******************* CREATE GUI ***************************/
	mGui = new BetaGUI::GUI("DopplerGui", "Arial", 14);
	BetaGUI::Window *mainWindow = mGui->createWindow(Vector4(mWindow->getWidth()-235, 10, 225, 95),
		"bgui.window", BetaGUI::MOVE, "Doppler Settings");
	mainWindow->createStaticText(Vector4(5, 25, 100, 15), "Doppler Factor:");
	doppler = mainWindow->createTextInput(Vector4(5, 40, 100, 20), "bgui.textinput", "1.0", 3);
	dfDn10 = mainWindow->createButton(Vector4(5, 65, 21, 24), "bgui.button", "<<", BetaGUI::Callback(this));
	dfDn01 = mainWindow->createButton(Vector4(31, 65, 21, 24), "bgui.button", "<", BetaGUI::Callback(this));
	dfUp01 = mainWindow->createButton(Vector4(57, 65, 21, 24), "bgui.button", " >", BetaGUI::Callback(this));
	dfUp10 = mainWindow->createButton(Vector4(83, 65, 21, 24), "bgui.button", ">>", BetaGUI::Callback(this));
	mainWindow->createStaticText(Vector4(115, 25, 100, 15), "Speed of Sound:");
	speed = mainWindow->createTextInput(Vector4(115, 40, 100, 20), "bgui.textinput", "343.3", 5);
	ssDn10 = mainWindow->createButton(Vector4(116, 65, 21, 24), "bgui.button", "<<", BetaGUI::Callback(this));
	ssDn01 = mainWindow->createButton(Vector4(142, 65, 21, 24), "bgui.button", "<", BetaGUI::Callback(this));
	ssUp01 = mainWindow->createButton(Vector4(168, 65, 21, 24), "bgui.button", " >", BetaGUI::Callback(this));
	ssUp10 = mainWindow->createButton(Vector4(194, 65, 21, 24), "bgui.button", ">>", BetaGUI::Callback(this));
}

DeviceListener::~DeviceListener()
{
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
}

void DeviceListener::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void DeviceListener::windowClosed(Ogre::RenderWindow* rw)
{
	if(rw == mWindow)
	{
		if(mInputManager)
		{
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}

		Ogre::Root::getSingleton().removeFrameListener(this);
		Ogre::Root::getSingleton().queueEndRendering();
	}
}

// Most code will go here. This gets called every frame
bool DeviceListener::frameStarted(const FrameEvent& evt)
{
	mMouse->capture();
	mKeyboard->capture();

	// Rotate the m_pkCamera
	mCamNode->yaw(yaw * evt.timeSinceLastFrame);
	mPitchNode->pitch(pitch * evt.timeSinceLastFrame);
	// Reset the variables
	yaw = pitch = 0;

	// Move the m_pkCamera
	mCamNode->translate(mPitchNode->getOrientation() * mDirection * evt.timeSinceLastFrame);

	carNode->yaw(Degree(-100 * evt.timeSinceLastFrame));
	//m_pkSoundManager->GetSound("BusSound")->SetVelocity(carNode->getOrientation().zAxis() * 100);

	const RenderWindow::FrameStats& stats = mWindow->getStatistics();
	OverlayManager::getSingleton().getOverlayElement("TextAreaName")->setCaption(
		StringConverter::toString(mWindow->getAverageFPS()) +
		"\n\nDoppler:    Speed of Sound:\nF1 = Up    F3 = Up\nF2 = Down F6 = Down");

	return  mContinue;
}

// Mouse event callbacks
bool DeviceListener::mouseMoved(const OIS::MouseEvent &arg)
{
	if(arg.state.buttonDown(OIS::MB_Right))
	{
		// This populates the m_pkCamera rotation variables
		yaw = -Degree(arg.state.X.rel * ROTATION_SPEED);
		pitch = -Degree(arg.state.Y.rel * ROTATION_SPEED);
	}
	else if(arg.state.buttonDown(OIS::MB_Left))
	{
		mGui->injectMouse(arg.state.X.abs, arg.state.Y.abs, true);
	}
	else
	{
		mGui->injectMouse(arg.state.X.abs, arg.state.Y.abs, false);
	}

	return true;
}

// MouseListener Callbacks
bool DeviceListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	mGui->injectMouse(arg.state.X.abs, arg.state.Y.abs, true);
	return true;
}

// KeyListener Callbacks
bool DeviceListener::keyPressed(const OIS::KeyEvent &arg)
{
	static int count = 0;
	float factor;
	switch(arg.key)
	{
	case OIS::KC_ESCAPE:
		mContinue = false;
		break;

	case OIS::KC_SYSRQ:
		mWindow->writeContentsToFile("ScreenShot"+StringConverter::toString(count++)+".png");
		break;

	case OIS::KC_F1:
		m_pkSoundManager->SetDopplerFactor(m_pkSoundManager->GetDopplerFactor() + 1.0);
		break;

	case OIS::KC_F2:
		factor = m_pkSoundManager->GetDopplerFactor();
		if(factor - 1.0 >= 0)
			m_pkSoundManager->SetDopplerFactor(factor - 1.0);
		break;

	case OIS::KC_F3:
		m_pkSoundManager->SetSpeedOfSound(m_pkSoundManager->GetSpeedOfSound() + 50.0);
		break;

	case OIS::KC_F4:
		factor = m_pkSoundManager->GetSpeedOfSound();
		if(factor - 50.0 >= 0)
			m_pkSoundManager->SetSpeedOfSound(factor - 50.0);
		break;

	case OIS::KC_UP:
	case OIS::KC_W:
		mDirection.z -= MOVEMENT_SPEED;
		break;

	case OIS::KC_DOWN:
	case OIS::KC_S:
		mDirection.z += MOVEMENT_SPEED;
		break;

	case OIS::KC_LEFT:
	case OIS::KC_A:
		mDirection.x -= MOVEMENT_SPEED;
		break;

	case OIS::KC_RIGHT:
	case OIS::KC_D:
		mDirection.x += MOVEMENT_SPEED;
		break;

	case OIS::KC_PGUP:
	case OIS::KC_E:
		mDirection.y += MOVEMENT_SPEED;
		break;

	case OIS::KC_PGDOWN:
	case OIS::KC_Q:
		mDirection.y -= MOVEMENT_SPEED;
		break;
	}
	return true;
}

bool DeviceListener::keyReleased(const OIS::KeyEvent &arg)
{
	switch(arg.key)
	{
	case OIS::KC_UP:
	case OIS::KC_W:
		mDirection.z += MOVEMENT_SPEED;
		break;

	case OIS::KC_DOWN:
	case OIS::KC_S:
		mDirection.z -= MOVEMENT_SPEED;
		break;

	case OIS::KC_LEFT:
	case OIS::KC_A:
		mDirection.x += MOVEMENT_SPEED;
		break;

	case OIS::KC_RIGHT:
	case OIS::KC_D:
		mDirection.x -= MOVEMENT_SPEED;
		break;

	case OIS::KC_PGUP:
	case OIS::KC_E:
		mDirection.y -= MOVEMENT_SPEED;
		break;

	case OIS::KC_PGDOWN:
	case OIS::KC_Q:
		mDirection.y += MOVEMENT_SPEED;
		break;
	}

	return true;
}


FvSoundTestApp::FvSoundTestApp()
{
    m_pkRoot = new Root;
	m_pkPlugin = new D3D9Plugin;
	m_pkRoot->installPlugin(m_pkPlugin);

    ConfigFile cf;
    cf.load("resources.cfg");

    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }

	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	if(!m_pkRoot->restoreConfig())
	{
		if(!m_pkRoot->showConfigDialog())
		{
			exit(0);
		}
	}

    m_pkWin = m_pkRoot->initialise(true, "Ogre Framework");

	m_pkSceneMgr = m_pkRoot->createSceneManager(ST_GENERIC, "MainSceneManager");

    m_pkCamera = m_pkSceneMgr->createCamera("SimpleCamera");
    m_pkViewPort = m_pkRoot->getAutoCreatedWindow()->addViewport(m_pkCamera);
    m_pkViewPort->setBackgroundColour(ColourValue(0,0,0));
    m_pkCamera->setAspectRatio(Real(m_pkViewPort->getActualWidth()) / Real(m_pkViewPort->getActualHeight()));
    m_pkCamera->setNearClipDistance(1.0);

	m_pkSoundManager = new FvSoundManager();

	CreateScene();

    m_pkListener = new DeviceListener(m_pkWin, m_pkCamera, m_pkSceneMgr);
    m_pkRoot->addFrameListener(m_pkListener);
}

FvSoundTestApp::~FvSoundTestApp()
{
    delete m_pkListener;
	delete m_pkSoundManager;
    delete m_pkRoot;
}

void FvSoundTestApp::Start()
{
    m_pkRoot->startRendering();
}

void FvSoundTestApp::CreateScene()
{
	m_pkSceneMgr->setSkyBox(true, "Sky", 5, 8, 4000);
	m_pkSceneMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));
	m_pkSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	m_pkNode = m_pkSceneMgr->getRootSceneNode()->createChildSceneNode("CarNode");
	m_pkNode = m_pkNode->createChildSceneNode(Vector3(50, 0, 0));
	m_pkEnt = m_pkSceneMgr->createEntity("Civic", "Civic.mesh");
	m_pkNode->attachObject(m_pkEnt);

	//FvSound *sound = m_pkSoundManager->CreateSound("BusSound", "test\\UR_Kologarn_Aggro01.wav", true);
	FvSound *sound = m_pkSoundManager->CreateSound("BusSound", "motor_b8.wav", true);
	//FvSound *sound = m_pkSoundManager->CreateSound("BusSound", "Zero Factor - Untitled.ogg", true);
	sound->SetMaxGain(10.0f);
	//m_pkNode->attachObject(sound);
	sound->SetPosition(0,0,0);
	sound->Play();

	m_pkNode = m_pkSceneMgr->getRootSceneNode()->createChildSceneNode("CameraNode");
	m_pkNode->setPosition(-1.6, 1.46, 60.39);
	m_pkNode = m_pkNode->createChildSceneNode("PitchNode");
	m_pkNode->attachObject(m_pkCamera);
	//m_pkNode->attachObject(m_pkSoundManager->GetListener());
	m_pkSoundManager->GetListener()->SetPosition(0,0,0);

	// Create a ground plane
	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane("ground",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		15000,15000,20,20,true,1,20,20,Vector3::UNIT_Z);
	m_pkEnt = m_pkSceneMgr->createEntity("GroundEntity", "ground");
	m_pkSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(m_pkEnt);
	m_pkEnt->setMaterialName("Ground");
	m_pkEnt->setCastShadows(false);

	m_pkLight = m_pkSceneMgr->createLight("sun");
	m_pkLight->setType(Light::LT_DIRECTIONAL);
	m_pkLight->setDirection(-1,-1,-1);

	OverlayManager& overlayMgr = OverlayManager::getSingleton();
	Ogre::OverlayContainer* panel = static_cast<OverlayContainer*>(
		overlayMgr.createOverlayElement("Panel", "PanelName"));
	panel->setMetricsMode(Ogre::GMM_PIXELS);
	panel->setPosition(10, 10);
	panel->setDimensions(100, 100);

	TextAreaOverlayElement* textArea = static_cast<TextAreaOverlayElement*>(
		overlayMgr.createOverlayElement("TextArea", "TextAreaName"));
	textArea->setMetricsMode(Ogre::GMM_PIXELS);
	textArea->setPosition(0, 0);
	textArea->setDimensions(100, 100);
	textArea->setCharHeight(16);
	textArea->setFontName("Arial");
	textArea->setCaption("Hello, World!");

	Ogre::Overlay* overlay = overlayMgr.create("AverageFps");
	overlay->add2D(panel);
	panel->addChild(textArea);
	overlay->show();
}

//#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
//#else
int main(int argc, char **argv)
//#endif
{
	try
	{
		FvSoundTestApp kApp;
		kApp.Start();
	}
	catch(Exception& e)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!",
			MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		fprintf(stderr, "An exception has occured: %s\n", e.getFullDescription().c_str());
#endif

		LogManager::getSingleton().logMessage(e.getFullDescription().c_str());
	}
	return 0;
}
