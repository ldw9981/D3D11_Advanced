#include "pch.h"
#include "Animation.h"

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Create(aiNodeAnim* nodeAnimation)
{
	assert(nodeAnimation != nullptr);
	assert(nodeAnimation->mNumPositionKeys == nodeAnimation->mNumRotationKeys);
	assert(nodeAnimation->mNumRotationKeys == nodeAnimation->mNumScalingKeys);
	
	m_NodeName = nodeAnimation->mNodeName.C_Str();
	size_t numkeys = nodeAnimation->mNumPositionKeys;	
	m_AnimationKeys.resize(numkeys);
	
	for (size_t i = 0;i < numkeys;i++)
	{		
		aiVectorKey& pos = nodeAnimation->mPositionKeys[i];
		aiQuatKey& rot = nodeAnimation->mRotationKeys[i];
		aiVectorKey& scl = nodeAnimation->mScalingKeys[i];

		assert(pos.mTime == rot.mTime);
		assert(rot.mTime == scl.mTime);

		m_AnimationKeys[i].Time = pos.mTime;
		m_AnimationKeys[i].Position = Vector3(pos.mValue.x,pos.mValue.y,pos.mValue.z);
		m_AnimationKeys[i].Rotation = Quaternion(rot.mValue.x, rot.mValue.y, rot.mValue.z, rot.mValue.w);
		m_AnimationKeys[i].Scaling = Vector3(scl.mValue.x,scl.mValue.y,scl.mValue.z);
	}
}
