#include "Sprite.hlsli"
float4 main(VS_OUT pin) : SV_TARGET
{
    const float2 center = float2(1280 / 2, 720 / 2);
    float distance = length(center - pin.position.xy);
    if (distance > 200)
        return 1;
    else
        return float4(1, 0, 0, 1);
}