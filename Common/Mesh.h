#pragma once


// 정점 선언.
struct Vertex
{
	Math::Vector3 Position;		// 정점 위치 정보.	
	Math::Vector2 TexCoord;
	Math::Vector3 Normal;
	Math::Vector3 Tangent;
};

struct BoneWeightVertex
{
	Math::Vector3 Position;		// 정점 위치 정보.	
	Math::Vector2 TexCoord;
	Math::Vector3 Normal;
	Math::Vector3 Tangent;
	// 영향받는 본수는 최대4개로 제한한다.
	int BlendIndeces[4] = {};		// 참조하는 본의 인덱스의 범위는 최대128개로 일단처리하자
	float BlendWeights[4] = {};	// 가중치 총합은 1이 되어야한다.

	void AddBoneData(int boneIndex, float weight)
	{
		// 적어도 하나는 데이터가 비어있어야 한다.
		assert(BlendWeights[0] == 0.0f || BlendWeights[1] == 0.0f ||
		BlendWeights[2] == 0.0f || BlendWeights[3] == 0.0f);
		for (int i = 0; i < 4; i++)
		{
			if (BlendWeights[i] == 0.0f)
			{
				BlendIndeces[i] = boneIndex;
				BlendWeights[i] = weight;
				return;
			}
		}
	}
};

struct BoneReference
{
	std::string NodeName;		 
	int BoneIndex = -1;			 // 본 인덱스
	Math::Matrix* NodeWorldMatrixPtr;
};

struct CB_MatrixPalette
{
	Math::Matrix Array[128];
};

struct aiMesh;
struct Skeleton;
class Node;
class Mesh
{
public:
	Mesh();
	~Mesh();
public:	
	std::vector<Vertex>				m_Vertices;
	std::vector<BoneWeightVertex>	m_BoneWeightVertices;
	std::vector<WORD>				m_Indices;

	std::vector<BoneReference>		m_BoneReferences;

	Math::Matrix* m_pNodeWorldTransform = nullptr;		// StaticMesh의 월드행렬을 가진 노드의 포인터
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	UINT m_VertexCount=0;
	UINT m_VertexBufferStride = 0;						// 버텍스 하나의 크기.
	UINT m_VertexBufferOffset = 0;						// 버텍스 버퍼의 오프셋.
	UINT m_IndexCount = 0;				// 인덱스 개수.
	UINT m_MaterialIndex = 0;			// 메테리얼 인덱스.
	std::string m_Name;					// 메쉬 이름.

	void CreateVertexBuffer(ID3D11Device* device, Vertex* vertices, UINT vertexCount);
	void CreateBoneWeightVertexBuffer(ID3D11Device* device, BoneWeightVertex* vertices, UINT vertexCount);

	void CreateIndexBuffer(ID3D11Device* device, WORD* indies, UINT indexCount);
	void Create(ID3D11Device* device,aiMesh* mesh, Skeleton* skeleton);
	// 계층 구조 노드가 소유한 World의 포인터를 설정
	void SetNodeWorldPtr(Math::Matrix* world) { m_pNodeWorldTransform = world; }
	void UpdateNodeInstancePtr(Node* pRootNode, Skeleton* skeleton);
	void UpdateMatrixPallete(CB_MatrixPalette* pMatrixPallete, Skeleton* skeleton);
	bool IsSkeletalMesh() { return !m_BoneReferences.empty(); }		
	void Render(ID3D11DeviceContext* deviceContext);
};

class StaticMesh
{
public:
	StaticMesh();
	~StaticMesh();

	std::vector<Vertex>				m_Vertices;
	std::vector<WORD>				m_Indices;	

	Math::Matrix* m_pNodeWorldTransform = nullptr;		// StaticMesh의 월드행렬을 가진 노드의 포인터
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	UINT m_VertexCount = 0;
	UINT m_VertexBufferStride = 0;						// 버텍스 하나의 크기.
	UINT m_VertexBufferOffset = 0;						// 버텍스 버퍼의 오프셋.
	UINT m_IndexCount = 0;				// 인덱스 개수.
	UINT m_MaterialIndex = 0;			// 메테리얼 인덱스.
	std::string m_Name;					// 메쉬 이름.

	template<class T>
	void CreateVertexBuffer(ID3D11Device* device, T* vertices, UINT vertexCount);
	void CreateIndexBuffer(ID3D11Device* device, WORD* indies, UINT indexCount);
	virtual void Create(ID3D11Device* device, aiMesh* mesh, Skeleton* skeleton);
	// 계층 구조 노드가 소유한 World의 포인터를 설정
	void SetNodeWorldPtr(Math::Matrix* world) { m_pNodeWorldTransform = world; }
	virtual void UpdateNodeInstancePtr(Node* pRootNode, Skeleton* skeleton);
	void Render(ID3D11DeviceContext* deviceContext);
};

class SkinnedMesh:public StaticMesh
{
public:
	SkinnedMesh();
	~SkinnedMesh();
public:
	std::vector<BoneWeightVertex>	m_BoneWeightVertices;
	std::vector<BoneReference>		m_BoneReferences;		
	
	virtual void Create(ID3D11Device* device, aiMesh* mesh, Skeleton* skeleton);
	void UpdateMatrixPallete(CB_MatrixPalette* pMatrixPallete, Skeleton* skeleton);	
	virtual void UpdateNodeInstancePtr(Node* pRootNode, Skeleton* skeleton);
};