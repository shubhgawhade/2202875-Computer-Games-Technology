// colour vertex shader
// Simple geometry pass
// texture coordinates and normals will be ignored.

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct InputType
{
	float4 position : POSITION;
	float4 colour : COLOR;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

OutputType main(InputType input)
{
	OutputType output;
	
	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	//output.tex = input.tex;

	// Store normals for the pixel shader
	//output.normal = mul(input.normal, (float3x3)worldMatrix);
	//output.normal = normalize(output.normal);

	output.colour = input.colour;

	return output;
}