#include "FvZoneTest.h"

bool TestApplication::frameStarted(const FvFrameEvent &kEvent)
{
	Past_Time=kEvent.timeSinceLastFrame;
	FvGraphicsCommon::Tick(Past_Time);
	FvLocalPlayer::Instance().GetPlayer()->Space();
	FvEntity *pkPlayer = FvLocalPlayer::pInstance()?FvLocalPlayer::Instance().GetPlayer():NULL;
	
	static float Zone_Time=0.0f;
	Zone_Time+=Past_Time;
	FvZoneManager::Instance().Tick(Past_Time);
	m_GameUnit._UpdateMatrix(Past_Time);
	FvClientCamera::Instance().Tick(Past_Time);
	FvPhysicsBody::TickAll(Past_Time);

	FvCamera* pkCamera = FvClientCamera::Instance().GetCamera();
	
	if (Key[OIS::KC_1])
	{
		m_Time+=Past_Time;
		FvGraphicsCommon::SetClock(m_Time);
	}
	else if (Key[OIS::KC_2])
	{
		m_Time-=Past_Time;
		FvGraphicsCommon::SetClock(m_Time);
	}
	if (FreeCamera)
	{
		if (Key[OIS::KC_W])
		{
			kPos+=pkCamera->getDirection()*m_GameUnit.m_spd*Past_Time;
		}
		if (Key[OIS::KC_S])
		{
			kPos-=pkCamera->getDirection()*m_GameUnit.m_spd*Past_Time;
		}
		Vector3 Pos=-pkCamera->getDirection()*10.0f+Vector3(kPos.x,kPos.y,kPos.z);
		m_GameUnit.SetPos(FvVector3(kPos.x,kPos.y,kPos.z));
		pkCamera->setPosition(Pos);
		
	}
	else
	{
		if (Key[OIS::KC_W])
		{
			PlayAnimation("Walk");
		}
		else if (Key[OIS::KC_S])
		{
			PlayAnimation("Walk_B");
		}
		else
		{
			m_GameUnit.GameUnitAvatar().GetAvatarAttachMent()->GetCharacter()->RemoveLevel("Move");
			PlayAnimation("Idle",FvAnimatedCharacter::DEFAULT);
			m_Controller->OnKeyReleased(OIS::KC_W);
		}
		FvClientCamera::Instance().Tick(Past_Time);
		FvVector3 m_pos=m_GameUnit._PhysicsBody()->Position();

		m_GameUnit.SetPos(m_pos);
		pkAttachment1[0]->SetPosition(m_pos);
		pkAttachment1[1]->SetPosition(m_pos);
		pkAttachment->SetPosition(m_pos);
		FvQuaternion kQuat;
		Vector3 Der = pkCamera->getDirection();
		m_fAvatarRadian = atan2(Der.y,Der.x)+3.14f/2.0f;
		m_Controller->k_fAvatarRadianX=m_fAvatarRadian;
		kQuat.FromAngleAxis(m_fAvatarRadian,FvVector3::UNIT_Z);

		pkAttachment->SetQuaternion(kQuat);
		kPos = Vector3(m_pos.x,m_pos.y,m_pos.z);
	}
	const float fYaw = FvClientCamera::Instance().DiretionCursor().Direction()->GetData0().Yaw();
	char str[100];
	sprintf(str,"%f %f %f %f",kPos.x,kPos.y,kPos.z,fYaw);
	SetWindowTile(FvString(str));
	return FvApplication::frameStarted(kEvent);
}
void TestApplication::UpCamera()
{
	//Quaternion m_Qrien,m_Qrien1;
	//m_Qrien.FromAngleAxis(Radian(k_fAvatarRadianX),Vector3::UNIT_Z);
	//m_Qrien1.FromAngleAxis(Radian(k_fAvatarRadianY),Vector3::UNIT_X);
	//m_Qrien=m_Qrien*m_Qrien1;
	//FvCamera* pkCamera = FvClientCamera::Instance();
	//pkCamera->setOrientation(m_Qrien);
}
bool TestApplication::mouseMoved( const FvMouseEvent &kEvent )
{
	//if (FreeCamera)
	//{
	//	if (MouseKey[OIS::MB_Right])
	//	{
	//		float moue_x=((float)kEvent.state.X.rel*Past_Time/1.0f);
	//		float moue_y=((float)kEvent.state.Y.rel*Past_Time/1.0f);

	//		k_fAvatarRadianX-=moue_x*MouseSpd/10.0f;
	//		k_fAvatarRadianY-=moue_y*MouseSpd/10.0f;

	//		UpCamera();
	//	}
	//}
	//else
	{
		if (MouseKey[OIS::MB_Right])
		{
			m_Controller->OnMouseMove(kEvent.state);
			m_Controller->_OnLeftMoving2(kEvent.state);
		}
	}
	float AddSpd=((float)kEvent.state.Z.rel*Past_Time/1.0f);
	m_GameUnit.m_spd+=AddSpd*1.0f;
	if (m_GameUnit.m_spd<0.1f)
	{
		m_GameUnit.m_spd=0.1f;
	}
	if (m_GameUnit.m_spd>70.1f)
	{
		m_GameUnit.m_spd=70.0f;
	}
	return FvApplication::mouseMoved(kEvent);
}
bool TestApplication::keyPressed( const FvKeyEvent &kEvent )
{
	if (kEvent.key == OIS::KC_ESCAPE)
	{
		exit(0);
	}
	if (kEvent.key == OIS::KC_3)
	{
		FreeCamera=!FreeCamera;
	}
	if (!FreeCamera)
	{
		m_Controller->OnKeyPressed(kEvent.key);
	}
	
	Key[kEvent.key]=true;
	return FvApplication::keyPressed(kEvent);
}
bool TestApplication::keyReleased( const FvKeyEvent &kEvent )
{
	if (!FreeCamera)
	{
		m_Controller->OnKeyReleased(kEvent.key);
	}
	Key[kEvent.key]=false;
	return FvApplication::keyReleased(kEvent);
}
bool TestApplication::mousePressed( const FvMouseEvent &kEvent, FvMouseButtonID eID )
{	
	MouseKey[eID]=true;
	return FvApplication::mousePressed(kEvent,eID);
}
bool TestApplication::mouseReleased( const FvMouseEvent &kEvent, FvMouseButtonID eID )
{	
	MouseKey[eID]=false;
	return FvApplication::mouseReleased(kEvent,eID);
}