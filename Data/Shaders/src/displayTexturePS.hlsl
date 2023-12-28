Texture2D p_texture : register(t0);
SamplerState p_sampler : register(s0);

float4 PSMain(
    float4 position : SV_POSITION,
    float2 uv : UV) : SV_Target
{
    float4 color = p_texture.Sample(p_sampler, uv);
    return color;
}
