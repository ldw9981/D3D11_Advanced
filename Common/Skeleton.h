#pragma once

struct Bone
{
	Math::Matrix RelativeTransform;
	Math::Matrix OffsetMatrix;
	std::string Name;
	int NumChildren=0;
	int ParentBoneIndex=-1;
	std::vector<std::string> Meshes;
	Bone(const aiNode* pNode)
	{
		Name = std::string(pNode->mName.C_Str());
		RelativeTransform = Math::Matrix(&pNode->mTransformation.a1).Transpose();
		NumChildren = pNode->mNumChildren;
	}
};
struct Skeleton
{
public:
	std::vector<Bone> Bones;
	std::map<std::string,int> BoneMappingTable;
	std::map<std::string,int> MeshMappingTable;

	Bone* Create(const aiScene* pScene,const aiNode* pNode);
	Bone* FindBone(const std::string& name);
	Bone* GetBone(int index);
	int GetBoneIndexByBoneName(const std::string& boneName);	
	int GetBoneIndexByMeshName(const std::string& meshName);
	int GetBoneCount()  {  return (int)Bones.size(); }
	const std::string& GetBoneName(int index) { return Bones[index].Name; }
};

