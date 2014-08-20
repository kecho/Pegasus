//
//Cubefractal2 vs, by Kevin Boulanger 2013
//translated to hlsl by Kleber Garcia 2014
//


cbuffer uniformState
{
float screenRatio;
float time;
}
struct VS_OUT
{
	float3 p : TEXCOORD1;
	float3 d : TEXCOORD0;
};
VS_OUT main(
	in float2 p0 : POSITION0,
	out float4 pos : SV_Position
){
	VS_OUT vo;
    pos=float4(p0.x,p0.y*screenRatio,0.0,1.0);
    vo.p = float3(0,0,time*.6);
    vo.d = float3(p0.x * cos(time) + sin(time), p0.y + .4*sin(time), cos(time) - p0.x * sin(time));
	return vo;
}
