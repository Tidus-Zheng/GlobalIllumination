#pragma once

#include "DXUTCamera.h"

class PLight
{
public:
	PLight(void){}
	~ PLight(void){}

	D3DXVECTOR4 pos;
	D3DXVECTOR4 color;
	D3DXVECTOR3 normal;
	D3DXMATRIX worldMatrix;
};
