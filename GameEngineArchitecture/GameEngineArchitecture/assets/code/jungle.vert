#version 410

// Input attributes
in vec3 vertexPos;
in vec2 vUV;

// Output uvs
out vec2 texCoords;

void main() {
	// Plain passthrough of vertex
	gl_Position = vec4(vertexPos, 1.0f);

	// Plain passthrough of uvs
	texCoords = vUV;
}

