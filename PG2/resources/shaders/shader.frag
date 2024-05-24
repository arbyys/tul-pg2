#version 460 core

// 99 = dočasné vypnutí světel, poté nastavit na odpovídající hodnotu a odstranit v main()

// slunce
#define MAX_DIRECTIONAL_LIGHTS 99
// lampa vedle stolu 
#define MAX_SPOT_LIGHTS 99
// židle
#define MAX_POINT_LIGHTS 99


// [IN] Atributy vertexu;  VS -> FS
in vec3 oPos;
in vec3 oNormal;
in vec2 oTextureCoords;

// [GLOBAL] Hodnoty z aplikace;  Globálně dostupné ve všech shaderech
uniform vec3 uCameraPos;
uniform float uAmbientAlpha;
uniform float uDiffuseAlpha;

// [OUT] Vypočítaná barva pixelu;  FS -> App
out vec4 result;


// struktura pro materiál
struct Material 
{
    sampler2D texture;
    vec3 ambient;
    vec3 specular;
    float shininess;
};
uniform Material uMaterial;


// struktury pro světla
struct DirectionalLight
{
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
};
uniform DirectionalLight uDirectionalLights[MAX_DIRECTIONAL_LIGHTS];

struct PointLight
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float exponent;
};
uniform PointLight uPointLights[MAX_POINT_LIGHTS];

struct SpotLight
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	float cosInnerCone;
	float cosOuterCone;

	float constant;
	float linear;
	float exponent;
};
uniform SpotLight uSpotLights[MAX_SPOT_LIGHTS];


// metody pro výpočet světel
vec4 getAmbientLight() {
    return vec4(uMaterial.ambient, uAmbientAlpha) * texture(uMaterial.texture, oTextureCoords);
}

vec4 getDirectionalLight(DirectionalLight light, vec3 normal, vec3 fragToCam)
{
	vec3 fragToLight = normalize(-light.direction);
    vec4 diffuse = vec4(light.diffuse * max(dot(normal, fragToLight), 0.0f), uDiffuseAlpha) * texture(uMaterial.texture, oTextureCoords);
	vec3 specular = light.specular * uMaterial.specular * pow(max(dot(normal, normalize(fragToLight + fragToCam)), 0.0f), uMaterial.shininess);

	return (diffuse + vec4(specular, 0.0f));
}

vec4 getPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 fragToCam)
{
	vec3 fragToLight = normalize(light.position - fragPos);
    vec4 diffuse = vec4(light.diffuse * max(dot(normal, fragToLight), 0.0f), uDiffuseAlpha) * texture(uMaterial.texture, oTextureCoords);
	vec3 specular = light.specular * uMaterial.specular * pow(max(dot(normal, normalize(fragToLight + fragToCam)), 0.0f), uMaterial.shininess);
	float d = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * d + light.exponent * (d * d));

	specular *= attenuation;
	diffuse *= attenuation;

	return (diffuse + vec4(specular, 0.0f));
}

vec4 getSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 fragToCam)
{
	vec3 fragToLight = normalize(light.position - fragPos);
    vec4 diffuse = vec4(light.diffuse * max(dot(normal, fragToLight), 0.0f), uDiffuseAlpha) * texture(uMaterial.texture, oTextureCoords);
	vec3 specular = light.specular * uMaterial.specular * pow(max(dot(normal, normalize(fragToLight + fragToCam)), 0.0f), uMaterial.shininess);
	float d = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * d + light.exponent * (d * d));
	float intensity = smoothstep(light.cosOuterCone, light.cosInnerCone, dot(-fragToLight, normalize(light.direction)));

	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (diffuse + vec4(specular, 0.0f));
}


void main()
{

	vec3 normal = normalize(oNormal);
	vec3 fragToCam = normalize(uCameraPos - oPos);
	vec4 outColor = vec4(0.0f);

	// výpočet Directional světel
	if (MAX_DIRECTIONAL_LIGHTS != 99) {
		for (int i = 0; i < MAX_DIRECTIONAL_LIGHTS; i++) {
			outColor += getDirectionalLight(uDirectionalLights[i], normal, fragToCam);  
		}
	}

	// výpočet Point světel
	if (MAX_POINT_LIGHTS != 99) {
		for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
			outColor += getPointLight(uPointLights[i], normal, oPos, fragToCam);  
		}
	}

	// výpočet Spotlight světel
	if (MAX_SPOT_LIGHTS != 99) {
		for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {
			outColor += getSpotLight(uSpotLights[i], normal, oPos, fragToCam);  
		}
	}

	// výpočet Ambient světla (nakonec)
	outColor += getAmbientLight();

	result = outColor;

}
