#include "Mask.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

Texture2D maskTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord) * pin.color;

	// �}�X�N�摜����ԐF���擾
    float mask = maskTexture.Sample(sampler0, pin.texcoord).r;

	// color�̓��ߒl�ɏ�Z����
    color.a *= mask;

    return color;
}
