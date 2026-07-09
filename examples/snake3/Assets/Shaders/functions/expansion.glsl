uniform float expansion = 1.0;

vec2 compute_2d_expansion(vec2 pos);

vec2 compute_2d_expansion(vec2 pos) {
    return vec2(pos.x * expansion, pos.y * expansion);
}