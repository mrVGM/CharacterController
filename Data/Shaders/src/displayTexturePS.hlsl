Texture2D p_texture : register(t0);
SamplerState p_sampler : register(s0);

float4 PSMain(
    float4 position : SV_POSITION,
    float2 uv : UV) : SV_Target
{
    float2 coords = float2(uv.x, 1 - uv.y);
    float4 color = p_texture.Sample(p_sampler, coords);
    return color;
}
