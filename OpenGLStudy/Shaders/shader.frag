#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;

struct Light {
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight {
	Light base;
	
	vec3 direction;
};

struct PointLight {
	Light base;

	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform DirectionalLight directionalLight;

uniform int pointLightCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];


uniform sampler2D tex;
uniform Material material;

uniform vec3 eyePosition; // The camera position


vec4 CalcLightByDirection(Light base, vec3 direction) {
	vec4 ambientColor = vec4(base.color, 1.0f) * base.ambientIntensity;

	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
	vec4 diffuseColor = vec4(base.color, 1.f) * base.diffuseIntensity * diffuseFactor;

	vec4 specularColor = vec4(0.f, 0.f, 0.f, 0.f);

	// Check if it is even hit by diffuse lightning to check if there even is specular
	if (diffuseFactor > 0.0f) {
		vec3 fragToEye = normalize(eyePosition - FragPos); // Vector from fragPos to eyePos
		// First argument of reflect is what we want to reflect, and second what we reflect around. 
		vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));
		// Now we can check if those two vectors are close to the same. The more equal they are the more bright the specular highlight becomes

		float specularFactor = dot(fragToEye, reflectedVertex); // Get cosine of angle between the vectors. (that's why we normalize)
		if (specularFactor > 0) {
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(base.color * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	return (ambientColor + diffuseColor + specularColor);
}

vec4 CalcDirectionalLight() {
	return CalcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 CalcPointLights() {
	vec4 totalColor = vec4(0,0,0,0);

	for(int i = 0; i < pointLightCount; i++) {
		// Calculate direction of point Light
		vec3 direction = FragPos - pointLights[i].position;

		float distance = length(direction);
		direction = normalize(direction);

		vec4 color = CalcLightByDirection(pointLights[i].base, direction); // Until here we basically calculated a directional Light

		float attenuation = pointLights[i].exponent * distance * distance + 
							pointLights[i].linear * distance +
							pointLights[i].constant;

		totalColor += (color / attenuation);
	}

	return totalColor;
}

void main()
{
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();

	color = texture(tex, TexCoord) * finalColor;
}