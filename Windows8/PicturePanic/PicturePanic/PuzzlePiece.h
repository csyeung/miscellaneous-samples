#pragma once
#include "pch.h"

#include "Utility/BasicLoader.h"
#include "Utility/BasicMath.h"
#include "Camera/PerspectiveCamera.h"
#include "Utility/BasicShapes.h"
#include "Sprites/SpriteRenderer.h"
#include "Effects/BumpyGlossyMetalEffect.h"
#include "Effects/PostCoronaEffect.h"
#include "Effects/PostHaloEffect.h"
#include "Resources/Texture2DResource.h"
#include "Resources/RenderTargetResource.h"


using namespace Microsoft::WRL;
using namespace Platform;
using namespace std;

using namespace GameFramework::Camera;
using namespace GameFramework::Sprites;
using namespace GameFramework::Utility;
using namespace GameFramework::Effects;


struct SpecularMapVertexType
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
	XMFLOAT2 texture;
};

struct DirectionalLight
{
	XMFLOAT4 color;
	XMFLOAT3 direction;
};

struct Material
{
	float ambient, diffuse, specular, shininess;
};

struct ShaderConstantBuffer
{
    XMMATRIX world;
    XMMATRIX view;
    XMMATRIX projection;
	DirectionalLight light;
	Material material;
	XMFLOAT4 ambientLight;
	XMFLOAT3 eyePosition;
};

struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

struct LightBufferType
{
	float4 diffuseColor;
	float4 specularColor;
	float specularPower;
	float3 lightDirection;
};

struct CameraBufferType
{
	XMFLOAT3 cameraPosition;
	float padding;
};

struct GlowSettingsType
{
	float4 GlowColor;
    float2 ViewportSize;
    float Glowness;
	float Bias;
	float Trailfade;
	float NoiseSpeed;		 
	float2 NoiseCrawl;	 
	float NoiseBright;
	float NoiseScale;
};

struct GlowMatrixAndTimeType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
	float Time;		 	
};

ref class PuzzlePiece;

enum STATE
{
	INTRO = 0,
	NORMAL = 1,
	SELECTED = 2,
	HIDE = 3,
	HIDE_DURING_GAME = 4,
	SHOW = 5,
	SHOW_DURING_GAME = 6,
	FLIPANDSWAP = 7,
};

ref class PuzzlePiece
{
	

internal:
	PuzzlePiece(float xScale, float yScale, float zScale);
	void InitializePiece(ID3D11Device1* pDevice);
	void InitializeCustomBackPiece(_In_ ID3D11Device1* pDevice);
	static void Initialize(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, D3D_FEATURE_LEVEL featureLevel);
	static void CreateResources(_In_ int width, _In_ int height);
	static void OnWindowSizeChange(ID3D11Device1 *pDevice, ID3D11DeviceContext1* pContext, float windowWidth, float windowHeight);
	static void LoadNewPuzzle(Platform::String^ modePath, int puzzleID);

	static void SetCustomFrontTexture(Texture2DResource^ texture) { _texturePuzzlePiece = texture; }
	static void SetCustomBackTexture(Texture2DResource^ texture) { _textureCustomColorBack = texture; }
	static void SetCustomSidesTexture(Texture2DResource^ texture) { _textureCustomColorSides = texture; }

	void Update(float timeTotal, float timeDelta, PerspectiveCamera* pCamera);
	void Render(ID3D11DeviceContext1* pContext, float timeTotal);
	void RenderSelected(ID3D11DeviceContext1* pContext);
	void RenderCorrect(_In_ ID3D11DeviceContext1* pContext, _In_ PostEffect^ postEffect);
	void RenderAsTransition(_In_ ID3D11DeviceContext1* pContext);

	void SetLocation(float x, float y, float z);
	void SetCorrectUVs(ID3D11DeviceContext1* pContext, float uMin, float vMin, float uMax, float vMax);
	void SetUVs(ID3D11DeviceContext1* pContext, float uMin, float vMin, float uMax, float vMax);
	void ResetUVs(ID3D11DeviceContext1* pContext);
	void FlipAndSwapWithPiece(ID3D11DeviceContext1* pContext, PuzzlePiece^ swapPiece);
	bool HitTest( _In_ FXMVECTOR vOrigin, _In_ FXMVECTOR vDirection);
	void RotateOnce();
	void Select();
	void UnSelect();
	inline bool IsSelected() { return _isSelected; }
	void Reset();

	void Hide(_In_ float delay, _In_ float timeToHide);
	void Show(_In_ float delay, _In_ float timeToShow);
	//void Scramble(float delay);
	void SwapWith(ID3D11DeviceContext1* pContext, PuzzlePiece^ swapPiece);

	bool IsCorrect();

	void Release();

	// inlines
	inline XMMATRIX GetWorldMatrix()							{ return _matWorld; }

	property bool IsAnimating
	{
		bool get()
		{
			return (_vecFlipData.size() != 0);
		}
	}
	
private:
	struct FlipValues
	{
		STATE state;
		float delay;
		float totalSpinTime;
		float currentSpinTime;
		float targetRotationInRadians;
		PuzzlePiece^ swapTargetPiece;
		ID3D11DeviceContext1* pContext;
		float lastRotation;
	};

	static void CreateBuffers(ID3D11Device1* pDevice);
	static void CreateFrontFaceData();
	static void CreateBackFace(ID3D11Device1* pDevice);
	static void CreateSideFaces(ID3D11Device1* pDevice);
	static void CreateConstantBuffers(ID3D11Device1* pDevice);
	static void CreateTextures();
	static void CreateRasterizerStates(ID3D11Device1* pDevice);

	static void SetupSideFace(SpecularMapVertexType* pFace, BasicVertex *pBasicFace, float3 normal, float3 tangent, float3 binormal);

	static void CreateVertexBuffer(_In_ ID3D11Device1* pDevice, _In_ unsigned int numVertices, _In_ SpecularMapVertexType *pVertexData, _Out_ ID3D11Buffer **ppVertexBuffer);
	static void CreateIndexBuffer(_In_ ID3D11Device1* pDevice, _In_ unsigned int numIndices, _In_ unsigned short *pIndexData, _Out_ ID3D11Buffer **ppIndexBuffer);
	
	void InitBoundingBoxBuffers();
	void UpdateWorldMatrix();
	void UpdateUVs(ID3D11DeviceContext1* pContext);
	void RenderPieceFront(ID3D11DeviceContext1 *pContext);
	void RenderPieceBack(ID3D11DeviceContext1 *pContext);
	void UpdateBackShaderConstants(ID3D11DeviceContext1* pContext);
	void RenderPieceSides(ID3D11DeviceContext1 *pContext);
	void RenderBoundingBox(ID3D11DeviceContext1* pContext);
	void FlipAndSwap(ID3D11DeviceContext1* pContext, float delay, PuzzlePiece^ swapPiece);
	void UpdateShaderConstants(PerspectiveCamera* pCamera, float timeTotal);

	inline float2 GetMinUV() { return float2(_currentUMin, _currentVMin); }
	inline float2 GetMaxUV() { return float2(_currentUMax, _currentVMax); }

	void ScaleTo(float scaleTarget);
	void HideDuringGame();
	void ShowDuringGame(float delay);


	static const float PIECE_WIDTH_NORMALIZED;
	static const float PIECE_HEIGHT_NORMALIZED;
	static const float PIECE_DEPTH_NORMALIZED;

	static const float REVEAL_SPIN_TIME;
	static const float SPIN_TIME; 
	static const float SCALE_TIME;
	static const float SELECTED_SCALEFACTOR;


	float3			_vLocation;
	float			_scaleCurrent;
	float			_scaleStart;
	float			_scaleTargetDelta;
	float			_scaleToTime;
	float3			_scale;

	XMMATRIX		_matScale;
	XMMATRIX		_matTranslation;
	XMMATRIX		_matRotation;

	XMMATRIX		_matWorld;

	bool			_isAnimating;
	bool			_isSelected;

	ShaderConstantBuffer _constantBufferData;
	MatrixBufferType	 _matrixBufferData;
	LightBufferType		 _lightBufferData;
	CameraBufferType	 _cameraBufferData;


	

	float _degreesLeftToRotate;

	

	vector<FlipValues*>		_vecFlipData;
	float			   _lastRotation;

	STATE	_state;

	float	_currentUMin;
	float	_currentUMax;
	float	_currentVMin;
	float	_currentVMax;

	float	_correctUMin;
	float	_correctUMax;
	float	_correctVMin;
	float	_correctVMax;

	bool _selectedFirstTime;

	SpecularMapVertexType _verticesFront[4];
	ID3D11Buffer*		  _pVertexBufferPieceFront; 

	SpecularMapVertexType _verticesCustomBack[4];
	ID3D11Buffer*		  _pVertexBufferPieceCustomBack;

	// one time data
	static Texture2DResource^           _texturePuzzlePiece;
	static ID3D11RasterizerState*		_pRasterizerStateWireframe;
	static ID3D11RasterizerState*       _pRasterizerStateBackfaceCull;

	static ID3D11Buffer*				_pConstantBufferPuzzlePiece;
	static ID3D11Buffer*				_pMatrixBuffer;
	static ID3D11Buffer*				_pLightBuffer;
	static ID3D11Buffer*				_pCameraBuffer;
	static ID3D11Buffer*				_pVertexBufferPieceBack;  
    static ID3D11Buffer*				_pVertexBufferPieceSides;  
    static ID3D11Buffer*				_pIndexBufferPieceFrontAndBack;
	static ID3D11Buffer*				_pIndexBufferPieceSides;
	static BasicVertex*					_pVertexDataFront;


	static ID3D11Buffer*				_pVertexBufferBoundingBox;

	static BoundingBox*					_pBoundingBox;
	static ID3D11PixelShader*			_pPixelShaderBoundingBox;

	static SpecularMapVertexType*		_frontVertexData;
	static SpecularMapVertexType*		_customBackVertexData;


	// selected puzzlepiece data
	static ID3D11DepthStencilView*		_pDepthStencilView;
	static ID3D11DepthStencilState*		_pDepthStencilState;

	static ID3D11Buffer*				_pConstantBufferGlowSettings;
	static ID3D11Buffer*				_pConstantBufferGlowMatrixAndTime;
	
	static ID3D11PixelShader*			_pPixelShaderGlow_Original;
	static ID3D11PixelShader*			_pPixelShaderGlow_Glow;
	static ID3D11PixelShader*			_pPixelShaderGlow_BlurHorizontal;
	static ID3D11PixelShader*			_pPixelShaderGlow_BlurVertical;
	static ID3D11VertexShader*			_pVertexShaderGlow_Quad;
	static ID3D11VertexShader*			_pVertexShaderGlow_QuadHorizontal;
	static ID3D11VertexShader*			_pVertexShaderGlow_QuadVertical;
	static ID3D11InputLayout*           _pInputLayoutGlow;
	static ID3D11InputLayout*           _pInputLayoutBlurHorizontal; 
	static ID3D11InputLayout*           _pInputLayoutBlurVertical; 

	static BumpyGlossyMetalEffect^		_effect;
	static PostCoronaEffect^			_effectSelected;
	
	static Texture2DResource^		_textureColorBack;
	static Texture2DResource^		_textureNormalMapBack;
	static Texture2DResource^		_textureGlossBack;
	static Texture2DResource^		_textureEnvironmentMap;

	static Texture2DResource^		_textureCustomColorBack;
	static Texture2DResource^		_textureCustomColorSides;

	
};