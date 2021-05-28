struct VS_INPUT
{
	float4 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 Tex		: TEXCOORD0;
};

struct VS_OUTPUT1
{
	float4 Pos	: POSITION;
	float4 Tex	: TEXCOORD0;
};

struct VS_OUTPUT2
{
	float4 Pos				: POSITION;
	float4 PosInCameraView	: TEXCOORD0;		
	float4 PosInLightProj	: TEXCOORD1;
	float3 Normal			: TEXCOORD2;
	float4 Tex				: TEXCOORD3;
};

struct VS_OUTPUT3
{
	float4 Pos				: POSITION;
	float4 PosInCameraView  : TEXCOORD0;
	float4 PosInWorld		: TEXCOORD1;
	float3 Normal			: TEXCOORD2;
	float2 Tex				: TEXCOORD3;
};

struct PS_OUTPUT
{
	float4 color		: COLOR0;
	float depth			: DEPTH0;
};

struct pointLight
{
	float4 pos;
	float4 color;
};

texture	tShadowMap;
sampler	sShadowMap = sampler_state
{
    Texture = <tShadowMap>;
    MipFilter = Point;
    MinFilter = Point;
    MagFilter = Point;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

texture tIndirectShadowMap;
sampler	sIndirectShadowMap = sampler_state
{
    Texture = <tIndirectShadowMap>;
    MipFilter = Point;
    MinFilter = Point;
    MagFilter = Point;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

texture tRemovableShadowMap;
sampler	sRemovableShadowMap = sampler_state
{
    Texture = <tRemovableShadowMap>;
    MipFilter = Point;
    MinFilter = Point;
    MagFilter = Point;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

texture tRemovableColorMap;
sampler	sRemovableColorMap = sampler_state
{
    Texture = <tRemovableColorMap>;
    MipFilter = Point;
    MinFilter = Point;
    MagFilter = Point;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

texture tSquareRemovableShadowMap;
sampler	sSquareRemovableShadowMap = sampler_state
{
    Texture = <tSquareRemovableShadowMap>;
    MipFilter = Point;
    MinFilter = Point;
    MagFilter = Point;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

texture	tDiffuse;
sampler	sDiffuse = sampler_state
{
    Texture = <tDiffuse>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

texture isTex0;
texture isTex1;
texture isTex2;
texture isTex3;
texture isTex4;
texture isTex5;
texture isTex6;
texture isTex7;

sampler	sIndirectShadowMaps[8] = 
{
	sampler_state
	{
		Texture = <isTex0>;
		MipFilter = Point;
		MinFilter = Point;
		MagFilter = Point;
		AddressU  = CLAMP;
		AddressV  = CLAMP;
	},

	sampler_state
	{
		Texture = <isTex1>;
		MipFilter = Point;
		MinFilter = Point;
		MagFilter = Point;
		AddressU  = CLAMP;
		AddressV  = CLAMP;
	},

	sampler_state
	{
		Texture = <isTex2>;
		MipFilter = Point;
		MinFilter = Point;
		MagFilter = Point;
		AddressU  = CLAMP;
		AddressV  = CLAMP;
	},

	sampler_state
	{
		Texture = <isTex3>;
		MipFilter = Point;
		MinFilter = Point;
		MagFilter = Point;
		AddressU  = CLAMP;
		AddressV  = CLAMP;
	},

	sampler_state
	{
		Texture = <isTex4>;
		MipFilter = Point;
		MinFilter = Point;
		MagFilter = Point;
		AddressU  = CLAMP;
		AddressV  = CLAMP;
	},

	sampler_state
	{
		Texture = <isTex5>;
		MipFilter = Point;
		MinFilter = Point;
		MagFilter = Point;
		AddressU  = CLAMP;
		AddressV  = CLAMP;
	},

	sampler_state
	{
		Texture = <isTex6>;
		MipFilter = Point;
		MinFilter = Point;
		MagFilter = Point;
		AddressU  = CLAMP;
		AddressV  = CLAMP;
	},

	sampler_state
	{
		Texture = <isTex7>;
		MipFilter = Point;
		MinFilter = Point;
		MagFilter = Point;
		AddressU  = CLAMP;
		AddressV  = CLAMP;
	},
};

float4x4	mCameraViewToLightViewProjs[8];
float4		vLightPosInCameraViews[8];
float4		vLightNormals[8];
float4		vLightColors[8];

float4x4    mWorld;
float4x4	mWorldView;
float4x4	mWorldViewProj;
float4x4	mCameraViewToLightViewProj;
float4x4	mLightViewInverse;
float4x4	mLightProjInverse;

float4		vLightPosInCameraView;
float4		vLightColor;
float4      vLightNormal;
float		shadowBias;
float		lightBias;

float4		pointLightPos;
float4		pointLightColor;
int			count;
bool		RSM;

pointLight  pointLights[1000];


