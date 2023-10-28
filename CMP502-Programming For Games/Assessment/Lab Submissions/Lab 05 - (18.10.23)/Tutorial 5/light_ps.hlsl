// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture1 : register(t0);
Texture2D shaderTexture2 : register(t1);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
	float4	textureColor;
	float4	textureColor1;
	float4	textureColor2;
	float4	textureBlend;
    float3	lightDir;
    float	lightIntensity;
    float4	color;

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
	color = saturate(color);

	// TASK 3 : streaked / stretched appearance of over half the model
	//input.tex += float2(0.5f, 0.5f);
	//input.tex *= 10;

	// TASK 5 : INVERT COLOURS
	//color = float4(1 - color.x, 1 - color.y, 1 - color.z, 1);

	// TASK 6 :  DIFFERENT TEXTURES ON EACH MODEL
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	//textureColor = shaderTexture1.Sample(SampleType, input.tex);
	//color = color * textureColor;

	// TASK 7 : BLENDING 2 TEXTURES
	textureColor1 = shaderTexture1.Sample(SampleType, input.tex);
	textureColor2 = shaderTexture2.Sample(SampleType, input.tex);
	textureBlend = lerp(textureColor1.rgba, textureColor2.rgba, 0.5f);
	
	// BLEND TEXTURES
	color = color * textureBlend;
	
	// UNBLENDED TEXTURES
	//color = color * textureColor1;

    return color;
}