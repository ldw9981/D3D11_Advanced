#pragma once
#include <windows.h>
#include "../Common/GameApp.h"
#include <d3d11.h>
#include <directxtk/SimpleMath.h>


using namespace DirectX::SimpleMath;
using namespace DirectX;


// 정점 선언.
struct Vertex
{
	Vector3 Position;		// 정점 위치 정보.	
	Vector2 TexCoord;
	Vector3 Normal;
	Vector3 Tangent;
};

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
	Vector4 Ambient=	{ 0.1f,0.1f,0.1f,0.1f};
	Vector4 Diffuse =	{ 1.0f,1.0f,1.0f,1.0f };
	Vector4 Specular =	{ 1.0f,1.0f,1.0f,1.0f };
	Vector3 EyePosition;
	bool UseNormalMap=true;
};

static_assert((sizeof(CB_DirectionLight) % 16) == 0,
	"Constant Buffer size must be 16-byte aligned");

struct CB_Marterial
{
	Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
	float  SpecularPower=80;
	Vector3 dummy;
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
	ID3D11ShaderResourceView* m_pTextureRV = nullptr;	// 텍스처 리소스 뷰.
	ID3D11ShaderResourceView* m_pNormalRV = nullptr;	// 노말맵 리소스 뷰.
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 샘플러 상태.

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
	Vector3 m_CameraPos = Vector3(0.0f, 0.0f, -200.0f);

	CB_Transform m_Transform;
	CB_Marterial m_Material;
	CB_DirectionLight m_Light;
	float m_MeshScale=50.0f;

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

