#include "FvXMLSection.h"
#include <stdio.h>
#include <vld.h>
#include <string>
#include <FvPowerDefines.h>

using namespace Ogre;

struct PartitionSource
{
	FvString m_kName;
	std::vector<FvString> m_kBoneNames;
};

void main(int iArgc, char *psArgv[])
{
	if(iArgc <= 1)
		return;

	std::string kName(psArgv[1]);

	FILE* pkFile = fopen(kName.c_str(),"rb");

	if(pkFile)
	{
		printf_s("Open file success\n");

		Ogre::DataStreamPtr spDataStream(OGRE_NEW Ogre::FileHandleDataStream(pkFile));
		FvXMLSectionPtr spAvatarSec = FvXMLSection::OpenSection(spDataStream);
		
		if(spAvatarSec)
		{
			spAvatarSec->Save(kName + ".bak");

			std::vector<FvXMLSectionPtr> kAvatarSkeletonSecs;
			spAvatarSec->OpenSections("AvatarSkeleton", kAvatarSkeletonSecs);

			for(FvUInt32 i(0); i < kAvatarSkeletonSecs.size(); ++i)
			{
				std::vector<FvXMLSectionPtr> kPartitionSecs;

				kAvatarSkeletonSecs[i]->OpenSections("Partition", kPartitionSecs);
				
				if(!kPartitionSecs.size()) continue;

				std::vector<PartitionSource> m_kPartitions(kPartitionSecs.size());
				
				for(FvUInt32 j(0); j < kPartitionSecs.size(); ++j)
				{
					m_kPartitions[j].m_kName = kPartitionSecs[j]->AsAttributeString("name");
					
					std::vector<FvXMLSectionPtr> kBoneSecs;

					kPartitionSecs[j]->OpenSections("Bone", kBoneSecs);

					for(FvUInt32 k(0); k < kBoneSecs.size(); ++k)
					{
						FvString kBone = kBoneSecs[k]->AsAttributeString("name");
						if(kBone == "") continue;

						m_kPartitions[j].m_kBoneNames.push_back(kBone);
					}
				}

				for(FvUInt32 j(0); j < kPartitionSecs.size(); ++j)
				{
					kAvatarSkeletonSecs[i]->DelChild(kPartitionSecs[j]);
				}

				FvUInt32 u32PartitionSourceNumber = m_kPartitions.size();
				FvUInt32 u32PartitionNumber = (1 << u32PartitionSourceNumber);

				for(FvUInt32 j(0); j < u32PartitionNumber; ++j)
				{
					std::vector<FvUInt32> m_kCache;

					for(FvUInt32 k(0); k < u32PartitionSourceNumber; ++k)
					{
						if(FV_MASK_HAS_ALL(j, FV_MASK(k)))
							m_kCache.push_back(k);
					}

					FvXMLSectionPtr spNewPartition =
						kAvatarSkeletonSecs[i]->NewSection("Partition");

					FvString kPartitionName("");

					for(FvUInt32 k(0); k < m_kCache.size(); ++k)
					{
						if(k > 0) kPartitionName += "_";
						kPartitionName += m_kPartitions[m_kCache[k]].m_kName;

						for(FvUInt32 l(0);
							l < m_kPartitions[m_kCache[k]].m_kBoneNames.size();
							++l)
						{
							FvXMLSectionPtr spNewBone = spNewPartition->NewSection("Bone");
							spNewBone->SetAttributeString("name",
								m_kPartitions[m_kCache[k]].m_kBoneNames[l]);
							spNewBone->SetString("1.0");
						}
					}

					if(kPartitionName == "") kPartitionName = "None";

					spNewPartition->SetAttributeString("name", kPartitionName);
					spNewPartition->SetAttributeString("default", "0.0");
				}
			}

			spAvatarSec->Save(kName + ".new");
		}
		else
		{
			printf_s("Error: wrong xml file\n");
		}

		spDataStream.setNull();
		fclose(pkFile);

	}
	else
	{
		printf_s("Error: can not open file\n");
	}
}