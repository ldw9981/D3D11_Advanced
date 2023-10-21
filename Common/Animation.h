#pragma once

using namespace std;
using namespace DirectX::SimpleMath;

struct aiNodeAnim;

struct AnimationKey
{
	float Time = 0.0f;		// 키의 시간
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scaling;
};

struct NodeAnimation
{	
	string NodeName;
	vector<AnimationKey> AnimationKeys;
	void Create(aiNodeAnim* nodeAnimation);
	void Evaluate(float time, Vector3& position, Quaternion& rotation, Vector3& scaling);
};

struct ClipAnimation
{
	float Duration = 10.0f;	// 전체 길이
	vector<NodeAnimation> NodeAnimations;
};
