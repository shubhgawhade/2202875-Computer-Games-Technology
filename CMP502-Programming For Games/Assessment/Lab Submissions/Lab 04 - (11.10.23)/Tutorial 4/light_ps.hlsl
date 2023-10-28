// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

//cbuffer LightBuffer : register(b0)
//{
//    float4 diffuseColor;
//    float3 lightDirection;
//    float padding;
//};

cbuffer LightBuffer : register (b0)
{
    float4 	ambientColor;  //new line
    float4  diffuseColor;
    float3 	lightDirection;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    float3	lightDir;
    float	lightIntensity;
    float4	color;
	
    // TASK 3
    // Set the default output color to the ambient light value for all pixels.
    color = ambientColor;

    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));

    if (lightIntensity > 0.0f)
    {
        color += (diffuseColor * lightIntensity);
    }


    return color;
}

