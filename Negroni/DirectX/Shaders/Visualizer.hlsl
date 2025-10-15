struct VSOutput
{
    float4 Pos : SV_Position;
    float2 tex : TEXCOORD0;
};

// Full-screen triangle vertex shader
VSOutput VisualizeVS(uint vertexID : SV_VertexID)
{
    VSOutput output;
    
    // Generate full-screen triangle
    // Vertex 0: (-1, -1) -> (0, 1) in texture coordinates
    // Vertex 1: (-1,  3) -> (0, -1) in texture coordinates  
    // Vertex 2: ( 3, -1) -> (2, 1) in texture coordinates
    
    float2 texcoord = float2((vertexID << 1) & 2, vertexID & 2);
    output.tex = texcoord;
    output.Pos = float4(texcoord * float2(2, -2) + float2(-1, 1), 0, 1);
    
    return output;
}
