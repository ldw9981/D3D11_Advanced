#pragma once

using namespace std;
using namespace DirectX::SimpleMath;

struct aiNodeAnim;

struct AnimationKey
{
	float Time;
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scaling;
};

class Animation
{
public:
	Animation();
	~Animation();
public:
	float m_TimeLength;
	string m_NodeName;
	vector<AnimationKey> m_AnimationKeys;
	void Create(aiNodeAnim* nodeAnimation);
};

