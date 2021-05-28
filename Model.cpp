#include "DXUT.h"
#include "Model.h"


//--------------------------------------------------------------------------------------
// 생성자
//--------------------------------------------------------------------------------------
CModel::CModel(LPDIRECT3DDEVICE9 pD3DDevice, LPCWSTR fileName, LPCWSTR textureName,
			D3DXMATRIX& mScal, D3DXMATRIX& mRot, D3DXMATRIX& mTrans)
{
	m_pD3DDevice	= pD3DDevice;
	m_fileName		= fileName;
	m_textureName   = textureName;
	m_mScal			= mScal;
	m_mRot			= mRot;
	m_mTrans		= mTrans;
	m_mWorld		= m_mScal * m_mRot * m_mTrans;

	LoadMeshFromFile();
}

//--------------------------------------------------------------------------------------
// 소멸자
//--------------------------------------------------------------------------------------
CModel::~CModel(void)
{
	Release();
}

//--------------------------------------------------------------------------------------
// 렌더링한다
//--------------------------------------------------------------------------------------
void CModel::Render(void)
{
	for(DWORD i = 0; i < m_dwNumMaterials; i++)
	{
		//m_pD3DDevice->SetMaterial(&m_pMaterials[i]);
		//m_pD3DDevice->SetTexture(0, m_pTextures[i]);
		m_pMesh->DrawSubset(i);
	}
}

void CModel::ChangeWorldMatrix(D3DXMATRIX& trans)
{
	m_mWorld *= trans;
}

//--------------------------------------------------------------------------------------
// Mesh 파일을 로드한다
//--------------------------------------------------------------------------------------
void CModel::LoadMeshFromFile(void)
{
	TCHAR currdir[256] = {0, };
	GetCurrentDirectory(256, currdir);
	SetCurrentDirectory(L"Data"); // Mesh 파일이 있는 폴더 경로
	
	// Mesh 로드
	LPD3DXBUFFER pD3DXMtrlBuffer;

	D3DXLoadMeshFromX(m_fileName, D3DXMESH_SYSTEMMEM, m_pD3DDevice, NULL, &pD3DXMtrlBuffer, 
					NULL, &m_dwNumMaterials, &m_pMesh);

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMaterials = new D3DMATERIAL9[m_dwNumMaterials];
	m_pTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];

	D3DXCreateTextureFromFile(m_pD3DDevice, m_textureName, m_pTextures);

	/*for(DWORD i = 0; i < m_dwNumMaterials; i++)
	{
		m_pMaterials[i] = d3dxMaterials[i].MatD3D;
		m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;

		m_pTextures[i] = NULL;
		if(d3dxMaterials[i].pTextureFilename != NULL && lstrlenA(d3dxMaterials[i].pTextureFilename) > 0)
		{
			D3DXCreateTextureFromFileA(m_pD3DDevice, d3dxMaterials[i].pTextureFilename, &m_pTextures[i]);
		}
	}*/

	pD3DXMtrlBuffer->Release();

	SetCurrentDirectory(currdir);
}

//--------------------------------------------------------------------------------------
// 메모리 해제한다
//--------------------------------------------------------------------------------------
void CModel::Release(void)
{
	SAFE_DELETE_ARRAY(m_pMaterials);

	if(m_pTextures)
	{
		for(DWORD i = 0; i < m_dwNumMaterials; i++)
			SAFE_RELEASE(m_pTextures[i]);

		SAFE_DELETE_ARRAY(m_pTextures);
	}

	SAFE_RELEASE(m_pMesh);	
}