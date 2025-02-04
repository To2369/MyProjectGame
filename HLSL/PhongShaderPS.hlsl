#include "PhongShader.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
Texture2D color_map : register(t0);
SamplerState color_sampler_states[3] : register(s0);
Texture2D normal_map : register(t1);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse_color = color_map.Sample(color_sampler_states[ANISOTROPIC], pin.texcoord);

    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
    float3 L = normalize(directional_light_direction.xyz);
    float3x3 mat = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
    float3 N = normal_map.Sample(color_sampler_states[ANISOTROPIC], pin.texcoord).rgb;

	//ノーマルテクスチャ法線をワールドへ変換
    N = normalize(mul(N * 2.0f - 1.0f, mat));

    float3 ambient = ambient_color.rgb * ka.rgb;
    ambient += CalcHemiSphereLight(N, float3(0, 1, 0), sky_color.rgb, ground_color.rgb, hemisphere_weight);

	//	float3 directional_diffuse = CalcLambert(N, L, directional_light_color.rgb, kd.rgb);
    float3 directional_diffuse = ClacHalfLambert(N, L, directional_light_color.rgb, kd.rgb);
    float3 directional_specular = CalcPhongSpecular(N, L, E, directional_light_color.rgb, ks.rgb);
	
	// 点光源の処理
    float3 point_diffuse = 0;
    float3 point_specular = 0;
    for (int i = 0; i < 8; ++i)
    {
        float3 LP = pin.world_position.xyz - point_light[i].position.xyz;
        float len = length(LP);
        if (len >= point_light[i].range)
            continue;
        float attenuateLength = saturate(1.0f - len / point_light[i].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        point_diffuse += CalcLambert(N, LP, point_light[i].color.rgb, kd.rgb) * attenuation;
        point_specular += CalcPhongSpecular(N, LP, E, point_light[i].color.rgb, ks.rgb) * attenuation;
    }

	// スポットライトの処理
    float3 spot_diffuse = 0;
    float3 spot_specular = 0;
    for (int j = 0; j < 8; ++j)
    {
        float3 LP = pin.world_position.xyz - spot_light[j].position.xyz;
        float len = length(LP);
        if (len >= spot_light[j].range)
            continue;
        float attenuateLength = saturate(1.0f - len / spot_light[j].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        float3 spotDirection = normalize(spot_light[j].direction.xyz);
        float angle = dot(spotDirection, LP);
        float area = spot_light[j].innerCorn - spot_light[j].outerCorn;
        attenuation *= saturate(1.0f - (spot_light[j].innerCorn - angle) / area);
        spot_diffuse += CalcLambert(N, LP, spot_light[j].color.rgb, kd.rgb) * attenuation;
        spot_specular += CalcPhongSpecular(N, LP, E, spot_light[j].color.rgb, ks.rgb) * attenuation;
    }
    float4 color = float4(ambient, diffuse_color.a);
    color.rgb += diffuse_color.rgb * (directional_diffuse + point_diffuse + spot_diffuse);
    color.rgb += directional_specular + spot_specular + point_specular;

	/*float3 rim_color = CalcRimLight(N, E, L, directional_light_color.rgb);
	float4 color = float4(ambient, diffuse_color.a);
	color.rgb += diffuse_color.rgb * directional_diffuse;
	color.rgb += directional_specular;
	color.rgb += rim_color;*/

    color = CalcFog(color, fog_color, fog_range.xy, length(pin.world_position.xyz - camera_position.xyz));
    return color;
}