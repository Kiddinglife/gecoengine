//{future header message}
#include "FvMathAlgorithmsTest.h"
#include <stdio.h>
#include <stdlib.h>
#include <d3dx9.h>
#include <Ogre.h>
#pragma comment(lib, "d3dx9.lib")

//----------------------------------------------------------------------------
void OutputVector2(const D3DXVECTOR2& kVector)
{
	printf_s("Vector2: %f,%f\n", kVector.x, kVector.y);
}
//----------------------------------------------------------------------------
void OutputVector3(const D3DXVECTOR3& kVector)
{
	printf_s("Vector3: %f,%f,%f\n", kVector.x, kVector.y, kVector.z);
}
//----------------------------------------------------------------------------
void OutputVector4(const D3DXVECTOR4& kVector)
{
	printf_s("Vector4: %f,%f,%f,%f\n", kVector.x, kVector.y, kVector.z,
		kVector.w);
}
//---------------------------------------------------------------------------
FvMathAlgorithmsTest::FvMathAlgorithmsTest()
{
	m_bTestRandom = true;
	m_bTestAngle = false;
	m_bTestQuaternion = true;
	m_bTestOther = true;
	m_bTestVector = true;
}
//---------------------------------------------------------------------------
bool FvMathAlgorithmsTest::RunTest()
{
	bool bRes = true;
	if(m_bTestRandom)
		if(!TestRandom())
			bRes = false;
	if(m_bTestAngle)
		if(!TestAngle())
			bRes = false;
	if(m_bTestOther)
		if(!TestOther())
			bRes = false;
	if(m_bTestVector)
		if(!TestVector())
			bRes = false;
	if(m_bTestOther)
		if(!TestMatrix())
			bRes = false;
	if(m_bTestQuaternion)
		if(!TestQuaternion())
			bRes = false;

	return bRes;
}
//---------------------------------------------------------------------------
void FvMathAlgorithmsTest::SetTestRandom(bool bTest)
{
	m_bTestRandom = bTest;
}
//---------------------------------------------------------------------------
bool FvMathAlgorithmsTest::TestRandom()
{
	printf_s("----------Random Test Start----------\n");
	printf_s("RAND_MAX is: %d\n", RAND_MAX);
	for(int i(0); i <= 100; i++)
		printf_s("FvRand%d is: %d\n", i, FvRand());
	for(int i(0); i <= 100; i++)
		printf_s("FvSymmetricRandom%d is: %f\n", i, FvSymmetricRandomf());
	for(int i(0); i <= 100; i++)
		printf_s("FvUnitRandom%d is: %f\n", i, FvUnitRandomf());
	for(int i(0); i <= 100; i++)
		printf_s("FvRange[50, 100]Random%d is: %f\n", i, FvRangeRandomf(50, 100));
	printf_s("-----------Random Test End-----------\n");
	return true;
}
//---------------------------------------------------------------------------
void FvMathAlgorithmsTest::SetTestAngle(bool bTest)
{
	m_bTestAngle = bTest;
}
//---------------------------------------------------------------------------
bool FvMathAlgorithmsTest::TestAngle()
{
	/*printf_s("----------Angle Test Start----------\n");
	printf_s("(FV_MATH_TWOPI + 0.3) to normalise is %f\n", FvAngleNormalise(FV_MATH_TWOPI + 0.3f));
	printf_s("(-FV_MATH_TWOPI - 0.3) to normalise is %f\n", FvAngleNormalise(-FV_MATH_TWOPI - 0.3f));
	printf_s("(FV_MATH_3PI + 0.3) to normalise is %f\n", FvAngleNormalise(FV_MATH_3PI + 0.3f));
	printf_s("(-FV_MATH_3PI - 0.3) to normalise is %f\n", FvAngleNormalise(-FV_MATH_3PI - 0.3f));
	printf_s("(0.3) to normalise is %f\n", FvAngleNormalise(0.3f));
	printf_s("(-0.3) to normalise is %f\n", FvAngleNormalise(-0.3f));
	printf_s("(FV_MATH_3PI * 5 + 0.3) to normalise is %f\n", FvAngleNormalise(FV_MATH_3PI * 5 + 0.3f));
	printf_s("(-FV_MATH_3PI * 5 - 0.3) to normalise is %f\n", FvAngleNormalise(-FV_MATH_3PI * 5 - 0.3f));

	printf_s("FvAngleSameSign(0.7f, 1.8f) is %f\n", FvAngleSameSign(0.7f, 1.8f));
	printf_s("FvAngleSameSign(0.7f, 4.8f) is %f\n", FvAngleSameSign(0.7f, 4.8f));
	printf_s("FvAngleSameSign(0.7f, -3.8f) is %f\n", FvAngleSameSign(0.7f, -3.8f));

	printf_s("FvAngleTurnRange(0.3f, 7.8f) is %f\n", FvAngleTurnRange(0.3f, 7.8f));
	printf_s("FvAngleTurnRange(0.3f, -7.8f) is %f\n", FvAngleTurnRange(0.3f, -7.8f));
	printf_s("-----------Angle Test End-----------\n");*/
	return true;
}
//---------------------------------------------------------------------------
void FvMathAlgorithmsTest::SetTestQuaternion(bool bTest)
{
	m_bTestQuaternion = bTest;
}
//---------------------------------------------------------------------------
void OutputQuaternion(const D3DXQUATERNION& kQuat)
{
	printf_s("Quaternion: x = %f, y = %f, z = %f, w = %f\n",
		kQuat.x, kQuat.y, kQuat.z, kQuat.w);
}
//---------------------------------------------------------------------------
void OutputMatrix(const D3DXMATRIX& kMat)
{
	printf_s("Matrix: \n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n",
		kMat._11, kMat._12, kMat._13, kMat._14, kMat._21, kMat._22, kMat._23, kMat._24,
		kMat._31, kMat._32, kMat._33, kMat._34, kMat._41, kMat._42, kMat._43, kMat._44);
}
//---------------------------------------------------------------------------
bool FvMathAlgorithmsTest::TestQuaternion()
{
	printf_s("----------Quaternion Test Start----------\n");

	D3DXQUATERNION kQuat0(1.0f, 4.0f, 1.5f, 2.0f);
	D3DXQUATERNION kQuat1(3.5f, 7.2f, 6.2f, 5.5f);

	D3DXQUATERNION kQuatDxOut, kQuatFvOut;

	printf_s("Quat0 and Quat1 is follow:\n");
	OutputQuaternion(kQuat0);
	OutputQuaternion(kQuat1);

	D3DXQuaternionNormalize(&kQuatDxOut, &kQuat0);
	FvNormalize4f(kQuatFvOut, kQuat0);
	printf_s("Dx normalize Quat0 is:\n");
	OutputQuaternion(kQuatDxOut);
	printf_s("Fv normalize Quat0 is:\n");
	OutputQuaternion(kQuatFvOut);

	float fDxOut, fFvOut;
	fDxOut = D3DXQuaternionDot(&kQuat0, &kQuat1);
	fFvOut = FvDot4f(kQuat0, kQuat1);
	printf_s("Dx Quat0 dot Quat1 is: %f\n", fDxOut);
	printf_s("Fv Quat0 dot Quat1 is: %f\n", fFvOut);

	D3DXMATRIX kMatRotate0, kMatRotate1;
	D3DXMatrixRotationYawPitchRoll(&kMatRotate0, 0.5f, 1.2f, 0.1f);
	printf_s("MatRotate0 is follow:\n");
	OutputMatrix(kMatRotate0);
	D3DXMatrixRotationYawPitchRoll(&kMatRotate1, 3.1f, 2.9f, 0.1f);
	printf_s("MatRotate1 is follow:\n");
	OutputMatrix(kMatRotate1);
	D3DXQuaternionRotationMatrix(&kQuatDxOut, &kMatRotate0);
	FvQuaternionRotationMatrixf(kQuatFvOut, kMatRotate0);
	printf_s("Dx quaternion rotation MatRotate0 is:\n");
	OutputQuaternion(kQuatDxOut);
	printf_s("Fv quaternion rotation MatRotate0 is:\n");
	OutputQuaternion(kQuatFvOut);
	D3DXQuaternionRotationMatrix(&kQuatDxOut, &kMatRotate1);
	FvQuaternionRotationMatrixf(kQuatFvOut, kMatRotate1);
	printf_s("Dx quaternion rotation MatRotate1 is:\n");
	OutputQuaternion(kQuatDxOut);
	printf_s("Fv quaternion rotation MatRotate1 is:\n");
	OutputQuaternion(kQuatFvOut);

	D3DXQuaternionSlerp(&kQuatDxOut, &kQuat0, &kQuat1, 0.3f);
	FvLerp4f(kQuatFvOut, kQuat0, kQuat1, 0.3f);
	printf_s("Dx slerp Quat0 and Quat1 is:\n");
	OutputQuaternion(kQuatDxOut);
	printf_s("Fv slerp Quat0 and Quat1 is:\n");
	OutputQuaternion(kQuatFvOut);

	D3DXVECTOR3 kVec3(0.5f, 0.7f, 0.6f);
	D3DXVec3Normalize(&kVec3, &kVec3);
	printf_s("Vec3 is follow:\n");
	OutputVector3(kVec3);
	D3DXQuaternionRotationAxis(&kQuatDxOut, &kVec3, 1.2f);
	FvQuaternionRotationAxisf(kQuatFvOut, kVec3, 1.2f);
	printf_s("Dx quaternion rotation axis Vec3 and 1.2 is:\n");
	OutputQuaternion(kQuatDxOut);
	printf_s("Fv quaternion rotation axis Vec3 and 1.2 is:\n");
	OutputQuaternion(kQuatFvOut);

	D3DXQuaternionMultiply(&kQuatDxOut, &kQuat0, &kQuat1);
	FvQuaternionMultiplyf(kQuatFvOut, kQuat0, kQuat1);
	printf_s("Dx Quat0 mul Quat1 is:\n");
	OutputQuaternion(kQuatDxOut);
	printf_s("Fv Quat0 mul Quat1 is:\n");
	OutputQuaternion(kQuatFvOut);

	D3DXQuaternionInverse(&kQuatDxOut, &kQuat0);
	FvQuaternionInversef(kQuatFvOut, kQuat0);
	printf_s("Dx inverse Quat0 is:\n");
	OutputQuaternion(kQuatDxOut);
	printf_s("Fv inverse Quat0 is:\n");
	OutputQuaternion(kQuatFvOut);

	printf_s("-----------Quaternion Test End-----------\n");

	return true;
}
//---------------------------------------------------------------------------
bool FvMathAlgorithmsTest::TestMatrix()
{
	printf_s("----------Matrix Test Start----------\n");

	D3DXMATRIX kMat4Src0(1.0f,2.0f,3.0f,0.5f,2.0f,3.0f,1.0f,0.4f,3.0f,1.0f,2.0f,0.2f,0.5f,0.6f,0.7f,1.5f);
	D3DXMATRIX kMat4Src1(1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f,9.0f,10.f,11.f,12.f,13.f,14.f,15.f,16.f);
	D3DXMATRIX kMat4DxOut, kMat4FvOut;
	
	printf_s("Mat4Src0 and Mat4Src1 is follow:\n");
	OutputMatrix(kMat4Src0);
	OutputMatrix(kMat4Src1);

	D3DXMatrixIdentity(&kMat4DxOut);
	FvMatrixIdenty4f(kMat4FvOut);
	printf_s("Dx Matrix identity:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix identity:\n");
	OutputMatrix(kMat4FvOut);

	float fDxDeterminant, fFvDeterminant;
	D3DXMatrixInverse(&kMat4DxOut, &fDxDeterminant, &kMat4Src0);
	FvMatrixInverse4f(kMat4FvOut, &fFvDeterminant, kMat4Src0);
	printf_s("Dx Matrix determinant: %f\n", fDxDeterminant);
	printf_s("Fv Matrix determinant: %f\n", fFvDeterminant);
	printf_s("Dx Matrix inverse:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix inverse:\n");
	OutputMatrix(kMat4FvOut);

	//Ogre::Matrix4 aa(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
	//Ogre::Matrix4 bb = aa.inverse();

	//OutputMatrix(*(D3DXMATRIX*)&bb);

	D3DXMatrixTranspose(&kMat4DxOut, &kMat4Src0);
	FvMatrixTranspose4f(kMat4FvOut, kMat4Src0);
	printf_s("Dx Matrix transpose Mat4Src0:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix transpose Mat4Src0:\n");
	OutputMatrix(kMat4FvOut);

	fDxDeterminant = D3DXMatrixDeterminant(&kMat4Src0);
	fFvDeterminant = FvMatrixDeterminant4f(kMat4Src0);
	printf_s("Dx Matrix determinant: %f\n", fDxDeterminant);
	printf_s("Fv Matrix determinant: %f\n", fFvDeterminant);

	D3DXMatrixMultiply(&kMat4DxOut, &kMat4Src0, &kMat4Src1);
	FvMatrixMultiply4f(kMat4FvOut, kMat4Src0, kMat4Src1);
	printf_s("Dx Matrix mul Mat4Src0 and Mat4Src1:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix mul Mat4Src0 and Mat4Src1:\n");
	OutputMatrix(kMat4FvOut);

	D3DXMatrixRotationX(&kMat4DxOut, 0.5f);
	FvMatrixRotationX4f(kMat4FvOut, 0.5f);
	printf_s("Dx Matrix rotation x 0.5:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix rotation x 0.5:\n");
	OutputMatrix(kMat4FvOut);

	D3DXMatrixRotationY(&kMat4DxOut, 0.5f);
	FvMatrixRotationY4f(kMat4FvOut, 0.5f);
	printf_s("Dx Matrix rotation y 0.5:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix rotation y 0.5:\n");
	OutputMatrix(kMat4FvOut);

	D3DXMatrixRotationZ(&kMat4DxOut, 0.5f);
	FvMatrixRotationZ4f(kMat4FvOut, 0.5f);
	printf_s("Dx Matrix rotation z 0.5:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix rotation z 0.5:\n");
	OutputMatrix(kMat4FvOut);

	D3DXMatrixScaling(&kMat4DxOut, 0.5f, 0.7f, 0.4f);
	FvMatrixScaling4f(kMat4FvOut, 0.5f, 0.7f, 0.4f);
	printf_s("Dx Matrix scaling 0.5 0.7 0.4:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix scaling 0.5 0.7 0.4:\n");
	OutputMatrix(kMat4FvOut);

	D3DXMatrixTranslation(&kMat4DxOut, 0.4f, 0.5f, 0.8f);
	FvMatrixTranslation4f(kMat4FvOut, 0.4f, 0.5f, 0.8f);
	printf_s("Dx Matrix transpose 0.4f 0.5f 0.8f:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix transpose 0.4f 0.5f 0.8f:\n");
	OutputMatrix(kMat4FvOut);

	D3DXMatrixRotationYawPitchRoll(&kMat4DxOut, 0.1f, 0.5f, 0.4f);
	FvMatrixRotationYawPitchRoll4f(kMat4FvOut, 0.1f, 0.5f, 0.4f);
	printf_s("Dx Matrix transpose 0.4f 0.5f 0.8f:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix transpose 0.4f 0.5f 0.8f:\n");
	OutputMatrix(kMat4FvOut);

	D3DXQUATERNION kQuat(0.5f, 0.7f, 0.4f, 0.2f);
	D3DXQuaternionNormalize(&kQuat, &kQuat);
	printf_s("Quat is follow:\n");
	OutputQuaternion(kQuat);
	D3DXMatrixRotationQuaternion(&kMat4DxOut, &kQuat);
	FvMatrixRotationQuaternion4f(kMat4FvOut, kQuat);
	printf_s("Dx Matrix rotation Quat is:\n");
	OutputMatrix(kMat4DxOut);
	printf_s("Fv Matrix rotation Quat is:\n");
	OutputMatrix(kMat4FvOut);

	D3DXQUATERNION kRotate(0.5f, 0.7f, 0.4f, 0.2f);
	D3DXQuaternionNormalize(&kRotate, &kRotate);
	D3DXVECTOR3 kScale(0.2f, 0.3f, 0.4f);
	D3DXVECTOR3 kTrans(15.0f, 17.2f, 22.3f);
	D3DXMATRIX kMatRotate, kMatTrans, kMatScale, kMatFinal;
	FvMatrixRotationQuaternion4f(kMatRotate, kRotate);
	FvMatrixScaling4f(kMatScale, kScale.x, kScale.y, kScale.z);
	FvMatrixTranslation4f(kMatTrans, kTrans.x, kTrans.y, kTrans.z);
	FvMatrixMultiply4f(kMatFinal, kMatScale, kMatRotate);
	FvMatrixMultiply4f(kMatFinal, kMatFinal, kMatTrans);
	D3DXQUATERNION kRotateOut;
	D3DXVECTOR3 kScaleOut;
	D3DXVECTOR3 kTransOut;
	FvMatrixDecomposeWorld4f(kRotateOut, kScaleOut, kTransOut, kMatFinal);

	printf_s("Source Rotate is follow:\n");
	OutputQuaternion(kRotate);
	printf_s("Source Scale is follow:\n");
	OutputVector3(kScale);
	printf_s("Source Translate is follow:\n");
	OutputVector3(kTrans);

	printf_s("Composed matrix is follow:\n");
	OutputMatrix(kMatFinal);

	printf_s("Decompose Rotate is follow:\n");
	OutputQuaternion(kRotateOut);
	printf_s("Decompose Scale is follow:\n");
	OutputVector3(kScaleOut);
	printf_s("Decompose Translate is follow:\n");
	OutputVector3(kTransOut);

	printf_s("-----------Matrix Test End-----------\n");
	return true;
}
//----------------------------------------------------------------------------
void FvMathAlgorithmsTest::SetTestOther(bool bTest)
{
	m_bTestOther = bTest;	
}
//----------------------------------------------------------------------------
bool FvMathAlgorithmsTest::TestOther()
{
	printf_s("----------Other Test Start----------\n");
	float fValue = FV_INTERVAL_MAPPING(0.3f, 0.0f, 1.0f, 1.0f, 0.0f);
	printf_s("0.3 in [0.0, 1.0] to [1.0, 0.0] is %f\n", fValue);
	
	fValue = FV_LERP(0.1f, 0.0f, 1.0f);
	printf_s("Lerp(0.1, 0.0, 1.0) is %f\n", fValue);

	fValue = FvFastSqrtf(9.0f);
	printf_s("Sqrt(9) with fast method is %f\n", fValue);

	printf_s("-----------Other Test End-----------\n");
	return true;
}
//----------------------------------------------------------------------------
void FvMathAlgorithmsTest::SetTestVector(bool bTest)
{
	m_bTestVector = bTest;
}
//----------------------------------------------------------------------------
bool FvMathAlgorithmsTest::TestVector()
{
	printf_s("----------Vector Test Start----------\n");

	printf_s("Vector2 test begin\n");

	D3DXVECTOR2 kVec2Src0(1.0f, 0.5f), kVec2Src1(0.4f, 1.5f);
	D3DXVECTOR2 kVec2DxOut, kVec2FvOut;

	printf_s("Vec2Src0 and Vec2Src1 is follow:\n");
	OutputVector2(kVec2Src0);
	OutputVector2(kVec2Src1);

	float fDxLength2 = D3DXVec2Length(&kVec2Src0);
	float fFvLength2 = FvLength2f(kVec2Src0);
	printf_s("Length of Vec2Src0 with Dx is %f\n", fDxLength2);
	printf_s("Length of Vec2Src0 with Fv is %f\n", fFvLength2);

	float fDxLengthSqr2 = D3DXVec2LengthSq(&kVec2Src0);
	float fFvLengthSqr2 = FvLengthSqr2f(kVec2Src0);
	printf_s("LengthSqr of Vec2Src0 with Dx is %f\n", fDxLengthSqr2);
	printf_s("LengthSqr of Vec2Src0 with Fv is %f\n", fFvLengthSqr2);

	D3DXVec2Normalize(&kVec2DxOut, &kVec2Src0);
	FvNormalize2f(kVec2FvOut, kVec2Src0);
	printf_s("Dx normalize Vec2Src0:\t");
	OutputVector2(kVec2DxOut);
	printf_s("Fv normalize Vec2Src0:\t");
	OutputVector2(kVec2FvOut);

	float fDxDot2 = D3DXVec2Dot(&kVec2Src0, &kVec2Src1);
	float fFvDot2 = FvDot2f(kVec2Src0, kVec2Src1);
	printf_s("Vec2Src0 dot Vec2Src1 with Dx is %f\n", fDxDot2);
	printf_s("Vec2Src0 dot Vec2Src1 with Fv is %f\n", fFvDot2);

	D3DXVec2Lerp(&kVec2DxOut, &kVec2Src0, &kVec2Src1, 0.4f);
	FvLerp2f(kVec2FvOut, kVec2Src0, kVec2Src1, 0.4f);
	printf_s("Dx lerp Vec2Src0 and Vec2Src1 with 0.4:\t");
	OutputVector2(kVec2DxOut);
	printf_s("Fv lerp Vec2Src0 and Vec2Src1 with 0.4:\t");
	OutputVector2(kVec2FvOut);

	printf_s("Vector2 test end\n");

	printf_s("Vector3 test begin\n");

	D3DXVECTOR3 kVec3Src0(1.0f, 0.5f, 7.5f), kVec3Src1(0.4f, 1.5f, -5.1f);
	D3DXVECTOR3 kVec3DxOut, kVec3FvOut;

	printf_s("Vec3Src0 and Vec3Src1 is follow:\n");
	OutputVector3(kVec3Src0);
	OutputVector3(kVec3Src1);

	float fDxLength3 = D3DXVec3Length(&kVec3Src0);
	float fFvLength3 = FvLength3f(kVec3Src0);
	printf_s("Length of Vec3Src0 with Dx is %f\n", fDxLength3);
	printf_s("Length of Vec3Src0 with Fv is %f\n", fFvLength3);

	float fDxLengthSqr3 = D3DXVec3LengthSq(&kVec3Src0);
	float fFvLengthSqr3 = FvLengthSqr3f(kVec3Src0);
	printf_s("LengthSqr of Vec3Src0 with Dx is %f\n", fDxLengthSqr3);
	printf_s("LengthSqr of Vec3Src0 with Fv is %f\n", fFvLengthSqr3);

	D3DXVec3Normalize(&kVec3DxOut, &kVec3Src0);
	FvNormalize3f(kVec3FvOut, kVec3Src0);
	printf_s("Dx normalize Vec3Src0:\t");
	OutputVector3(kVec3DxOut);
	printf_s("Fv normalize Vec3Src0:\t");
	OutputVector3(kVec3FvOut);

	float fDxDot3 = D3DXVec3Dot(&kVec3Src0, &kVec3Src1);
	float fFvDot3 = FvDot3f(kVec3Src0, kVec3Src1);
	printf_s("Vec3Src0 dot Vec3Src1 with Dx is %f\n", fDxDot3);
	printf_s("Vec3Src0 dot Vec3Src1 with Fv is %f\n", fFvDot3);

	D3DXVec3Lerp(&kVec3DxOut, &kVec3Src0, &kVec3Src1, 0.4f);
	FvLerp3f(kVec3FvOut, kVec3Src0, kVec3Src1, 0.4f);
	printf_s("Dx lerp Vec3Src0 and Vec3Src1 with 0.4:\t");
	OutputVector3(kVec3DxOut);
	printf_s("Fv lerp Vec3Src0 and Vec3Src1 with 0.4:\t");
	OutputVector3(kVec3FvOut);

	D3DXVec3Cross(&kVec3DxOut, &kVec3Src0, &kVec3Src1);
	FvVector3Crossf(kVec3FvOut, kVec3Src0, kVec3Src1);
	printf_s("Dx cross Vec3Src0 and Vec3Src1:\t");
	OutputVector3(kVec3DxOut);
	printf_s("Fv cross Vec3Src0 and Vec3Src1:\t");
	OutputVector3(kVec3FvOut);

	D3DXMATRIX kMat(1.0f,2.0f,3.0f,0.5f,2.0f,3.0f,1.0f,0.4f,3.0f,1.0f,2.0f,0.2f,0.5f,0.6f,0.7f,1.5f);
	D3DXVECTOR4 kVec4DxOut, kVec4FvOut;

	printf_s("Mat is:\n");
	OutputMatrix(kMat);

	D3DXVec3Transform(&kVec4DxOut, &kVec3Src0, &kMat);
	FvVector3TransformM4f(kVec4FvOut, kVec3Src0, kMat);
	printf_s("Dx transform Vec3Src0 with Mat:\t");
	OutputVector4(kVec4DxOut);
	printf_s("Fv transform Vec3Src0 with Mat:\t");
	OutputVector4(kVec4FvOut);

	D3DXVec3TransformCoord(&kVec3DxOut, &kVec3Src0, &kMat);
	FvVector3TransformCoordM4f(kVec3FvOut, kVec3Src0, kMat);
	printf_s("Dx TransformCoord Vec3Src0 with Mat:\t");
	OutputVector3(kVec3DxOut);
	printf_s("Fv TransformCoord Vec3Src0 with Mat:\t");
	OutputVector3(kVec3FvOut);

	D3DXVec3TransformNormal(&kVec3DxOut, &kVec3Src0, &kMat);
	FvVector3TransformNormalM4f(kVec3FvOut, kVec3Src0, kMat);
	printf_s("Dx TransformNormal Vec3Src0 with Mat:\t");
	OutputVector3(kVec3DxOut);
	printf_s("Fv TransformNormal Vec3Src0 with Mat:\t");
	OutputVector3(kVec3FvOut);

	printf_s("Vector3 test end\n");

	printf_s("Vector4 test begin\n");

	D3DXVECTOR4 kVec4Src0(1.0f, 0.5f, 7.5f, 0.3f), kVec4Src1(0.4f, 1.5f, -5.1f, 0.2f);

	printf_s("Vec4Src0 and Vec4Src1 is follow:\n");
	OutputVector4(kVec4Src0);
	OutputVector4(kVec4Src1);

	float fDxLength4 = D3DXVec4Length(&kVec4Src0);
	float fFvLength4 = FvLength4f(kVec4Src0);
	printf_s("Length of Vec4Src0 with Dx is %f\n", fDxLength4);
	printf_s("Length of Vec4Src0 with Fv is %f\n", fFvLength4);

	float fDxLengthSqr4 = D3DXVec4LengthSq(&kVec4Src0);
	float fFvLengthSqr4 = FvLengthSqr4f(kVec4Src0);
	printf_s("LengthSqr of Vec4Src0 with Dx is %f\n", fDxLengthSqr4);
	printf_s("LengthSqr of Vec4Src0 with Fv is %f\n", fFvLengthSqr4);

	D3DXVec4Normalize(&kVec4DxOut, &kVec4Src0);
	FvNormalize4f(kVec4FvOut, kVec4Src0);
	printf_s("Dx normalize Vec4Src0:\t");
	OutputVector4(kVec4DxOut);
	printf_s("Fv normalize Vec4Src0:\t");
	OutputVector4(kVec4FvOut);

	float fDxDot4 = D3DXVec4Dot(&kVec4Src0, &kVec4Src1);
	float fFvDot4 = FvDot4f(kVec4Src0, kVec4Src1);
	printf_s("Vec4Src0 dot Vec4Src1 with Dx is %f\n", fDxDot4);
	printf_s("Vec4Src0 dot Vec4Src1 with Fv is %f\n", fFvDot4);

	D3DXVec4Lerp(&kVec4DxOut, &kVec4Src0, &kVec4Src1, 0.4f);
	FvLerp4f(kVec4FvOut, kVec4Src0, kVec4Src1, 0.4f);
	printf_s("Dx lerp Vec4Src0 and Vec4Src1 with 0.4:\t");
	OutputVector4(kVec4DxOut);
	printf_s("Fv lerp Vec4Src0 and Vec4Src1 with 0.4:\t");
	OutputVector4(kVec4FvOut);

	D3DXVec4Transform(&kVec4DxOut, &kVec4Src0, &kMat);
	FvVector4Transformf(kVec4FvOut, kVec4Src0, kMat);
	printf_s("Dx Transform Vec4Src0 with Mat:\t");
	OutputVector4(kVec4DxOut);
	printf_s("Fv Transform Vec4Src0 with Mat:\t");
	OutputVector4(kVec4FvOut);

	printf_s("Vector4 test end\n");

	printf_s("-----------Vector Test End-----------\n");
	return true;
}
//----------------------------------------------------------------------------
void FvMathAlgorithmsTest::SetTestMatrix( bool bTest )
{
	m_bTestMatrix = bTest;
}
//----------------------------------------------------------------------------
