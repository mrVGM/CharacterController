#include "common_buffers_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

struct PSInput
{
    float4 position         : SV_POSITION;
    float4 world_position   : WORLD_POSITION;
    float4 normal           : NORMAL;
    float2 uv               : UV;
};

PSInput VSMain(VertexInput3D vertexInput)
{
    PSInput result;

    float3 worldPos = vertexInput.position;
    float3 worldNormal = vertexInput.normal;

    result.position = float4(0.3 * worldPos.xy, 0, 1);
    //result.position = mul(m_camBuff.m_matrix, float4(worldPos, 1));

    result.world_position = float4(worldPos, 1);
    result.normal = float4(worldNormal, 1);
    result.uv = vertexInput.uv;

    return result;
}
