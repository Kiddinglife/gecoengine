/// Betajaen's GUI 016 Uncompressed
/// Written by Robin "Betajaen" Southern 07-Nov-2006, http://www.ogre3d.org/wiki/index.php/BetaGUI
/// This code is under the Whatevar! licence. Do what you want; but keep the original copyright header.
#include "BetaGUI.h"

using namespace Ogre;
using namespace std;

namespace BetaGUI {
	GUI::GUI(String baseOverlay, String font, uint fontSize)
		:mXW(0),
		 mMP(0),
		 mFont(font),
		 mFontSize(fontSize),
		 wc(0),
		 bc(0),
		 tc(0)
	{
			mO = OverlayManager::getSingleton().create(baseOverlay);
			mO->show();
	}
	
	GUI::~GUI() {
		for(uint i=0;i < WN.size();i++)
			delete WN[i];
		WN.clear();
	}
	
	bool GUI::injectMouse(uint x, uint y, bool LMB) {
		
		if (mMP)
			mMP->setPosition(x,y);

		if (mXW) {
			for(std::vector<Window*>::iterator i=WN.begin();i!=WN.end();i++) {
				if(mXW==(*i)) {
					delete mXW;
					WN.erase(i);
					mXW=0;
					return false;
				}
			}
		}

		
		for(uint i=0;i<WN.size();i++) {
			if(WN[i]->check(x,y,LMB)) {
				return true;
			}
		}

		return false;
	}	
	bool GUI::injectKey(String key, uint x, uint y) {
		for(uint i=0;i<WN.size();i++) {
			if(WN[i]->checkKey(key,x,y)) {
				return true;
			}
		}
		return false;
	}
	
	void GUI::injectBackspace(uint x, uint y) {
		injectKey("!b", x, y);
	}

	OverlayContainer* GUI::createOverlay(String name, Vector2 position, Vector2 dimensions, String material, String caption, bool autoAdd) {
		String type="Panel";
		
		if (caption!="")
			type="TextArea";
		
		OverlayContainer* e=static_cast<OverlayContainer*>(OverlayManager::getSingleton().createOverlayElement(type, name));

		e->setMetricsMode(Ogre::GMM_PIXELS);
		e->setDimensions(dimensions.x, dimensions.y);
		e->setPosition(position.x, position.y);

		if (material!="")
			e->setMaterialName(material);
		
		if (caption!="") {
			e->setCaption(caption);
			e->setParameter("font_name",mFont);
			e->setParameter("char_height",StringConverter::toString(mFontSize));
		}

		if(autoAdd) {
			mO->add2D(e);
			e->show();
		}
		
		return e;
	}

	OverlayContainer* GUI::createMousePointer(Vector2 d, String m) {
		Overlay* o = OverlayManager::getSingleton().create("BetaGUI.MP");
		o->setZOrder(649);
		mMP = createOverlay("bg.mp", Vector2(0,0), d, m, "", false);
		o->add2D(mMP);
		o->show();
		mMP->show();
		return mMP;
	}

	Window* GUI::createWindow(Vector4 Dimensions, String Material, wt type, String caption) {
		Window* window = new BetaGUI::Window(Dimensions, Material, type, caption, this);
		WN.push_back(window);
		return window;
	}

	Window::Window(Vector4 Dimensions, String Material, wt t, String caption, GUI *gui)
		: x(Dimensions.x), 
		  y(Dimensions.y),
		  w(Dimensions.z),
		  h(Dimensions.w),
		  mGUI(gui),
		  mTB(0),
		  mRZ(0),
		  mATI(0),
		  mAB(0) 
	{

		mO = gui->createOverlay(gui->mO->getName() + ".w" + StringConverter::toString(gui->wc++), Vector2(Dimensions.x,Dimensions.y), Vector2(Dimensions.z,Dimensions.w), Material); 
		if (t >= 2) {
			Callback c;c.t=4;
			mRZ=createButton(Vector4(Dimensions.z-16,Dimensions.w-16,16,16),Material+".resize", "",c);
		}

		if (t == 1 || t == 3) {
			Callback c;c.t=3;
			mTB = createButton(Vector4(0,0,Dimensions.z,22),Material + ".titlebar", caption, c);
		}

	}

	Window::~Window() {
		for(uint i=0;i<mB.size();i++)
			delete mB[i];
		
		for(uint i=0;i<mT.size();i++)
			delete mT[i];
		
		mGUI->mO->remove2D(mO);
	}

	void Window::setPosition(uint X,uint Y) {
		mO->setPosition(x=X,y=Y);
	}

	Vector2 Window::getPosition() {
		return Vector2(x,y);
	}

	Vector2 Window::getSize() {
		return Vector2(w,h);
	}

	void Window::setSize(uint X,uint Y) {
		mO->setDimensions(w=X,h=Y);
		mRZ->mO->setPosition(mRZ->x=w-16,mRZ->y=h-16);
		if(mTB){
			mTB->mO->setWidth(mTB->w=w);
		}
	}

	Button* Window::createButton(Vector4 D, String M, String T, Callback C) {
		Button *x = new Button(D, M, T, C, this);
		mB.push_back(x);
		return x;
	}

	Button::Button(Vector4 Dimensions, String m, String Text, Callback cb, Window *parent)
		: x(Dimensions.x),
		  y(Dimensions.y),
		  w(Dimensions.z),
		  h(Dimensions.w),
		  mmn(m),
		  mma(m)
	{
		Ogre::ResourcePtr ma=Ogre::MaterialManager::getSingleton().getByName(mmn + ".active");
		if(!ma.isNull())
			mma += ".active";
		
		mO = parent->mGUI->createOverlay(parent->mO->getName() + "b" + StringConverter::toString(parent->mGUI->bc++), Vector2(x,y),Vector2(w,h),m,"",false);
		mCP = parent->mGUI->createOverlay(mO->getName() + "c",Vector2(4,(h - parent->mGUI->mFontSize) / 2),Vector2(w, h),"",Text,false);
		parent->mO->addChild(mO);
		mO->show();
		mO->addChild(mCP);
		mCP->show();
		callback = cb;
	}

	TextInput* Window::createTextInput(Vector4 D, String M, String V, uint L) {
		TextInput *x = new TextInput(D,M,V,L,this);
		mT.push_back(x);
		return x;
	}

	OverlayContainer* Window::createStaticText(Vector4 D, String T) {
		OverlayContainer* x = mGUI->createOverlay(mO->getName() + "st." + StringConverter::toString(mGUI->tc++),Vector2(D.x,D.y),Vector2(D.z,D.w),"", T,false);
		mO->addChild(x);
		x->show();
		return x;
	}
	
	TextInput::TextInput(Vector4 D, String M, String V, uint L, Window *P)
		: x(D.x),y(D.y),w(D.z),h(D.w),value(V),mmn(M), mma(M), length(L) 
	{
		ResourcePtr ma=Ogre::MaterialManager::getSingleton().getByName(mmn + ".active");
		if(!ma.isNull())
			mma += ".active";

		mO=P->mGUI->createOverlay(P->mO->getName() + "t" + StringConverter::toString(P->mGUI->tc++) ,Vector2(x,y),Vector2(w,h),M,"",false);
		mCP=P->mGUI->createOverlay(mO->getName() + "c",Vector2(4,(h - P->mGUI->mFontSize) / 2),Vector2(w,h),"",V,false);
		P->mO->addChild(mO);
		mO->show();
		mO->addChild(mCP);
		mCP->show();
	}
	
	bool Window::checkKey(String k, uint px, uint py) {
		
		if (!mO->isVisible())
			return false;

		if(!(px>=x&&py>=y)||!(px<=x+w&&py<=y+h))return false;
		
		if(mATI == 0)
			return false;
		
		if(k=="!b") {
			mATI->setValue(mATI->value.substr(0,mATI->value.length()-1));
			return true;
		}

		if(mATI->value.length() >= mATI->length)
			return true;
		
		mATI->mCP->setCaption(mATI->value+=k);
		return true;
	}
	
	bool Window::check(uint px, uint py, bool LMB) {
		
		if (!mO->isVisible())
			return false;
		
		if (!(px >= x && py >= y) || !(px <= x + w && py <= y + h))	{
			if (mAB) {
				if (mAB->callback.t == 2) {
					mAB->callback.LS->onButtonPress(mAB, 3);
				}
				mAB->activate(false);
				mAB = 0;
			}
			return false;
		}
		
		for (uint i=0;i < mB.size();i++) {
			if (mB[i]->in(px,py,x,y))
				continue;
		
			if (mAB) {
				if (mAB != mB[i]) {
					mAB->activate(false);
					if (mAB->callback.t == 2) {
						mAB->callback.LS->onButtonPress(mAB, 3);
					}
				}
			}


			mAB=mB[i];
			mAB->activate(true);
			
			if(mATI && LMB) {
				mATI->mO->setMaterialName(mATI->mmn);
				mATI=0;
			}

			switch(mAB->callback.t) {
				default: return true;
				case 1: 
					mAB->callback.fp(mAB, LMB);
					return true;
				case 2:
					mAB->callback.LS->onButtonPress(mAB, LMB);
					return true;
				case 3:
					if (LMB)
						setPosition(px-(mAB->w/2),py-(mAB->h/2));
					return true;
				case 4:
					if (LMB)
						setSize(px-x+8,py-y+8);
				return true;
			}

		}

		if (!LMB)
			return false;

		for (uint i=0;i<mT.size();i++) {
	
			if (mT[i]->in(px,py,x,y))
				continue;

			mATI=mT[i];
			mATI->mO->setMaterialName(mATI->mma);
			return true;
		}
		
		if(mATI) {
			mATI->mO->setMaterialName(mATI->mmn);
			mATI=0;
			return true;
		}
		
		return false;
	}

} // End of Betajaen's GUI. Normal programming can resume now.
