
FV_INLINE void FvDataDescription::AddToStream( FvObjPtr pNewValue,
			FvBinaryOStream & stream, bool isPersistentOnly ) const
{
	m_spDataType->AddToStream( pNewValue, stream, isPersistentOnly );
}

FV_INLINE void FvDataDescription::AddToSection( FvObjPtr pNewValue,
			FvXMLSectionPtr pSection ) const
{
	m_spDataType->AddToSection( pNewValue, pSection );
}

FV_INLINE FvObjPtr FvDataDescription::CreateFromStream( FvBinaryIStream & stream,
	bool isPersistentOnly ) const
{
	return m_spDataType->CreateFromStream( stream, isPersistentOnly );
}

FV_INLINE FvObjPtr FvDataDescription::CreateFromSection( FvXMLSectionPtr pSection ) const
{
	return m_spDataType->CreateFromSection( pSection );
}

FV_INLINE const FvString& FvDataDescription::Name() const
{
	return m_kName;
}

FV_INLINE FvObjPtr FvDataDescription::pInitialValue() const
{
	return m_spInitialValue;
}

FV_INLINE bool FvDataDescription::IsGhostedData() const
{
	return !!(m_uiDataFlags & FV_DATA_GHOSTED_OLD);
}

FV_INLINE bool FvDataDescription::IsOtherClientData() const
{
	return !!(m_uiDataFlags & FV_DATA_OTHER_CLIENT_OLD);
}

FV_INLINE bool FvDataDescription::IsOwnClientData() const
{
	return !!(m_uiDataFlags & FV_DATA_OWN_CLIENT_OLD);
}

FV_INLINE bool FvDataDescription::IsCellData() const
{
	return !this->IsBaseData();
}

FV_INLINE bool FvDataDescription::IsBaseData() const
{
	return !!(m_uiDataFlags & FV_DATA_BASE_OLD);
}

FV_INLINE bool FvDataDescription::IsClientOnlyData() const
{
	return !!(m_uiDataFlags & FV_DATA_CLIENT_ONLY_OLD);
}

FV_INLINE bool FvDataDescription::IsClientServerData() const
{
	return !!(m_uiDataFlags & (FV_DATA_OTHER_CLIENT_OLD | FV_DATA_OWN_CLIENT_OLD));
}

FV_INLINE bool FvDataDescription::IsPersistent() const
{
	return !!(m_uiDataFlags & FV_DATA_PERSISTENT_OLD);
}

FV_INLINE bool FvDataDescription::IsEditorOnly() const
{
	return !!(m_uiDataFlags & FV_DATA_EDITOR_ONLY_OLD);
}

FV_INLINE bool FvDataDescription::IsIdentifier() const
{
	return !!(m_uiDataFlags & FV_DATA_ID_OLD);
}

FV_INLINE bool FvDataDescription::IsOfType( FvEntityDataFlags flags )
{
	return (m_uiDataFlags & flags) == flags;
}

FV_INLINE FvInt32 FvDataDescription::Index() const
{
	return m_iIndex;
}

FV_INLINE void FvDataDescription::Index( FvInt32 index )
{
	m_iIndex = index;
}

FV_INLINE FvInt32 FvDataDescription::LocalIndex() const
{
	return m_iLocalIndex;
}

FV_INLINE void FvDataDescription::LocalIndex( FvInt32 i )
{
	m_iLocalIndex = i;
}

FV_INLINE FvInt32 FvDataDescription::EventStampIndex() const
{
	return m_iEventStampIndex;
}

FV_INLINE void FvDataDescription::EventStampIndex( FvInt32 index )
{
	m_iEventStampIndex = index;
}

FV_INLINE FvInt32 FvDataDescription::ClientServerFullIndex() const
{
	return m_iClientServerFullIndex;
}

FV_INLINE void FvDataDescription::ClientServerFullIndex( FvInt32 i )
{ 
	m_iClientServerFullIndex = i;
}

FV_INLINE FvInt32 FvDataDescription::DetailLevel() const
{
	return m_iDetailLevel;
}

FV_INLINE void FvDataDescription::DetailLevel( FvInt32 level )
{
	m_iDetailLevel = level;
}

FV_INLINE FvInt32 FvDataDescription::DatabaseLength() const
{
	return m_iDatabaseLength;
}

FV_INLINE FvDataType* FvDataDescription::DataType()
{
	return &*m_spDataType;
}

FV_INLINE const FvDataType*	FvDataDescription::DataType() const
{
	return &*m_spDataType;
}

#ifdef FV_EDITOR_ENABLED
FV_INLINE bool FvDataDescription::Editable() const
{
	return m_bEditable;
}

FV_INLINE void FvDataDescription::Editable( bool v )
{
	m_bEditable = v;
}
#endif	// FV_EDITOR_ENABLED

