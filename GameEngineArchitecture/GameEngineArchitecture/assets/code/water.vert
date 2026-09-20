#version 410

// Inputs to vertex shader
in vec3 vertexPos3D;
in vec3 vertexNormal;

in vec2 vUV;

// Output
out vec2 uv;

// Fog amount
out float fog_amount;

// MVP transformations
uniform mat4 transformMat;
uniform mat4 viewMat;
uniform mat4 projMat;

// Fog uniforms
uniform float fog_density;

// Time uniform
uniform float time;

// Shader main code
void main() {
	vec4 v = vec4(vertexPos3D.x, vertexPos3D.y + sin((vertexPos3D.x + time) * 0.75) * 0.25, vertexPos3D.z, 1);
	vec4 posInWorld = transformMat * v;
	v = projMat * viewMat * transformMat * v;

	gl_Position = v;
	uv = vUV;
	
	// Calculate fog amount
	vec4 pos_rel_eye = viewMat * posInWorld;
	float distance = length(pos_rel_eye.xyz);

	// Exponential fog
	fog_amount = 1.0 - exp(-pow(distance * fog_density, 2.0f));
	fog_amount = clamp(fog_amount, 0.0f, 1.0f);
}