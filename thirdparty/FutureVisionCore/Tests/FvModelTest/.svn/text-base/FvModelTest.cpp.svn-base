#include "FvModelTest.h"

void main()
{
	FvModelTest *pkApp = new FvModelTest;

	try
	{
		pkApp->go();

	} 
	catch(Ogre::Exception& e)
	{
		std::cerr << "An exception has occured:" <<
			e.getFullDescription().c_str() << std::endl;
	}

	delete pkApp;
}
