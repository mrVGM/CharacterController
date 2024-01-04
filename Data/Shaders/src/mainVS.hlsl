#include "common_buffers_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer ObjectTransform : register(b1)
{
    float4x4 m_transform;
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

    float4 worldPos = float4(vertexInput.position, 1);
    worldPos = mul(m_transform, worldPos);
    float3 worldNormal = vertexInput.normal;

    result.position = mul(m_camBuff.m_matrix, worldPos);

    result.world_position = worldPos;
    result.normal = float4(worldNormal, 1);
    result.uv = vertexInput.uv;

    return result;
}
