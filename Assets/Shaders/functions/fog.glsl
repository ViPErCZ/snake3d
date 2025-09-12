uniform bool fogEnable = false;

float getFogFactor(float d)
{
    const float FogMax = 9.0;
    const float FogMin = 1.0;

    if (d>=FogMax) return 1.0;
    if (d<=FogMin) return 0.0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}
