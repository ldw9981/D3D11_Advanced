#pragma once
#include "Helper.h"

struct Bone
{
	Math::Matrix RelativeTransform;			//부모로부터의 상대적인 변환
	Math::Matrix OffsetMatrix;				// 본에서의 메쉬의 상대적인 위치(변환)
	std::string Name;						// 본이름
	int NumChildren=0;						// 자식의 수
	int ParentBoneIndex=-1;
	std::vector<std::string> MeshNames;		// 본에 연결된 메쉬들의 이름
	Bone() {}
	void Set(const aiNode* pNode)
	{
		Name = std::string(pNode->mName.C_Str());
		RelativeTransform = Math::Matrix(&pNode->mTransformation.a1).Transpose();
		NumChildren = pNode->mNumChildren;
	}
	~Bone()
	{
		LOG_MESSAGEA("~Bone() %s", Name.c_str());
	}
};

// 본 정보를 저장하는 구조체
struct Skeleton
{
public:
	std::vector<Bone> Bones;
	std::map<std::string,int> BoneMappingTable;
	std::map<std::string,int> MeshMappingTable;

	void ReadFromAssimp(const aiScene* pScene);
	Bone* CreateBone(const aiScene* pScene,const aiNode* pNode);
	Bone* FindBone(const std::string& name);
	Bone* GetBone(int index);
	int GetBoneIndexByBoneName(const std::string& boneName);	
	int GetBoneIndexByMeshName(const std::string& meshName);
	int GetBoneCount()  {  return (int)Bones.size(); }
	const std::string& GetBoneName(int index) { return Bones[index].Name; }
	void CountNode(int& Count,const aiNode* pNode);
};

