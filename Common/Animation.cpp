#include "pch.h"
#include "Animation.h"
#include "Helper.h"

void NodeAnimation::Create(aiNodeAnim* nodeAnimation,double tickPerSecond)
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

		AnimationKeys[i].Time = (float)(pos.mTime / tickPerSecond);
		AnimationKeys[i].Position = { pos.mValue.x,pos.mValue.y,pos.mValue.z };
		AnimationKeys[i].Rotation = { rot.mValue.x,rot.mValue.y,rot.mValue.z, rot.mValue.w };
		AnimationKeys[i].Scaling =  { scl.mValue.x,scl.mValue.y,scl.mValue.z };
	}
}

void NodeAnimation::Evaluate(float time, Vector3& position, Quaternion& rotation, Vector3& scaling)
{
	assert(AnimationKeys.size() > 0);
	size_t i = 0;
	for (i=0;i< AnimationKeys.size();i++)
	{
		if (AnimationKeys[i].Time > time)
		{
			break;	
		}
	}		

	if (i == 0)
	{
		position = AnimationKeys[i].Position;
		rotation = AnimationKeys[i].Rotation;
		scaling = AnimationKeys[i].Scaling;
	}
	else
	{
		float t = (time - AnimationKeys[i - 1].Time) / (AnimationKeys[i].Time - AnimationKeys[i - 1].Time);
		position = Vector3::Lerp(AnimationKeys[i - 1].Position, AnimationKeys[i].Position, t);
		rotation = Quaternion::Slerp(AnimationKeys[i - 1].Rotation, AnimationKeys[i].Rotation, t);
		scaling = Vector3::Lerp(AnimationKeys[i - 1].Scaling, AnimationKeys[i].Scaling, t);
	}
}


