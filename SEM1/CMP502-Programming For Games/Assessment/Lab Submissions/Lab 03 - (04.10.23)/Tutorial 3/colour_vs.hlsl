// colour vertex shader
// Simple geometry pass


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
	float4 screenPosition : SCREEN_POSITION;
};

OutputType main(InputType input)
{
	OutputType output;
	
	// Change the position vector to be 4 units for proper matrix calculations.
	//input.position.w = 0.2f;

	// TASK 2
	input.position.x = input.position.x * 5;
	input.position.y = input.position.y * 5;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	//input.position.z = 4;
	//input.position.x += 1;
	
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.screenPosition = output.position;
	output.colour = input.colour;

	return output;
}