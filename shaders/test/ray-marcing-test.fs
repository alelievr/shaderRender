vec4 orb;

float map(vec3 p, float s)
{

	/*vec3 q = fract(p) * 2 - 1;

	return length(p) * 2 - 1;*/
	//return length(q) - .1;
	float scale = 1.0;

	orb = vec4(100.0); 
	
	for( int i=0; i<8;i++ )
	{
		p = -1.0 + 2.0*fract(0.5*p+0.5);

		float r2 = dot(p,p);
		
        orb = min( orb, vec4(abs(p),r2) );
		
		float k = max(s/r2,0.1);
		p     *= k;
		scale *= k;
	}
	
	return 0.25*abs(p.y)/scale;
}

float trace(vec3 o, vec3 r, float s)
{
	float totalDistance = 0;

	for (int i = 0; i < 30; i++)
	{
		vec3 p = o + r * totalDistance;
		float distance = map(p, s);
		totalDistance += distance * 0.1;
	}
	return totalDistance;
}

void		mainImage(vec2 coord)
{
	vec2 uv = (coord / iResolution) * 2 - 1;
	uv.x *= iResolution.x / iResolution.y;

	float   fov = 1.5;
	vec3    right = normalize(cross(iForward, vec3(0, 1, 0)));
	vec3    up = normalize(cross(right, iForward));
	vec3    r = normalize(uv.x * right + uv.y * up + fov * iForward);
	vec3	o = iMoveAmount.xyz + vec3(0, 0, 0);
	float t = trace(o, r, 1 + fract(iGlobalTime));
	//float fog = 1 / (1 + t * t * 0.1);

    vec3 rgb = vec3(1.0);
    rgb = mix( rgb, vec3(1.0,0.80,0.2), clamp(6.0*orb.y,0.0,1.0) );
    rgb = mix( rgb, vec3(1.0,0.55,0.0), pow(clamp(1.0-2.0*orb.z,0.0,1.0),8.0) );
	vec3 col = rgb;

	fragColor = vec4(col, 1);
}
