// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    // bool hasNormal;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    // float3 tangent : TANGENT;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
    // float3 tangent : TANGENT;
    // bool hasNormal : HASNORMAL;
};

OutputType main(InputType input)
{
    OutputType output;
    
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    // Normalize the normal vector.
    output.normal = normalize(output.normal);
    
    // output.hasNormal = false;
    // if(hasNormal)
    // {
    //     output.hasNormal = true;
    //     
    //     output.tangent = mul(input.normal, (float3x3)worldMatrix);
    //     output.tangent = normalize(output.tangent);
    //     
    //     // Calculate the binormal vector against the world matrix only and then normalize the final value.
    //     output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    //     output.binormal = normalize(output.binormal);
    // }

	// world position of vertex (for point light)
	output.position3D = (float3)mul(input.position, worldMatrix);

    return output;
}