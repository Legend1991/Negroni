//--------------------------------------------------------------------------------------
// File: Tutorial05.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 TintColor;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float4 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD1;
    float4 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Norm = mul(float4(input.Norm, 0), World).xyz;
    output.Tex = input.Tex;
    
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{  
    float4 output = TintColor;
    
    //--------------------------------------------------------------------------
    // Edge dimming. Does not work well for anything except cubes or planes
    //--------------------------------------------------------------------------
    
    if (input.Tex.x < 0.03f || input.Tex.x > 0.97f || input.Tex.y < 0.03f || input.Tex.y > 0.97f)
    {
        output *= float4(0.8f, 0.8f, 0.8f, 1.0f);
    }
    
    //--------------------------------------------------------------------------
    // Directional light (also called a parallel light)
    //--------------------------------------------------------------------------
    
    float3 lightDirection = float3(0.3f, -1.0f, 0.2f);
    float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float lightIntensity = 1.0;
    
    float softness = 0.2; // 0.0 to 1.0 - adjust this value

    float3 normal = normalize(input.Norm);
    float3 lightDir = normalize(-lightDirection);

    float cosAngle = dot(normal, lightDir);
    // Calculate threshold based on softness
    // softness 0.0 -> threshold =  1.0 (no remapping, normal lighting)
    // softness 1.0 -> threshold = -1.0 (entire range is remapped)
    float threshold = 1.0 - (softness * 2.0);
    // Calculate minimum brightness (darkest surfaces)
    // softness 0.0 -> minBrightness = 0.0 (fully dark)
    // softness 1.0 -> minBrightness = 1.0 (no shadows)
    float minBrightness = softness;

    float remappedDiffuse = 1.0; // Fully lit
    if (cosAngle < threshold)
    {
        // Map [threshold to -1.0] -> [1.0 to minBrightness]
        float t = (cosAngle - (-1.0)) / (threshold - (-1.0));
        remappedDiffuse = lerp(minBrightness, 1.0, t);
    }

    float3 lighting = remappedDiffuse * lightColor.rgb * lightIntensity;
    float3 ambient = float3(0.0, 0.0, 0.0);

    return float4((ambient + lighting) * output.rgb, 1.0);
}
