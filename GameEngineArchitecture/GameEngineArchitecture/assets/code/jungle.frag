#version 410

// Input attributes
in vec2 texCoords;

// Output fragment colour
out vec4 fragColour;

// Original texture to sample from
uniform sampler2D sceneTexture;

uniform bool apply_greyscale;

void main() {
	// Get colour for fragment from texture
	vec4 sceneColour = texture(sceneTexture, texCoords);

	if(apply_greyscale) {
		// Calculate greyscale value
		vec3 warmTint = vec3(1.1, 1.0, 0.9); // add warmth
		vec3 jungleColour = sceneColour.rgb * warmTint;

		// Output fragment in greyscale
		fragColour = vec4(jungleColour, 1.0);
	} else {
	    fragColour = sceneColour;
	}
}

