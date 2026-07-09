//#version 330 core
//out vec4 FragColor;
//in vec2 uv;
//
//uniform float iTime;
//uniform vec2 iResolution;
//
//void main() {
//    vec2 uv_fixed = uv;
//    uv_fixed.x *= iResolution.x / iResolution.y;
//
//    vec3 finalColor = vec3(0.0);
//
//    for (float i = 0.0; i < 4.0; i++) {
//        vec2 uv_loop = uv_fixed;
//
//        // fract(uv * 1.5) - 0.5;
//        uv_loop = fract(uv_loop * 1.5) - 0.5;
//
//        float d = length(uv_loop) * exp(-length(uv_fixed));
//
//        vec3 col = 0.5 + 0.5 * cos(iTime + uv_fixed.xyx + vec3(0,2,4));
//
//        d = sin(d * 8.0 + iTime) / 8.0;
//        d = abs(d);
//
//        d = pow(0.01 / d, 1.2);
//
//        finalColor += col * d;
//    }
//
//    FragColor = vec4(finalColor, 1.0);
//}

// #version 330 core
// out vec4 FragColor;
// in vec2 uv;
//
// uniform float iTime;
// uniform vec2 iResolution;
//
// vec3 palette(float t) {
//     vec3 a = vec3(0.5, 0.5, 0.5);
//     vec3 b = vec3(0.5, 0.5, 0.5);
//     vec3 c = vec3(1.0, 1.0, 1.0);
//     vec3 d = vec3(0.263, 0.416, 0.557);
//     return a + b * cos(6.28318 * (c * t + d));
// }
//
// void main() {
//     vec2 p = uv;
//     p.x *= iResolution.x / iResolution.y;
//
//     vec2 p0 = p;
//     vec3 finalColor = vec3(0.0);
//
//     for (float i = 0.0; i < 4.0; i++) {
//         p = fract(p * 1.5) - 0.5;
//
//         float d = length(p) * exp(-length(p0));
//
//         vec3 col = palette(length(p0) + i * 0.4 + iTime * 0.4);
//
//         d = sin(d * 8.0 + iTime) / 8.0;
//         d = abs(d);
//
//         d = pow(0.01 / d, 1.2);
//
//         finalColor += col * d;
//     }
//
//     FragColor = vec4(finalColor, 1.0);
// }

#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;

const float DOTS = 8.0;
const vec3 COLOR = vec3(0.3, 0.6, 1.0);

void main() {
   vec2 p = uv;
   p.x *= iResolution.x / iResolution.y;

   float f = 0.0;

   for(float i = 1.0; i <= DOTS; i++)
   {
       // Pohyb koulí
       float s = sin(0.7 * iTime + (i * 0.5) * iTime) * 0.2;
       float c = cos(0.2 * iTime + (i * 0.5) * iTime) * 0.2;

       // p*0.5 zmenšuje měřítko pohybu, aby koule nelétaly mimo obraz
       f += 0.01 / abs(length(p * 0.5 + vec2(c, s)));
   }

   // Finální barva
   FragColor = vec4(COLOR * f, 1.0);
}