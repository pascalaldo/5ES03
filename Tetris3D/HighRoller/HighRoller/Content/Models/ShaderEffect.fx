// Shader example taken from http://digitalerr0r.wordpress.com/category/xna-shader-tutorial/
// XNA 4.0 Shader Programming #2 - Diffuse light 
// A very simple diffuse light shader with texture

// Matrices
float4x4 World;
float4x4 View;
float4x4 Projection;

// Light related constants
float4	AmbientColor;
float	AmbientIntensity; 
float3	DiffuseDirection;
float4	DiffuseColor;
float	DiffuseIntensity;
float	TextureIntensity;

// the texture to be used
shared Texture currentTexture;
// the texture to be used
shared bool hasTexture;
// the color to be used
shared float4 currentColor;

// defines how to interpolate points on the texture
sampler TextureSampler = sampler_state { texture = <currentTexture> ; magfilter = LINEAR; minfilter = LINEAR; mipfilter=LINEAR; AddressU = Mirror; AddressV = Mirror;};

struct VertexShaderInput
{
    float4 Position : POSITION0;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
	float3 Normal : TEXCOORD0;
	float2 TextureCoordinate : TEXCOORD1;
};

VertexShaderOutput VertexShaderFunction(
	VertexShaderInput input, 
	float2 texCoord : TEXCOORD0,
    float3 Normal : NORMAL)
{
    VertexShaderOutput output;

    float4 worldPosition = mul(input.Position, World);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = mul(viewPosition, Projection);

	float3 normal = normalize(mul(Normal, World));    
	output.Normal = normal;     

	output.TextureCoordinate = texCoord;

	return output;
}

// The Pixel Shader
float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
    float4 norm = float4(input.Normal, 1.0);    
	float4 diffuse = saturate(dot(-DiffuseDirection,norm));     
	float4 color = AmbientColor*AmbientIntensity+DiffuseIntensity*DiffuseColor*diffuse;

	if(hasTexture){
		color += TextureIntensity * tex2D(TextureSampler, input.TextureCoordinate);
	} else {
		color += currentColor;
	}
	return color;
}

technique Technique1
{
    pass Pass1
    {
        VertexShader = compile vs_2_0 VertexShaderFunction();
        PixelShader = compile ps_2_0 PixelShaderFunction();
    }
}
