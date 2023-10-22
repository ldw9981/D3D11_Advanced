#pragma once

using namespace std;
using namespace DirectX::SimpleMath;

struct aiNodeAnim;

//Transform
struct AnimationKey
{
	float Time = 0.0f;		// Ű�� �ð�
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scaling;
};

// Ư�� ����� �ð������� Transform
struct NodeAnimation
{	
	string NodeName;		
	vector<AnimationKey> AnimationKeys;
	void Create(aiNodeAnim* nodeAnimation,double tickPerSecond);
	void Evaluate(float time, Vector3& position, Quaternion& rotation, Vector3& scaling);
};

// ��� ����� �� �ð������� Transform
struct ClipAnimation
{
	float Duration = 0.0f;	// ��ü ����
	vector<NodeAnimation> NodeAnimations;
};
