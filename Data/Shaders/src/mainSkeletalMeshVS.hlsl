#include "common_buffers_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

struct VertexWeights
{
    int     m_jointIndex[8];
    float   m_jointWeight[8];
};

StructuredBuffer<VertexWeights> v_weights : register(t0);
StructuredBuffer<float4x4> v_bindShape  : register(t1);
StructuredBuffer<float4x4> v_pose       : register(t2);

struct PSInput
{
    float4 position         : SV_POSITION;
    float4 world_position   : WORLD_POSITION;
    float4 normal           : NORMAL;
    float2 uv               : UV;
};

PSInput VSMain(SkMVertexInput3D vertexInput)
{
    PSInput result;

    float4 vertexPos = float4(vertexInput.position, 0);
    float4 worldPos = float4(0, 0, 0, 0);
    {
        VertexWeights weights = v_weights[vertexInput.weightsIndex];
        for (int i = 0; i < 8; ++i)
        {
            int jointIndex = weights.m_jointIndex[i];
            float weight = weights.m_jointWeight[i];
            if (jointIndex < 0)
            {
                continue;
            }
            
            worldPos += weight * mul(v_pose[jointIndex], mul(v_bindShape[jointIndex + 1], mul(v_bindShape[0], vertexPos)));
        }
    }
    
    worldPos.w = 1;
    float3 worldNormal = vertexInput.normal;
    
    result.position = mul(m_camBuff.m_matrix, worldPos);
    result.world_position = worldPos;
    result.normal = float4(worldNormal, 1);
    result.uv = vertexInput.uv;

    return result;
}
