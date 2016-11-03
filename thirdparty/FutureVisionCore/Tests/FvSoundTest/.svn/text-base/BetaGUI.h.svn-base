#ifndef BETAGUI
#define BETAGUI

/// Betajaen's GUI 016 Uncompressed
/// Written by Robin "Betajaen" Southern 07-Nov-2006, http://www.ogre3d.org/wiki/index.php/BetaGUI
/// This code is under the Whatevar! licence. Do what you want; but keep the original copyright header.

#include "Ogre.h"
using namespace Ogre;
using namespace std;

namespace BetaGUI {
	
	/// This code is not meant to be readable, if you base your future source on this, I will laugh at you.
	
	class GUI;
	class Window;
	class Button;
	class TextInput;
	class Callback;
	
	enum wt { /* Window Type */
		NONE=0,
		MOVE=1,
		RESIZE=2,
		RESIZE_AND_MOVE=3
	};

	typedef unsigned int uint;

	class GUI {
		public:
			friend class Window;
			friend class Button;
			friend class TextInput;
			GUI(String baseOverlay, String font, uint fontSize);
			~GUI();
			void setZOrder(unsigned int z) {
				mO->setZOrder(z);
			}

			bool injectMouse(uint x, uint y, bool LMB);
			bool injectKey(String key, uint x, uint y);
			void injectBackspace(uint x, uint y);
			Window*	 createWindow(Vector4 Dimensions, String Material, wt type, String caption = "");
			void destroyWindow(Window *window) {
				mXW=window;
			}
			OverlayContainer* createOverlay(Ogre::String name, Vector2 position, Vector2 dimensions, Ogre::String material = "", Ogre::String caption = "", bool autoAdd = true);
			OverlayContainer* createMousePointer(Vector2 dimensions, Ogre::String material);
				
			

	protected:
			Overlay* mO;						// Main sheet overlay
			std::vector<Window*>WN;					// Windows
			Window *mXW;						// Window to destroy
			OverlayContainer* mMP;				// Mouse Pointer Overlay
			String mFont;
			uint mFontSize;
			uint wc, bc, tc;
	};

	class Window {
		public:
			friend class Button;
			friend class TextInput;
			friend class GUI;
		
			Button*		createButton(Vector4 Dimensions, String Material, String Text, Callback callback);
			TextInput*	createTextInput(Vector4 Dimensions, String Material, String Value, uint length);
			OverlayContainer* createStaticText(Vector4 Dimensions, String Text);
		
			void hide(){
				mO->hide();
			}
			
			void show(){
				mO->show();
			}
			
			bool isVisible() {
				return mO->isVisible();
			}

			void setPosition(uint X,uint Y);
			Vector2 getPosition();
			Vector2 getSize();
			void setSize(uint X,uint Y);

			OverlayContainer* mO;			// Overlay

		protected:
			Window(Vector4 Dimensions, String Material, wt type, String caption, GUI *gui);
			~Window();
			
			bool check(uint x, uint y, bool LMB); bool checkKey(String key, uint x, uint y);
			TextInput* mATI;				// Active TextInput
			Button *mRZ,*mAB,*mTB;			// Resizer, ActiveButton, Titlebar
			uint x,y,w,h;					// Dimensions
			GUI *mGUI;						// mGUI pointer
			
			std::vector<BetaGUI::Button*> mB;	// Buttons
			std::vector<BetaGUI::TextInput*> mT;	// TextInputs
	};

	class BetaGUIListener {
		public:
			virtual void onButtonPress(Button*, uchar) = 0;
	};

	class Callback {
		public:
			friend class Window;
			friend class Button;
			
			Callback() {
				t=0;
			}
			
			Callback( void(*functionPointer)(BetaGUI::Button*, bool) ) {
				t=1;
				fp = functionPointer;
			}

			Callback(BetaGUIListener *l){
				t = 2;
				LS = l;
			}
		protected:
			uchar t;					// Type of callback: 0 - None, 1 - FunctionPointer 2 - GUIListener, 3 - Move Window, 4 - Resize
			void(*fp)(BetaGUI::Button*, bool);	// Function pointer (if 1)
			BetaGUIListener *LS;		// GuiListener (if 2)
	};

	class Button {
		public:
			friend class Window;
		
			Button(Vector4 Dimensions, String Material, String Text, Callback callback, Window *parent);
			~Button() {
				mO->getParent()->removeChild(mO->getName());
				mCP->getParent()->removeChild(mCP->getName());
			}
		protected:
			void activate(bool a) {
				if (!a && mmn!="")
				mO->setMaterialName(mmn);
				if (a && mma!="")
					mO->setMaterialName(mma);
			}
			bool in(uint mx, uint my, uint px, uint py) {
				return ( !(mx >= x + px && my >= y + py)) || ( ! (mx <= x + px + w && my <= y + py + h) );
			}
			OverlayContainer* mO,*mCP;			// Main Overlay and Caption
			String mmn,mma;						// MaterialName Normal, MaterialName Active
			BetaGUI::Callback callback;			// Callback to use
			uint x,y,w,h;						// Dimensions.
	};
	
	class TextInput {
		public:
			friend class Window;
			
			TextInput(Vector4 Dimensions, String Material, String Value, uint length, Window *parent);
			~TextInput(){}
			String getValue() {
				return value;
			}
			
			void setValue(String v) {
				mCP->setCaption(value=v);
			}

		protected:
			bool in(uint mx, uint my, uint px, uint py) {
				return ( !(mx >= x + px && my >= y + py)) || ( ! (mx <= x + px + w && my <= y + py + h) );
			}
			OverlayContainer* mO,*mCP;
			String mmn, mma, value;
			uint x,y,w,h,length;
	};
} // End of Namespace
#endif
