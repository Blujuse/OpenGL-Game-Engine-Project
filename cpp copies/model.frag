#version 410

// Input to fragment shader
in vec2 uv;
in vec3 outNormal;
in vec3 posW;

// From vertex shader
in float fog_amount;

// Fog colour from cpp
uniform vec3 fog_Colour;

// Texture
uniform sampler2D sampler;

// viewPos is the location of the viewer, the camera
uniform vec3 viewPos;

// Final fragment colour
out vec4 fragmentColour;
out vec4 fragmentLight;

// Lighting params
uniform vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
uniform vec3 lightPos = vec3(0.0f, 50.0f, 0.0f);
uniform float shininess = 64.0f;
uniform float specularStrength = 0.2f; // Specular light intensity

// Shader body
void main()
{
	// Determine fragment colour from texture based on uv co-oords
	vec4 texColour = texture(sampler, uv).rgba;

	// Normalise the transformed normal so calculations
	// are performed on unit vectors to determine cosine
	// of the angle and resulting proportion of light
	vec3 normalizedNormal = normalize(outNormal);

	// Diffuse light
	// Determine the direction of the light with respect to pos of vertex
	vec3 lightDirection = normalize(lightPos - posW);
	// Determine the amount of diffuse light from source
	float diffIllum = max(dot(normalizedNormal, lightDirection), 0.0f);
	// Calculate diffuse component of the light
	vec3 diffuse = diffIllum * lightColour;

	// Specular light
	// Specular light respect to camera
	vec3 viewDir = normalize(viewPos - posW);

	// Determine direction of reflected light with light direction and normal
	// in mind
	vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);

	// Calculate the angle between reflect light direction and camera
	// Raised to a power to determine shininess of specular light
	// larger power, shinier specular light
	float spec = pow(max(dot(viewDir, reflectDirection), 0.0f), shininess);

	// Calculate specular amount
	vec3 specular = specularStrength * spec * lightColour;

	// Final colour for frag from ambient, diffuse, and specular
	vec3 finalColour = ambient + diffuse * texColour.rgb + specular;

	fragmentColour = mix(texColour.rgba, vec4(fog_Colour, 1.0), fog_amount);

	// Calculate the final colour, with transparency
	fragmentLight = vec4(finalColour, texColour.a);
}