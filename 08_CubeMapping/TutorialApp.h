#pragma once
#include "../Common/GameApp.h"
#include "../Common/Mesh.h"
#include "../Common/Material.h"
#include "../Common/Model.h"
#include "../Common/Texture.h"	

struct CB_Transform
{
	Math::Matrix mWorld;
	Math::Matrix mView;
	Math::Matrix mProjection;
}; 

struct CB_DirectionLight
{
	Math::Vector3 Direction = { 0.5f, 0.0f, 0.8f };
	float pad0;
	Math::Vector4 Ambient = { 0.1f,0.1f,0.1f,0.1f };
	Math::Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Math::Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
	Math::Vector3 EyePosition;
	float pad1;
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
	ID3D11RasterizerState* m_CCWcullMode = nullptr;
	ID3D11RasterizerState* m_CWcullMode = nullptr;

	ID3D11VertexShader* m_pVertexShader = nullptr;		// 정점 셰이더.
	ID3D11PixelShader* m_pPixelShader = nullptr;		// 픽셀 셰이더.

	ID3D11VertexShader* m_pVS_SkyBox = nullptr;		// 정점 셰이더.
	ID3D11PixelShader* m_pPS_SkyBox = nullptr;		// 픽셀 셰이더.

	ID3D11InputLayout* m_pInputLayout = nullptr;		// 입력 레이아웃.
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 샘플러 상태.
	ID3D11SamplerState* m_pSamplerCube = nullptr;		// 큐브맵 샘플러

	ID3D11BlendState* m_pAlphaBlendState = nullptr;		// 블렌드 상태 변경 (반투명처리를위한 블렌드상태)
	ID3D11Buffer* m_pGpuCbMaterial = nullptr;				// 상수 버퍼: 변환행렬
	ID3D11Buffer* m_pCBTransform = nullptr;				// 상수 버퍼: 변환행렬
	ID3D11Buffer* m_pCBDirectionLight = nullptr;		// 상수 버퍼: 방향광


	// 렌더링 파이프라인에 적용하는 정보
	UINT m_VertexBufferStride = 0;						// 버텍스 하나의 크기.
	UINT m_VertexBufferOffset = 0;						// 버텍스 버퍼의 오프셋.
	int m_nIndices = 0;				// 인덱스 개수.
	
	Math::Matrix  m_View;					// 뷰좌표계 공간으로 변환을 위한 행렬.
	Math::Matrix  m_Projection;			// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

	Math::Vector3 m_ClearColor ={ 0.0f, 0.0f, 0.0f};
	Math::Vector2 m_Rotation = Math::Vector2(0.0f, 0.0f);

	Math::Vector3 m_CameraRot = { 0.0f, 0.0f, 0.0f }; // degree
	Math::Vector3 m_CameraPos = Math::Vector3(0.0f, 200.0f, -1000.0f);
	

	CB_Transform m_Transform;
	CB_Marterial m_CpuCbMaterial;
	CB_DirectionLight m_Light;
	float m_MeshScale=1.0f;
	
	
	Model m_Cube;
	Model m_Sphere;

	Texture m_CubeMapTexture;
	ComPtr<ID3D11Texture3D> m_pTexture3D;
	ComPtr<ID3D11ShaderResourceView> m_pTexture3DSRV;

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	bool InitD3D();			
	void UninitD3D();

	bool InitScene();		// 쉐이더,버텍스,인덱스
	void UninitScene();		 

	bool InitImGUI();
	void UninitImGUI();

	void RenderModel(Model& model);
	void ApplyMaterial(const Material* material);

	void CreateStaticMesh_VS_IL();
	void CreateStaticMesh_PS();
	void CreateSkyBox();

	Texture CreateCubeMapTexture(UINT width, UINT height, DXGI_FORMAT format, UINT levels);

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

