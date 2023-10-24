#pragma once

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace std;

// 정점 선언.
struct Vertex
{
	Vector3 Position;		// 정점 위치 정보.	
	Vector2 TexCoord;
	Vector3 Normal;
	Vector3 Tangent;
};

struct aiMesh;
class Mesh
{
public:
	Mesh();
	~Mesh();
public:	
	void CreateVertexBuffer(ID3D11Device* device, Vertex* vertices, UINT vertexCount);
	void CreateIndexBuffer(ID3D11Device* device,WORD* indies, UINT indexCount);
	void Create(ID3D11Device* device, aiMesh* mesh);
	// 계층 구조 노드가 소유한 World의 포인터를 설정
	void SetNodeWorldPtr(Matrix* world) { m_pNodeWorld = world; }

	std::vector<Vertex> m_Vertices;
	std::vector<WORD> m_Indices;
	Matrix* m_pNodeWorld = nullptr;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	UINT m_VertexCount=0;
	UINT m_VertexBufferStride = 0;						// 버텍스 하나의 크기.
	UINT m_VertexBufferOffset = 0;						// 버텍스 버퍼의 오프셋.
	UINT m_IndexCount = 0;				// 인덱스 개수.
	UINT m_MaterialIndex = 0;			// 메테리얼 인덱스.
};

