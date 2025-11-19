#version 330 core

in vec2 TexCoords;
in vec3 meshColor;
in vec3 Normal;
in vec3 camPos;
in vec3 fragPos;

uniform sampler2D u_NoiseTexture;
uniform vec4 u_LightColor;
uniform float u_Speed;
uniform float u_FloatParameter;
uniform float u_Time;
uniform float u_Delay;

out vec4 FragColor;

#include "../functions/lights.glsl"

void main()
{
    // 1. Získání hodnoty šumu
    vec4 noiseSample = texture(u_NoiseTexture, TexCoords);
    float noiseR = noiseSample.r;
    float pi = 3.14159;

    // 2. Výpočet pulzující hodnoty animace (0 -> 1)
    // Čas animace s delay
    // Délka animace v sekundách, můžeš ji např. počítat jako
    float duration = pi / u_Speed; // π / u_Speed, tj. půlvlnu sin

    // Čas animace s delay a ořezem na max duration
    float animTime = clamp(max(u_Time - u_Delay, 0.0), 0.0, duration);

    // Výpočet fáze s posunem -π/2, aby začínala od 0
    float t = animTime / duration * pi; // 0 až π
    float remapped = (sin(t - (pi / 2)) + 1.0) * 0.5;

    // =================================================================
    // Pokud je hodnota šumu menší než práh animace, pixel se nezobrazí.
    // =================================================================
    if (noiseR > remapped) {
        discard;
    }

    // 3. Výpočet prahu pro zářící okraj
    float edgeThreshold = remapped + u_FloatParameter;
    float stepValue = step(noiseR, edgeThreshold);

    // =================================================================
    // Místo `vec3(stepValue)` (což dává bílou), použijeme `stepValue`
    // jako násobič pro u_LightColor.
    // =================================================================
    vec3 emission = stepValue * u_LightColor.rgb;

    // 4. Finální barva
    // Sečteme základní barvu a barvu emise.
    // Alfa kanál vezmeme z textury šumu, aby okraj byl jemnější.
    vec3 finalColor = mix(meshColor.rgb, emission, stepValue);
    vec3 viewDir = normalize(camPos - fragPos);

    finalColor = CalcDirLight(dirLight, Normal, viewDir, emission, 0.0);
    finalColor /= 1;

    FragColor = vec4(pow(finalColor, vec3(1.0/2.2)), 1.0);
}