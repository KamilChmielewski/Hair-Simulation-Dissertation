#pragma once
//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register(t0);

SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

struct SurfaceInfo
{
    float4 AmbientMtrl;
    float4 DiffuseMtrl;
    float4 SpecularMtrl;
};

struct Light
{
    float4 AmbientLight;
    float4 DiffuseLight;
    float4 SpecularLight;

    float SpecularPower;
    float3 LightVecW;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    SurfaceInfo surface;
    Light light;

    float3 EyePosW;
    float HasTexture;
}

struct VS_INPUT
{
    float4 PosL : POSITION;
    float3 NormL : NORMAL;
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float3 NormW : NORMAL;

    float3 PosW : POSITION;
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    float4 posW = mul(input.PosL, World);
    output.PosW = posW.xyz;

    output.PosH = mul(posW, View);
    output.PosH = mul(output.PosH, Projection);
    output.Tex = input.Tex;

    float3 normalW = mul(float4(input.NormL, 0.0f), World).xyz;
    output.NormW = normalize(normalW);

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 finalColour = float4(255, 00, 49, 1.0f);

    return finalColour;
}