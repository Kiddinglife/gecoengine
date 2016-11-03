
FV_INLINE FvInt32 FvVolatileInfo::DirType( float priority ) const
{
	return (priority > m_fYawPriority) +
		(priority > m_fPitchPriority) +
		(priority > m_fRollPriority);
}

FV_INLINE bool FvVolatileInfo::HasVolatile( float priority ) const
{
	return (priority < m_fPositionPriority) || (priority < m_fYawPriority);
}

FV_INLINE bool operator==( const FvVolatileInfo & info1, const FvVolatileInfo & info2 )
{
	return info1.m_fPositionPriority == info2.m_fPositionPriority &&
			info1.m_fYawPriority == info2.m_fYawPriority &&
			info1.m_fPitchPriority == info2.m_fPitchPriority &&
			info1.m_fRollPriority == info2.m_fRollPriority;
}

FV_INLINE bool operator!=( const FvVolatileInfo & info1, const FvVolatileInfo & info2 )
{
	return !(info1 == info2);
}

FV_INLINE void FvEntityDescription::PropertyEventStamps::Init(
		const FvEntityDescription & entityDescription )
{
	m_kEventStamps.resize( entityDescription.NumEventStampedProperties(), 1 );
}

FV_INLINE void FvEntityDescription::PropertyEventStamps::Init(
		const FvEntityDescription & entityDescription, FvEventNumber number )
{
	m_kEventStamps.resize( entityDescription.NumEventStampedProperties(), number );
}

FV_INLINE void FvEntityDescription::PropertyEventStamps::Set(
		const FvDataDescription & dataDescription, FvEventNumber eventNumber )
{
	const FvInt32 index = dataDescription.EventStampIndex();
	FV_IF_NOT_ASSERT_DEV( 0 <= index && index < (FvInt32)m_kEventStamps.size() )
	{
		FV_EXIT( "invalid event stamp index" );
	}

	m_kEventStamps[ index ] = eventNumber;
}

FV_INLINE FvEventNumber FvEntityDescription::PropertyEventStamps::Get(
		const FvDataDescription & dataDescription ) const
{
	const FvInt32 index = dataDescription.EventStampIndex();
	FV_IF_NOT_ASSERT_DEV( 0 <= index && index < (FvInt32)m_kEventStamps.size() )
	{
		FV_EXIT( "invalid event stamp index" );
	}

	return m_kEventStamps[ index ];
}

FV_INLINE FvEntityTypeID FvEntityDescription::Index() const
{
   	return m_uiIndex;
}


FV_INLINE void FvEntityDescription::Index( FvEntityTypeID index )
{
	m_uiIndex = index;
}

FV_INLINE FvEntityTypeID FvEntityDescription::ClientIndex() const
{
   	return m_uiClientIndex;
}

FV_INLINE void FvEntityDescription::ClientIndex( FvEntityTypeID index )
{
	m_uiClientIndex = index;
}

FV_INLINE const FvString& FvEntityDescription::ClientName() const
{
	return m_kClientName;
}

FV_INLINE const FvVolatileInfo & FvEntityDescription::VolatileInfo() const
{
	return m_kVolatileInfo;
}

FV_INLINE FvUInt32 FvEntityDescription::NumEventStampedProperties() const
{
	return m_uiNumEventStampedProperties;
}

FV_INLINE FvInt32 FvDataLoDLevels::Size() const
{
	return m_iSize;
}

FV_INLINE bool FvDataLoDLevels::NeedsMoreDetail( FvInt32 level, float priority ) const
{
	return priority < m_akLevel[ level ].Low();
}

FV_INLINE bool FvDataLoDLevels::NeedsLessDetail( FvInt32 level, float priority ) const
{
	return priority > m_akLevel[ level ].High();
}

