// ================================================================================ //
// Copyright (c) 2011 Arturo Garcia, Francisco Avila, Sergio Murguia and Leo Reyes	//
//																					//
// Permission is hereby granted, free of charge, to any person obtaining a copy of	//
// this software and associated documentation files (the "Software"), to deal in	//
// the Software without restriction, including without limitation the rights to		//
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies	//
// of the Software, and to permit persons to whom the Software is furnished to do	//
// so, subject to the following conditions:											//
//																					//
// The above copyright notice and this permission notice shall be included in all	//
// copies or substantial portions of the Software.									//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR		//
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,			//
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE		//
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER			//
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	//
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE	//
// SOFTWARE.																		//
// ================================================================================ //

// Ray Tracing on the Compute Shader.

cbuffer cbCamera : register(b0)	// 64 bytes
{
	float4x4 g_mfWorld;		// 64 bytes
};

cbuffer cbInputOutput : register(b1)	//32  bytes
{
	int	g_bIsShadowOn;				// 4 byte
	int	g_bIsPhongShadingOn;		// 4 byte
	int	g_bIsNormalMapspingOn;		// 4 byte
	int	g_bIsGlossMappingOn;		// 4 bytes
	int	g_iAccelerationStructure;	// 4 byte
	int	g_iEnvMappingFlag;			// 4 bytes
	uint g_iNumBounces;				// 4 bytes
	int g_viPadding;				// 4 bytes
};

cbuffer cbLight : register(b2)	// 32 bytes
{
	float3 g_vfLightPosition;		// 12 bytes
	int g_vfPaddingLight[5];		// 20 bytes
};

cbuffer cbGlobalIllumination : register(b3) //	32 bytes
{
	float g_fRandX, g_fRandY;	// 8 bytes
	float g_fFactorAnt;			// 4 bytes
	float g_fFactorAct;			// 4 bytes
	uint g_uiNumMuestras;		// 4 bytes
	int3 g_vfPadding;			// 12 bytes
};

struct Node						// 32 bytes	
{
	// Bounds
	float3 vfMin;				// 12 bytes
	float3 vfMax;				// 12 bytes

	int nPrimitives;			// 4 bytes
	uint primitivesOffset;		// 4 bytes
};

struct LBVHNode					// 32 bytes
{
	// Bounds
	float3 vfMin;				// 12 bytes
	float3 vfMax;				// 12 bytes

	int iPrimCount;				// 4 bytes
	int iPrimPos;				// 4 bytes
};

struct Ray						// 48 bytes
{
	float3 vfOrigin;			// 12 bytes
	float3 vfDirection;			// 12 bytes
	float3 vfReflectiveFactor;	// 12 bytes
	float fMinT, fMaxT;			// 8 bytes
	int iTriangleId;			// 4 bytes
};

struct Vertex					// 32 bytes
{
	float3 vfPosition;			// 12 bytes
	float3 vfNormal;			// 12 bytes
	float2 vfUvs;				// 8 bytes
};

struct Intersection				// 24 bytes
{
	int iTriangleId;			// 4 bytes
	float fU, fV, fT;			// 12 bytes
	int iVisitedNodes;			// 4 bytes
	int iRoot;					// 4 bytes
};

struct Material					// 4 bytes
{
	int iMaterialId;			// 4 bytes
};

struct MortonCode				// 8 bytes
{
	uint iCode;					// 4 bytes
	int iId;					// 4 bytes
};

struct Primitive				// 64 bytes
{
	float3 vfCentroid;			// 12 bytes
	float3 vfMin;				// 12 bytes
	float3 vfMax;				// 12 bytes
	float3 g_vfPadding1;		// 12 bytes
	float3 g_vfPadding2;		// 12 bytes
	int iMaterial;				// 4 bytes
};

// SRVs
StructuredBuffer<int>				g_sPrimitives		:	register( t0 );
StructuredBuffer<Node>				g_sNodes			:	register( t1 );
StructuredBuffer<Vertex>			g_sVertices			:	register( t2 );
StructuredBuffer<DWORD>				g_sIndices			:	register( t3 );
StructuredBuffer<LBVHNode>			g_sLBVHNodes		:	register( t10 );
Texture2DArray						g_sTextures			:	register( t4 );
Texture2DArray						g_sSpecularMaps		:	register( t5 );
Texture2DArray						g_sNormalMaps		:	register( t6 );
StructuredBuffer<Material>			g_sMaterials		:	register( t7 );
Texture2D							g_sRandomTx			:	register( t8 );
TextureCube							g_sEnvironmentTx	:	register( t9 );

// UAVs
RWTexture2D<float4>					g_uResultTexture	:	register( u0 );
RWStructuredBuffer<Ray>				g_uRays				:	register( u1 );
RWStructuredBuffer<Intersection>	g_uIntersections	:	register( u2 );
RWStructuredBuffer<float4>			g_uAccumulation		:	register( u3 );
RWStructuredBuffer<Primitive>		g_uPrimitives		:	register( u4 );
RWStructuredBuffer<LBVHNode>		g_uLVBH				:	register( u5 );
RWStructuredBuffer<MortonCode>		g_uMortonCode		:	register( u6 );

// Samplers
SamplerState g_ssSampler : register(s0);

Intersection getIntersection(Ray ray, float3 A, float3 B, float3 C)
{
	Intersection intersection;
	intersection.iTriangleId = -1;
	float3 P, T, Q;
	float3 E1 = B - A;
		float3 E2 = C - A;
		P = cross(ray.vfDirection, E2);
	float det = 1.0f / dot(E1, P);
	T = ray.vfOrigin - A;
	intersection.fU = dot(T, P) * det;
	Q = cross(T, E1);
	intersection.fV = dot(ray.vfDirection, Q)*det;
	intersection.fT = dot(E2, Q)*det;
	return intersection;
}
bool RayTriangleTest(Intersection intersection)
{
	return ((intersection.fU >= 0.0f) && (intersection.fV >= 0.0f) && ((intersection.fU + intersection.fV) <= 1.0f) && (intersection.fT >= 0.0f));
}
Intersection IntersectP(Ray ray)
{
	Intersection cIntersection;
	Intersection bIntersection;
	
	bIntersection.iTriangleId = -1;
	bIntersection.fT = 10000.f;
	bIntersection.fU = -1;
	bIntersection.fV = -1;
	const int iNumPrimitives = 10;
	for (int i = 0; i < iNumPrimitives; ++i)
	{
		unsigned int offset = i * 3;
		float3 A = g_sVertices[g_sIndices[offset]].vfPosition;
			float3 B = g_sVertices[g_sIndices[offset + 1]].vfPosition;
			float3 C = g_sVertices[g_sIndices[offset + 2]].vfPosition;
			cIntersection = getIntersection(ray, A, B, C);
		if (ray.iTriangleId != i && RayTriangleTest(cIntersection) && cIntersection.fT < bIntersection.fT)
		{
			bIntersection = cIntersection;
			bIntersection.iTriangleId = i;
		}
	}
	return bIntersection;
}