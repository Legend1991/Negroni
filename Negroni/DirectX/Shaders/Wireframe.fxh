// Wireframe shader that works with your existing pipeline
// This version expects vertex shader output (transformed vertices)

cbuffer WireframeCBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    float4 wireframeColor;
}

// Input from vertex shader (same as your main pass)
struct VSInput
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

// Vertex shader output / Geometry shader input
struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

// Geometry shader output
struct GSOutput
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 barycentric : BARYCENTRIC;
};

// Vertex shader (reuse your transformation logic)
VSOutput VS(VSInput input)
{
    VSOutput output;
    
    // Transform to world space
    float4 worldPos = mul(float4(input.Pos, 1.0f), world);
    
    // Transform to view space
    float4 viewPos = mul(worldPos, view);
    
    // Transform to clip space
    output.Pos = mul(viewPos, projection);
    output.Tex = input.Tex;
    
    return output;
}

[maxvertexcount(3)]
void GS(triangle VSOutput input[3], inout TriangleStream<GSOutput> triStream)
{
    GSOutput output;
    
    // Vertex 0: (1, 0, 0)
    output.Pos = input[0].Pos;
    output.Tex = input[0].Tex;
    output.barycentric = float3(1.0, 0.0, 0.0);
    triStream.Append(output);
    
    // Vertex 1: (0, 1, 0)
    output.Pos = input[1].Pos;
    output.Tex = input[1].Tex;
    output.barycentric = float3(0.0, 1.0, 0.0);
    triStream.Append(output);
    
    // Vertex 2: (0, 0, 1)
    output.Pos = input[2].Pos;
    output.Tex = input[2].Tex;
    output.barycentric = float3(0.0, 0.0, 1.0);
    triStream.Append(output);
    
    triStream.RestartStrip();
}

float4 PS(GSOutput input) : SV_Target
{
    // Calculate distance to nearest edge
    float3 d = fwidth(input.barycentric);
    float lineWidth = 1.5; // Make lines thicker for visibility
    float3 smoothed = smoothstep(float3(0.0, 0.0, 0.0), d * lineWidth, input.barycentric);
    float minDist = min(smoothed.x, min(smoothed.y, smoothed.z));
    
    // Sharp edge detection
    if (minDist > 0.4) // Lower threshold for thicker lines
        discard;
 
    return wireframeColor;
}