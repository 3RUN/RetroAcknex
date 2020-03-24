
// comment this to disable certain effects
#define VERTEX_SNAPPING

float4x4 matWorldViewProj;
float4x4 matWorld;

float4 vecViewPos;
float4 vecFog;
float4 vecFogColor;
float4 vecSkill1;

texture mtlSkin1;

samplerCUBE skyCUBE = sampler_state 
{
	Texture = <mtlSkin1>;
	Mipfilter = None;
	Minfilter = None;
	Magfilter = None;
};

void VS(
in float4 inposition: POSITION,
out float4 outposition: POSITION,
out float4 outWorld: TEXCOORD1)
{
	// vertex snapping
	#ifdef VERTEX_SNAPPING
		float4 snapToPixel = mul(inposition, matWorldViewProj);
		float4 vertex = snapToPixel;
		vertex.xyz = snapToPixel.xyz / snapToPixel.w;
		vertex.x = floor((vecSkill1.y + 40) * vertex.x) / (vecSkill1.y + 40); // default 160
		vertex.y = floor(vecSkill1.y * vertex.y) / vecSkill1.y; // default 120
		vertex.xyz *= snapToPixel.w;
		outposition = vertex;
		#else
		outposition = mul(inposition, matWorldViewProj);
	#endif
	
	outWorld = mul(inposition, matWorld);
}

float4 PS(in float4 worldPos: TEXCOORD1) : COLOR0
{
	float3 Dir = normalize(worldPos.xyz - vecViewPos.xyz);
	float4 Color = texCUBE(skyCUBE, Dir);
	Color.a = 1.0f;
	
	float Fog = 1.0f - pow(Dir.y, vecSkill1.x);
	Color.rgb = lerp(Color.rgb, vecFogColor.rgb, Fog);
	
	return Color;
}

technique Sky
{
	pass p0
	{
		CullMode = None;
		
		VertexShader = compile vs_3_0 VS();
		PixelShader  = compile ps_3_0 PS();
	}
}

technique fallback { pass one { } }