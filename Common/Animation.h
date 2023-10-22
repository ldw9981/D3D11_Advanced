#pragma once

using namespace std;
using namespace DirectX::SimpleMath;

struct aiNodeAnim;

//Transform
struct AnimationKey
{
	float Time = 0.0f;		// 키의 시간
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scaling;
};

// 특정 노드의 시간에서의 Transform
struct NodeAnimation
{	
	string NodeName;		
	vector<AnimationKey> AnimationKeys;
	void Create(aiNodeAnim* nodeAnimation,double tickPerSecond);
	void Evaluate(float time, Vector3& position, Quaternion& rotation, Vector3& scaling);
};

// 모든 노드의 각 시간에서의 Transform
struct ClipAnimation
{
	float Duration = 0.0f;	// 전체 길이
	vector<NodeAnimation> NodeAnimations;
};
