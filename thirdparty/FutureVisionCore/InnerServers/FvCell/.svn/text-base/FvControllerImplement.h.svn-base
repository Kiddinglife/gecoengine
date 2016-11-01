void FvEntity::Serializer(FvBinaryOStream& kOStream)
{
	kOStream << FvUInt16(m_kControllers.size());
	for(size_t i = 0; i < m_kControllers.size(); i++)
	{
		FvController* pkController = m_kControllers[i];
		if(pkController == NULL)
		{
			m_kFreeIDs.push(i);
			continue;
		}

		FV_ASSERT((pkController->GetParent() == NULL)?pkController->m_bIsNew:true);
		FvController* pkParent = pkController->GetParent();

		kOStream << pkController->GetID();
		kOStream << pkController->GetType();

		if(pkParent == NULL)
		{
			kOStream << FvController::INVALID_ID;
		}
		else
		{
			kOStream << pkParent->GetID();
		}

		if(!pkController->m_bIsNew)
		{
			FV_ASSERT(pkController->GetMemoryOffset() > 0);
			kOStream << FvUInt32(pkController->GetMemoryOffset());
		}
		else
		{
			kOStream << FvUInt32(0);
		}
	}

	std::vector<FvController*> kFreeList;
	for(size_t i = 0; i < m_kControllers.size(); i++)
	{
		FvController* pkController = m_kControllers[i];
		if(pkController == NULL) continue;
		FvController* pkParent = pkController->GetParent();

		if(pkParent == NULL)
			pkController->Serializer(kOStream);

		if(pkController->m_bIsNew && pkParent == NULL)
			kFreeList.push_back(pkController);
	}

	MemberSerializer(kOStream);

	for(size_t i = 0; i < kFreeList.size(); i++)
		delete kFreeList[i];

	m_kControllers.clear();
	while(!m_kFreeIDs.empty())
		m_kFreeIDs.pop();
}

void FvEntity::Unserializer(FvBinaryIStream& kIStream)
{
	FvUInt16 uiSize;
	kIStream >> uiSize;
	m_kControllers.resize(uiSize);

	struct Remap
	{
		Remap(FvUInt16 uiID,
			FvUInt16 uiParentID,
			FvUInt32 uiMemoryOffset) :
		m_uiID(uiID),
			m_uiParentID(uiParentID),
			m_uiMemoryOffset(uiMemoryOffset){}

		FvUInt16 m_uiID;
		FvUInt16 m_uiParentID;
		FvUInt32 m_uiMemoryOffset;
	};
	std::vector<Remap> kRemapMemorys;

	for(size_t i = 0; i < m_kControllers.size(); i++)
	{
		FvUInt16 uiID,uiType,uiParentID;
		FvUInt32 uiMemoryOffset;
		kIStream >> uiID;
		kIStream >> uiType;
		kIStream >> uiParentID;
		kIStream >> uiMemoryOffset;
		if(uiMemoryOffset == 0)
		{
			FvController* pkController = FvControllerFactory::Create(uiType);
			pkController->SetID(uiID);
			pkController->SetEntity(this);
			if(uiParentID != FvController::INVALID_ID)
			{
				FV_ASSERT(m_kControllers[uiParentID]);
				pkController->SetParent(m_kControllers[uiParentID]);
			}
			m_kControllers[i] = pkController;
		}
		else
		{
			FV_ASSERT(uiMemoryOffset > 0);
			if(uiParentID == FvController::INVALID_ID)
			{
				FvController* pkController = (FvController*)(FvUInt32(this) + uiMemoryOffset);
				pkController->SetID(uiID);
				pkController->SetEntity(this);
				pkController->SetMemoryOffset(uiMemoryOffset);
				m_kControllers[i] = pkController;
			}
			else
			{
				kRemapMemorys.push_back(Remap(uiID,uiParentID,uiMemoryOffset));
			}
		}
	}

	for(size_t i = 0; i < kRemapMemorys.size(); i++)
	{
		Remap& kRemap = kRemapMemorys[i];
		FvController*& pkController = m_kControllers[kRemap.m_uiID];
		FV_ASSERT(m_kControllers[kRemap.m_uiParentID]);
		pkController = (FvController*)(FvUInt32(m_kControllers[kRemap.m_uiParentID]) + kRemap.m_uiMemoryOffset);
		pkController->SetID(kRemap.m_uiID);
		pkController->SetEntity(this);
		pkController->SetParent(m_kControllers[kRemap.m_uiParentID]);
		pkController->SetMemoryOffset(kRemap.m_uiMemoryOffset);
	}

	for(size_t i = 0; i < m_kControllers.size(); i++)
	{
		FvController* pkController = m_kControllers[i];
		if(pkController == NULL)
		{
			m_kFreeIDs.push(i);
			continue;
		}
		FV_ASSERT((pkController->GetParent() == NULL)?pkController->m_bIsNew:true);
		FvController* pkParent = pkController->GetParent();

		if(pkParent == NULL)
			pkController->Unserializer(kIStream);
	}

	MemberUnserializer(kIStream);
}

FvController* FvEntity::GetController(FvUInt16 uiID)
{
	FV_ASSERT(uiID <= m_kControllers.size());
	return m_kControllers[uiID];
}

void FvEntity::ExcuteController(FvUInt16 uiID,FvUInt16 uiMethod,
								FvBinaryIStream& kIStream)
{
	FvController* pkContorller = GetController(uiID);
	FV_ASSERT(pkContorller);
	pkContorller->Excute(uiMethod,kIStream);
}