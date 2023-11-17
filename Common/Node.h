#pragma once

struct aiNode;
class Model;
struct NodeAnimation;
struct Skeleton;
struct Bone;
class Node
{
public:
	Node() {};
	~Node();

	std::string m_Name;	
	Math::Matrix m_Local;	
	Math::Matrix m_World;
	std::vector<Node> m_Children;		// 계층 구조를 이루는 자식 노드
	std::vector<UINT> m_MeshIndices;	// 해당 노드와 연결되는 메시 인덱스
	Node* m_pParent = nullptr;
	NodeAnimation* m_pNodeAnimation = nullptr; // 노드가 사용할 NodeAnimation가 있는 주소

	void LoadSkeleton(Model* model,aiNode* node);
	void LoadSkeleton(Skeleton* skeleton);

	void UpdateAnimation(float progressTime);
	Node* FindNode(const std::string& name);
};	

