
// comment this to disable certain effects
#define VERTEX_SNAPPING
#define AFFINE_TEX_MAPPING
#define POLY_CUTOFF

// set to zero to disable effect
#define ANGLE_SURFACE_DARKEN 0.25 // less number - less darken

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
float4 vecSkill41;

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
	
	outnormal = normalize(mul(innormal, (float3x3)matWorld));
	outtex.xy = intex1.xy;
	outtex.zw = intex2.xy;
	outtex.x /= vecSkill41.x;
	outtex.y /= vecSkill41.y;
	outtex.x += vecSkill41.z;
	outtex.y += vecSkill41.w;
	outworldPos = mul(inposition, matWorld);
	
	// vertex lightning
	outcolor = (vecAmbient * vecLight) + float4(vecEmissive.xyz * vecColor.xyz, vecLight.w);
	
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
float4 color : COLOR0,
float3 normal : TEXCOORD0,
float4 tex : TEXCOORD1,
float4 worldPos : TEXCOORD2) : COLOR0
{
	float4 textureColor = tex2D ( ColorSampler, tex.xy );
	
	// darken surface on angle
	if(ANGLE_SURFACE_DARKEN > 0)
	{
		float3 vPixelToViewDir = normalize(vecViewPos.xyz - worldPos.xyz); // *** direction vector from the surface to the camera
		float dot_result = dot(vPixelToViewDir, normal.xyz); // *** get the angle ( cos(angle) ) between these vectors; both vectors in the dot product have to be normalized (length = 1)
		color.rgb *= saturate(1.0 - (1.0 - dot_result) * ANGLE_SURFACE_DARKEN); // *** apply the darkening factor with adjustable intensity; saturate() to prevent negative numbers (and numbers > 1)
	}
	
	color.rgb *= textureColor.rgb;
	
	float fDepth = distance(vecViewPos.xyz, worldPos.xyz);
	float Fog = saturate((fDepth - vecFog.x) * vecFog.z);
	color.rgb = lerp(color.rgb, vecFogColor, Fog);
	
	color.a = min(vecLight.w, textureColor.a);
	
	return color;
}

technique
{
	pass pass0
	{
		ZWriteEnable = True;
		AlphaBlendEnable = True;
		AlphaTestEnable = True;
		AlphaFunc = Greater;
		
		VertexShader = compile vs_3_0 VS(); 
		PixelShader  = compile ps_3_0 PS(); 
	}
}

technique fallback { pass one { } }