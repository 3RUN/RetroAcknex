
// comment this to disable certain effects
#define VERTEX_SNAPPING

float4x4 matWorldViewProj;
float4x4 matWorld;

float iLights;
float4 vecLightPos[8];
float4 vecLightColor[8];
float4 vecLightDir[8];

float4 vecViewPos;
float4 vecViewDir;
float4 vecFog;
float4 vecFogColor;
float4 vecLight;
float4 vecColor;
float4 vecAmbient;
float4 vecEmissive;
float4 vecSkill1;

texture entSkin1;

sampler ColorSampler = sampler_state
{
	Texture = <entSkin1>;
	Mipfilter = None;
	Minfilter = None;
	Magfilter = None;
};

void VS(
in float4 inposition : POSITION,
in float3 innormal : NORMAL,
in float4 intex1 : TEXCOORD0,
in float4 intex2 : TEXCOORD1,
out float4 outposition : POSITION,
out float4 outcolor : COLOR0,
out float3 outnormal : TEXCOORD0,
out float4 outtex : TEXCOORD1,
out float4 outworldPos : TEXCOORD2)
{
	inposition.w = 1.0f;
	
	// vertex snapping
	#ifdef VERTEX_SNAPPING
		float4 snapToPixel = mul(inposition, matWorldViewProj);
		float4 vertex = snapToPixel;
		vertex.xyz = snapToPixel.xyz / snapToPixel.w;
		vertex.x = floor((vecSkill1.x + 40) * vertex.x) / (vecSkill1.x + 40); // default 160
		vertex.y = floor(vecSkill1.x * vertex.y) / vecSkill1.x; // default 120
		vertex.xyz *= snapToPixel.w;
		outposition = vertex;
		#else
		outposition = mul(inposition, matWorldViewProj);
	#endif
	
	outnormal = normalize(mul(innormal, (float3x3)matWorld));
	outtex.xy = intex1.xy;
	outtex.zw = intex2.xy;
	outworldPos = mul(inposition, matWorld);
	
	outcolor = (vecAmbient * vecLight) + float4(vecEmissive.xyz * vecColor.xyz, vecLight.w);
	outcolor.rgb = saturate(outcolor.rgb);
}

float4 PS(
float4 color : COLOR0,
float3 normal : TEXCOORD0,
float4 tex : TEXCOORD1,
float4 worldPos : TEXCOORD2) : COLOR0
{
	float4 textureColor = tex2D(ColorSampler, tex.xy);
	color.rgb *= textureColor.rgb;
	color.a = min(vecLight.w, textureColor.a);
	
	return color;
}

technique
{
	pass pass0
	{
		//ZWriteEnable = True;
		//AlphaBlendEnable = True;
		//AlphaFunc = Greater;
		
		VertexShader = compile vs_3_0 VS(); 
		PixelShader  = compile ps_3_0 PS(); 
	}
}

technique fallback { pass one { } }