#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

struct DirectionalLight {
	vec3 color;
	float ambientIntensity;

	vec3 direction;
	float diffuseIntensity;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform sampler2D tex;
uniform DirectionalLight directionalLight;
uniform Material material;

uniform vec3 eyePosition; // The camera position

void main()
{
	vec4 ambientColor = vec4(directionalLight.color, 1.0f) * directionalLight.ambientIntensity;

	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
	vec4 diffuseColor = vec4(directionalLight.color, 1.f) * directionalLight.diffuseIntensity * diffuseFactor;

	vec4 specularColor = vec4(0.f, 0.f, 0.f, 0.f);

	// Check if it is even hit by diffuse lightning to check if there even is specular
	if (diffuseFactor > 0.0f) {
		vec3 fragToEye = normalize(eyePosition - FragPos); // Vector from fragPos to eyePos
		// First argument of reflect is what we want to reflect, and second what we reflect around. 
		vec3 reflectedVertex = normalize(reflect(directionalLight.direction, normalize(Normal)));
		// Now we can check if those two vectors are close to the same. The more equal they are the more bright the specular highlight becomes

		float specularFactor = dot(fragToEye, reflectedVertex); // Get cosine of angle between the vectors. (that's why we normalize)
		if (specularFactor > 0) {
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(directionalLight.color * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	color = texture(tex, TexCoord) * (ambientColor + diffuseColor + specularColor);
}