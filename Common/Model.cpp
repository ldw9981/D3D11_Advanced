#include "pch.h"
#include "Model.h"
#include "Helper.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <assimp/cimport.h>


bool Model::ReadFile(ID3D11Device* device,const char* filePath)
{
	LOG_MESSAGEA("Loading file: %s", filePath);
	Assimp::Importer importer;
	
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS,0);	// $assimp_fbx$ 노드 생성안함
	
	
	unsigned int importFlags = aiProcess_Triangulate | // 삼각형으로 변환
		aiProcess_GenNormals |	// 노말 생성/
		aiProcess_GenUVCoords |		// UV 생성
		aiProcess_CalcTangentSpace |  // 탄젠트 생성
		aiProcess_LimitBoneWeights |	// 본의 영향을 받는 정점의 최대 개수를 4개로 제한
		aiProcess_ConvertToLeftHanded;	// 왼손 좌표계로 변환

	const aiScene* scene = importer.ReadFile(filePath, importFlags);
	if (!scene) {
		LOG_ERRORA("Error loading file: %s", importer.GetErrorString());
		return false;
	}

	aiMetadata* metadata = scene->mMetaData;

	std::string metadataStr;
	float metadataFloat;
	int metadataInt;
	aiString  key;
	if (metadata) {
		// Iterate through the metadata
		for (unsigned int i = 0; i < metadata->mNumProperties; i++) {
			const aiMetadataEntry& entry = metadata->mValues[i];
			key = metadata->mKeys[i];
			
			
			// Check the data type of the metadata entry
			if (entry.mType == aiMetadataType::AI_AISTRING) {
				metadataStr = std::string((char*)entry.mData);
			}
			else if (entry.mType == aiMetadataType::AI_INT32) {
				metadataInt = *reinterpret_cast<const int*>(entry.mData);
			}
			else if (entry.mType == aiMetadataType::AI_FLOAT) {
				metadataFloat  = *reinterpret_cast<const float*>(entry.mData);
			}			
		}
	}
	
	m_Skeleton.ReadFromAssimp(scene);
	


	m_Materials.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		m_Materials[i].Create(device, scene->mMaterials[i]);
	}

	m_Meshes.resize(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		m_Meshes[i].Create(device, scene->mMeshes[i],&m_Skeleton);
	}

	LoadSkeleton(&m_Skeleton);
	//LoadSkeleton(this, scene->mRootNode);

	assert(scene->mNumAnimations < 2); // 애니메이션은 없거나 1개여야한다. 
	// 노드의 애니메이션을 하나로 합치는 방법은 FBX export에서 NLA스트립,모든 액션 옵션을 끕니다.

	
	if (scene->HasAnimations())
	{
		const aiAnimation* animation = scene->mAnimations[0];
		// 채널수는 aiAnimation 안에서 애니메이션 정보를  표현하는 aiNode의 개수이다.
		assert(animation->mNumChannels > 1); // 애니메이션이 있다면 aiNode 는 하나 이상 있어야한다.

		m_Animations.resize(1);
		m_Animations[0].NodeAnimations.resize(animation->mNumChannels);
		// 전체 시간길이 = 프레임수 / 1초당 프레임수
		m_Animations[0].Duration = (float)(animation->mDuration / animation->mTicksPerSecond);
		for (size_t iChannel = 0; iChannel < animation->mNumChannels; iChannel++)
		{
			aiNodeAnim* pAiNodeAnim = animation->mChannels[iChannel];
			NodeAnimation& refNodeAnim = m_Animations[0].NodeAnimations[iChannel];
			refNodeAnim.Create(pAiNodeAnim, animation->mTicksPerSecond);
		}
		// 각 노드는 참조하는 노드애니메이션 ptr가 null이므로 0번 Index 애니메이션의 노드애니메이션을 연결한다.
		UpdateNodeAnimationReference(0);		
	}

	for (auto& mesh : m_Meshes)
	{
		mesh.UpdateNodeInstancePtr(this, &m_Skeleton);
	}


	importer.FreeScene();
	LOG_MESSAGEA("Complete file: %s", filePath);
	return true;
}

Material* Model::GetMaterial(UINT index)
{
	assert(index < m_Materials.size());
	return &m_Materials[index];
}
void Model::Update(float deltaTime)
{
	if (!m_Animations.empty())
	{
		m_AnimationProressTime += deltaTime;
		m_AnimationProressTime = fmod(m_AnimationProressTime, m_Animations[0].Duration);
		UpdateAnimation(m_AnimationProressTime);
	}	
}

void Model::UpdateNodeAnimationReference(UINT index)
{
	assert(index < m_Animations.size());
	Animation& animation = m_Animations[index];
	for (size_t i = 0; i < animation.NodeAnimations.size(); i++)
	{
		NodeAnimation& nodeAnimation = animation.NodeAnimations[i];
		Node* node = FindNode(nodeAnimation.NodeName);
		node->m_pNodeAnimation = &animation.NodeAnimations[i];
	}
}

void Model::SetWorldTransform(const Math::Matrix& transform)
{
	m_Local = transform;
}


