#version 410

// Inputs to vertex shader
in vec3 vertexPos3D;
in vec3 vertexNormal;
in vec3 vertexTangent;
in vec3 vertexBitangent;
in vec2 vUV;

// Output
out vec2 uv;
out vec3 posW;
out vec3 outNormal;
out mat3 TBNMat;

// Output fog amount
out float fog_amount;

// MVP transformations
uniform mat4 transformMat;
uniform mat4 viewMat;
uniform mat4 projMat;

// Fog uniforms
uniform float fog_Density;

// Shader main code
void main() {
	// Convert input vertex to 4D 
	vec4 v = vec4(vertexPos3D.xyz, 1);
	vec4 posInWorld = transformMat * v;
	v = projMat * viewMat * posInWorld;

	// Calculate where vertex is in view space
	posW = vec3(viewMat * transformMat * v);

	// Transform the normal so it reflects model orientation
	vec3 N = normalize(vec3(transformMat * vec4(vertexNormal, 0.0f)));

	// Transform the tangent so it reflects model orientation
	vec3 T = normalize(vec3(transformMat * vec4(vertexTangent, 0.0f)));

	// Transform the bitangent so it reflects model orientation
	vec3 B = normalize(vec3(transformMat * vec4(vertexBitangent, 0.0f)));

	// Create TBN matrix
	TBNMat = mat3(T, B, N);

	// Pass transformed vertex to next stage
	gl_Position = v;

	// As well as uv.  Don't transform uv
	uv = vUV;

	// Fog Calculations
	vec4 pos_rel_eye = viewMat * posInWorld;
	float distance = length(pos_rel_eye.xyz);

	// Exponential fog
	fog_amount = 1.0 - exp(-pow(distance * fog_Density, 2.0f));

	fog_amount = clamp(fog_amount, 0.0f, 1.0f);
}