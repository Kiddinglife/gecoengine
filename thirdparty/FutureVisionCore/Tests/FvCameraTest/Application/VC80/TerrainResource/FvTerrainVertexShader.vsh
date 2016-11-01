float4x4 g_World			: register(c0);
float4x4 g_ViewProj		: register(c4);
float4   g_CamPos			: register(c8);

float4   g_TerrainDis	: register(c9);
float4   g_NormalBlendSize	: register(c10);

float3   g_LayerDirU0	: register(c11);
float3   g_LayerDirV0	: register(c12);
float3   g_LayerDirU1	: register(c13);
float3   g_LayerDirV1	: register(c14);
float3   g_LayerDirU2	: register(c15);
float3   g_LayerDirV2	: register(c16);
float3   g_LayerDirU3	: register(c17);
float3   g_LayerDirV3	: register(c18);

struct INPUT
{
	float height : POSITION0;
	float morph  : POSITION1;
	float2 xy	 : TEXCOORD0;
};

struct OUTPUT
{
	float4 PosProjected		: POSITION;
	float4 NormalBlendUV	: TEXCOORD0;
	float4 Layer01UV			: TEXCOORD1;
	float4 Layer23UV			: TEXCOORD2;
};

float2 GetUV(in const float2 dimensions, in const float2 xy)
{
	float2 scale = float2( float(dimensions.x -1 ) / float( dimensions.x ) , float(dimensions.y -1 ) / float( dimensions.y ) );
	
	// calculate the offset to the middle of the first texel
	float2 offset = float2( 0.5f / float( dimensions.x ), 0.5f / float( dimensions.y ) );
	
	return xy * scale + offset;
}

OUTPUT Main(INPUT In)
{
	OUTPUT Out = (OUTPUT) 0;

	float4 position = mul(g_World, float4( In.xy.x * g_TerrainDis.x, In.xy.y * g_TerrainDis.y, 0, 1 ));
	
	float distance = length( position.xy - g_CamPos.xy );
	float lod = saturate( (distance - g_TerrainDis.z) / (g_TerrainDis.w - g_TerrainDis.z) );
	float height = lerp( In.height, In.morph, lod );
	position.xyz += mul(g_World, float3( 0, 0, height));
	Out.PosProjected = mul(g_ViewProj, position);
	
	Out.NormalBlendUV.xy = GetUV(g_NormalBlendSize.xy, In.xy);
	Out.NormalBlendUV.zw = GetUV(g_NormalBlendSize.zw, In.xy);
	
	Out.Layer01UV.xy = float2(dot(g_LayerDirU0, position.xyz), dot(g_LayerDirV0, position.xyz));
	Out.Layer01UV.zw = float2(dot(g_LayerDirU1, position.xyz), dot(g_LayerDirV1, position.xyz));
	Out.Layer23UV.xy = float2(dot(g_LayerDirU2, position.xyz), dot(g_LayerDirV2, position.xyz));
	Out.Layer23UV.zw = float2(dot(g_LayerDirU3, position.xyz), dot(g_LayerDirV3, position.xyz));

	return Out;
}