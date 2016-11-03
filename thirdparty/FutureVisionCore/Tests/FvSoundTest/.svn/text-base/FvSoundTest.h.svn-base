#ifndef __FvSoundTestApp_H__
#define __FvSoundTestApp_H__

#include <OgreCamera.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreException.h>
#include <OgreLight.h>
#include <OgreLogManager.h>
#include <OgreMeshManager.h>
#include <OgreOverlayContainer.h>
#include <OgreRoot.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreFontManager.h>
#include <RenderSystems/Direct3D9/include/OgreD3D9Plugin.h>

#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <FvSoundDefines.h>
#include <FvSoundListener.h>
#include <FvSoundManager.h>
#include "BetaGUI.h"


using namespace Ogre;

class DeviceListener : public FrameListener, public Ogre::WindowEventListener,
	public OIS::MouseListener, public OIS::KeyListener, public BetaGUI::BetaGUIListener
{
public:
	DeviceListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr);
	~DeviceListener();

	bool frameStarted(const FrameEvent& evt);

	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);

	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id){return false;}

	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);

	void onButtonPress(BetaGUI::Button *ref, Ogre::uchar type);

protected:
	OIS::Mouse *mMouse;
	OIS::Keyboard *mKeyboard;
	OIS::InputManager *mInputManager;
	Ogre::SceneManager *mSceneMgr;
	Ogre::SceneNode *mCamNode, *mPitchNode;
	Ogre::Camera *m_pkCamera;
	Ogre::RenderWindow *mWindow;
	bool mContinue;
	Ogre::Degree pitch, yaw;
	Ogre::Vector3 mDirection;
	FvSoundManager *m_pkSoundManager;
	SceneNode *carNode;
	Vector3 target;
	BetaGUI::GUI *mGui;
	BetaGUI::TextInput *doppler, *speed;
	BetaGUI::Button *dfUp01, *dfUp10, *dfDn01, *dfDn10;
	BetaGUI::Button *ssUp01, *ssUp10, *ssDn01, *ssDn10;
};

class FvSoundTestApp
{
	public:
		FvSoundTestApp();
		~FvSoundTestApp();
		void Start();

	protected:
		void CreateScene();	
	
	private:
		DeviceListener *m_pkListener;		
		Root *m_pkRoot;			
		D3D9Plugin *m_pkPlugin;					
		SceneManager *m_pkSceneMgr;			
		Camera *m_pkCamera;					
		Viewport *m_pkViewPort;				
		RenderWindow *m_pkWin;				
		Entity *m_pkEnt;					
		SceneNode *m_pkNode;				
		Light *m_pkLight;					
		FvSoundManager *m_pkSoundManager;
};

#endif // __FvSoundTestApp_H__
