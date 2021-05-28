#include "DXUT.h"
#include "Model.h"


//--------------------------------------------------------------------------------------
// ������
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
// �Ҹ���
//--------------------------------------------------------------------------------------
CModel::~CModel(void)
{
	Release();
}

//--------------------------------------------------------------------------------------
// �������Ѵ�
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
// Mesh ������ �ε��Ѵ�
//--------------------------------------------------------------------------------------
void CModel::LoadMeshFromFile(void)
{
	TCHAR currdir[256] = {0, };
	GetCurrentDirectory(256, currdir);
	SetCurrentDirectory(L"Data"); // Mesh ������ �ִ� ���� ���
	
	// Mesh �ε�
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
// �޸� �����Ѵ�
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