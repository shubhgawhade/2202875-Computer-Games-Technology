// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture : register(t0);
// Texture2D normalTexture : register(t1);
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
	// float3 tangent : TANGENT;
	// bool hasNormal : HASNORMAL;
};

float4 main(InputType input) : SV_TARGET
{
	float4	textureColor;
    float3	lightDir;
    float	lightIntensity;
    float4	color;
	// float4	normal = float4(0,0,0,1);

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// NORMAL MAP
	// if(input.hasNormal)
	// {
	// 	//Load normal from normal map
	// 	float4 normalMap = normalTexture.Sample( SampleType, input.tex );
	// 	
	// 	//Change normal map range from [0, 1] to [-1, 1]
	// 	normalMap = (2.0f*normalMap) - 1.0f;
	// 	
	// 	//Make sure tangent is completely orthogonal to normal
	// 	input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);
	// 	
	// 	//Create the biTangent
	// 	float3 biTangent = cross(input.normal, input.tangent);
	// 	
	// 	//Create the "Texture Space"
	// 	float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);
	// 	
	// 	//Convert normal from normal map to texture space and store in input.normal
	// 	input.normal = normalize(mul(normalMap, texSpace));
	// 	normal = dot(lightDir, input.normal);
	// 	
	// 	color += normal;
	// 	
	// 	color.r=1;
	// 	return color;
	// }
	
	color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
	color = saturate(color);


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// If pixel has alpha
	if(textureColor.a < 1)
	{
		float averagecol = 1. - (textureColor.r + textureColor.g + textureColor.b)/3.;
		float whitetoalpha = (averagecol) + textureColor.a;
		color = color * averagecol;
		color.a = whitetoalpha;
		// color.r = 1;
		return color;
	}
	color = color * textureColor;
	// color.a = 1;

    return color;
}

