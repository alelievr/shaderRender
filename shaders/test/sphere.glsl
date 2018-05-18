#define ITER		120
#define EPSY		0.04
#define IFS_ITER	9

mat3	m;

float sdSphere(in vec3 p, float size)
{
	return length(p) - size;
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


float map(in vec3 p)
{
	const float	scaleFactor = 1;
	const float	scale = 1.55;
	vec3 offs = vec3(-.25,-.5,-0.05);

	float	totalScale = 1;
	float	t = iGlobalTime;

	p += vec3(0, .0, 0);

//	p  = abs(vec3(fract(p.x * .5), p.y, fract(p.z * .5))*2. - 1.); // Standard spacial repetition.

	for (int i = 0; i < IFS_ITER; i++)
	{
		p = abs(p);
		p *= scale;
		p += offs;
		p *= m;

		totalScale *= scale;
	}

	float l = length(p) / totalScale;
	return l - .03;
}

vec2 raymarch(in vec3 ro, in vec3 rd)
{
	float t = 0, d;

	for (int i = 0; i < ITER; i++)
	{
		d = map(ro + rd * t);
		t += d;

		if (d < EPSY)
			break ;
	}
    return vec2(t, d);
}

vec3	calcNormal(in vec3 p)
{
	vec3	e = vec3(0.001, 0, 0);

	return normalize(vec3(
		map(p + e.xyy) - map(p - e.xyy),
		map(p + e.yxy) - map(p - e.yxy),
		map(p + e.yyx) - map(p - e.yyx)
	));
}

bool	boundingSphere(inout vec3 ro, in vec3 rd)
{
	float		minSphereDist = 1.2;
	float		l = length(ro);

	if (l < minSphereDist)
		return true;
	l = length(ro + rd * l);
	if (l > minSphereDist)
		return false;
	else
		ro += rd * l;

	return true;
}

vec3 Colorizer( const in vec3 vIn, const in vec2 fragCoord )
{
	vec2 vUV = fragCoord.xy / iResolution.xy;
	vec2 vCentreOffset = (vUV - 0.5) * 2.0;

	vec3 vResult = vIn;
	vResult.xyz *= clamp(1.0 - dot(vCentreOffset, vCentreOffset) * 0.4, 0.0, 1.0);

	vResult.xyz = 1.0 - exp(vResult.xyz * - 1);

	vResult.xyz = pow(vResult.xyz, vec3(1.0 / 2.2));

	return vResult;
}

void	mainImage(in vec2 fragCoord)
{
//	vec3 rd = normalize(iMoveAmount.xyz - position);
//	vec3 ro = -iMoveAmount.xyz + iCenter;

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

	vec3 col = vec3(.3);
	//vec3 lightPos = iCenter - vec3(0, 10, 0);
	vec3 lightPos = vec3(0, 10, 0);

	if (!boundingSphere(ro, rd))
	{
		fragColor = vec4(0, 1, 1, 1);
		return ;
	}

	vec3 rotationVector = vec3(1.0, 1.0, 1.0);

	rotationVector *= SetRot(vec3(0, 0.1, 1.001), iGlobalTime * .2);
	rotationVector *= SetRot(vec3(1, 0.0, 0), iGlobalTime * .2);

	m = SetRot(rotationVector, sin(iGlobalTime / 3) * .7);

    vec2 res = raymarch(ro, rd);

    vec3 p = ro + rd * res.x;
    vec3 norm = calcNormal(p);
    float ambient = 1 + .5 * norm.y;
    float bright = max(0, dot(norm, normalize(lightPos)));
    col = ambient * vec3(0.3);
    col += bright * vec3(1);

    vec3 albedo = Colorizer(p, fragCoord);
    
    col *= albedo;

    fragColor.a = step(length(albedo), 0.1);

    fragColor.rgb = col;
}
