#include "pch.h"
#include "Skeleton.h"
#include "Node.h"

Bone* Skeleton::Create(const aiNode* pNode)
{ 	
	Bones.push_back(Bone(pNode));
	int BoneIndex = (int)(Bones.size() - 1);	
	Bone& bone = Bones[BoneIndex];
	BoneMappingTable[bone.Name] = BoneIndex;
	UINT numChild = pNode->mNumChildren;	
	for (UINT i = 0; i < numChild; ++i)
	{
		Bone* child = Create(pNode->mChildren[i]);
		child->ParentBoneIndex = BoneIndex;
	}
	return &Bones[BoneIndex];
}


Bone* Skeleton::FindBone(const std::string& name)
{
	auto iter = BoneMappingTable.find(name);
	if (iter == BoneMappingTable.end())
		return nullptr;
	return &Bones[iter->second];
}

Bone* Skeleton::GetBone(int index)
{
	if (index < 0 || index >= Bones.size())
		return nullptr;
	return &Bones[index];
}

int Skeleton::GetBoneIndex(const std::string& name)
{
	auto iter = BoneMappingTable.find(name);
	if (iter == BoneMappingTable.end())
		return -1;
	return iter->second;
}
