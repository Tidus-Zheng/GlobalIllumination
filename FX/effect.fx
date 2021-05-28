#include "defines.fx"

//--------------------------------------------------------------------------------------
// Tech_ShadowMap : Standard Shadow Mapping
//--------------------------------------------------------------------------------------
// pass0 : 그림자 맵을 생성한다
VS_OUTPUT1 VS_GenerateShadowMap(VS_INPUT In)
{		
	VS_OUTPUT1 Out = (VS_OUTPUT1)0;

	Out.Pos = mul(float4(In.Pos.xyz, 1.f), mWorldViewProj);
	Out.Tex = Out.Pos;
	//Out.Tex = float4(In.Tex, 0.f, 1.f);

	return Out;
}

float4 PS_GenerateShadowMap(VS_OUTPUT1 In) : COLOR0
{
	float4 Color = (float4)0.f;
	
	//Color.x = In.Tex.z;				// depth in light view space
	//Color.y = In.Tex.z / In.Tex.w;	// normalized depth in light view space
	Color = In.Tex.z/In.Tex.w;
	//Color = In.Tex;

	return Color;
}

// pass1 : 장면을 렌더링 한다
VS_OUTPUT2 VS_RenderWithShadowMap(VS_INPUT In)
{	
	VS_OUTPUT2 Out = (VS_OUTPUT2)0;
	
	Out.Pos					= mul(In.Pos, mWorldViewProj);
	Out.PosInCameraView		= mul(In.Pos, mWorldView);
	Out.PosInLightProj		= mul(Out.PosInCameraView, mCameraViewToLightViewProj);
	Out.Normal				= mul(In.Normal, (float3x3)mWorldView);
	Out.Tex					= float4(In.Tex, 0.f, 0.f);	

	return Out;
}

float4 PS_RenderWithShadowMap(VS_OUTPUT2 In) : COLOR0
{
	float4 color;
	float2 uv = float2(0.5f, -0.5f) * In.PosInLightProj.xy / In.PosInLightProj.w + float2(0.5f, 0.5f);	
		
	float z = tex2D(sShadowMap, uv).y;

	float depth = In.PosInLightProj.z / In.PosInLightProj.w;

	float shadow = ((depth - z) > shadowBias) ? 0.f : 1.0f;	

	if(uv.x>1||uv.x<0||uv.y>1||uv.y<0)
		shadow = 1;

	float4 lightDir = normalize(In.PosInCameraView - vLightPosInCameraView);	
	color =	shadow * max(0, abs(dot(-lightDir, normalize(In.Normal)))) * tex2D(sDiffuse, In.Tex);

	return color;
}

//pass2
float4 PS_RenderWithIndirectShadowMap(VS_OUTPUT2 In) : COLOR0
{
	float4 color;
	float2 uv = float2(0.5f, -0.5f) * In.PosInLightProj.xy / In.PosInLightProj.w + float2(0.5f, 0.5f);	

	float z = tex2D(sShadowMap, uv).y;

	float depth = In.PosInLightProj.z / In.PosInLightProj.w;

	float shadow = ((depth - z) > shadowBias) ? 0.f : 1.0f;	

	//if(uv.x>1||uv.x<0||uv.y>1||uv.y<0)
		//shadow = 1;

	float3 lightnormal = mul(vLightNormal.xyz, (float3x3)mWorldView);

	float4 lightDir = normalize(In.PosInCameraView - vLightPosInCameraView);	

	float dis = distance(In.PosInCameraView, vLightPosInCameraView);
	dis = sqrt(sqrt(sqrt(dis)));
	// * max(0, abs(dot(lightDir, normalize(lightnormal))))
	if(RSM)
		color = max(0, dot(-lightDir, normalize(In.Normal))) * max(0, dot(lightDir, normalize(lightnormal))) * tex2D(sDiffuse, In.Tex);
	else
		color = shadow * max(0, dot(-lightDir, normalize(In.Normal))) * max(0, dot(lightDir, normalize(lightnormal))) * tex2D(sDiffuse, In.Tex);
	
	color /= dis;

	return color*vLightColor*lightBias;
}

//pass2
float4 PS_NewRenderWithIndirectShadowMap(VS_OUTPUT2 In) : COLOR0
{
	float4 color, finalColor;
	float2 uv = float2(0.5f, -0.5f) * In.PosInLightProj.xy / In.PosInLightProj.w + float2(0.5f, 0.5f);	

	finalColor = (float4)0;
	for(int i = 0; i< 8; i++)
	{
		float z = tex2Dlod(sIndirectShadowMaps[i], float4(uv,0,0)).y;

		float depth = In.PosInLightProj.z / In.PosInLightProj.w;

		float shadow = ((depth - z) > shadowBias) ? 0.f : 1.0f;	

		if(uv.x>1||uv.x<0||uv.y>1||uv.y<0)
			shadow = 1;

		float3 lightnormal = mul(vLightNormals[i].xyz, (float3x3)mWorldView);

		float4 lightDir = normalize(In.PosInCameraView - vLightPosInCameraViews[i]);	

		float dis = distance(In.PosInCameraView, vLightPosInCameraViews[i]);
		dis = sqrt(sqrt(sqrt(dis)));
		// * max(0, abs(dot(lightDir, normalize(lightnormal))))
		if(RSM)
			color = max(0, dot(-lightDir, normalize(In.Normal))) * max(0, dot(lightDir, normalize(lightnormal))) * tex2D(sDiffuse, In.Tex);
		else
			color = shadow * max(0, dot(-lightDir, normalize(In.Normal))) * max(0, dot(lightDir, normalize(lightnormal))) * tex2D(sDiffuse, In.Tex);
	
		color /= dis;
		color = color*vLightColors[i] * lightBias;

		finalColor += color;
	}
	//finalcolor = vLightColor
	return finalColor ;
}

// pass 3 : Point Light
VS_OUTPUT3 VS_RenderWithPointLight(VS_INPUT In)
{
	VS_OUTPUT3 Out = (VS_OUTPUT3)0;
	
	Out.Pos					= mul(In.Pos, mWorldViewProj);
	Out.PosInCameraView		= mul(In.Pos, mWorldView);
	Out.PosInWorld			= mul(In.Pos, mWorld);
	Out.Normal				= mul(In.Normal, (float3x3)mWorld);
	Out.Tex					= float4(In.Tex, 0.f, 0.f);	

	return Out;
}

float4 PS_RenderWithPointLight(VS_OUTPUT3 In):COLOR0
{
	float4 color;

	float4 l = -(In.PosInWorld - pointLightPos)/100;
	float atten = saturate(1.0f - dot(l, l));
	l = normalize(l);
	float3 n = normalize(In.Normal);
	float d = saturate(dot(n, l));
	
	//float dis = distance(In.PosInWorld, pointLightPos);
	//dis = sqrt(sqrt(dis));

	float4 c = float4(tex2D(sDiffuse, In.Tex.xy).xyz, 1.f);

	color = d * c * pointLightColor * atten * lightBias;
	//color = float4(0.f,0.f,0.f,0.f);

	return color;
}

pointLight AddPointLight(int x, int y)
{
	float4 depth, pos, color;
	pointLight pL;
	depth = tex2D(sRemovableShadowMap, float2(x,y));

	pos.x = x;
	pos.y = y;
	pos.z = depth.z/256;
	pos.w = 1;

	pos.x = pos.x*2-1;
	pos.y = 1-pos.y*2;

	pos = mul(pos, mLightViewInverse);
	pos = mul(pos, mLightProjInverse);
	pos /= pos.w;

	color = tex2D(sRemovableColorMap, float2(x,y));	
	
	//pL.pos = pos;
	//pL.color = color;	
	pointLights[1].pos = pos;
	pointLights[1].color = color;

	//count++;
	return pL;
}	

float GetVariance(int x, int y, int frequency)
{
	float variance, ave, squAve;
	float4 colorAve, colorSquAve;

	colorAve = tex2Dlod(sRemovableShadowMap, float4(x, y, 0, frequency));
	colorSquAve = tex2Dlod(sSquareRemovableShadowMap, float4(x, y, 0, frequency));

	variance = colorSquAve.z - sqrt(colorAve.z);

	if(colorAve.z == 255)
		variance = -1;

	return variance;
}

int AddOne(int number)
{
	return number++;
}

void SplitNormalMap(int xStart, int xEnd, int yStart, int yEnd, int frequency)
{
	int ltxStart, ltxEnd, ltyStart, ltyEnd, ltbxStart, ltbxEnd, ltbyStart, ltbyEnd;
	int ldxStart, ldxEnd, ldyStart, ldyEnd, ldbxStart, ldbxEnd, ldbyStart, ldbyEnd;
	int rtxStart, rtxEnd, rtyStart, rtyEnd, rtbxStart, rtbxEnd, rtbyStart, rtbyEnd;
	int rdxStart, rdxEnd, rdyStart, rdyEnd, rdbxStart, rdbxEnd, rdbyStart, rdbyEnd;
	float variance;
	int fre = 8;
	int id[8] = {1,1,1,1,1,1,1,1};
	int idFre = 11111111;
	pointLight pL;

	ltxStart = xStart;
	ltxEnd = (xStart+xEnd)/2;
	ltyStart = yStart;
	ltyEnd = (yStart+yEnd)/2;

	ldxStart = xStart;
	ldxEnd = (xStart+xEnd)/2;
	ldyStart = (xStart+xEnd)/2;
	ldyEnd = yEnd;

	rtxStart = (xStart+xEnd)/2;
	rtxEnd = xEnd;
	rtyStart = yStart;
	rtyEnd = (yStart+yEnd)/2;

	rdxStart = (xStart+xEnd)/2;
	rdxEnd = xEnd;
	rdyStart = (xStart+xEnd)/2;
	rdyEnd = yEnd;

	while(fre>0)
	{
		ltbxStart = ltxStart;
		ltbxEnd = ltxEnd;
		ltbyStart = ltyStart;
		ltbyEnd = ltyEnd;

		ldbxStart = ldxStart;
		ldbxEnd = ldxEnd;
		ldbyStart = ldyStart;
		ldbyEnd = ldyEnd;

		rtbxStart = rtxStart;
		rtbxEnd = rtxEnd;
		rtbyStart = rtyStart;
		rtbyEnd = rtyEnd;

		rdbxStart = rdxStart;
		rdbxEnd = rdxEnd;
		rdbyStart = rdyStart;
		rdbyEnd = rdyEnd;

		if(id[fre] == 1)
		{
			variance = GetVariance(ltbxStart,ltbyStart,frequency);
			if(variance>250)
			{
				ltxStart = ltbxStart;
				ltxEnd = (ltbxStart+ltbxEnd)/2;
				ltyStart = ltbyEnd;
				ltyEnd = (ltbyStart+ltbyEnd)/2;

				ldxStart = ltbxStart;
				ldxEnd = (ltbxStart+ltbxEnd)/2;
				ldyStart = (ltbyStart+ltbyEnd)/2;
				ldyEnd = ltbyEnd;

				rtxStart = (ltbxStart+ltbxEnd)/2;
				rtxEnd = ltbxEnd;
				rtyStart = ltbyStart;
				rtyEnd = (ltbyStart+ltbyEnd)/2;

				rdxStart = (ltbxStart+ltbxEnd)/2;
				rdxEnd = ltbxEnd;
				rdyStart = (ltbyStart+ltbyEnd)/2;
				rdyEnd = ltbyEnd;

				fre--;
			}
			else
			{
				pL = AddPointLight(ltbxStart/2+ltbxEnd/2, ltbyStart/2+ltbyEnd/2);
				pointLights[count] = pL;
				count ++;
				int fr = id[fre];
				id[fre] = AddOne(fr);
			}
		}
		else if(id[fre] == 2)
		{
			variance = GetVariance(ldbxStart,ldbyEnd,frequency);
			if(variance>250)
		{
				ltxStart = ldbxStart;
				ltxEnd = (ldbxStart+ldbxEnd)/2;
				ltyStart = ldbyEnd;
				ltyEnd = (ldbyStart+ldbyEnd)/2;

				ldxStart = ldbxStart;
				ldxEnd = (ldbxStart+ldbxEnd)/2;
				ldyStart = (ldbyStart+ldbyEnd)/2;
				ldyEnd = ldbyEnd;

				rtxStart = (ldbxStart+ldbxEnd)/2;
				rtxEnd = ldbxEnd;
				rtyStart = ldbyStart;
				rtyEnd = (ldbyStart+ldbyEnd)/2;

				rdxStart = (ldbxStart+ldbxEnd)/2;
				rdxEnd = ltbxEnd;
				rdyStart = (ldbyStart+ldbyEnd)/2;
				rdyEnd = ltbyEnd;

				fre--;
			}
			else
			{
				pL = AddPointLight(ldbxStart/2+ldbxEnd/2, ldbyStart/2+ldbyEnd/2);
				id[fre]++;
			}
		}
		else if(id[fre] == 3)
		{
			variance = GetVariance(rtbxStart, rtbyStart, frequency);
			if(variance > 250)
			{
				ltxStart = rtbxStart;
				ltxEnd = (rtbxStart+rtbxEnd)/2;
				ltyStart = ltbyEnd;
				ltyEnd = (rtbyStart+rtbyEnd)/2;

				ldxStart = rtbxStart;
				ldxEnd = (rtbxStart+rtbxEnd)/2;
				ldyStart = (rtbyStart+rtbyEnd)/2;
				ldyEnd = rtbyEnd;

				rtxStart = (rtbxStart+rtbxEnd)/2;
				rtxEnd = rtbxEnd;
				rtyStart = rtbyStart;
				rtyEnd = (rtbyStart+rtbyEnd)/2;

				rdxStart = (rtbxStart+rtbxEnd)/2;
				rdxEnd = rtbxEnd;
				rdyStart = (rtbyStart+rtbyEnd)/2;
				rdyEnd = rtbyEnd;

				fre--;
			}
			else
			{
				pL = AddPointLight(rtbxStart/2+rtbxEnd/2, rtbyStart/2+rtbyEnd/2);
				id[fre]++;
			}
		}
		else if(id[fre] == 4)
		{
			variance = GetVariance(rdbxStart, rdbyStart, frequency);
			if(variance > 250)
			{
				ltxStart = rdbxStart;
				ltxEnd = (rdbxStart+rdbxEnd)/2;
				ltyStart = ldbyEnd;
				ltyEnd = (rdbyStart+rdbyEnd)/2;

				ldxStart = rdbxStart;
				ldxEnd = (rdbxStart+rdbxEnd)/2;
				ldyStart = (rdbyStart+rdbyEnd)/2;
				ldyEnd = rdbyEnd;

				rtxStart = (rdbxStart+rdbxEnd)/2;
				rtxEnd = rdbxEnd;
				rtyStart = rdbyStart;
				rtyEnd = (rdbyStart+rdbyEnd)/2;

				rdxStart = (rdbxStart+rdbxEnd)/2;
				rdxEnd = rdbxEnd;
				rdyStart = (rdbyStart+rdbyEnd)/2;
				rdyEnd = rdbyEnd;

				fre--;
			}
			else
			{
				pL = AddPointLight(rtbxStart/2+rtbxEnd/2, rtbyStart/2+rtbyEnd/2);
				fre ++;

				ltxStart = ltbxStart;
				ltxEnd = ltbxEnd;
				ltyStart = ltbyStart;
				ltyEnd = ltbyEnd;

				ldxStart = ldbxStart;
				ldxEnd = ldbxEnd;
				ldyStart = ldbyStart;
				ldyEnd = ldbyEnd;

				rtxStart = rtbxStart;
				rtxEnd = rtbxEnd;
				rtyStart = rtbyStart;
				rtyEnd = rtbyEnd;

				rdxStart = rdbxStart;
				rdxEnd = rdbxEnd;
				rdyStart = rdbyStart;
				rdyEnd = rdbyEnd;
			}
		}
	}
}

//new pass3
float4 PS_NewRenderWithPointLight(VS_OUTPUT3 In) : COLOR0
{
	float4 finalColor, color;
	count = 2;	
	finalColor = (float4)0.f;

	SplitNormalMap(0, 1, 0, 1, 8);

	//AddPointLight(1,1);

	
	for(int i = 0; i<count; i++)
	{
		float4 l = normalize(In.PosInWorld - pointLights[i].pos);
		float3 n = normalize(In.Normal);
		float d = dot(n, l);
		d = max(0, d);
	
		float dis = distance(In.PosInWorld, pointLightPos);
		dis = sqrt(sqrt(dis));

		float4 c = float4(tex2D(sDiffuse, In.Tex.xy).xyz, 1.f)/dis;

		color = d * c * pointLightColor * 0.01;
		finalColor += color;
	}

	return finalColor;
}
//--------------------------------------------------------------------------------------
// Tech_Color
//--------------------------------------------------------------------------------------
// pass0 : color
VS_OUTPUT1 VS_GenerateColorMap(VS_INPUT In)
{		
	VS_OUTPUT1 Out = (VS_OUTPUT1)0;

	Out.Pos = mul(float4(In.Pos.xyz, 1.f), mWorldViewProj);
	Out.Tex = float4(In.Tex, 0.f, 1.f);

	return Out;
}

float4 PS_GenerateColorMap(VS_OUTPUT1 In) : COLOR0
{
	float4 Color = (float4)0.f;
	
	Color = float4(tex2D(sDiffuse, In.Tex.xy).xyz, 1.f);

	return Color;
}

//--------------------------------------------------------------------------------------
// Tech_Normal
//--------------------------------------------------------------------------------------
VS_OUTPUT2 VS_GenerateNormalMap(VS_INPUT In)
{
	VS_OUTPUT2 Out = (VS_OUTPUT2)0;
	float4 nor = float4(In.Normal.xyz, 1.f);
	float4 worldNor = mul(In.Normal, mWorld);
	
	Out.Pos					= mul(In.Pos, mWorldViewProj);
	Out.PosInCameraView		= mul(In.Pos, mWorldView);
	Out.PosInLightProj		= mul(Out.PosInCameraView, mCameraViewToLightViewProj);
	Out.Normal				= worldNor.xyz;
	//Out.Normal			= mul(In.Normal, (float3x3)mWorldView);
	Out.Tex					= mul(In.Pos, mWorldViewProj);
	//Out.Tex/=Out.Tex.w;	

	return Out;
}

float4 PS_GenerateNormalMap(VS_OUTPUT2 In):COLOR0
{
	float4 color;

	color = float4(In.Normal, 1.f);
	//color = float4(In.Normal.z, 0.f, 0.f, 1.f);

	return color;
}

//--------------------------------------------------------------------------------------
// Tech_ShadowMapUpdate
//--------------------------------------------------------------------------------------
VS_OUTPUT2 VS_UpdateShadowMap(VS_INPUT In)
{
	VS_OUTPUT2 Out = (VS_OUTPUT2)0;
	
	Out.Pos					= mul(In.Pos, mWorldViewProj);
	Out.PosInCameraView		= mul(In.Pos, mWorldView);
	Out.PosInLightProj		= mul(Out.PosInCameraView, mCameraViewToLightViewProj);
	Out.Normal				= mul(In.Normal, (float3x3)mWorldView);
	Out.Tex					= mul(In.Pos, mWorldViewProj);
	//Out.Tex/=Out.Tex.w;	

	return Out;
}

PS_OUTPUT PS_UpdateShadwoMap(VS_OUTPUT2 In)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;

	float z = In.Tex.z/In.Tex.w;

	Out.depth = z;
	Out.color = Out.depth;

	return Out;
}

VS_OUTPUT1 VS_UpdateShadowMapBackGround(VS_INPUT In)
{
	VS_OUTPUT1 Out = (VS_OUTPUT1)0;

	//Out.Pos = mul(float4(In.Pos.xyz, 1.f), mWorldViewProj);
	Out.Pos = float4(In.Pos.xyz*2, 1.f);
	Out.Tex = float4(In.Tex, 0.f, 0.f);
	

	return Out;
}

PS_OUTPUT PS_UpdateShadowMapBackGround(VS_OUTPUT1 In)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;
	float2 uv = In.Tex.xy;
	float depth = tex2D(sShadowMap, uv).z;
	Out.depth = depth;
	Out.color = Out.depth;
	return Out;
}

//--------------------------------------------------------------------------------------
// Tech_Light : 조명 그리기
//--------------------------------------------------------------------------------------
// pass0
VS_OUTPUT1 VS_RenderLight(VS_INPUT In)
{		
	VS_OUTPUT1 Out = (VS_OUTPUT1)0;

	Out.Pos = mul(float4(In.Pos.xyz, 1.f), mWorldViewProj);	

	return Out;
}

float4 PS_RenderIndirectLight(VS_OUTPUT1 In) : COLOR0
{
	return vLightColor;
}

//pass1
float4 PS_RenderLight(VS_OUTPUT1 In) : COLOR0
{
	return float4(1,1,0,0);
}

//--------------------------------------------------------------------------------------
// Tech_Test
//--------------------------------------------------------------------------------------

VS_OUTPUT1 VS_Test(VS_INPUT In)
{		
	VS_OUTPUT1 Out = (VS_OUTPUT1)0;

	Out.Pos = float4(In.Pos.xyz*2, 1.f);
	Out.Tex = float4(In.Tex, 0.f, 0.f);

	return Out;
}

PS_OUTPUT PS_Test(VS_OUTPUT1 In)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;
	float2 uv = In.Tex.xy;
	float depth = tex2Dlod(sShadowMap, float4(uv, 0.f, 1.f)).z;
	Out.depth = depth;
	Out.color = Out.depth;
	return Out;
}

//--------------------------------------------------------------------------------------
// Tech_Test
//--------------------------------------------------------------------------------------

float4 PS_GenerateSquareShadowMap(VS_OUTPUT1 In) : COLOR0
{
	float4 Color = (float4)0.f;
	
	//Color.x = In.Tex.z;				// depth in light view space
	//Color.y = In.Tex.z / In.Tex.w;	// normalized depth in light view space
	Color = In.Tex.z/In.Tex.w;
	//Color = In.Tex;
	Color = sqrt(Color);

	return Color;
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique Tech_ShadowMap
{
	pass pass0
	{
		CullMode = CCW;
	    VertexShader = compile vs_3_0 VS_GenerateShadowMap();
		PixelShader  = compile ps_3_0 PS_GenerateShadowMap();
	}
	
	pass pass1
	{
		CullMode = CCW;
		AlphaBlendEnable = TRUE;
	    VertexShader = compile vs_3_0 VS_RenderWithShadowMap();
		PixelShader  = compile ps_3_0 PS_RenderWithShadowMap();
	}	
	pass pass2
	{
		CullMode = CCW;
		AlphaBlendEnable = TRUE;
		DestBlend = ONE;
		SrcBlend = ONE;
	    VertexShader = compile vs_3_0 VS_RenderWithShadowMap();
		PixelShader  = compile ps_3_0 PS_NewRenderWithIndirectShadowMap();
	}

	pass pass3
	{
		CullMode = CCW;
		AlphaBlendEnable = TRUE;
		DestBlend = ONE;
		SrcBlend = ONE;
		VertexShader = compile vs_3_0 VS_RenderWithPointLight();
		PixelShader  = compile ps_3_0 PS_RenderWithPointLight();
	}
}

technique Tech_Square
{
	pass pass0
	{
		CullMode = CCW;
		VertexShader = compile vs_3_0 VS_GenerateShadowMap();
		Pixelshader = compile ps_3_0 PS_GenerateSquareShadowMap();
	}
}

technique Tech_UpdateShadowMap
{
	pass pass0
	{
		AlphaBlendEnable = FALSE;
		VertexShader = compile vs_3_0 VS_UpdateShadowMapBackGround();
		PixelShader = compile ps_3_0  PS_UpdateShadowMapBackGround();
	}
	pass pass1
	{
		AlphaBlendEnable = FALSE;
		VertexShader = compile vs_3_0 VS_UpdateShadowMap();
		PixelShader = compile ps_3_0 PS_UpdateShadwoMap();
		
	}
}

technique Tech_Color
{
	pass pass0
	{
		CullMode = CCW;
		VertexShader = compile vs_3_0 VS_GenerateColorMap();
		PixelShader  = compile ps_3_0 PS_GenerateColorMap();
	}
}

technique Tech_Light
{
	pass pass0
	{
		CullMode = None;
	    VertexShader = compile vs_3_0 VS_RenderLight();
		PixelShader  = compile ps_3_0 PS_RenderLight();
	}

	pass pass1
	{
		CullMode = None;
	    VertexShader = compile vs_3_0 VS_RenderLight();
		PixelShader  = compile ps_3_0 PS_RenderIndirectLight();
	}
}

technique Tech_Normal
{
	pass pass0
	{
		CullMode = None;
		VertexShader = compile vs_3_0 VS_GenerateNormalMap();
		PixelShader = compile ps_3_0 PS_GenerateNormalMap();
	}
}

technique Tech_Test
{
	pass pass0
	{
		Cullmode = None;
		VertexShader = compile vs_3_0 VS_Test();
		PixelShader = compile ps_3_0 PS_Test();
	}
}
