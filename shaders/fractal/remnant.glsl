// Remnant X
// by David Hoskins.
// Thanks to boxplorer and the folks at 'Fractalforums.com'
// HD Video:- https://www.youtube.com/watch?v=BjkK9fLXXo0

// #define STEREO

vec3 sunDir  = normalize( vec3(  0.35, 0.1,  0.3 ) );
const vec3 sunColour = vec3(1.0, .95, .8);


#define SCALE 2.8
#define MINRAD2 (sin(iGlobalTime / 4) + 1) / 2
float minRad2 = clamp(MINRAD2, 1.0e-9, 1.0);
vec4 scale = vec4(SCALE, SCALE, SCALE, abs(SCALE)) / minRad2;
float absScalem1 = abs(SCALE - 1.0);
float AbsScaleRaisedTo1mIters = pow(abs(SCALE), float(1-10));
float fixedRadius2 = 1;
vec3 fogCol = vec3(0.4, 0.4, 0.4);
float gTime;


//----------------------------------------------------------------------------------------
float Noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	
	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
	vec2 rg = vec2(0);
	return mix( rg.x, rg.y, f.z );
}

//----------------------------------------------------------------------------------------
float Map(vec3 pos) 
{
//	float s = 4;
//	pos = (fract(pos / s) * 2 - 1) * s;

	vec4 p = vec4(pos,1);
	vec4 p0 = vec4(p.xyz, 1);  // p.w is the distance estimate

	for (int i = 0; i < 8; i++)
	{
		//box folding
		p.xyz = clamp(p.xyz, -1, 1) * 2.0 - p.xyz;

		float r2 = dot(p.xyz, p.xyz);
		// sphere folding:
		/*if (r2 < minRad2)
			p /= minRad2;
		else if (r2 < fixedRadius2)
			p /= r2;*/
		p *= clamp(max(minRad2 / r2, minRad2), 0.0, 1.0);

		// scale, translate
		p = p * scale + p0;
	}
	return ((length(p.xyz) - absScalem1) / p.w - AbsScaleRaisedTo1mIters);
}

//----------------------------------------------------------------------------------------
vec3 GetNormal(vec3 pos, float distance)
{
	vec2 eps = vec2(distance, 0.0);
	vec3 nor = vec3(
	    Map(pos+eps.xyy) - Map(pos-eps.xyy),
	    Map(pos+eps.yxy) - Map(pos-eps.yxy),
	    Map(pos+eps.yyx) - Map(pos-eps.yyx));
	return normalize(nor);
}

//----------------------------------------------------------------------------------------
float GetSky(vec3 pos)
{
    pos *= 2.3;
	float t = Noise(pos);
    t += Noise(pos * 2.1) * .5;
    t += Noise(pos * 4.3) * .25;
    t += Noise(pos * 7.9) * .15;
	return t;
}

//----------------------------------------------------------------------------------------
vec2 Scene(in vec3 rO, in vec3 rD, in vec2 fragCoord)
{
	float t = .05 + 0.05 * 1;
	vec3 p = vec3(0.0);
    float oldT = 0.0;
    bool hit = false;
    float glow = 0.0;
    vec2 dist;
	for( int j=0; j < 100; j++ )
	{
		if (t > 42)
			break ;
        p = rO + t*rD;
       
		float h = Map(p);
        
		if( h <0.002)
		{
            dist = vec2(t, oldT);
            hit = true;
            break;
        }
        else
        {
        	glow += clamp(.05-h, 0.0, .4);
        	t += h + t * 0.001;
	        oldT = t;
        }
 	}
    if (!hit)
        t = 10000.0;
    return vec2(t, clamp(glow*.25, 0.0, 1.0));

}

//----------------------------------------------------------------------------------------
float Hash(vec2 p)
{
	return fract(sin(dot(p, vec2(12.9898, 78.233))) * 33758.5453)-.5;
} 

//----------------------------------------------------------------------------------------
vec3 PostEffects(vec3 rgb, vec2 xy)
{
	// Gamma first...
	rgb = pow(rgb, vec3(0.37));

	// Then...
	#define CONTRAST 1.4
	#define SATURATION 1.4
	#define BRIGHTNESS 1.2
	rgb = mix(vec3(.5), mix(vec3(dot(vec3(.2125, .7154, .0721), rgb*BRIGHTNESS)), rgb*BRIGHTNESS, SATURATION), CONTRAST);
	// Noise...
	//rgb = clamp(rgb+Hash(xy*iGlobalTime)*.1, 0.0, 1.0);
	// Vignette...
	rgb *= .5 + 0.5*pow(20.0*xy.x*xy.y*(1.0-xy.x)*(1.0-xy.y), 0.2);	

	return rgb;
}

//----------------------------------------------------------------------------------------
float Shadow( in vec3 ro, in vec3 rd)
{
	float res = 1.0;
    float t = 0.05;
	float h;
	
    for (int i = 0; i < 8; i++)
	{
		h = Map( ro + rd*t );
		res = min(6.0*h / t, res);
		t += h;
	}
    return max(res, 0.0);
}

//----------------------------------------------------------------------------------------
vec3 LightSource(vec3 spotLight, vec3 dir, float dis)
{
    float g = 0.0;
    if (length(spotLight) < dis)
    {
		g = pow(max(dot(normalize(spotLight), dir), 0.0), 600.0) * 1.5;
    }
   
    return vec3(1.0) * g;
}

//---------------------------------------------------------------------------------------

vec2 hash( vec2 x )  // replace this by something better
{
	const vec2 k = vec2( 0.3183099, 0.3678794 );
	x = x*k + k.yx;
	return -1.0 + 2.0*fract( 16.0 * k*fract( x.x*x.y*(x.x+x.y)) );
}

float noise( in vec2 p )
{
	vec2 i = floor( p );
	vec2 f = fract( p );

	vec2 u = f*f*(3.0-2.0*f);

	return mix( mix( dot( hash( i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ), 
	        	dot( hash( i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
	        mix( dot( hash( i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ), 
	        	dot( hash( i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

//----------------------------------------------------------------------------------------
void mainImage( in vec2 fragCoord )
{
	float m = 0;
	gTime = (iGlobalTime+m)*.01 + 15.00;
    vec2 xy = fragCoord.xy / iResolution.xy;

	vec2    uv = (fragCoord / iResolution) * 2 - 1;
	vec3    cameraDir = iForward;

	//window ratio correciton:
	uv.x *= iResolution.x / iResolution.y;

	//perspective view
	float   fov = 1.5;
	vec3    forw = normalize(iForward);
	vec3    right = normalize(cross(forw, vec3(0, 1, 0)));
	vec3    up = normalize(cross(right, forw));
	vec3    dir = normalize(uv.x * right + uv.y * up + fov * forw);

	vec3 ro = iMoveAmount.xyz / 2;

    vec3 spotLight = ro;
	vec3 col = vec3(0.0);
    vec3 sky = vec3(0.03, .04, .05) * GetSky(dir);
	vec2 ret = Scene(ro, dir, fragCoord);
    
    if (ret.x < 1000.0)
    {
    	col = vec3(1, 1, 1);
		vec3 p = ro + ret.x*dir; 

       	vec2 t1 = vec2(iGlobalTime / 4, 0);
       	vec2 t2 = vec2(10, iGlobalTime / 8);
       	vec2 t3 = vec2(10, iGlobalTime / 16 + 10);
       	float s = 2;
       	float n1 = noise((p.xy / 2 + t1) * s) * 2;
       	float n2 = noise((p.yz / 4 + t2 + 10) * s) * 2;
       	float n3 = noise((p.xz / 3 + t3 + 100) * s) * 2;
       	vec3 addColor = normalize(vec3(n1 + n2, n2 + n3, n3 + n1)) / 3;
       	col = addColor;

		col = PostEffects(col, xy);	
    }
	
	#ifdef STEREO	
	col *= vec3( isRed, 1.0-isRed, 1.0-isRed );	
	#endif
	
	fragColor=vec4(col,1.0);
}

//--------------------------------------------------------------------------
