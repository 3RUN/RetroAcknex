
// comment this to disable certain effects
#define VERTEX_SNAPPING
#define AFFINE_TEX_MAPPING
#define POLY_CUTOFF

// set to zero to disable effect
#define ANGLE_SURFACE_DARKEN 0.25 // less number - less darken

float4x4 matWorldViewProj;
float4x4 matWorld;

int iRepeat;
float4x4 matEffect1;

float iLights;
float4 vecLightPos[8];
float4 vecLightColor[8];
float4 vecLightDir[8];

float4 vecViewPos;
float4 vecViewDir;
float4 vecFog;
float4 vecFogColor;
float4 vecSkill1;

texture entSkin1;
texture LightMap;

sampler ColorSampler = sampler_state 
{ 
	Texture = <entSkin1>;
	Mipfilter = None;
	Minfilter = None;
	Magfilter = None;
};

sampler ShadowSampler = sampler_state
{
	Texture = <LightMap>;
	Mipfilter = Linear;
	Minfilter = Linear;
	Magfilter = Linear;
};

void VS(
in float4 inposition : POSITION,
in float2 intex1 : TEXCOORD0,
in float2 innormal : NORMAL,
out float4 outposition : POSITION,
out float4 outcolor : COLOR0,
out float4 outworldPos : TEXCOORD0,
out float2 outtex : TEXCOORD1,
out float4 outnormal : TEXCOORD2)
{
	inposition.w = 1.0f;
	
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
	
	// affine texture mapping
	#ifdef AFFINE_TEX_MAPPING
		outposition *= inposition.w / length(mul(inposition, matWorldViewProj));
	#endif
	
	outworldPos = mul(inposition, matWorld);
	outtex.xy = intex1.xy;
	outnormal = mul(innormal, matWorld);
	
	// vertex lightning
	outcolor = 0;
	
	float light = 0;
	if(vecSkill1.w <= 0){ vecSkill1.w = 1; } // make sure we do have lights at all !
	for(light = 0; light < iLights; light += 1.0)
	{
		// light ray
		float3 ray = outworldPos - vecLightPos[light].xyz;
		
		// spotlight factor
		float depth = saturate(dot(vecLightDir[light].xyz, ray) / vecLightPos[light].w);
		float spot = 1.0 - saturate(length(cross(vecLightDir[light].xyz, ray)) / (vecLightPos[light].w * depth));
		
		// normalize the light ray
		float dist = length(ray);
		ray /= dist; 
		
		// attenuation
		float att = 1.0 - saturate(dist / vecLightPos[light].w);
		
		// final light factor
		float strength = vecLightDir[light].w ? spot * att : att;
		
		// diffuse term
		outcolor.rgb += vecLightColor[light].rgb * saturate(-dot(ray, outnormal.xyz)) * strength * vecSkill1.w;
	}
	
	outcolor.rgb = saturate(outcolor.rgb);
	outcolor.rgb = pow(outcolor.rgb, vecSkill1.x); // adjustable brightness
	
	// cut out polygons
	#ifdef POLY_CUTOFF
		float distance = length(mul(inposition, matWorldViewProj));
		if(distance > vecSkill1.z)
		{
			outposition.w = 0;
		}
	#endif
}

float4 PS(
in float4 incolor : COLOR0,
in float4 inworldPos : TEXCOORD0,
in float2 intex : TEXCOORD1,
in float4 inNormal : TEXCOORD2) : COLOR0
{
	float4 lightmap = tex2D(ShadowSampler, intex.xy);
	float Alpha = tex2D(ColorSampler, intex.xy).a;
	float3 textureColor = tex2D(ColorSampler, intex.xy * matEffect1[iRepeat/4][iRepeat%4]).rgb;
	
	incolor += lightmap;
	
	// darken surface on angle
	if(ANGLE_SURFACE_DARKEN > 0)
	{
		float3 vPixelToViewDir = normalize(vecViewPos.xyz - inworldPos.xyz); // *** direction vector from the surface to the camera
		float dot_result = dot(vPixelToViewDir, inNormal.xyz); // *** get the angle ( cos(angle) ) between these vectors; both vectors in the dot product have to be normalized (length = 1)
		incolor.rgb *= saturate(1.0 - (1.0 - dot_result) * ANGLE_SURFACE_DARKEN); // *** apply the darkening factor with adjustable intensity; saturate() to prevent negative numbers (and numbers > 1)
	}
	
	incolor.rgb *= textureColor.rgb;
	
	float fDepth = distance(vecViewPos.xyz, inworldPos.xyz);
	float Fog = saturate((fDepth - vecFog.x) * vecFog.z);
	incolor.rgb = lerp(incolor.rgb, vecFogColor.rgb, Fog);
	
	incolor.a = Alpha;
	
	return incolor;
}

technique terraintex
{
	pass multi_repeat11
	{
		ZWriteEnable = True;
		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		
		VertexShader = compile vs_3_0 VS();
		PixelShader  = compile ps_3_0 PS();
	}
	
}

technique fallback { pass one { } }