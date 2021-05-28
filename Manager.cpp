#include "DXUT.h"
#include "tchar.h"
#include "strsafe.h"
#include "Manager.h"

//--------------------------------------------------------------------------------------
// 그림자맵을 생성한다
//--------------------------------------------------------------------------------------
void CManager::GenerateShadowMap(void)
{
	HRESULT hr;

	// Set RenderTarget
	V(m_pD3DDevice->GetRenderTarget(0, &m_pOldRenderTarget));

	if(SUCCEEDED(m_Light.m_pShadowMap->GetSurfaceLevel(0, &m_pSurface)))
	{
		m_pD3DDevice->SetRenderTarget(0, m_pSurface);
		SAFE_RELEASE(m_pSurface);
	}

	if(SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pOldDepthStencil)))
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pShadowMapDepthStencil);
	}

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

	// Render
	V(m_pEffect->SetTechnique("Tech_ShadowMap"));

	V(m_pEffect->Begin(NULL, 0));
	V(m_pEffect->BeginPass(0));

	for(Itor i = m_pModelList.begin(); i != m_pModelList.end(); i++)
	{
		D3DXMATRIX mWorldViewProj = (*i)->GetWolrdMtx() * m_mLightView * m_mLightProj;

		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));

		V(m_pEffect->CommitChanges());
		(*i)->Render();
	}
	
	V(m_pEffect->EndPass());
	V(m_pEffect->End());

	// Restoref
	if(m_pOldDepthStencil)
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pOldDepthStencil);
		SAFE_RELEASE(m_pOldDepthStencil);
	}
	m_pD3DDevice->SetRenderTarget(0, m_pOldRenderTarget);
	SAFE_RELEASE(m_pOldRenderTarget);

	IDirect3DSurface9 * pSurfaceLevel;

	GenerateColorMap();
	GenerateNormalMap();

	generatedShadowmap = true;

	SplitNormalMap();

	GenerateIndirectShadowMap();
}

void CManager::ShadowMapWithRemovable(void)
{
	HRESULT hr;

	// Set RenderTarget
	V(m_pD3DDevice->GetRenderTarget(0, &m_pOldRenderTarget));
	// 1 surface
	if(SUCCEEDED(m_pRemovable->GetSurfaceLevel(0, &m_pSurface)))
	{
		m_pD3DDevice->SetRenderTarget(0, m_pSurface);
		SAFE_RELEASE(m_pSurface);
	}

	if(SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pOldDepthStencil)))
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pShadowMapDepthStencil);
	}

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

	// Render
	V(m_pEffect->SetTechnique("Tech_ShadowMap"));

	V(m_pEffect->Begin(NULL, 0));
	V(m_pEffect->BeginPass(0));

	for(Itor i = m_pMoveModelList.begin(); i != m_pMoveModelList.end(); i++)
	{
		D3DXMATRIX mWorldViewProj = (*i)->GetWolrdMtx() * m_mLightView * m_mLightProj;

		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
		//V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

		V(m_pEffect->CommitChanges());
		(*i)->Render();
	}
	
	V(m_pEffect->EndPass());
	V(m_pEffect->End());

	// Restoref
	if(m_pOldDepthStencil)
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pOldDepthStencil);
		SAFE_RELEASE(m_pOldDepthStencil);
	}
	m_pD3DDevice->SetRenderTarget(0, m_pOldRenderTarget);
	SAFE_RELEASE(m_pOldRenderTarget);

	//D3DXSaveTextureToFile(L"MoveShadowMap.jpg", D3DXIFF_JPG, m_pRemovable, NULL);
}

void CManager::ShadowMapWithSquareRemovable(void)
{
	HRESULT hr;

	// Set RenderTarget
	V(m_pD3DDevice->GetRenderTarget(0, &m_pOldRenderTarget));
	// 1 surface
	if(SUCCEEDED(m_pSquareRemovable->GetSurfaceLevel(0, &m_pSurface)))
	{
		m_pD3DDevice->SetRenderTarget(0, m_pSurface);
		SAFE_RELEASE(m_pSurface);
	}

	if(SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pOldDepthStencil)))
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pShadowMapDepthStencil);
	}

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

	// Render
	V(m_pEffect->SetTechnique("Tech_Square"));

	V(m_pEffect->Begin(NULL, 0));
	V(m_pEffect->BeginPass(0));

	for(Itor i = m_pMoveModelList.begin(); i != m_pMoveModelList.end(); i++)
	{
		D3DXMATRIX mWorldViewProj = (*i)->GetWolrdMtx() * m_mLightView * m_mLightProj;

		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
		V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

		V(m_pEffect->CommitChanges());
		(*i)->Render();
	}
	
	V(m_pEffect->EndPass());
	V(m_pEffect->End());

	// Restoref
	if(m_pOldDepthStencil)
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pOldDepthStencil);
		SAFE_RELEASE(m_pOldDepthStencil);
	}
	m_pD3DDevice->SetRenderTarget(0, m_pOldRenderTarget);
	SAFE_RELEASE(m_pOldRenderTarget);

	//D3DXSaveTextureToFile(L"SquareMoveShadowMap.jpg", D3DXIFF_JPG, m_pSquareRemovable, NULL);
}

void CManager::ColorMapWithRemovable(void)
{
	HRESULT hr;

	// Set RenderTarget
	V(m_pD3DDevice->GetRenderTarget(0, &m_pOldRenderTarget));
	// 1 surface
	if(SUCCEEDED(m_pColorMapRemovable->GetSurfaceLevel(0, &m_pSurface)))
	{
		m_pD3DDevice->SetRenderTarget(0, m_pSurface);
		SAFE_RELEASE(m_pSurface);
	}

	if(SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pOldDepthStencil)))
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pShadowMapDepthStencil);
	}

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

	// Render
	V(m_pEffect->SetTechnique("Tech_Color"));

	V(m_pEffect->Begin(NULL, 0));
	V(m_pEffect->BeginPass(0));

	for(Itor i = m_pMoveModelList.begin(); i != m_pMoveModelList.end(); i++)
	{
		D3DXMATRIX mWorldViewProj = (*i)->GetWolrdMtx() * m_mLightView * m_mLightProj;

		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
		V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

		V(m_pEffect->CommitChanges());
		(*i)->Render();
	}
	
	V(m_pEffect->EndPass());
	V(m_pEffect->End());

	// Restoref
	if(m_pOldDepthStencil)
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pOldDepthStencil);
		SAFE_RELEASE(m_pOldDepthStencil);
	}
	m_pD3DDevice->SetRenderTarget(0, m_pOldRenderTarget);
	SAFE_RELEASE(m_pOldRenderTarget);

	//D3DXSaveTextureToFile(L"MoveShadowMap.jpg", D3DXIFF_JPG, m_pColorMapRemovable, NULL);
}

void CManager::TestRendering(void)
{
	HRESULT hr;

	// Set RenderTarget
	V(m_pD3DDevice->GetRenderTarget(0, &m_pOldRenderTarget));
	// 1 surface
	if(SUCCEEDED(m_pTestMap->GetSurfaceLevel(0, &m_pSurface)))
	{
		m_pD3DDevice->SetRenderTarget(0, m_pSurface);
		SAFE_RELEASE(m_pSurface);
	}

	if(SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pOldDepthStencil)))
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pShadowMapDepthStencil);
	}

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

	// Render
	V(m_pEffect->SetTechnique("Tech_Test"));
	D3DXMATRIX mLightView, mLightProj;
	D3DXMATRIX mWorldViewProj, mLightViewInverse, mLightProjInverse;

	mLightView = *(m_Light.GetViewMatrix());
	mLightProj = *(m_Light.GetProjMatrix());

	V(m_pEffect->Begin(NULL, 0));
	V(m_pEffect->BeginPass(0));

		D3DXMatrixInverse(&mLightViewInverse, NULL, &mLightView);
		D3DXMatrixInverse(&mLightViewInverse, NULL, &mLightView);
		mWorldViewProj = m_pBackground->GetWolrdMtx()*mLightProjInverse*mLightViewInverse;

		V(m_pEffect->SetTexture("tShadowMap", m_pRemovable));
		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
		V(m_pEffect->CommitChanges());
		m_pBackground->Render();
	
	V(m_pEffect->EndPass());
	V(m_pEffect->End());

	// Restoref
	if(m_pOldDepthStencil)
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pOldDepthStencil);
		SAFE_RELEASE(m_pOldDepthStencil);
	}
	m_pD3DDevice->SetRenderTarget(0, m_pOldRenderTarget);
	SAFE_RELEASE(m_pOldRenderTarget);

	D3DXSaveTextureToFile(L"Test.jpg", D3DXIFF_JPG, m_pTestMap, NULL);
}

//--------------------------------------------------------------------------------------
// Generate Color Map in Light
//--------------------------------------------------------------------------------------
void CManager::GenerateColorMap(void)
{
	HRESULT hr;

	// Set RenderTarget
	V(m_pD3DDevice->GetRenderTarget(0, &m_pOldRenderTarget));

	if(SUCCEEDED(m_pColorMap->GetSurfaceLevel(0, &m_pSurface)))
	{
		m_pD3DDevice->SetRenderTarget(0, m_pSurface);
		SAFE_RELEASE(m_pSurface);
	}

	if(SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pOldDepthStencil)))
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pShadowMapDepthStencil);
	}

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

	// Render
	V(m_pEffect->SetTechnique("Tech_Color"));

	V(m_pEffect->Begin(NULL, 0));
	V(m_pEffect->BeginPass(0));

	for(Itor i = m_pModelList.begin(); i != m_pModelList.end(); i++)
	{
		D3DXMATRIX mWorldViewProj = (*i)->GetWolrdMtx() * m_mLightView * m_mLightProj;

		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
		V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

		V(m_pEffect->CommitChanges());
		(*i)->Render();
	}
	
	V(m_pEffect->EndPass());
	V(m_pEffect->End());

	// Restoref
	if(m_pOldDepthStencil)
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pOldDepthStencil);
		SAFE_RELEASE(m_pOldDepthStencil);
	}
	m_pD3DDevice->SetRenderTarget(0, m_pOldRenderTarget);
	SAFE_RELEASE(m_pOldRenderTarget);
	//D3DXSaveTextureToFile(L"color.jpg", D3DXIFF_JPG, m_pColorMap, NULL);
}

void CManager::GenerateNormalMap(void)
{
	HRESULT hr;

	// Set RenderTarget
	V(m_pD3DDevice->GetRenderTarget(0, &m_pOldRenderTarget));

	if(SUCCEEDED(m_pNormalMap->GetSurfaceLevel(0, &m_pSurface)))
	{
		m_pD3DDevice->SetRenderTarget(0, m_pSurface);
		SAFE_RELEASE(m_pSurface);
	}

	if(SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pOldDepthStencil)))
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pShadowMapDepthStencil);
	}

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

	// Render
	V(m_pEffect->SetTechnique("Tech_Normal"));

	V(m_pEffect->Begin(NULL, 0));
	V(m_pEffect->BeginPass(0));

	for(Itor i = m_pModelList.begin(); i != m_pModelList.end(); i++)
	{
		D3DXMATRIX mWorldViewProj = (*i)->GetWolrdMtx() * m_mLightView * m_mLightProj;

		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
		V(m_pEffect->SetMatrix("mWorld", &(*i)->GetWolrdMtx()));
		V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

		V(m_pEffect->CommitChanges());
		(*i)->Render();
	}
	
	V(m_pEffect->EndPass());
	V(m_pEffect->End());

	// Restoref
	if(m_pOldDepthStencil)
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pOldDepthStencil);
		SAFE_RELEASE(m_pOldDepthStencil);
	}
	m_pD3DDevice->SetRenderTarget(0, m_pOldRenderTarget);
	SAFE_RELEASE(m_pOldRenderTarget);

	//D3DXSaveTextureToFile(L"Normal.jpg", D3DXIFF_JPG, m_pNormalMap, NULL);
}

//--------------------------------------------------------------------------------------
// Generate Indirect Light's ShadowMap
//--------------------------------------------------------------------------------------
void CManager::GenerateIndirectShadowMap(void)
{
	HRESULT hr;

	for(lightItor iL = m_pIndirectLighting.begin(); iL != m_pIndirectLighting.end();iL++)
	{
		HRESULT hr;
		D3DXMATRIX iLightView = *((*iL)->GetViewMatrix());
		D3DXMATRIX iLightProj = *((*iL)->GetProjMatrix());

		//m_mCameraView = *(m_Camera.GetViewMatrix());
		//m_mCameraProj = *(m_Camera.GetProjMatrix());

		//D3DXMATRIX mInverse;
		//D3DXMatrixInverse(&mInverse, NULL, &m_mCameraView);
		//m_mCameraViewToLightView = mInverse * iLightView;
		//(*iL)->m_mCameraViewToLightViewProj = mInverse * iLightView * iLightProj;

		//D3DXMatrixInverse(&m_mLightViewInverse, NULL, &iLightView);
		//D3DXMatrixInverse(&m_mLightProjInverse, NULL, &iLightProj);
	
		//D3DXVec3Transform(&(*iL)->m_vLightPosInCameraView, (*iL)->GetEyePt(), &m_mCameraView);

		m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 1, D3DUSAGE_RENDERTARGET, 
						D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &(*iL)->m_pShadowMap, NULL);

		m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 1, D3DUSAGE_RENDERTARGET, 
					D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &(*iL)->m_pUpdateShadowMap, NULL);

		// Set RenderTarget
		V(m_pD3DDevice->GetRenderTarget(0, &m_pOldRenderTarget));
		if(SUCCEEDED((*iL)->m_pShadowMap->GetSurfaceLevel(0, &m_pSurface)))
		{
			m_pD3DDevice->SetRenderTarget(0, m_pSurface);
			SAFE_RELEASE(m_pSurface);
		}

		if(SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pOldDepthStencil)))
		{
			m_pD3DDevice->SetDepthStencilSurface(m_pShadowMapDepthStencil);
		}

		m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

		// Render
		V(m_pEffect->SetTechnique("Tech_ShadowMap"));

		V(m_pEffect->Begin(NULL, 0));
		V(m_pEffect->BeginPass(0));

		for(Itor i = m_pModelList.begin(); i != m_pModelList.end(); i++)
		{
			D3DXMATRIX mWorldViewProj = (*i)->GetWolrdMtx() * iLightView * iLightProj;

			V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));

			V(m_pEffect->CommitChanges());
			(*i)->Render();
		}
	
		V(m_pEffect->EndPass());
		V(m_pEffect->End());

		// Restoref
		if(m_pOldDepthStencil)
		{
			m_pD3DDevice->SetDepthStencilSurface(m_pOldDepthStencil);
			SAFE_RELEASE(m_pOldDepthStencil);
		}
		m_pD3DDevice->SetRenderTarget(0, m_pOldRenderTarget);
		SAFE_RELEASE(m_pOldRenderTarget);

		//TCHAR portName[100];
		//_stprintf_s(portName, sizeof(portName)/sizeof(TCHAR), _T("shadowMap\\IndirectShadowMap%d .jpg"), count);
		//D3DXSaveTextureToFile(portName, D3DXIFF_JPG, (*iL)->m_pShadowMap, NULL);
		//count ++;
	}
}

void CManager::ShadowUpdate(CCamera* light)
{
	HRESULT hr;

	// Set RenderTarget
	V(m_pD3DDevice->GetRenderTarget(0, &m_pOldRenderTarget));

	if(SUCCEEDED((*light).m_pUpdateShadowMap->GetSurfaceLevel(0, &m_pSurface)))
	{
		m_pD3DDevice->SetRenderTarget(0, m_pSurface);
		SAFE_RELEASE(m_pSurface);
	}

	if(SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pOldDepthStencil)))
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pShadowMapDepthStencil);
	}

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

	D3DXMATRIX mLightView, mLightProj;
	D3DXMATRIX mWorldViewProj, mLightViewInverse, mLightProjInverse;
	mLightView = *((*light).GetViewMatrix());
	mLightProj = *((*light).GetProjMatrix());

	// Render
	V(m_pEffect->SetTechnique("Tech_UpdateShadowMap"));
	V(m_pEffect->Begin(NULL, 0));
	//Pass 1
	V(m_pEffect->BeginPass(1));
	for(Itor i = m_pMoveModelList.begin(); i != m_pMoveModelList.end(); i++)
	{
		D3DXMATRIX mWorldViewProj = (*i)->GetWolrdMtx() * mLightView * mLightProj;

		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));

		V(m_pEffect->CommitChanges());
		(*i)->Render();
	}
	V(m_pEffect->EndPass());

	//pass 0
	V(m_pEffect->BeginPass(0));
	D3DXMatrixInverse(&mLightViewInverse, NULL, &mLightView);
	D3DXMatrixInverse(&mLightViewInverse, NULL, &mLightView);
	mWorldViewProj = m_pBackground->GetWolrdMtx()*mLightProjInverse*mLightViewInverse;

	V(m_pEffect->SetTexture("tShadowMap", (*light).m_pShadowMap));
	V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
	V(m_pEffect->CommitChanges());
	m_pBackground->Render();
	
	V(m_pEffect->EndPass());
	V(m_pEffect->End());

	// Restoref
	if(m_pOldDepthStencil)
	{
		m_pD3DDevice->SetDepthStencilSurface(m_pOldDepthStencil);
		SAFE_RELEASE(m_pOldDepthStencil);
	}
	m_pD3DDevice->SetRenderTarget(0, m_pOldRenderTarget);
	SAFE_RELEASE(m_pOldRenderTarget);

	//D3DXSaveTextureToFile(L"UpdateShadowMap.jpg", D3DXIFF_JPG, (*light).m_pUpdateShadowMap, NULL);
}

void CManager::SplitRemovableShadowMap(void)
{
	D3DLOCKED_RECT d3drc;
	LPDIRECT3DSURFACE9		m_pShadowMapZ;

	for(pLightItor iP = m_pPointLightList.begin(); iP!= m_pPointLightList.end();)
	{
		if(!m_pPointLightList.empty())
		{
			delete (*iP);
			m_pPointLightList.erase(iP++);
		}
		else
			iP++;
	}

	m_pRemovable->GetSurfaceLevel(0,&m_pShadowMapZ);
	m_pD3DDevice->GetRenderTargetData(m_pShadowMapZ,m_UpdateSurface);
	//D3DXSaveSurfaceToFile(L"update.jpg", D3DXIFF_JPG, m_UpdateSurface, NULL, NULL);
	m_UpdateSurface->LockRect(&d3drc, NULL, D3DLOCK_READONLY);

	Split(d3drc, 0, SHADOWMAP_RES, 0, SHADOWMAP_RES);

	m_UpdateSurface->UnlockRect();
}

D3DXVECTOR4 CManager::GetRemovableColor(int x, int y)
{
	LPDIRECT3DSURFACE9 m_pColorMapZ;
	D3DLOCKED_RECT d3drc;
	D3DXVECTOR4 color;

	m_pColorMapRemovable->GetSurfaceLevel(0, &m_pColorMapZ);
	m_pD3DDevice->GetRenderTargetData(m_pColorMapZ,m_RColorSurface);
	//D3DXSaveSurfaceToFile(L"shadowMap.jpg", D3DXIFF_JPG, m_ShadowSurface, NULL, NULL);
	m_RColorSurface->LockRect(&d3drc, NULL, D3DLOCK_READONLY);

	BYTE *bytePointer=(BYTE*)d3drc.pBits;
	DWORD index=(x*4+(y*(d3drc.Pitch)));
	// Blue
    BYTE b=bytePointer[index];

    // Green
    BYTE g=bytePointer[index+1];

    // Red
    BYTE r=bytePointer[index+2];

	color = D3DXVECTOR4(r,g,b,256);
	color/=color.w;

	m_RColorSurface->UnlockRect();

	return color;
}

void CManager::AddPointLight(D3DLOCKED_RECT d3drc, int x, int y)
{
	D3DXVECTOR4 color, pos;
	D3DXVECTOR3 normal;
	D3DXMATRIX mScal, mRot, mTrans;
	D3DLOCKED_RECT heightD3drc;
	LPDIRECT3DSURFACE9		m_pShadowMapZ;

	m_Light.m_pShadowMap->GetSurfaceLevel(0, &m_pShadowMapZ);
	m_pD3DDevice->GetRenderTargetData(m_pShadowMapZ,m_ShadowSurface);
	//D3DXSaveSurfaceToFile(L"shadowMap.jpg", D3DXIFF_JPG, m_ShadowSurface, NULL, NULL);
	m_ShadowSurface->LockRect(&heightD3drc, NULL, D3DLOCK_READONLY);

	DWORD depthColor = ((DWORD*)heightD3drc.pBits)[y*(heightD3drc.Pitch/sizeof(DWORD))+x];
	USHORT dz = GetRValue(depthColor);

	DWORD dwColor = ((DWORD*)d3drc.pBits)[y*(d3drc.Pitch/sizeof(DWORD))+x];
	USHORT z = GetRValue(dwColor);

	if(z<=dz)
	{
		pos.x = (double)x/SHADOWMAP_RES;
		pos.y = (double)y/SHADOWMAP_RES;
		pos.z = (double)z/256;
		pos.w = 1;

		PLight* pLight = new PLight();

		pos.x = pos.x*2-1;
		pos.y = 1-pos.y*2;

		D3DXVec4Transform(&pos, &pos, &m_mLightProjInverse);
		D3DXVec4Transform(&pos, &pos, &m_mLightViewInverse);
		pos/=pos.w;

		//color = D3DXVECTOR4(1.f, 1.f, 0.f, 1.f);

		D3DXMATRIX world;
		D3DXMatrixTranslation(&world, pos.x, pos.y, pos.z);

		pLight->color = GetRemovableColor(x, y);
		pLight->pos = pos;
		pLight->worldMatrix = world;
		m_pPointLightList.push_back(pLight);

		//add indirect light's model 
		//float size = 1.0f;
		//D3DXMatrixScaling(&mScal, size, size, size);
		//D3DXMatrixRotationYawPitchRoll(&mRot, 0.0f, 0.0f, 0.0f);
		//D3DXMatrixTranslation(&mTrans, pos.x, pos.y, pos.z);
		//m_pPlane = new CModel(m_pD3DDevice, L"plane.x", mScal, mRot, mTrans);
		//m_pModelList.push_back(m_pPlane);

		pointLightNumber++;
	}
	m_ShadowSurface->UnlockRect();
}

void CManager::Split(D3DLOCKED_RECT d3drc,  int xStart,int xEnd,int yStart,int yEnd)
{
	double var;
	int midX, midY;
	USHORT color;

	var = GetDepthVariance(d3drc, xStart, (xStart+xEnd)/2, yStart, (yStart+yEnd)/2);
	if(var != -1)
	{	
		midX = ((xStart+xEnd)/2 - xStart)/2;
		midY = ((yStart+yEnd)/2 - yStart)/2;
		DWORD dwColor = ((DWORD*)d3drc.pBits)[midY*(d3drc.Pitch/sizeof(DWORD))+midX];
		color = GetRValue(dwColor);
		if(((var > 250 || color == 255)&&(xEnd-xStart>50)) )
			Split(d3drc, xStart, (xStart+xEnd)/2, yStart, (yStart+yEnd)/2);
		else
			AddPointLight(d3drc, xEnd/4+xStart*3/4, yEnd/4+yStart*3/4);
	}

	var = GetDepthVariance(d3drc, xStart, (xStart+xEnd)/2, (yStart+yEnd)/2, yEnd);
	if(var != -1)
	{
		midX = ((xStart+xEnd)/2 - xStart)/2;
		midY = (yEnd - (yStart+yEnd)/2)/2;
		DWORD dwColor = ((DWORD*)d3drc.pBits)[midY*(d3drc.Pitch/sizeof(DWORD))+midX];
		color = GetRValue(dwColor);
		if(((var > 250 || color == 255)&&(xEnd-xStart>50)) )
			Split(d3drc, xStart, (xStart+xEnd)/2, (yStart+yEnd)/2, yEnd);
		else
			AddPointLight(d3drc, xEnd/4+xStart*3/4, yEnd*3/4+yStart/4);
	}
	
	var = GetDepthVariance(d3drc, (xStart+xEnd)/2, xEnd, yStart, (yStart+yEnd)/2);
	if(var != -1)
	{
		midX = (xEnd - (xStart+xEnd)/2)/2;
		midY = ((yStart+yEnd)/2 - yStart)/2;
		DWORD dwColor = ((DWORD*)d3drc.pBits)[midY*(d3drc.Pitch/sizeof(DWORD))+midX];
		color = GetRValue(dwColor);
		if(((var > 250 || color == 255)&&(xEnd-xStart>50)) )
			Split(d3drc, (xStart+xEnd)/2, xEnd, yStart, (yStart+yEnd)/2);
		else 
			AddPointLight(d3drc, xEnd*3/4+xStart/4, yEnd/4+yStart*3/4);
	}

	var = GetDepthVariance(d3drc, (xStart+xEnd)/2, xEnd, (yStart+yEnd)/2, yEnd);
	if(var != -1)
	{
		midX = (xEnd - (xStart+xEnd)/2)/2;
		midY = (yEnd - (yStart+yEnd)/2)/2;
		DWORD dwColor = ((DWORD*)d3drc.pBits)[midY*(d3drc.Pitch/sizeof(DWORD))+midX];
		color = GetRValue(dwColor);
		if(((var > 250 || color == 255)&&(xEnd-xStart>50)) )
			Split(d3drc, (xStart+xEnd)/2, xEnd, (yStart+yEnd)/2, yEnd);
		else 
			AddPointLight(d3drc, xEnd*3/4+xStart/4, yEnd*3/4+yStart/4);
	}
}

//Visual check of indirect light
void CManager::CheckVisibility(void)
{
	D3DXMATRIX lightView, lightProj, posInLight;
	LPDIRECT3DSURFACE9		m_pShadowMapZ;
	D3DLOCKED_RECT d3drc;
	lightView = *m_Light.GetViewMatrix();
	lightProj = *m_Light.GetProjMatrix();

	m_Light.m_pUpdateShadowMap->GetSurfaceLevel(0, &m_pShadowMapZ);
	m_pD3DDevice->GetRenderTargetData(m_pShadowMapZ,m_TmpSurface);
	m_TmpSurface->LockRect(&d3drc, NULL, D3DLOCK_READONLY);

	for(lightItor iL = m_pIndirectLighting.begin(); iL != m_pIndirectLighting.end(); iL++)
	{
		D3DXVECTOR4 lightPos;
		D3DXVECTOR3 pos;
		int x, y, depth, depthInShadowMap;

		x = (*iL)->posInTex.x;
		y = (*iL)->posInTex.y;
		depth = (*iL)->posInTex.z;

		int fValue = * (float*) (((char*)d3drc.pBits) + y*d3drc.Pitch + x*sizeof(float))*256;

		if(depth < fValue)
			(*iL)->visible = false;
	}

	m_TmpSurface->UnlockRect();
}

double CManager::GetDepthVariance(D3DLOCKED_RECT d3drc,int xStart,int xEnd,int yStart,int yEnd)
{
	int sumPixel;
	double sum, varSum, ave, var;
	
	sumPixel = (yEnd-yStart)*(xEnd-xStart);

	sum = 0;
	for(int i=yStart; i<yEnd; i++)
	{
		for(int j=xStart;j<xEnd; j++)
		{
			DWORD dwColor = ((DWORD*)d3drc.pBits)[i*(d3drc.Pitch/sizeof(DWORD))+j];//Y*Width+X
			
			USHORT r= GetRValue(dwColor);

			sum += r;
		}
	}
	if(sum == 0)
		ave = 0;
	else
		ave = sum/sumPixel;

	if(ave == 255)
		var = -1;
	else
	{
		varSum = 0;
		for(int i=yStart; i<yEnd; i++)
		{
			for(int j=xStart;j<xEnd; j++)
			{
				double diff;
			
				DWORD dwColor = ((DWORD*)d3drc.pBits)[i*(d3drc.Pitch/sizeof(DWORD))+j];
			
				USHORT r = GetRValue(dwColor);

				diff = pow(r - ave,2);

				varSum += diff;
			}
		}
		if(varSum == 0)
			var = 0;
		else
			var = varSum / sumPixel;
	}
	return var;
}

// Get Variance
D3DXVECTOR3 CManager::GetNormalVariance(D3DLOCKED_RECT d3drc,int xStart,int xEnd,int yStart,int yEnd)
{
	double bSum, rSum, gSum;
	double bAve, rAve, gAve;
	double rVar, gVar, bVar;
	double varRSum, varGSum, varBSum;
	D3DXVECTOR3 var;
	int sumPixel;
	
	sumPixel = (yEnd-yStart)*(xEnd-xStart);
	bSum = rSum = gSum = 0;
	for(int i=yStart; i<yEnd; i++)
	{
		for(int j=xStart;j<xEnd; j++)
		{
			DWORD dwColor = ((DWORD*)d3drc.pBits)[i*(d3drc.Pitch/sizeof(DWORD))+j];//Y*Width+X
			
			USHORT r= GetRValue(dwColor);
			USHORT b = GetBValue(dwColor);
			USHORT g = GetGValue(dwColor);

			bSum += b;
			rSum += r;
			gSum += g;
		}
	}
	bAve = bSum/sumPixel;
	rAve = rSum/sumPixel;
	gAve = gSum/sumPixel;

	varRSum = varGSum = varBSum = 0;
	for(int i=yStart; i<yEnd; i++)
	{
		for(int j=xStart;j<xEnd; j++)
		{
			double rDiff, gDiff, bDiff;
			//int fValue = * (float*) (((char*)d3drc.pBits) + i*d3drc.Pitch + j*sizeof(DWORD))*256;
			//diff = pow(((float)fValue) - ave,2);
			//varSum += diff;
			DWORD dwColor = ((DWORD*)d3drc.pBits)[i*(d3drc.Pitch/sizeof(DWORD))+j];
			
			USHORT r = GetRValue(dwColor);
			USHORT b = GetBValue(dwColor);
			USHORT g = GetGValue(dwColor);

			rDiff = pow(r - rAve,2);
			gDiff = pow(g - gAve,2);
			bDiff = pow(b - bAve,2);

			varRSum += rDiff;
			varGSum += gDiff;
			varBSum += bDiff;

		}
	}
	if(varRSum == 0)
		rVar = 0;
	else
		rVar = varRSum / sumPixel;

	if(varGSum == 0)
		gVar = 0;
	else
		gVar = varGSum / sumPixel;

	if(varBSum == 0)
		bVar = 0;
	else
		bVar = varBSum / sumPixel;

	var = D3DXVECTOR3(rVar,gVar,bVar);
	return var;
}

//Get Normal
D3DXVECTOR4 CManager::GetNormal(D3DLOCKED_RECT d3drc, int x, int y)
{
	D3DXVECTOR4 normal;

	DWORD dwColor = ((DWORD*)d3drc.pBits)[y*(d3drc.Pitch/sizeof(DWORD))+x];//Y*Width+X
	
	USHORT b= GetRValue(dwColor);
	USHORT r = GetBValue(dwColor);
	USHORT g = GetGValue(dwColor);

	normal = D3DXVECTOR4(r,g,b,255);

	return normal;
}

//Get Color
D3DXVECTOR4 CManager::GetColor(int x, int y)
{
	D3DXVECTOR4 color;
	D3DLOCKED_RECT d3drc;
	LPDIRECT3DSURFACE9 m_pColorMapZ;

	m_pColorMap->GetSurfaceLevel(0, &m_pColorMapZ);
	m_pD3DDevice->GetRenderTargetData(m_pColorMapZ,m_ColorSurface);
	m_ColorSurface->LockRect(&d3drc, NULL, D3DLOCK_READONLY);

	BYTE *bytePointer=(BYTE*)d3drc.pBits;
	DWORD index=(x*4+(y*(d3drc.Pitch)));
	// Blue
    BYTE b=bytePointer[index];

    // Green
    BYTE g=bytePointer[index+1];

    // Red
    BYTE r=bytePointer[index+2];

	color = D3DXVECTOR4(r,g,b,256);
	color/=color.w;

	m_ColorSurface->UnlockRect();
	return color;
}

// Indirect Light
void CManager::AddIndirectLight(D3DLOCKED_RECT d3drc, D3DLOCKED_RECT heightD3drc, int x, int y)
{
	D3DXVECTOR4 pos, normal;
	D3DXVECTOR3 position, lookAt, posInTex;
	D3DXVECTOR3 d3Normal;
	int z;
	D3DXMATRIX mScal, mRot, mTrans;

	D3DXMatrixIdentity(&mScal);
	D3DXMatrixIdentity(&mRot);
	D3DXMatrixIdentity(&mTrans);

	z = *((LPDWORD)heightD3drc.pBits+x+y*(heightD3drc.Pitch/4))&0x000000ff;

	pos.x = (double)x/SHADOWMAP_RES;
	pos.y = (double)y/SHADOWMAP_RES;
	pos.z = (double)z/256;
	pos.w = 1;
	posInTex = D3DXVECTOR3(x,y,z);
	
	pos.x = pos.x*2-1;
	pos.y = 1-pos.y*2;

	D3DXVec4Transform(&pos, &pos, &m_mLightProjInverse);
	D3DXVec4Transform(&pos, &pos, &m_mLightViewInverse);
	pos/=pos.w;

	normal = GetNormal(d3drc,x,y);
	D3DXVec3Normalize(&d3Normal, &D3DXVECTOR3(normal.x, normal.y, normal.z));

	//D3DXVec4Transform(&normal, &normal, &m_mLightProjInverse);
	//D3DXVec4Transform(&normal, &normal, &m_mLightViewInverse);
	//normal/=normal.w;
	//
	//D3DXVec3Normalize(&d3Normal, &D3DXVECTOR3(normal.x, normal.y, normal.z));

	position = D3DXVECTOR3(pos.x, pos.y, pos.z);
	if(d3Normal.x == 0 && d3Normal.y == 0 && d3Normal.z == 0)
		d3Normal.z = -1;

	lookAt = position + d3Normal;

	CCamera *iLight = new CCamera();

	iLight->posInTex = posInTex;
	iLight->visible = true;
	iLight->SetViewParams(&position, &lookAt);
	iLight->SetProjParams(D3DX_PI*0.8, 1, 0.1f, 1000.f);
	iLight->SetEnablePositionMovement(false);
	iLight->color = GetColor(x, y);
	iLight->normal = D3DXVECTOR4(d3Normal.x, d3Normal.y, d3Normal.z, 1.f);
	D3DXMATRIX world;
	D3DXMatrixTranslation(&world, position.x, position.y, position.z);
	iLight->SetWorldMatrix( world);
	m_pIndirectLighting.push_back(iLight);

	//count
	totalNumber++;
}

// Check Variance
void CManager::CheckVariance(D3DLOCKED_RECT d3drc, D3DLOCKED_RECT heightD3drc, int xStart,int xEnd,int yStart,int yEnd)
{
	double heightVar;
	D3DXVECTOR3 var;
	
	var = GetNormalVariance(d3drc, xStart, (xStart+xEnd)/2, yStart, (yStart+yEnd)/2);
	heightVar = GetDepthVariance(heightD3drc, xStart, (xStart+xEnd)/2, yStart, (yStart+yEnd)/2);
	if(((var.x > 200 || var.y>200 || var.z > 200 || heightVar>200)&&(xEnd-xStart>30)) || xEnd-xStart > 200 )
		CheckVariance(d3drc, heightD3drc, xStart, (xStart+xEnd)/2, yStart, (yStart+yEnd)/2);
	else
		AddIndirectLight(d3drc, heightD3drc ,xEnd/4+xStart*3/4, yEnd/4+yStart*3/4);

	var = GetNormalVariance(d3drc, xStart, (xStart+xEnd)/2, (yStart+yEnd)/2, yEnd);
	heightVar = GetDepthVariance(heightD3drc, xStart, (xStart+xEnd)/2, (yStart+yEnd)/2, yEnd);
	if(((var.x > 200 || var.y>200 || var.z > 200 || heightVar>200)&&(xEnd-xStart>30)) || xEnd-xStart > 200 )
		CheckVariance(d3drc, heightD3drc, xStart, (xStart+xEnd)/2, (yStart+yEnd)/2, yEnd);
	else
		AddIndirectLight(d3drc, heightD3drc, xEnd/4+xStart*3/4, yEnd*3/4+yStart/4);
	
	var = GetNormalVariance(d3drc, (xStart+xEnd)/2, xEnd, yStart, (yStart+yEnd)/2);
	heightVar = GetDepthVariance(heightD3drc, (xStart+xEnd)/2, xEnd, yStart, (yStart+yEnd)/2);
	if(((var.x > 200 || var.y>200 || var.z > 200 || heightVar>200)&&(xEnd-xStart>30)) || xEnd-xStart > 200 )
		CheckVariance(d3drc, heightD3drc, (xStart+xEnd)/2, xEnd, yStart, (yStart+yEnd)/2);
	else 
		AddIndirectLight(d3drc, heightD3drc, xEnd*3/4+xStart/4, yEnd/4+yStart*3/4);

	var = GetNormalVariance(d3drc, (xStart+xEnd)/2, xEnd, (yStart+yEnd)/2, yEnd);
	heightVar = GetDepthVariance(heightD3drc, (xStart+xEnd)/2, xEnd, (yStart+yEnd)/2, yEnd);
	if(((var.x > 200 || var.y>200 || var.z > 200 || heightVar>200)&&(xEnd-xStart>30)) || xEnd-xStart > 200 )
		CheckVariance(d3drc, heightD3drc, (xStart+xEnd)/2, xEnd, (yStart+yEnd)/2, yEnd);
	else 
		AddIndirectLight(d3drc, heightD3drc, xEnd*3/4+xStart/4, yEnd*3/4+yStart/4);
}

//--------------------------------------------------------------------------------------
// 노멀맵을 나누어서 2차 광원으로 사용한다
//--------------------------------------------------------------------------------------
void CManager::SplitNormalMap(void)
{	
	D3DSURFACE_DESC ddsd, heightDdsd;
	D3DLOCKED_RECT d3drc, heightD3drc;
	LPDIRECT3DSURFACE9 m_pShadowMapZ, m_pNormalMapZ;


	//D3DXComputeNormalMap(m_pNormalMap, m_Light.m_pShadowMap, NULL, 0, D3DX_CHANNEL_RED, 50.f);
	
	m_Light.m_pShadowMap->GetSurfaceLevel(0, &m_pShadowMapZ);
	m_pD3DDevice->GetRenderTargetData(m_pShadowMapZ,m_ShadowSurface);
	//D3DXSaveSurfaceToFile(L"shadowMap.jpg", D3DXIFF_JPG, m_ShadowSurface, NULL, NULL);
	m_ShadowSurface->LockRect(&heightD3drc, NULL, D3DLOCK_READONLY);

	m_pNormalMap->GetSurfaceLevel(0, &m_pNormalMapZ);
	m_pD3DDevice->GetRenderTargetData(m_pNormalMapZ,m_NormalSurface);
	D3DXSaveSurfaceToFile(L"NormalMap.png", D3DXIFF_PNG, m_NormalSurface, NULL, NULL);
	m_NormalSurface->LockRect(&d3drc, NULL, D3DLOCK_READONLY);

	CheckVariance(d3drc, heightD3drc, 0, SHADOWMAP_RES, 0, SHADOWMAP_RES);

	m_ShadowSurface->UnlockRect();
	m_NormalSurface->UnlockRect();


}

//--------------------------------------------------------------------------------------
// 그림자매핑으로 렌더링한다
//--------------------------------------------------------------------------------------
void CManager::RenderWithShadowMap(void)
{	
	HRESULT hr;

	D3DXVECTOR4 vLightPosInCameraView[8];
	D3DXMATRIX mCameraViewToLightViewProj[8];
	D3DXVECTOR4 vLightNormal[8];
	D3DXVECTOR4 vLightColor[8];


	V(m_pEffect->SetTechnique("Tech_ShadowMap"));

	V(m_pEffect->Begin(NULL, 0));
	V(m_pEffect->BeginPass(1));

	V(m_pEffect->SetFloat("shadowBias", m_shadowBias));
	V(m_pEffect->SetFloat("lightBias", m_lightBias));

	V(m_pEffect->SetVector("vLightPosInCameraView", &m_vLightPosInCameraView));

	V(m_pEffect->SetMatrix("mCameraViewToLightViewProj", &m_mCameraViewToLightViewProj));

	V(m_pEffect->SetTexture("tShadowMap", m_Light.m_pUpdateShadowMap));

	for(Itor i = m_pModelList.begin(); i != m_pModelList.end(); i++)
	{
		D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
		D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;

		V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
		V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

		V(m_pEffect->CommitChanges());
		(*i)->Render();
	}

	for(Itor i = m_pMoveModelList.begin(); i!= m_pMoveModelList.end(); i++)
	{
		D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
		D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;

		V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
		V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

		V(m_pEffect->CommitChanges());
		(*i)->Render();
	}

	V(m_pEffect->EndPass());

	//V(m_pEffect->BeginPass(2));
	//for(lightItor iL = m_pIndirectLighting.begin(); iL != m_pIndirectLighting.end(); iL++)
	//{
	//	if((*iL)->visible)
	//	{
	//		V(m_pEffect->SetVector("vLightPosInCameraView", &(*iL)->m_vLightPosInCameraView));

	//		V(m_pEffect->SetMatrix("mCameraViewToLightViewProj", &(*iL)->m_mCameraViewToLightViewProj));

	//		V(m_pEffect->SetTexture("tShadowMap", (*iL)->m_pUpdateShadowMap));

	//		V(m_pEffect->SetVector("vLightNormal", &(*iL)->normal));
	//	
	//		V(m_pEffect->SetVector("vLightColor", &(*iL)->color));

	//		for(Itor i = m_pModelList.begin(); i != m_pModelList.end(); i++)
	//		{
	//			D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
	//			D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;

	//			V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
	//			V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
	//			V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

	//			V(m_pEffect->CommitChanges());
	//			(*i)->Render();
	//		}

	//		for(Itor i = m_pMoveModelList.begin(); i!= m_pMoveModelList.end(); i++)
	//		{
	//			D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
	//			D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;

	//			V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
	//			V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
	//			V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

	//			V(m_pEffect->CommitChanges());
	//			(*i)->Render();
	//		}
	//	}
	//	else
	//		(*iL)->visible = true;
	//}
	//V(m_pEffect->EndPass());

	V(m_pEffect->BeginPass(2));

	V(m_pEffect->SetBool("RSM", RSM));

	for(lightItor iL = m_pIndirectLighting.begin(); iL != m_pIndirectLighting.end();)
	{
		for(int x = 0; x <8;)
		{
			if((*iL)->visible)
			{
				switch(x)
				{
				case 0:
					V(m_pEffect->SetTexture("isTex0", (*iL)->m_pUpdateShadowMap));
					break;
				case 1:
					V(m_pEffect->SetTexture("isTex1", (*iL)->m_pUpdateShadowMap));
					break;
				case 2:
					V(m_pEffect->SetTexture("isTex2", (*iL)->m_pUpdateShadowMap));
					break;
				case 3:
					V(m_pEffect->SetTexture("isTex3", (*iL)->m_pUpdateShadowMap));
					break;
				case 4:
					V(m_pEffect->SetTexture("isTex4", (*iL)->m_pUpdateShadowMap));
					break;
				case 5:
					V(m_pEffect->SetTexture("isTex5", (*iL)->m_pUpdateShadowMap));
					break;
				case 6:
					V(m_pEffect->SetTexture("isTex6", (*iL)->m_pUpdateShadowMap));
					break;
				case 7:
					V(m_pEffect->SetTexture("isTex7", (*iL)->m_pUpdateShadowMap));
					break;
				default:
					break;
				}
				vLightPosInCameraView[x] = (*iL)->m_vLightPosInCameraView;
				mCameraViewToLightViewProj[x] = (*iL)->m_mCameraViewToLightViewProj;
				vLightNormal[x] = (*iL)->normal;
				vLightColor[x] = (*iL)->color;
				x++;
				iL++;
				if(iL == m_pIndirectLighting.end())
					x = 8;
			}
				
		}
		V(m_pEffect->SetVectorArray("vLightPosInCameraViews", vLightPosInCameraView, 8));

		V(m_pEffect->SetMatrixArray("mCameraViewToLightViewProjs", mCameraViewToLightViewProj, 8));

		//V(m_pEffect->SetTexture("tShadowMap", (*iL)->m_pUpdateShadowMap));

		V(m_pEffect->SetVectorArray("vLightNormals", vLightNormal, 8));
		
		V(m_pEffect->SetVectorArray("vLightColors", vLightColor, 8));

		for(Itor i = m_pModelList.begin(); i != m_pModelList.end(); i++)
		{
			D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
			D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;

			V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
			V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
			V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

			V(m_pEffect->CommitChanges());
			(*i)->Render();
		}

		for(Itor i = m_pMoveModelList.begin(); i!= m_pMoveModelList.end(); i++)
		{
			D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
			D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;

			V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
			V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
			V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

			V(m_pEffect->CommitChanges());
			(*i)->Render();
		}
	}

	
	
	V(m_pEffect->EndPass());

	/*V(m_pEffect->BeginPass(3));
	{
		V(m_pEffect->SetTexture("tRemovableShadowMap", m_pRemovable));
		V(m_pEffect->SetTexture("tSquareRemovableShadowMap", m_pSquareRemovable));
		V(m_pEffect->SetTexture("tRemovableColorMap", m_pColorMapRemovable));
		V(m_pEffect->SetMatrix("mLightViewInverse", &m_mLightViewInverse));
		V(m_pEffect->SetMatrix("mLightProjInverse", &m_mLightProjInverse));
		
		for(Itor i = m_pModelList.begin(); i != m_pModelList.end(); i++)
		{
			D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
			D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;

			V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
			V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
			V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));
			
			V(m_pEffect->CommitChanges());
			(*i)->Render();
		}

		for(Itor i = m_pMoveModelList.begin(); i!= m_pMoveModelList.end(); i++)
		{
			D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
			D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;

			V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
			V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
			V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

			V(m_pEffect->CommitChanges());
			(*i)->Render();
		}

	}
	V(m_pEffect->EndPass());*/

	V(m_pEffect->BeginPass(3));
	{
		for(pLightItor pItor = m_pPointLightList.begin(); pItor != m_pPointLightList.end(); pItor++)
		{
			V(m_pEffect->SetVector("pointLightPos", &(*pItor)->pos));
			V(m_pEffect->SetVector("pointLightColor", &(*pItor)->color));

			for(Itor i = m_pModelList.begin(); i != m_pModelList.end(); i++)
			{
				D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
				D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;
				V(m_pEffect->SetMatrix("mWorld", &(*i)->GetWolrdMtx()));
				V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
				V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
				V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

				V(m_pEffect->CommitChanges());
				(*i)->Render();
			}

			for(Itor i = m_pMoveModelList.begin(); i!= m_pMoveModelList.end(); i++)
			{
				D3DXMATRIX mWorldView = (*i)->GetWolrdMtx() * m_mCameraView;
				D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;
				
				V(m_pEffect->SetMatrix("mWorld", &(*i)->GetWolrdMtx()));
				V(m_pEffect->SetMatrix("mWorldView", &mWorldView));
				V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
				V(m_pEffect->SetTexture("tDiffuse", (*i)->GetDiffuseTexture()));

				V(m_pEffect->CommitChanges());
				(*i)->Render();
			}
		}
	}
	V(m_pEffect->EndPass());
	V(m_pEffect->End());
}

//--------------------------------------------------------------------------------------
// 렌더링 한다
//--------------------------------------------------------------------------------------
void CManager::RenderLight(void)
{
	HRESULT hr;
	V(m_pEffect->SetTechnique("Tech_Light"));

	V(m_pEffect->Begin(NULL, 0));
	V(m_pEffect->BeginPass(1));
	
	for(lightItor iL = m_pIndirectLighting.begin(); iL != m_pIndirectLighting.end(); iL++)
	{
		if((*iL)->visible)
		{
			D3DXMATRIX world = *(*iL)->GetWorldMatrix();
			D3DXMATRIX mWorldView = *((*iL)->GetWorldMatrix()) * m_mCameraView;
			D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;

			V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
			V(m_pEffect->SetVector("vLightColor", &(*iL)->color));
			V(m_pEffect->CommitChanges());
			m_pPlane->Render();
		}
	}

	for(pLightItor pItor = m_pPointLightList.begin(); pItor != m_pPointLightList.end(); pItor++)
	{
		D3DXMATRIX world = (*pItor)->worldMatrix;
		D3DXMATRIX mWorldView = world * m_mCameraView;
		D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;
		V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
		V(m_pEffect->SetVector("vLightColor", &(*pItor)->color));
		V(m_pEffect->CommitChanges());
		m_pPlane->Render();
	}

	V(m_pEffect->EndPass());

	V(m_pEffect->BeginPass(0));
	
	D3DXMATRIX world;
	D3DXVECTOR3 pos;
	pos = *m_Light.GetEyePt();
	D3DXMatrixIdentity(&world);
	D3DXMatrixTranslation(&world, pos.x, pos.y, pos.z);
	m_Light.SetWorldMatrix(world);

	world = *(m_Light.GetWorldMatrix());
	D3DXMATRIX mWorldView = *(m_Light.GetWorldMatrix()) * m_mCameraView;
	D3DXMATRIX mWorldViewProj = mWorldView * m_mCameraProj;
	V(m_pEffect->SetMatrix("mWorldViewProj", &mWorldViewProj));
	V(m_pEffect->CommitChanges());
	m_pPlane->Render();

	V(m_pEffect->EndPass());

	V(m_pEffect->End());
}

//--------------------------------------------------------------------------------------
// 렌더링 한다
//--------------------------------------------------------------------------------------
void CManager::FrameRender(void)
{
	// Standard shadow mapping
	if(m_ShadowType == STANDARD_SHADOW_MAP) 
	{
		if(!generatedShadowmap)
			GenerateShadowMap();
		RenderWithShadowMap();
	}

	// Light
	if(renderLight)
		RenderLight();
}

//--------------------------------------------------------------------------------------
// 매 프레임마다 업데이트 해야할 일
//--------------------------------------------------------------------------------------
void CManager::FrameMove(float fElapsedTime)
{
	int speed = 2.f;
	D3DXMATRIX mTrans;
	D3DXMatrixIdentity(&mTrans);

	m_Camera.FrameMove(fElapsedTime);
	m_Light.FrameMove(fElapsedTime);

	m_mCameraView = *(m_Camera.GetViewMatrix());
	m_mCameraProj = *(m_Camera.GetProjMatrix());
	m_mLightView = *(m_Light.GetViewMatrix());
	m_mLightProj = *(m_Light.GetProjMatrix());

	D3DXMATRIX mInverse;
	D3DXMatrixInverse(&mInverse, NULL, &m_mCameraView);
	m_mCameraViewToLightView = mInverse * m_mLightView;
	m_mCameraViewToLightViewProj = mInverse * m_mLightView * m_mLightProj;

	D3DXMatrixInverse(&m_mLightViewInverse, NULL, &m_mLightView);
	D3DXMatrixInverse(&m_mLightProjInverse, NULL, &m_mLightProj);
	
	D3DXVec3Transform(&m_vLightPosInCameraView, m_Light.GetEyePt(), &m_mCameraView);

	for(Itor i = m_pMoveModelList.begin(); i != m_pMoveModelList.end(); i++)
	{
		
		D3DXMatrixTranslation(&mTrans, (*i)->velocity.x, (*i)->velocity.y, (*i)->velocity.z);
		(*i)->ChangeWorldMatrix(mTrans);
		D3DMATRIX world = (*i)->GetWolrdMtx();

		if (world._41 > (50 - 2 * 2.0f))
			(*i)->velocity.x = -(*i)->velocity.x;

		if (world._41 < -(50 - 2 * 2.0f))
			(*i)->velocity.x = -(*i)->velocity.x;

		if (world._42 > (50 - 2 * 2.0f))
			(*i)->velocity.y = -(*i)->velocity.y;

		if (world._42 < -(0 - 2 * 2.0f))
			(*i)->velocity.y = -(*i)->velocity.y;

		if (world._43 > (50 - 2 * 2.0f))
			(*i)->velocity.z = -(*i)->velocity.z;

		if (world._43 < -(50 - 2 * 2.0f))
			(*i)->velocity.z = -(*i)->velocity.z;
	}

	for(lightItor iL = m_pIndirectLighting.begin(); iL != m_pIndirectLighting.end();iL++)
	{
		(*iL)->visible = true;
		D3DXMATRIX iLightView = *((*iL)->GetViewMatrix());
		D3DXMATRIX iLightProj = *((*iL)->GetProjMatrix());

		(*iL)->m_mCameraViewToLightView = mInverse * iLightView;
		(*iL)->m_mCameraViewToLightViewProj = mInverse * iLightView * iLightProj;

		D3DXVec3Transform(&((*iL)->m_vLightPosInCameraView), (*iL)->GetEyePt(), &m_mCameraView);

		ShadowUpdate(*iL);
	}

	ShadowUpdate(&m_Light);
	CheckVisibility();
	ShadowMapWithRemovable();
	ColorMapWithRemovable();
	pointLightNumber = 0;
	SplitRemovableShadowMap();
	ShadowMapWithSquareRemovable();

	//TestRendering();
}

//--------------------------------------------------------------------------------------
// 메시지 프로시져
//--------------------------------------------------------------------------------------
void CManager::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//if(WM_KEYDOWN != uMsg)	
	m_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

	m_Light.HandleMessages( hWnd, uMsg, wParam, lParam );
	for(lightItor iL = m_pIndirectLighting.begin(); iL != m_pIndirectLighting.end();iL++)
	{
		(*iL)->HandleMessages(hWnd, uMsg, wParam, lParam );
	}
}

//--------------------------------------------------------------------------------------
// 셋업한다
//--------------------------------------------------------------------------------------
void CManager::SetUp(void)
{
	m_pEffect = NULL;

	D3DXMatrixIdentity(&m_mCameraView);
	D3DXMatrixIdentity(&m_mCameraProj);
	D3DXMatrixIdentity(&m_mLightView);
	D3DXMatrixIdentity(&m_mLightProj);

	generatedShadowmap = false;
	renderLight = false;
	m_shadowBias = 0.0045f;
	m_lightBias = 0.01f;
	m_ShadowType = STANDARD_SHADOW_MAP;
	totalNumber = 0;
	count = 0;
	pointLightNumber = 0;
	RSM = false;

	LoadModelFromFile();
	LoadEffectFromFile();
	CreateTextures();
	SetUpCameras();

	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
}

//--------------------------------------------------------------------------------------
// 모델 파일을 로드한다
//--------------------------------------------------------------------------------------
void CManager::LoadModelFromFile(void)
{
	CModel* pModel = NULL;
	D3DXMATRIX mScal, mRot, mTrans;
	D3DXMatrixIdentity(&mScal);
	D3DXMatrixIdentity(&mRot);
	D3DXMatrixIdentity(&mTrans);

	float size = 1.f;

	// 렌더타겟으로 사용하기 위한 사각형
	size = 0.5f;
	D3DXMatrixScaling(&mScal, size, size, size);
	D3DXMatrixRotationYawPitchRoll(&mRot, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&mTrans, 0.0f, 0.0f, 0.0f);
	m_pPlane = new CModel(m_pD3DDevice, L"plane.x", L"plane.jpg", mScal, mRot, mTrans);

	size = 2.0f;
	D3DXMatrixScaling(&mScal, size, size, size);
	D3DXMatrixRotationYawPitchRoll(&mRot, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&mTrans, 0.0f, 0.0f, 0.0f);
	m_pBackground = new CModel(m_pD3DDevice, L"plane.x", L"plane.jpg", mScal, mRot, mTrans);
	
	//-------------------------------//
	// 장면에 등장하는 모델
	//-------------------------------//
	// 바닥
	size = 1.f;
	D3DXMatrixScaling(&mScal, size, size, size);
	D3DXMatrixRotationYawPitchRoll(&mRot, 0.0, 0.0f, 0.0f);
	D3DXMatrixTranslation(&mTrans, 0.0f, 0.0f, 0.0f);
	pModel = new CModel(m_pD3DDevice, L"floor.x", L"red.jpg", mScal, mRot, mTrans);
	m_pModelList.push_back(pModel);

	size = 1.f;
	D3DXMatrixIdentity(&mScal);
	D3DXMatrixIdentity(&mRot);
	D3DXMatrixIdentity(&mTrans);
	D3DXMatrixScaling(&mScal, size, size, size);
	D3DXMatrixRotationYawPitchRoll(&mRot, 0.0, D3DX_PI/2, 0.0f);
	D3DXMatrixTranslation(&mTrans, 0.0f, 50.0f, -50.0f);
	pModel = new CModel(m_pD3DDevice, L"floor.x", L"green.jpg", mScal, mRot, mTrans);
	m_pModelList.push_back(pModel);

	size = 1.f;
	D3DXMatrixIdentity(&mScal);
	D3DXMatrixIdentity(&mRot);
	D3DXMatrixIdentity(&mTrans);
	D3DXMatrixScaling(&mScal, size, size, size);
	D3DXMatrixRotationYawPitchRoll(&mRot, 0.0, 0.0f, -D3DX_PI/2);
	D3DXMatrixTranslation(&mTrans, -70.0f, 50.0f, 0.0f);
	pModel = new CModel(m_pD3DDevice, L"floor.x", L"blue.jpg", mScal, mRot, mTrans);
	m_pModelList.push_back(pModel);

	//size = 1.f;
	//D3DXMatrixIdentity(&mScal);
	//D3DXMatrixIdentity(&mRot);
	//D3DXMatrixIdentity(&mTrans);
	//D3DXMatrixScaling(&mScal, 5, 30.f, 0.1f);
	//D3DXMatrixRotationYawPitchRoll(&mRot, 0.0, 0.0f, 0.f);
	//D3DXMatrixTranslation(&mTrans, 0.0f, 0.0f, 0.0f);
	//pModel = new CModel(m_pD3DDevice, L"box.x", L"white.jpg", mScal, mRot, mTrans);
	//m_pModelList.push_back(pModel);

	//D3DXMatrixIdentity(&mScal);
	//D3DXMatrixIdentity(&mRot);
	//D3DXMatrixIdentity(&mTrans);
	//D3DXMatrixScaling(&mScal, 0.01f, 30.f, 5.f);
	//D3DXMatrixRotationYawPitchRoll(&mRot, 0.0, 0.0f, 0.f);
	//D3DXMatrixTranslation(&mTrans, -40.0f, 0.0f, 50.0f);
	//pModel = new CModel(m_pD3DDevice, L"box.x", L"white.jpg", mScal, mRot, mTrans);
	//m_pModelList.push_back(pModel);

	 //박스
	//for(int i = 0; i<2; i++)
	//{
	//	for(int j = 0; j<2; j++)
	//	{
			size = 2.f;
			D3DXMatrixIdentity(&mScal);
			D3DXMatrixIdentity(&mRot);
			D3DXMatrixIdentity(&mTrans);
			D3DXMatrixScaling(&mScal, size, size, size);
			D3DXMatrixRotationYawPitchRoll(&mRot, 0.f, 0.0f, 0.0f);
			//D3DXMatrixTranslation(&mTrans, -20.0f+j*30.f, 17.0f, -10.0f+i*30.f);
			D3DXMatrixTranslation(&mTrans, -20.0f, 25.0f, -10.0f);
			pModel = new CModel(m_pD3DDevice, L"dragon.x", L"white.jpg",mScal, mRot, mTrans);
			m_pModelList.push_back(pModel);
	//	}
	//}
}

void CManager::AddMovemodel()
{
	D3DXVECTOR3 velocity;
	CModel* pModel = NULL;
	D3DXMATRIX mScal, mRot, mTrans;
	D3DXMatrixIdentity(&mScal);
	D3DXMatrixIdentity(&mRot);
	D3DXMatrixIdentity(&mTrans);

	float size = 1.f;

	srand((unsigned)time(NULL));
	int i = rand();
	i = i%7;
	// 렌더타겟으로 사용하기 위한 사각형
	size = 5.0f;
	D3DXMatrixScaling(&mScal, size, size, size);
	D3DXMatrixRotationYawPitchRoll(&mRot, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&mTrans, 0.0f, 30.0f, 0.0f);
	switch(i)
	{
	case 0:
		pModel = new CModel(m_pD3DDevice, L"sphere.x", L"purple.jpg", mScal, mRot, mTrans);
		break;
	case 1:
		pModel = new CModel(m_pD3DDevice, L"sphere.x", L"blue.jpg", mScal, mRot, mTrans);
		break;
	case 2:
		pModel = new CModel(m_pD3DDevice, L"sphere.x", L"pink.jpg", mScal, mRot, mTrans);
		break;
	case 4:
		pModel = new CModel(m_pD3DDevice, L"sphere.x", L"yellow.jpg", mScal, mRot, mTrans);
		break;
	case 3:
		pModel = new CModel(m_pD3DDevice, L"sphere.x", L"orange.jpg", mScal, mRot, mTrans);
		break;
	case 5:
		pModel = new CModel(m_pD3DDevice, L"sphere.x", L"purple.jpg", mScal, mRot, mTrans);
		break;
	case 6:
		pModel = new CModel(m_pD3DDevice, L"sphere.x", L"orange.jpg", mScal, mRot, mTrans);
		break;
	case 7:
		pModel = new CModel(m_pD3DDevice, L"sphere.x", L"pink.jpg", mScal, mRot, mTrans);
		break;
	default:
		break;
	}

	srand((unsigned)time(NULL));
	float rho = 1.f + 0.5f * (1.f * 
                (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)));

    float phi = 45.f * (D3DX_PI / 180.0f);

    float theta = (360.0f * (static_cast<float>(rand()) / static_cast<float>(
                  RAND_MAX))) * (D3DX_PI / 180.0f);

    velocity.x = rho * cosf(phi) * cosf(theta);
    velocity.y = rho * sinf(phi);
    velocity.z = rho * cosf(phi) * sinf(theta);
	pModel->velocity = velocity;

	m_pMoveModelList.push_back(pModel);

}
//--------------------------------------------------------------------------------------
// 이펙트 파일을 로드한다
//--------------------------------------------------------------------------------------
HRESULT CManager::LoadEffectFromFile(void)
{
	SAFE_RELEASE(m_pEffect);
	HRESULT hr;
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY;

	//dwShaderFlags |= D3DXSHADER_NO_PRESHADER;

	// prefer flow control
	//dwShaderFlags |= D3DXSHADER_PREFER_FLOW_CONTROL;

	// Read the D3DX effect file
	WCHAR str[MAX_PATH];
	V(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"Fx//effect.fx"));

	// If this fails, there should be debug output as to 
	// why the .fx file failed to compile
	LPD3DXBUFFER erBuf = NULL;
	hr = D3DXCreateEffectFromFile(m_pD3DDevice, str, NULL, NULL, dwShaderFlags, NULL, &m_pEffect, &erBuf);
	
	if(erBuf != NULL)
	{
		MessageBoxA(DXUTGetHWND(), (char*)erBuf->GetBufferPointer(), "ERROR!", 0);
	}

	if(FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;

	//D3DXCreateEffectFromFile(m_pD3DDevice, L"Fx//effect.fx", NULL, NULL, 
	//	dwShaderFlags, NULL, &m_pEffect, NULL);
}

//--------------------------------------------------------------------------------------
// 텍스쳐를 생성한다
//--------------------------------------------------------------------------------------
void CManager::CreateTextures(void)
{
	m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 1, D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_Light.m_pShadowMap, NULL);

	m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 1, D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_Light.m_pUpdateShadowMap, NULL);

	m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 0, D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pRemovable, NULL);

	m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 0, D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pSquareRemovable, NULL);

	m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 1, D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pNormalMap, NULL);

	m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 1, D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pDepthMap, NULL);

	m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 1, D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pColorMap, NULL);

	m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 1, D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pColorMapRemovable, NULL);

	m_pD3DDevice->CreateTexture(SHADOWMAP_RES, SHADOWMAP_RES, 1, D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTestMap, NULL);

	DXUTDeviceSettings d3dSettings = DXUTGetDeviceSettings();
	m_pD3DDevice->CreateDepthStencilSurface(SHADOWMAP_RES, SHADOWMAP_RES, d3dSettings.d3d9.pp.AutoDepthStencilFormat, 
							D3DMULTISAMPLE_NONE, 0, TRUE, &m_pShadowMapDepthStencil, NULL);

	m_pD3DDevice->CreateOffscreenPlainSurface(SHADOWMAP_RES,SHADOWMAP_RES,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&m_TmpSurface,NULL);
	m_pD3DDevice->CreateOffscreenPlainSurface(SHADOWMAP_RES,SHADOWMAP_RES,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&m_ShadowSurface,NULL);
	m_pD3DDevice->CreateOffscreenPlainSurface(SHADOWMAP_RES,SHADOWMAP_RES,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&m_NormalSurface,NULL);
	m_pD3DDevice->CreateOffscreenPlainSurface(SHADOWMAP_RES,SHADOWMAP_RES,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&m_ColorSurface,NULL);
	m_pD3DDevice->CreateOffscreenPlainSurface(SHADOWMAP_RES,SHADOWMAP_RES,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&m_UpdateSurface,NULL);
	m_pD3DDevice->CreateOffscreenPlainSurface(SHADOWMAP_RES,SHADOWMAP_RES,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&m_RColorSurface,NULL);

}

//--------------------------------------------------------------------------------------
// 카메라를 설정한다
//--------------------------------------------------------------------------------------
void CManager::SetUpCameras(void)
{
	D3DXVECTOR3	vFromPt, vLookatPt;
	float fov		= D3DX_PI/4;
	float aspect	= WINDOW_SIZE_W / WINDOW_SIZE_H;
	float nearPlane = 1.0f;
	float farPlane	= 1000.f;

	// View 카메라
	m_Camera.SetScalers(CAMERA_ROTATE_SCALER, CAMERA_MOVE_SCALER);
	m_Camera.SetRotateButtons(true, false, false);
	vFromPt		= D3DXVECTOR3(100.f, 100.f, 100.f);
	vLookatPt	= D3DXVECTOR3(-100.f, 0.f, -100.f);
	m_Camera.SetViewParams(&vFromPt, &vLookatPt);
	m_Camera.SetProjParams(fov, aspect, nearPlane, farPlane);

	// Light 카메라
	m_Light.SetScalers(0.001f, 1.0f);
	//m_Light.SetRotateButtons(false, false, false);
	vFromPt		= D3DXVECTOR3(50.0f, 110.f, 80.0f);
	vLookatPt	= D3DXVECTOR3(-100.0f, 0.0f, -100.0f);
	m_Light.SetViewParams(&vFromPt, &vLookatPt);
	m_Light.SetButtonMasks(NULL	,NULL ,NULL );
	//m_Light.SetProjParams(fov, aspect, nearPlane, farPlane);
	m_Light.SetProjParams(D3DX_PI*0.4f, aspect, 10.f, farPlane);
	m_Light.SetEnablePositionMovement(false);
}

//--------------------------------------------------------------------------------------
// 메모리 해제한다
//--------------------------------------------------------------------------------------
void CManager::Release(void)
{
	//SAFE_RELEASE(m_pShadowMap);
	SAFE_RELEASE(m_pShadowMapDepthStencil);
	SAFE_RELEASE(m_pOldRenderTarget);
	SAFE_RELEASE(m_pOldDepthStencil);
	SAFE_RELEASE(m_pSurface);
	SAFE_RELEASE(m_pNormalMap);
	SAFE_RELEASE(m_pColorMap);
	SAFE_RELEASE(m_pDepthMap);
	SAFE_RELEASE(m_pEffect);
	//delete(m_pPlane);

	for(Itor i = m_pModelList.begin(); i != m_pModelList.end();)
	{
		delete (*i);
		m_pModelList.erase(i++);
	}

	for(lightItor iL = m_pIndirectLighting.begin(); iL != m_pIndirectLighting.end();)
	{
		//(*iL)->Release();
		delete (*iL);
		m_pIndirectLighting.erase(iL++);
	}

	
	m_pD3DDevice->SetTexture(0, NULL);
}

//--------------------------------------------------------------------------------------
// 글자 출력
//--------------------------------------------------------------------------------------
void CManager::DrawText(CDXUTTextHelper* pTxtHelper)
{
	WCHAR text[256];

	D3DXVECTOR3 cameraPos = *(m_Camera.GetEyePt());
	StringCchPrintf(text, 256, L"Camera position: %0.2f, %0.2f, %0.2f", cameraPos.x, cameraPos.y, cameraPos.z);
	pTxtHelper->DrawTextLine(text);

	D3DXVECTOR3 cameraLookat = *(m_Camera.GetLookAtPt());
	StringCchPrintf(text, 256, L"Camera lookat:%0.2f, %0.2f, %0.2f", cameraLookat.x, cameraLookat.y, cameraLookat.z);
	pTxtHelper->DrawTextLine(text);

	D3DXVECTOR3 lightPos = *(m_Light.GetEyePt());
	StringCchPrintf(text, 256, L"Light position: %0.2f, %0.2f, %0.2f", lightPos.x, lightPos.y, lightPos.z);
	pTxtHelper->DrawTextLine(text);

	D3DXVECTOR3 lightLookat = *(m_Light.GetLookAtPt());
	StringCchPrintf(text, 256, L"Light lookat: %0.2f, %0.2f, %0.2f", lightLookat.x, lightLookat.y, lightLookat.z);
	pTxtHelper->DrawTextLine(text);

	StringCchPrintf(text, 256, L"Shadow bias[z, x]: %f", m_shadowBias);
	pTxtHelper->DrawTextLine(text);	

	StringCchPrintf(text, 256, L"Light bias[z, x]: %f", m_lightBias);
	pTxtHelper->DrawTextLine(text);	

	StringCchPrintf(text, 256, L"Number of Indirect Light: %d", totalNumber);
	pTxtHelper->DrawTextLine(text);	

	StringCchPrintf(text, 256, L"Number of Point Light: %d", pointLightNumber);
	pTxtHelper->DrawTextLine(text);	

	StringCchPrintf(text, 256, L"RSM: %d", RSM);
	pTxtHelper->DrawTextLine(text);	
}

//--------------------------------------------------------------------------------------
// 키입력 처리
//--------------------------------------------------------------------------------------
void CManager::OnKeyboard(UINT key)
{
	if(key == 'Z' || key == 'z') 
		m_shadowBias -= 0.0005f;
	else if(key == 'X' || key == 'x')
		m_shadowBias += 0.0005f;
	else if(key == VK_SPACE)
		CaptureScene();
	else if(key == 'C' || key == 'c') 
		m_lightBias+=0.002f;
	else if(key == 'V' || key == 'v') 
		m_lightBias-=0.002f;
	else if(key == 'L' || key == 'l') 
	{
		if(renderLight)
			renderLight = false;
		else
			renderLight = true;
	}
	else if(key == 'R' || key == 'r') 
	{
		if(RSM)
			RSM = false;
		else
			RSM = true;
	}
	else if(key == 'F' || key == 'f') 
		AddMovemodel();
}

void CManager::CaptureScene(void)
{
	HRESULT hr;
	SYSTEMTIME timer;
	GetLocalTime(&timer);

	WCHAR filename[256] = {};
	wsprintf(filename, L"Capture\\%04d년_%02d월%02d일_%02d시%02d분%02d초.png",
		timer.wYear, timer.wMonth, timer.wDay, timer.wHour, timer.wMinute, timer.wSecond);

	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	D3DXSaveSurfaceToFile(filename, D3DXIFF_PNG, pBackBuffer, NULL, NULL);
	pBackBuffer->Release();
}

//--------------------------------------------------------------------------------------
// 생성자
//--------------------------------------------------------------------------------------
CManager::CManager(LPDIRECT3DDEVICE9 pD3DDevice)
:m_pD3DDevice(pD3DDevice)
{	
	SetUp();
}

//--------------------------------------------------------------------------------------
// 소멸자
//--------------------------------------------------------------------------------------
CManager::~CManager(void)
{
	Release();
}