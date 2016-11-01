
template<class T>
FV_INLINE void ControllerSerializer(FvController* pkHost, FvBinaryOStream& kOStream, std::vector<T>& kValue)
{
	kOStream << (FvUInt32)kValue.size();
	for (std::vector<T>::iterator kIt = kValue.begin(); kIt != kValue.end(); ++kIt)
		ControllerSerializer(pkHost, kOStream, (*kIt));
}

template<class T>
FV_INLINE void ControllerUnserializer(FvController* pkHost, FvBinaryIStream& kIStream, std::vector<T>& kValue)
{
	kValue.clear();
	FvUInt32 uiSize = 0;
	kIStream >> uiSize;
	kValue.reserve(uiSize);
	T kItem;
	for(FvUInt32 i = 0; i < uiSize; i++)
	{
		ControllerUnserializer(pkHost, kIStream, kItem);
		kValue.push_back(kItem);
	}
}

template<>
FV_INLINE void ControllerSerializer<std::string>(FvController* pkHost, FvBinaryOStream& kOStream, std::string& kValue)
{
	kOStream << kValue;
}

template<>
FV_INLINE void ControllerUnserializer<std::string>(FvController* pkHost, FvBinaryIStream& kIStream, std::string& kValue)
{
	kIStream >> kValue;
}

template<>
FV_INLINE void ControllerSerializer<int>(FvController* pkHost, FvBinaryOStream& kOStream, int& kValue)
{
	kOStream << kValue;
}

template<>
FV_INLINE void ControllerUnserializer<int>(FvController* pkHost, FvBinaryIStream& kIStream, int& kValue)
{
	kIStream >> kValue;
}

template<class T>
FV_INLINE void EntityMemberSerializer(FvEntity* pkHost, FvBinaryOStream& kOStream, T*& pkValue)
{
	if(pkValue)
	{
		kOStream << pkValue->GetID();
		pkValue = NULL;
	}
	else
	{
		kOStream << FvController::INVALID_ID;
	}
}

template<class T>
FV_INLINE void EntityMemberUnserializer(FvEntity* pkHost, FvBinaryIStream& kOStream, T*& pkValue)
{
	FvUInt16 uiID;
	kOStream >> uiID;
	if(uiID != FvController::INVALID_ID)
	{
		pkValue = static_cast<T*>(pkHost->GetController(uiID));
	}
	else
	{
		pkValue = NULL;
	}
}
