// FROM
// https://www.shadertoy.com/view/tlc3DM

// ported to shaderToy by László Matuska / @BitOfGold
// from here: https://github.com/jmickle66666666/PSX-Dither-Shader/blob/master/PSX%20Dither.shader
// uses Shadertoy's 8x8 bayer dithering pattern instead of the original pattern

// Number of colors. 32 (5 bits) per channel
const float3 _Colors = float3(32.0, 32.0, 32.0);

const float4 vecTime;
const float4 vecViewPort;

texture TargetMap;
texture mtlSkin1;

sampler postTex = sampler_state
{
	texture = <TargetMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;  
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler noiseTex = sampler_state
{
	texture = <mtlSkin1>;
	MinFilter = linear; 
	MagFilter = linear; 
	MipFilter = linear; 
	AddressU = Wrap;
	AddressV = Wrap;
};

float channelError(float col, float colMin, float colMax)
{
	float range = abs(colMin - colMax);
	float aRange = abs(col - colMin);
	return aRange /range;
}

float ditheredChannel(float error, float2 ditherBlockUV)
{
	float pattern = tex2D(noiseTex, ditherBlockUV).r;
	if(error > pattern)
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}

float4 mix(float4 a, float4 b, float amt)
{
	return((1.0 - amt) * a) + (b * amt);
}

/// YUV/RGB color space calculations
float3 RGBtoYUV(float3 rgb)
{
	float3 yuv;
	yuv.r = rgb.r * 0.2126 + 0.7152 * rgb.g + 0.0722 * rgb.b;
	yuv.g = (rgb.b - yuv.r) / 1.8556;
	yuv.b = (rgb.r - yuv.r) / 1.5748;

	yuv.gb += 0.5;
	return yuv;
}

float3 YUVtoRGB(float3 yuv)
{
	yuv.gb -= 0.5;
	return float3(
	yuv.r * 1.0 + yuv.g * 0.0 + yuv.b * 1.5748,
	yuv.r * 1.0 + yuv.g * -0.187324 + yuv.b * -0.468124,
	yuv.r * 1.0 + yuv.g * 1.8556 + yuv.b * 0.0);
}

float3 ditherColor(float3 col, float2 uv, float xres, float yres)
{
	float3 yuv = RGBtoYUV(col);

	float3 col1 = floor(yuv * _Colors) / _Colors;
	float3 col2 = ceil(yuv * _Colors) / _Colors;
	
	// Calculate dither texture UV based on the input texture
	float2 ditherBlockUV = uv * float2(xres / 16.0, yres / 16.0);
	
	yuv.x = mix(col1.x, col2.x, ditheredChannel(channelError(yuv.x, col1.x, col2.x), ditherBlockUV));
	yuv.y = mix(col1.y, col2.y, ditheredChannel(channelError(yuv.y, col1.y, col2.y), ditherBlockUV));
	yuv.z = mix(col1.z, col2.z, ditheredChannel(channelError(yuv.z, col1.z, col2.z), ditherBlockUV));
	
	return(YUVtoRGB(yuv));
}

float3 getPixel(float2 uv)
{
	return tex2D(postTex, uv).rgb;
}

float4 FP(float2 fragCoord: VPOS) : COLOR
{
	// Normalized pixel coordinates (from 0 to 1)
	float2 uv = fragCoord / vecViewPort.xy;
	
	// Time varying pixel color
	float3 col = getPixel(uv);
	col = ditherColor(col, uv, vecViewPort.x, vecViewPort.y);
	
	float4 fragColor = float4(col, 1.0);
	return fragColor;
}

technique psx
{
	pass one
	{
		PixelShader = compile ps_3_0 FP();
	}
}