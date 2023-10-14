#pragma once
#include "../Common/GameApp.h"
#include "../Common/Mesh.h"
#include "../Common/Material.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct CB_Transform
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
}; 

struct CB_DirectionLight
{
	Vector3 Direction = { 0.5f, 0.0f, 0.8f };
	float pad0;
	Vector4 Ambient = { 0.1f,0.1f,0.1f,0.1f };
	Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 EyePosition;
	float pad1;
};



struct CB_Marterial
{
	Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };	// 16byte
	Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };	// 16byte
	Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };  // 16byte
	Vector4 Emissive = { 1.0f,1.0f,1.0f,1.0f };  // 16byte
	float  SpecularPower = 80;					// 4
	bool UseDiffuseMap = true;					
	bool pad1[3];								// 8
	bool UseNormalMap = true;
	bool pad2[3];								// 12
	bool UseSpecularMap = true;
	bool pad3[3];								// 16byte				
	bool UseEmissiveMap = true;			
	bool pad4[3];								// 4
	bool UseOpacityMap = true;
	bool pad5[3];								// 8
	Vector2 pad6;								// 16byte
};



class TutorialApp :
    public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	// 렌더링 파이프라인을 구성하는 필수 객체의 인터페이스
	ID3D11Device* m_pDevice = nullptr;						// 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;	// 깊이/스텐실 뷰

	// 렌더링 파이프라인에 적용하는 리소스 객체의 인터페이스
	ID3D11Buffer* m_pVertexBuffer = nullptr;			// 정점 버퍼.
	ID3D11Buffer* m_pIndexBuffer = nullptr;				// 인덱스 버퍼.
	ID3D11VertexShader* m_pVertexShader = nullptr;		// 정점 셰이더.
	ID3D11PixelShader* m_pPixelShader = nullptr;		// 픽셀 셰이더.
	ID3D11PixelShader* m_pPixelShaderSolid = nullptr;	// 픽셀 셰이더.
	ID3D11InputLayout* m_pInputLayout = nullptr;		// 입력 레이아웃.
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 샘플러 상태.
	ID3D11BlendState* m_AlphaBlendState = nullptr;		// 블렌드 상태 변경 (반투명처리를위한 블렌드상태)
	ID3D11Buffer* m_pCBMaterial = nullptr;				// 상수 버퍼: 변환행렬
	ID3D11Buffer* m_pCBTransform = nullptr;				// 상수 버퍼: 변환행렬
	ID3D11Buffer* m_pCBDirectionLight = nullptr;		// 상수 버퍼: 방향광

	// 렌더링 파이프라인에 적용하는 정보
	UINT m_VertexBufferStride = 0;						// 버텍스 하나의 크기.
	UINT m_VertexBufferOffset = 0;						// 버텍스 버퍼의 오프셋.
	int m_nIndices = 0;				// 인덱스 개수.
	Matrix	m_World;				// 월드좌표계 공간으로 변환을 위한 행렬.
	Matrix  m_View;					// 뷰좌표계 공간으로 변환을 위한 행렬.
	Matrix  m_Projection;			// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

	Vector3 m_ClearColor ={ 0.0f, 0.0f, 0.0f};
	Vector2 m_Rotation = Vector2(0.0f, 0.0f);	
	Vector3 m_CameraPos = Vector3(0.0f, 200.0f, -1000.0f);

	CB_Transform m_Transform;
	CB_Marterial m_CBMaterial;
	CB_DirectionLight m_Light;
	float m_MeshScale=1.0f;
	
	
	vector<Mesh> m_Meshes;	
	vector<Material> m_Materials;

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	bool InitD3D();			
	void UninitD3D();

	bool InitScene();		// 쉐이더,버텍스,인덱스
	void UninitScene();		 

	bool InitImGUI();
	void UninitImGUI();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

