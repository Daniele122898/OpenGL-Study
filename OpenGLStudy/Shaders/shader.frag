#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

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

struct SpotLight {
	PointLight base;

	vec3 direction;
	float edge;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform DirectionalLight directionalLight;

uniform int pointLightCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform int spotLightCount;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];


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

vec4 CalcPointLight(PointLight pLight) {
	// Calculate direction of point Light
	vec3 direction = FragPos - pLight.position;

	float distance = length(direction);
	direction = normalize(direction);

	vec4 color = CalcLightByDirection(pLight.base, direction); // Until here we basically calculated a directional Light

	float attenuation = pLight.exponent * distance * distance + 
						pLight.linear * distance +
						pLight.constant;

	return (color / attenuation);
}

vec4 CalcPointLights() {
	vec4 totalColor = vec4(0,0,0,0);

	for(int i = 0; i < pointLightCount; i++) {
		totalColor += CalcPointLight(pointLights[i]);
	}

	return totalColor;
}

vec4 CalcSpotLight(SpotLight spLight){
	vec3 rayDirection = normalize(FragPos - spLight.base.position); // Direction from spotlight to fragment
	float slFactor = dot(rayDirection, spLight.direction); // Get angle from direction of spotlight and rayDirection.

	if (slFactor > spLight.edge) { // The larger the slFactor the more inline it becomes with the ray
		vec4 color = CalcPointLight(spLight.base);
		// Since the processed edge and slFactor might be rly close and thus only give us a range of 
		// 0.98 - 1 we cannot use that difference to create a outwards fade. We must strech those values over 
		// a 0-1 scale to have a better fade
		return color * (1.0f - (1.f - slFactor) * (1.f/(1.f - spLight.edge)));
	} else {
		return vec4(0,0,0,0);
	}
}

vec4 CalcSpotLights() {
	vec4 totalColor = vec4(0,0,0,0);

	for(int i = 0; i < spotLightCount; i++) {
		totalColor += CalcSpotLight(spotLights[i]);
	}

	return totalColor;
}

void main()
{
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
	finalColor += CalcSpotLights();

	color = texture(tex, TexCoord) * finalColor;
}