
// Shape deformations
//#define LESS_ARTIFACTS // 2 next defines Only works with this defined
 //#define NO_DOORS
 //#define SQUARE
//#define BOULDERS
#define ROTATE

//#define SHOW_SUBSTRACTOR

// Colors Modes (only one enabled at a time)
#define BLACKBODY
//#define GRIDS
// #define GEODESIC // sub-mode of GRID
//#define SINUSITE
//#define HEATWAVES

float	t;		// time
float	a;		// angle used both for camera path and distance estimator
float	g;		// used for coloring
float	st;		// sin(t)

#define I_MAX		100
#define E			0.001

#define		FWD_SPEED	-7.	// the speed at wich the tunnel travel towards you

vec2	march(vec3 pos, vec3 dir);
vec3	camera(vec2 uv);
vec2	rot(vec2 p, vec2 ang);
void	rotate(inout vec2 v, float angle);


// blackbody by aiekick : https://www.shadertoy.com/view/lttXDn

// -------------blackbody----------------- //

// return color from temperature 
//http://www.physics.sfasu.edu/astro/color/blackbody.html
//http://www.vendian.org/mncharity/dir3/blackbody/
//http://www.vendian.org/mncharity/dir3/blackbody/UnstableURLs/bbr_color.html

vec3 blackbody(float Temp)
{
	vec3 col = vec3(255.);
    col.x = 56100000. * pow(Temp,(-3. / 2.)) + 148.;
   	col.y = 100.04 * log(Temp) - 623.6;
   	if (Temp > 6500.) col.y = 35200000. * pow(Temp,(-3. / 2.)) + 184.;
   	col.z = 194.18 * log(Temp) - 1448.6;
   	col = clamp(col, 0., 255.)/255.;
    if (Temp < 1000.) col *= Temp/1000.;
   	return col;
}

// -------------blackbody----------------- //

float	de_0(vec3 p)
{
	float	mind = 1e5;
	vec3	pr = p;

    #ifdef ROTATE
    // rotate x and y based on z and time
	rotate(pr.xy, (a)*.15) ;
	#endif
    
    // take the fractional part of the ray (p), 
    // and offset it to get a range from [0.,1.] to [-.5, .5]
    // this is a space partitioning trick I saw on "Data Transfer" by srtuss : https://www.shadertoy.com/view/MdXGDr
	pr.xyz = fract(pr.xyz);
	pr -= .5;
    
    // magic numbers : .666 == 2/3, 2.09 == 2*(3.14/3), 4.18 == 4*(3.14/3)
    // dephasing is needed in order to get the lattice
    pr.y *= sin(t*1.     +p.z+p.y-p.x);
    pr.x *= sin(t*1.+2.09+p.z+p.y-p.x);
    pr.z *= sin(t*1.+4.18+p.z+p.y-p.x);

    mind = min(mind, (length(pr.xyz)-.3025));
	return (mind * abs(sin(t*.05)) );
}

float	de_1(vec3 p) // substractor
{
	float	mind = 1e5;
	vec3	pr = p;

    #ifndef	LESS_ARTIFACTS
    pr.x/=max(.01,-fract(p.z*.01)+.5+sin( (pr.y)*.3675-.5+(sin(pr.z*.25))*.3)-.5)*9.;
    #else
        #ifdef NO_DOORS
	    pr.x/=max(.15,( length(sin(p.z*.25) + p.y*.5))-.5);
		#else
    	 #ifndef SQUARE
         pr.x/=max(.052,-fract(p.z*.01)+.5+sin( (pr.y)*.3675-.5+(sin(pr.z*.25))*.3)-.5)*9.;
    	 #else
    	 pr.x/= (fract( abs(pr.y)*.06125-.9+(sin(pr.z*.5))*.1)-.5);
    	#endif
     #endif
    #endif
    #ifdef BOULDERS
    pr.x*=sin(p.y)-sin(p.z*2.)*.5;
    #endif
	vec2	q;
    
	q = vec2(length(pr.yx) - 3., 0. );

    q.y = rot(q.xy, vec2(-1., 0.)).x;

	mind = length(q) - 3.;

	return mind;
}

float	de_2(vec3 p)
{
    #ifdef SHOW_SUBSTRACTOR
    return (20.-de_1(p));
    #else
    return (de_0(p)-de_1(p)*.125);
    #endif
}

float	scene(vec3 p)
{
    float	mind = 1e5;
    a = ( .8*(p.y*.015 + p.x*.015 + p.z *.15)  + t*3.);

	rotate(p.yx, ( (a*10.)*exp(-t))*.75) ;	// the starting spiral in the tunnel
	p.y += 4.;
	mind = de_2(p);
	
    return (mind);
}

vec2	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0, 0.0);
    vec3	p =    vec3(0.0, 0.0, 0.0);
    vec2	s =    vec2(0.0, 0.0);
    vec3	dirr = dir;						// temporary variable for rotation
    for (int i = -1; i < I_MAX; ++i)
    {
    	dirr = dir;
    	rotate(dirr.zx, .05*dist.y*st );
    	p = pos + dirr * dist.y;
        #ifdef GRIDS
        g = p.z*p.y*p.x*.001;
        g -= (step(sin(5.*p.z-1.57), .5) * step(sin(5.*p.x-1.57) , .5) 
             + step(sin(20.*p.z-1.57), .5) * step(sin(20.*p.x-1.57) , .5)); 	    
        #endif
        dist.x = scene(p);
        dist.y += dist.x;
        if (dist.x < E || dist.y > 30.)
        {
            break;
        }
        s.x++;
    }
    s.y = dist.y;
    return (s);
}

void mainImage( in vec2 f )
{
    g = 0.;
    t  = iGlobalTime*.5;
    st = sin(t);
    vec3	col = vec3(0., 0., 0.);
	vec2 R = iResolution.xy,
          uv  = vec2(f-R/2.) / R.y; // Thanks Fabrice
	vec3	dir = camera(uv);
    vec3	pos = vec3(.0, .0, t*FWD_SPEED);

    vec2	inter = (march(pos, dir));

    #ifdef	BLACKBODY
    fragColor.xyz = blackbody( ( 15.-inter.y+.051*inter.x ) * 75. );
    return ;
    #endif
    #ifdef	SINUSITE
    fragColor.xyz = inter.y*.05*vec3(abs(sin(dir.x*3.14+t+1.04)), abs(sin(dir.y*3.14+t+2.09)), abs(sin(dir.z*3.14+t+3.14)));
    return ;
    #endif
    #ifdef	GEODESIC
	float	g = (pos+dir*inter.y).y;
    #endif
    #ifdef	GRIDS
   	fragColor.xyz = vec3(abs(sin(t+1.32+1.04*g)), abs(sin(t+1.32+2.09*g)), abs(sin(t+1.32+3.14*g)))*inter.x*.005;
    return ;
	#endif
    #ifdef	HEATWAVES
    fragColor.xyz = vec3(abs(sin(t+1.04+(15.-inter.y*.05) )), abs(sin(t+2.09+(15.-inter.y*.05) )), abs(sin(t+3.14+(15.-inter.y*.05) )));
    return ;
    #endif
}

// Utilities

void rotate(inout vec2 v, float angle)
{
	v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);
}

vec2	rot(vec2 p, vec2 ang)
{
	float	c = cos(ang.x);
    float	s = sin(ang.y);
    mat2	m = mat2(c, -s, s, c);
    
    return (p * m);
}

vec3	camera(vec2 uv)
{
    float		fov = 1.;
	vec3		forw  = vec3(0.0, 0.0, -1.0);
	vec3    	right = vec3(1.0, 0.0, 0.0);
	vec3    	up    = vec3(0.0, 1.0, 0.0);

    return (normalize((uv.x) * right + (uv.y) * up + fov * forw));
}

