#include "pch.h"
#include "Skeleton.h"
#include "Node.h"



Bone* Skeleton::Create(const aiScene* pScene,const aiNode* pNode)
{ 	
	Bones.push_back(Bone(pNode));
	int BoneIndex = (int)(Bones.size() - 1);	
	Bone& bone = Bones[BoneIndex];
	BoneMappingTable[bone.Name] = BoneIndex;

	UINT numMesh = pNode->mNumMeshes;
	if (numMesh > 0)
	{
		bone.Meshes.resize(numMesh);
		for (UINT i = 0; i < numMesh; ++i)
		{
			UINT meshIndex = pNode->mMeshes[i];
			std::string meshName = pScene->mMeshes[meshIndex]->mName.C_Str();
			bone.Meshes[i] = meshName;
			MeshMappingTable[meshName] = BoneIndex;
		}
	}

	UINT numChild = pNode->mNumChildren;	
	for (UINT i = 0; i < numChild; ++i)
	{
		Bone* child = Create(pScene,pNode->mChildren[i]);
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

int Skeleton::GetBoneIndexByBoneName(const std::string& boneName)
{
	auto iter = BoneMappingTable.find(boneName);
	if (iter == BoneMappingTable.end())
		return -1;
	return iter->second;
}

int Skeleton::GetBoneIndexByMeshName(const std::string& meshName)
{
	auto iter = MeshMappingTable.find(meshName);
	if (iter == MeshMappingTable.end())
		return -1;
	return iter->second;
}