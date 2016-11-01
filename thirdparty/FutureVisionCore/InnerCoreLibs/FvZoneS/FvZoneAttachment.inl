
FV_INLINE
const FvVector3 &FvZoneAttachment::GetPosition() const 
{ 
	return m_spAttachment->GetPosition();
}

FV_INLINE
const FvQuaternion &FvZoneAttachment::GetQuaternion() const 
{ 
	return m_spAttachment->GetQuaternion(); 
}

FV_INLINE
const FvVector3 &FvZoneAttachment::GetScale() const 
{ 
	return m_spAttachment->GetScale(); 
}

FV_INLINE
void FvZoneAttachment::SetQuaternion( const FvQuaternion &kQuaternion )
{ 
	m_spAttachment->SetQuaternion(kQuaternion); 
}

FV_INLINE
void FvZoneAttachment::SetScale( const FvVector3 &kScale )
{ 
	m_spAttachment->SetScale(kScale); 
}

FV_INLINE
FvAttachment *FvZoneAttachment::GetAttachment() const	
{ 
	return (FvAttachment*)m_spAttachment.Get(); 
}
