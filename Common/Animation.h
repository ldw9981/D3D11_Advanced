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
	string NodeName;		// 애니메이션이 적용될 계층 구조에서의 노드 이름
	vector<AnimationKey> AnimationKeys;
	// aiNodeAnim으로 AnimationKey를 생성
	void Create(aiNodeAnim* nodeAnimation,double tickPerSecond);
	// 시간에 따른 보간된 Key값을 계산하는 함수
	void Evaluate(float time, Vector3& position, Quaternion& rotation, Vector3& scaling);
};

// 모든 노드의 각 시간에서의 Transform
struct Animation
{
	float Duration = 0.0f;	// 전체 길이
	vector<NodeAnimation> NodeAnimations;
};
