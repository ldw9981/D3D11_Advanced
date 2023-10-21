#include "pch.h"
#include "Animation.h"
#include "Helper.h"

void NodeAnimation::Create(aiNodeAnim* nodeAnimation)
{
	assert(nodeAnimation != nullptr);
	assert(nodeAnimation->mNumPositionKeys == nodeAnimation->mNumRotationKeys);
	assert(nodeAnimation->mNumRotationKeys == nodeAnimation->mNumScalingKeys);
	
	NodeName = nodeAnimation->mNodeName.C_Str();
	LOG_MESSAGEA(NodeName.c_str());
	size_t numkeys = nodeAnimation->mNumPositionKeys;	
	AnimationKeys.resize(numkeys);
	
	for (size_t i = 0;i < numkeys;i++)
	{	
		aiVectorKey& pos = nodeAnimation->mPositionKeys[i];
		aiQuatKey& rot = nodeAnimation->mRotationKeys[i];
		aiVectorKey& scl = nodeAnimation->mScalingKeys[i];

		assert(pos.mTime == rot.mTime);
		assert(rot.mTime == scl.mTime);

		AnimationKeys[i].Time = pos.mTime;
		AnimationKeys[i].Position = Vector3(pos.mValue.x,pos.mValue.y,pos.mValue.z);
		AnimationKeys[i].Rotation = Quaternion(rot.mValue.x, rot.mValue.y, rot.mValue.z, rot.mValue.w);
		AnimationKeys[i].Scaling = Vector3(scl.mValue.x,scl.mValue.y,scl.mValue.z);
	}
}
