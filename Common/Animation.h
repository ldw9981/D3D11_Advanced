#pragma once

using namespace std;
using namespace DirectX::SimpleMath;

struct aiNodeAnim;

struct AnimationKey
{
	float Time = 0.0f;
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scaling;
};

struct NodeAnimation
{
	float TimeLength = 0.0f;
	string NodeName;
	vector<AnimationKey> AnimationKeys;
	void Create(aiNodeAnim* nodeAnimation);
};

