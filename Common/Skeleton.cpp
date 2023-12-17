#include "pch.h"
#include "Skeleton.h"
#include "Node.h"



void Skeleton::ReadFromAssimp(const aiScene* pScene)
{
	int NumNode = 0;	
	CountNode(NumNode, pScene->mRootNode);
	Bones.reserve(NumNode);	// 본이 매번 재할당되지않도록 공간만 확보. 추가할때마다 인덱스가 결정되므로 크기는 결정하지 않는다.
	CreateBone(pScene, pScene->mRootNode);

	//본을 다 구성한 후에 OffsetMatrix설정
	for (UINT i = 0; i < pScene->mNumMeshes; i++)
	{
		aiMesh* pMesh = pScene->mMeshes[i];
		if (!pMesh->HasBones())
			continue;

		for (UINT iBone = 0; iBone < pMesh->mNumBones; iBone++)
		{
			aiBone* pAiBone = pMesh->mBones[iBone];
			Bone* pBone = FindBone(pAiBone->mName.C_Str());
			pBone->OffsetMatrix = Math::Matrix(&pAiBone->mOffsetMatrix.a1).Transpose();
		}
	}
}

Bone* Skeleton::CreateBone(const aiScene* pScene,const aiNode* pNode)
{ 	
	Bone& bone = Bones.emplace_back();
	bone.Set(pNode);
	
	int BoneIndex = (int)(Bones.size() - 1);		
	BoneMappingTable[bone.Name] = BoneIndex;
	UINT numMesh = pNode->mNumMeshes;
	if (numMesh > 0)
	{
		bone.MeshNames.resize(numMesh);
		for (UINT i = 0; i < numMesh; ++i)
		{			
			UINT meshIndex = pNode->mMeshes[i];
			aiMesh* pMesh = pScene->mMeshes[meshIndex];
			std::string meshName = pMesh->mName.C_Str();
			bone.MeshNames[i] = meshName;				
			// 메쉬가 어느본에 속해있는지 빨리찾기위해
			MeshMappingTable[meshName] = BoneIndex;		
		}
	}

	UINT numChild = pNode->mNumChildren;	
	for (UINT i = 0; i < numChild; ++i)
	{
		Bone* child = CreateBone(pScene,pNode->mChildren[i]);
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

void Skeleton::CountNode(int& Count,const aiNode* pNode)
{
	Count++;
	std::string name = pNode->mName.C_Str();
	BoneMappingTable[name] = Count;

	UINT numChild = pNode->mNumChildren;
	for (UINT i = 0; i < numChild; ++i)
	{
		CountNode(Count,pNode->mChildren[i]);
	}
}
