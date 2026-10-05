cbuffer cbPerFrame : register(c0)
{
    float4 colorAndTime : register(c0); // {R, G, B, time}
};
cbuffer cbPerFrame2 : register(c4)
{
    float4 adminpanel : register(c4); // {R, G, B, time}
};
    cbuffer cbPerFrame3 : register(c8)
{
    float4 lightng : register(c8); // {R, G, B, time}
};    cbuffer cbPerFrame4 : register(c12)
{
    float4 mapParam : register(c12); // {R, G, B, time}
};
cbuffer cbPerFrame4 : register(c16)
{
    float4 mapParamFogCol : register(c16); // {R, G, B, time}
};
//sampler2D DistorSampl : register(s0);
sampler2D TextureSampler2 : register(s1);
sampler2D reflectionTexture : register(s2);
sampler2D refractionTexture : register(s3);
sampler2D NormalSampl : register(s4);
#define M_PI 3.141592654f

struct VS_Output
{
    float4 pos : POSITION;
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
void Fog(inout float3 pix, in float4 view_space)
{
    float FogStart = 800 ;
    float FogEnd = 3000;
    float dist = length(view_space);
    float fog_factor = //(mapParam.g - dist - mapParam.b) / (mapParam.g - mapParam.r);
    (FogEnd - dist) / (FogEnd - FogStart);
    fog_factor = clamp(fog_factor, 0, 1);
    pix = lerp(float3(0, 0.3, 0.3), pix, fog_factor);
}
float3 Normal(in float2 normal_uv)
{
    float3 normal_color = tex2D(NormalSampl, normal_uv);
    float3 normal = float3(normal_color.x * 2 - 1, normal_color.z, normal_color.y * 2 - 1);
    return normalize(normal);
}

float FresnelTrans(in float pix_trans, in float3 to_cam, in float3 normal)
{
    static const float MaxTrans = 0.5;
    //colorAndTime.r;
  //  0.5;
//static const float MinTrans = 0.20; // minimum transparency will be taken from the texture transparency
    static const float FresnelFactor = 2.5;
  //  colorAndTime.g;
  //  2.5; // power by how much more reflective the water becomes when looking flat into it
    
    float min_trans = pix_trans;
    float cam_factor = dot(normalize(to_cam), normal);
    cam_factor = pow(abs(cam_factor), FresnelFactor);
	//return min_trans + cam_factor * (MaxTrans - min_trans);
    return MaxTrans - cam_factor * (MaxTrans - min_trans);
}
static const float3 LightDirection = lightng.rgb; //3.284, -2.338, -0.995
static const float3 LightColor = float3(0.9, 0.9, 1);
static const float LightDamper = 20;//adminpanel.
//r; //20; //adminpanel.r; //20; //-1.5f; //colorAndTime.r;
    //20; // how far light spreads accross the water (higher = smaller area)
static const float LightReflectivity = 1.343;//adminpanel.
//r; //1.542;//adminpanel.
//g; //0.7; //0.124f; //colorAndTime.g; //0.7; // how reflective the water is (percentage)
static const float NormalLightingDamp = 0.100; // adminpanel.g;//0.050; //adminpanel.b; //0.5; //colorAndTime.b; //0.5; //7.0f; //colorAndTime.b;
   // 0.5; // how sharp and specular the reflections are (lower value = more sharp, higher = more damped)
static const float NormalFresnelDamp = 7; //7; //adminpanel.r; //7;
void Lighting(inout float3 pix, in float3 to_camera, in float3 normal)
{
	// specular highlighting by how much the camera "looks into" the reflected light direction
    float3 reflect_light = reflect(normalize(LightDirection), normal);
    float specular = max(dot(reflect_light, normalize(to_camera)), 0);
    specular = pow(specular, LightDamper);
    float3 specular_highlights = LightColor * specular * LightReflectivity;

    pix += specular_highlights;
}


float4 main(VS_Output input) : SV_TARGET
{
    float2 uv = input.uv;
    float4 tex_pix = tex2D(TextureSampler2, uv); // float4(0, 0.35, 0.93, 1);
    float4 WaveColor = float4(1 / max(0.1, input.WaveHeight), 1 / max(0.1, input.WaveHeight), 1 / max(0.1, input.WaveHeight), 1);
    float3 pix = float4(tex_pix.rgb / min(0.53, lerp(tex_pix, WaveColor, 1)).rgb, tex_pix.a);
    float3 normal = Normal(uv);
    Lighting(pix, input.ToCamera, normalize(float3(normal.x, normal.y * NormalLightingDamp, normal.z)));
    Fog(pix, input.ViewSpace);
    float trans_factor = FresnelTrans(tex_pix.w, input.ToCamera, normalize(float3(normal.x, normal.y * NormalFresnelDamp, normal.z)));
    
    float2 reflectTexCoord;
    float2 refractTexCoord;
    float flipWaterRefracReflec = 0.5f;
    float flipWaterRefracReflec2 = -0.5f;
    float distanceWaterRefracReflec = 2.0f;
    float distanceWaterRefracReflec2 = -2.0f;
    refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / distanceWaterRefracReflec + flipWaterRefracReflec;
    refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / distanceWaterRefracReflec + flipWaterRefracReflec;    
    refractTexCoord = refractTexCoord + (normal.xy * 0.001);
    
    reflectTexCoord.x = (input.refractionPosition.x / input.refractionPosition.w / distanceWaterRefracReflec + flipWaterRefracReflec);
    reflectTexCoord.y = (1 - (-input.refractionPosition.y / input.refractionPosition.w / distanceWaterRefracReflec + flipWaterRefracReflec)) * 0.5f;
 
    reflectTexCoord = reflectTexCoord + (normal.xy * 0.060);
   
    float4 reflectionColor = tex2D(reflectionTexture, reflectTexCoord);
    float4 refractionColor = tex2D(refractionTexture, refractTexCoord);
    float4 ReflRefr = ReflRefr = lerp(reflectionColor, refractionColor, 0.5f);
    float4 color = lerp(ReflRefr, float4(pix, trans_factor), 0.45);
    
    float dist = length(input.ViewSpace);
    float dist_fac = (mapParam.g * 9.104 - dist) / (mapParam.g * 9.104 - mapParam.r * 5.323);
    dist_fac = clamp(dist_fac, 0, 1);
    return lerp(float4(mapParamFogCol.rgb, -5.323),color , dist_fac);
  //  return float4(color.rgb, clamp(dist_fac, 0, 0.8));
}
RasterizerState DisableCulling
{
    CullMode = NONE;
};

DepthStencilState DepthEnabling
{
    DepthEnable = TRUE;
};
