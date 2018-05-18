
// Larval - @P_Malin
// https://www.shadertoy.com/view/ldB3Rz
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// 
// Some kind of alien fractal thingy.
// A hacked together KIFS doodle.
// The fractal parameters aren't that exciting but I like the pretty colours :D

#define kRaymarchIterations 24
#define kIFSIterations 16

vec3 vGlowColour = vec3(1.0, 0.075, 0.01) * 5.0;
vec3 vEnvColour = vec3(0.1, 0.5, 1.0) * 2.0;

float kExposure = 0.1;

// KIFS parameters
const float fScale=1.25;
vec3 vOffset = vec3(-1.0,-2.0,-0.2);	
mat3 m;

const float kFarClip = 30.0;

float GetSceneDistance( in vec3 vPos )
{
	float fTrap = kFarClip;
	
	float fTotalScale = 1.0;
	for(int i=0; i<kIFSIterations; i++)
	{	
		vPos.xyz = abs(vPos.xyz);
		vPos *= fScale;
		fTotalScale *= fScale;
		vPos += vOffset;
		vPos.xyz *= m;
		
		float fCurrDist = length(vPos.xyz) * fTotalScale;
		fTrap = min(fTrap, fCurrDist);
	}

	float l = length(vPos.xyz) / fTotalScale;
	
	float fDist = l - 0.1;
	return fDist;
}

vec2 Raycast( const in vec3 vOrigin, const in vec3 vDir )
{
	float fClosest = kFarClip;
	float d;
	float t = 0.0;
	for(int i=0; i < kRaymarchIterations; i++)
	{
		d = GetSceneDistance(vOrigin + vDir * t);
		fClosest = min(fClosest, d / t);
		if(d < 0.0001)
			break;
		t += d;
		if(t > kFarClip)
		{
			t = kFarClip;
			break;
		}
	}
	
	return vec2(t, d);
}

vec3 TraceRay( const in vec3 vOrigin, const in vec3 vDir )
{	
	vec2 vHit = Raycast(vOrigin, vDir);
	
	vec3 vHitPos = vOrigin + vDir * vHit.x;

	return vHitPos;
}


// mat3 from quaternion
mat3 SetRot( const in vec4 q )
{
	vec4 qSq = q * q;
	float xy2 = q.x * q.y * 2.0;
	float xz2 = q.x * q.z * 2.0;
	float yz2 = q.y * q.z * 2.0;
	float wx2 = q.w * q.x * 2.0;
	float wy2 = q.w * q.y * 2.0;
	float wz2 = q.w * q.z * 2.0;
 
	return mat3 (	
     qSq.w + qSq.x - qSq.y - qSq.z, xy2 - wz2, xz2 + wy2,
     xy2 + wz2, qSq.w - qSq.x + qSq.y - qSq.z, yz2 - wx2,
     xz2 - wy2, yz2 + wx2, qSq.w - qSq.x - qSq.y + qSq.z );
}

// mat3 from axis / angle
mat3 SetRot( vec3 vAxis, float fAngle )
{	
	return SetRot( vec4(normalize(vAxis) * sin(fAngle), cos(fAngle)) );
}

vec3 ApplyPostFx( const in vec3 vIn, const in vec2 fragCoord )
{
	vec2 vUV = fragCoord.xy / iResolution.xy;
	vec2 vCentreOffset = (vUV - 0.5) * 2.0;
	
	vec3 vResult = vIn;
	vResult.xyz *= clamp(1.0 - dot(vCentreOffset, vCentreOffset) * 0.4, 0.0, 1.0);

	vResult.xyz = 1.0 - exp(vResult.xyz * -kExposure);
	
	vResult.xyz = pow(vResult.xyz, vec3(1.0 / 2.2));
	
	return vResult;
}

void mainImage( in vec2 fragCoord )
{
	vec3 vRotationAxis = vec3(1.0, 1.0, 1.0);

	// Rotate the rotation axis
	mat3 m2 = SetRot( vec3(0.1, 1.0, 0.01), iGlobalTime * 0.3 );		
	
	vRotationAxis = vRotationAxis * m2;
	
	float fRotationAngle = sin(iGlobalTime * 0.5);
	
	m = SetRot(vRotationAxis, fRotationAngle);

	vec2    uv = (fragCoord / iResolution) * 2 - 1;
	vec3    cameraDir = iForward;

	//window ratio correciton:
	uv.x *= iResolution.x / iResolution.y;

	//perspective view
	float   fov = 1.5;
	vec3    forw = normalize(iForward);
	vec3    right = normalize(cross(forw, vec3(0, 1, 0)));
	vec3    up = normalize(cross(right, forw));
	vec3    rd = normalize(uv.x * right + uv.y * up + fov * forw);

	vec3 ro = iMoveAmount.xyz / 2;
	
	vec3 vResult = TraceRay(ro, rd);
	
	vResult = ApplyPostFx(vResult,fragCoord);
	
	fragColor = vec4(vResult,1.0);
}
