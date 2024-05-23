#version 460 core

// [IN] Atributy vertexu;  GPU_memory -> VS
layout (location = 0) in vec4 aPos;           // pozice vertexu v modelu
layout (location = 1) in vec3 aNormal;        // normálový vektor vertexu
layout (location = 2) in vec2 aTextureCoords; // UV souřadnice textury

// [GLOBAL] Matice pro transformace souřadnic vertexů;  Globálně dostupné ve všech shaderech
uniform mat4 uMxModel;      // matice pro: převod z modelu do herní scény
uniform mat4 uMxView;       // matice pro: převod z herní scény do kamery
uniform mat4 uMxProjection; // matice pro: převod z kamery na obrazovku

// [OUT] Atributy vertexu;  VS -> FS
out vec3 oPos;           // výsledná pozice vertexu
out vec3 oNormal;        // normálový vektor vertexu
out vec2 oTextureCoords; // UV souřadnice textury


void main()
{
    // nastavení OUT proměnných
    oPos = vec3(uMxModel * aPos); // aplikování Model matice
    oNormal = mat3(transpose(inverse(uMxModel))) * aNormal; // výpočet normálového vektoru
    oTextureCoords = aTextureCoords; // přenesení textury beze změny

    // výsledné aplikování všech matic
    gl_Position = uMxProjection * uMxView * uMxModel * aPos;
}
