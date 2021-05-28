#pragma once

#include "SDKmisc.h"
#include "Camera.h"
#include "Model.h"
#include "PointLight.h"
#include "stdlib.h"
#include <time.h> 
#include <list>
#include <iterator>

using namespace std;
typedef list<CModel*>::iterator	Itor;
typedef list<CCamera*>::iterator lightItor;
typedef list<PLight*>::iterator pLightItor;

#define WINDOW_SIZE_W			800
#define WINDOW_SIZE_H			800
#define SHADOWMAP_RES			512
#define CAMERA_ROTATE_SCALER	0.01f
#define CAMERA_MOVE_SCALER		100.0f
#define M_PI					3.14159265358979323846

enum SHADOW_TYPE {STANDARD_SHADOW_MAP};

class CManager
{
private:
	// Standard shadow map 包访
	LPDIRECT3DTEXTURE9  m_pNormalMap;
	LPDIRECT3DTEXTURE9	m_pRemovable;
	LPDIRECT3DTEXTURE9	m_pSquareRemovable;
	LPDIRECT3DTEXTURE9	m_pShadowMap;
	LPDIRECT3DTEXTURE9	m_pDepthMap;
	LPDIRECT3DTEXTURE9	m_pColorMap;
	LPDIRECT3DSURFACE9	m_pShadowMapDepthStencil;
	LPDIRECT3DTEXTURE9	m_pColorMapRemovable;
	LPDIRECT3DTEXTURE9  m_pTestMap;

	IDirect3DSurface9*  m_TmpSurface; 
	IDirect3DSurface9*  m_UpdateSurface; 
	IDirect3DSurface9*	m_ShadowSurface;
	IDirect3DSurface9*  m_NormalSurface;
	IDirect3DSurface9*  m_ColorSurface;
	IDirect3DSurface9*  m_RColorSurface; 

	float				m_shadowBias;
	float				m_lightBias;
	float				x_move,y_move,z_move;
	int					count;
	bool				renderLight;
	bool				RSM;

	void GenerateShadowMap(void);
	void RenderWithShadowMap(void);

	// 扁鸥	
	LPDIRECT3DSURFACE9	m_pOldRenderTarget;
	LPDIRECT3DSURFACE9	m_pOldDepthStencil;
	LPDIRECT3DSURFACE9	m_pSurface;	

private:
	LPDIRECT3DDEVICE9	m_pD3DDevice;

	// Camera
	CFirstPersonCamera				m_Camera;
	D3DXMATRIX			m_mCameraView, m_mCameraProj;
	
	// Light
	CCamera				m_Light;
	D3DXMATRIX			m_mLightView, m_mLightProj;
	D3DXVECTOR4			m_vLightPosInCameraView;

	// Point Light
	list<PLight*>		m_pPointLightList;

	// Matrix
	D3DXMATRIX			m_mCameraViewToLightView;		// Camera View 开青纺 * Light View
	D3DXMATRIX			m_mCameraViewToLightViewProj;	// Camera View 开青纺 * Light View * Light Proj
	D3DXMATRIX			m_mLightViewInverse;
	D3DXMATRIX			m_mLightProjInverse;

	// Model
	list<CModel*>		m_pModelList;
	list<CModel*>		m_pMoveModelList;
	list<CModel*>		m_pLightModelList;
	CModel*				m_pPlane;
	CModel*				m_pBackground;

	// IndirectLighting
	list<CCamera*>		m_pIndirectLighting;

	// Effect
	LPD3DXEFFECT		m_pEffect;

	SHADOW_TYPE			m_ShadowType;

	bool				generatedShadowmap;
	int totalNumber, pointLightNumber;

private:
	void SetUp(void);	
	void LoadModelFromFile(void);
	HRESULT LoadEffectFromFile(void);
	void CreateTextures(void);
	void SetUpCameras(void);
	void Release(void);
	void RenderLight(void);
	void CaptureScene(void);
	void SplitNormalMap(void);
	D3DXVECTOR3 GetNormalVariance(D3DLOCKED_RECT d3drc, int xStart,int xEnd,int yStart,int yEnd);
	double GetDepthVariance(D3DLOCKED_RECT d3drc,int xStart,int xEnd,int yStart,int yEnd);
	void CheckVariance(D3DLOCKED_RECT d3drc, D3DLOCKED_RECT heightD3drc, int xStart,int xEnd,int yStart,int yEnd);
	void AddIndirectLight(D3DLOCKED_RECT d3drc, D3DLOCKED_RECT heightD3drc, int x, int y);
	D3DXVECTOR4 GetNormal(D3DLOCKED_RECT d3drc, int x, int y);
	void GenerateIndirectShadowMap(void);
	void GenerateColorMap(void);
	D3DXVECTOR4 GetColor(int x, int y);
	void ShadowUpdate(CCamera* light);
	void CheckVisibility(void);
	void ShadowMapWithRemovable(void);
	void SplitRemovableShadowMap(void);
	void Split(D3DLOCKED_RECT d3drc,  int xStart,int xEnd,int yStart,int yEnd);
	void AddPointLight(D3DLOCKED_RECT d3drc, int x, int y);
	void ColorMapWithRemovable(void);
	D3DXVECTOR4 GetRemovableColor(int x, int y);
	void GenerateNormalMap(void);
	void TestRendering(void);
	void ShadowMapWithSquareRemovable(void);
	void AddMovemodel(void);

public:
	CManager(LPDIRECT3DDEVICE9 pD3DDevice);
	~CManager(void);

	void FrameMove(float fElapsedTime);
	void FrameRender(void);
	void MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DrawText(CDXUTTextHelper* pTxtHelper);
	void OnKeyboard(UINT key);
	void SetShadowType(SHADOW_TYPE shadow) {}
};

