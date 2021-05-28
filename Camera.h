#pragma once

#include "DXUTCamera.h"

class CCamera : public CModelViewerCamera// CFirstPersonCamera
{
public:
	CCamera(void){}
	~CCamera(void){}

public:
	D3DXVECTOR4 m_vLightPosInCameraView;
	D3DXMATRIX m_mCameraViewToLightViewProj;
	D3DXMATRIX m_mCameraViewToLightView;
	LPDIRECT3DTEXTURE9	m_pShadowMap;
	LPDIRECT3DTEXTURE9	m_pUpdateShadowMap;
	D3DXVECTOR4 color;
	D3DXVECTOR3 posInTex;
	D3DXVECTOR4 normal;
	bool	visible;
};

