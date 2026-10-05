cbuffer cbPerFrame : register(c0)
{
    matrix WorldViewProj : register(c0);
};
    cbuffer cbPerFrame2 : register(c4)
{
    matrix WorldMatrix : register(c4);
}; 
cbuffer cbPerFrame3 : register(c8)
{
    matrix ProjectionMatrix : register(c8);
};
    cbuffer cbPerFrame3 : register(c12)
{
    matrix ReflectionMatrix : register(c12);
};
cbuffer cbPerFrame4 : register(c16)
{
    float4 TimeConst : register(c16);
};

 cbuffer cbPerFrame5 : register(c20)
{
    float4 _WorldSpaceCameraPos : register(c20);
};
cbuffer cbPerFrame5 : register(c24)
{
    matrix View : register(c24);
};

struct VS_Input
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};
 
struct VS_Output
{
    float4 TPos : POSITION;
    float2 uv : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
  //  float4 _WorldSpaceCameraPos : TEXCOORD2;
  //  float4 worldPos : TEXCOORD3;
    float4 refractionPosition : TEXCOORD4;
    float WaveHeight : TEXCOORD5;
    //float CameraDistance : TEXCOORD6;
    float4 ViewSpace : TEXCOORD7;
    float2 DistortUV : TEXCOORD8;
    float3 ToCamera : TEXCOORD9;
};
void WaveDown(inout float pix, in float WaveHeight, in float4 view_space)
{
    float FogStart = 400;
    float FogEnd = 140;
    float dist = length(view_space);
    float fog_factor = (FogEnd - dist) / (FogEnd - FogStart);
    fog_factor = clamp(fog_factor, 0, 1);
    if (WaveHeight > 0 && fog_factor > 0)
        pix += WaveHeight * -fog_factor;
}

float2 pseudo_rand(in float2 uv)
{
    float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
    float noiseY = sqrt(1 - noiseX * noiseX);
    return float2(noiseX, noiseY);
}
VS_Output main(VS_Input vin)
{
    VS_Output vout;
    float2 noise = pseudo_rand(float2(vin.pos.x, vin.pos.z));
    float HeightWave = (sin(vin.pos.x * noise.x + TimeConst.a / 15) + cos(vin.pos.z * noise.y + TimeConst.a / 15)) * 15;
   
    vout.ViewSpace = mul(float4(vin.pos.xyz, 1), WorldViewProj);
    if (HeightWave > 0.2)
        vin.pos.xz += HeightWave / 2;
    
    //vin.pos.y += max(0.01, HeightWave);
    WaveDown(vin.pos.y, HeightWave, vout.ViewSpace);

    vout.WaveHeight = 0;
    float2 uv = float2(vin.pos.x / 640, vin.pos.z / 640);
#ifdef FLIP_TEXTURE_Y
	vout.UV = float2(uv.x, (1.0 - uv.y));
#else /* !FLIP_TEXTURE_Y */
    vout.uv = uv;
#endif /* !FLIP_TEXTURE_Y */
   // vout.pos = mul(float4(vin.pos.x, vin.pos.y + HeightWave, vin.pos.z, 1.f), WorldViewProj);
    vout.TPos = mul(float4(vin.pos.xyz, 1), WorldViewProj);
    vout.DistortUV = float2(vin.pos.x / 320, vin.pos.z / 320);

    vout.ToCamera = mul(float4(_WorldSpaceCameraPos.xyz - vin.pos.xyz, 1), WorldViewProj);

//#ifdef LIGHTING
//	vout.FromLight = vin.Position.xyz - LightPos;
//#endif
    
    //	vout.ViewSpace = mul(float4(vin.pos.xyz, 1), View);
     // Transformed view space position. This output is needed for the rasterizer, else it culls the whole vertex out
    //vout.TPos = mul(float4(vin.pos.xyz, 1), ViewProj); // Transformed view space position. This output is needed for the rasterizer, else it culls the whole vertex out
    matrix viewProjectWorld;
    viewProjectWorld = mul(WorldViewProj, ProjectionMatrix);
    viewProjectWorld = mul(WorldMatrix, viewProjectWorld);
    vout.refractionPosition = mul(float4(vin.pos, 1.f), WorldViewProj);
    matrix reflectProjectWorld = (matrix) 0;
    reflectProjectWorld = mul(ReflectionMatrix, ProjectionMatrix);
    reflectProjectWorld = mul(WorldMatrix, reflectProjectWorld);
    vout.reflectionPosition = mul(float4(vin.pos, 1.f), reflectProjectWorld);
    return vout;
}
