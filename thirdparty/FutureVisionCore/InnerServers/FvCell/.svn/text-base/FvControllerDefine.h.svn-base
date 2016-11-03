public:
	void Serializer(FvBinaryOStream& kOStream);
	void Unserializer(FvBinaryIStream& kIStream);
	FvController* GetController(FvUInt16 uiID);
	void ExcuteController(FvUInt16 uiID,FvUInt16 uiMethodID, FvBinaryIStream& kIStream);

	virtual void MemberSerializer(FvBinaryOStream& kOStream) {}
	virtual void MemberUnserializer(FvBinaryIStream& kIStream) {}

private:
	std::vector<FvController*> m_kControllers;
	std::queue<FvUInt16> m_kFreeIDs;
	friend class FvController;