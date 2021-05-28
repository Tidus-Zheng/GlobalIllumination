#pragma once

class CModel
{
private:
	LPDIRECT3DDEVICE9	m_pD3DDevice;
	
	// Mesh ฐüทร
	LPCWSTR				m_fileName;
	LPCWSTR				m_textureName;
	LPD3DXMESH			m_pMesh;
	D3DMATERIAL9*		m_pMaterials;
	LPDIRECT3DTEXTURE9*	m_pTextures;
	DWORD				m_dwNumMaterials;

	// Matrix
	D3DXMATRIX			m_mScal;
	D3DXMATRIX			m_mRot;
	D3DXMATRIX			m_mTrans;
	D3DXMATRIX			m_mWorld;

private:
	void LoadMeshFromFile(void);
	void Release(void);

public:
	CModel(LPDIRECT3DDEVICE9 pD3DDevice, LPCWSTR fileName, LPCWSTR textureName,
		D3DXMATRIX& mScal, D3DXMATRIX& mRot, D3DXMATRIX& mTrans);
	~CModel(void);

	void Render(void);
	void ChangeWorldMatrix(D3DXMATRIX& trans);

	LPCTSTR				m_pTexFileName;
	D3DXMATRIX&			GetWolrdMtx(void)		{ return m_mWorld; }
	LPDIRECT3DTEXTURE9	GetDiffuseTexture(void)	{ return m_pTextures[0]; }
	D3DXVECTOR3 velocity;
};

