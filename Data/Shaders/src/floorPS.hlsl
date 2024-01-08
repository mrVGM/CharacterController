float4 PSMain(
    float4 position : SV_POSITION,
    float4 worldPosition : WORLD_POSITION,
    float4 normal : NORMAL,
    float2 uv : UV) : SV_Target
{
    float4 color1 = float4(0, 0, 0.3, 1);
    float4 color2 = float4(0.2, 0, 0.1, 1);
    
    float2 tmp = uv / 0.01;
    int2 index = floor(tmp);
    
    float4 color = (index.x + index.y) % 2 == 0 ? color1 : color2;
    
    return color;
}
