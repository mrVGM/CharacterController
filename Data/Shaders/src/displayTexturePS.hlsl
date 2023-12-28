float4 PSMain(
    float4 position : SV_POSITION,
    float4 worldPosition : WORLD_POSITION,
    float4 normal : NORMAL,
    float2 uv : UV) : SV_Target
{
    return float4(uv, 0, 1);
}
