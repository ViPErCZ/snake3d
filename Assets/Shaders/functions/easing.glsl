uniform float animProgress;

float easeOutBack(float x);
float easeOutElastic(float x);


// --- EASING FUNKCE ---
// BackOut: Prestreli cil a vrati se zpet. C1 urcuje velikost prestreleni.
float easeOutBack(float x) {
    const float c1 = 1.70158;
    const float c3 = c1 + 1.0;
    return 1.0 + c3 * pow(x - 1.0, 3.0) + c1 * pow(x - 1.0, 2.0);
}

// Nebo ElasticOut (jeste pruznejsi, jako zele)
float easeOutElastic(float x) {
    const float c4 = (2.0 * 3.14159) / 3.0;
    return x == 0.0 ? 0.0 : x == 1.0 ? 1.0 : pow(2.0, -10.0 * x) * sin((x * 10.0 - 0.75) * c4) + 1.0;
}

float easingScale = easeOutBack(animProgress);