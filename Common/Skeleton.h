#pragma once

struct Bone
{
	Math::Matrix RelativeTransform;
	Math::Matrix OffsetMatrix;
	std::string Name;
	int ParentBoneIndex=-1;
	Bone(const aiNode* pNode)
	{
		Name = std::string(pNode->mName.C_Str());
		RelativeTransform = Math::Matrix(&pNode->mTransformation.a1).Transpose();
	}
};
struct Skeleton
{
public:
	std::vector<Bone> Bones;
	std::map<std::string,int> BoneMappingTable;

	Bone* Create(const aiNode* pNode);
	Bone* FindBone(const std::string& name);
	Bone* GetBone(int index);
	int GetBoneIndex(const std::string& name);
};

