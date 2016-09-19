const int firstOctave = 2;
const int octaves = 2;
const float persistence = .7;

float noise(int x,int y)
{   
    float fx = float(x);
    float fy = float(y);
    
    return 2.0 * fract(sin(dot(vec2(fx, fy) ,vec2(12.9898,78.233))) * 43758.5453) - 1.0;
}

float smoothNoise(int x,int y)
{
    return noise(x,y)/4.0+(noise(x+1,y)+noise(x-1,y)+noise(x,y+1)+noise(x,y-1))/8.0+(noise(x+1,y+1)+noise(x+1,y-1)+noise(x-1,y+1)+noise(x-1,y-1))/16.0;
}

float COSInterpolation(float x,float y,float n)
{
    float r = n*3.1415926;
    float f = (1.0-cos(r))*0.5;
    return x*(1.0-f)+y*f;
    
}

float InterpolationNoise(float x, float y)
{
    int ix = int(x);
    int iy = int(y);
    float fracx = x-float(int(x));
    float fracy = y-float(int(y));
    
    float v1 = smoothNoise(ix,iy);
    float v2 = smoothNoise(ix+1,iy);
    float v3 = smoothNoise(ix,iy+1);
    float v4 = smoothNoise(ix+1,iy+1);
    
   	float i1 = COSInterpolation(v1,v2,fracx);
    float i2 = COSInterpolation(v3,v4,fracx);
    
    return COSInterpolation(i1,i2,fracy);
    
}

float PerlinNoise2D(float x,float y)
{
    float sum = 0.0;
    float frequency =0.0;
    float amplitude = 0.0;
    for(int i=firstOctave;i<octaves + firstOctave;i++)
    {
        frequency = pow(2.0,float(i));
        amplitude = pow(persistence,float(i));
        sum = sum + InterpolationNoise(x*frequency,y*frequency)*amplitude;
    }
    
    return sum;
}

float hash1( float n ) { return fract(sin(n)*43758.5453); }
vec2  hash2( vec2  p ) { p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) ); return fract(sin(p)*43758.5453); }

vec4 voronoi( in vec2 x, float w )
{
    vec2 n = floor( x );
    vec2 f = fract( x );

	vec4 m = vec4( 2.0, 0.0, 0.0, 0.0 );
    for( int j=-2; j<=2; j++ )
    for( int i=-2; i<=2; i++ )
    {
        
        vec2 g = vec2(float(i), float(j));
        vec2 o = hash2( n + g );
		
		// animate
        o = .5 + .2 * sin(iGlobalTime * 1.454 * o);

        // distance to cell		
		float d = length(g - f + o);
		
        // do the smoth min for colors and distances
        float rndPoint = hash1(dot(n + g, vec2(7.0,113.0)));
		vec3 col = abs(sin(vec3(rndPoint))) * vec3(hash1(rndPoint), hash1(rndPoint * 3.14159255), hash1(rndPoint / 3.14159255));
		float h = smoothstep( 0.0, 1.0, 0.5 + 0.5*(m.x-d)/w );
		
	    m.x   = mix( m.x,     d, h ) - h * (1.0 - h) * w / (1.0+3.0*w); // distance
		m.yzw = mix( m.yzw, col, h ) - h * (1.0 - h) * w / (1.0 + 3.0 * w); // color
    }

	return m;
}

void mainImage( vec2 fragCoord )
{
    vec2 p = fragCoord.xy/iResolution.yy;
	
    p += PerlinNoise2D(p.x * 2., p.y * 2.) / 1.5;
    
    vec4 c = voronoi( 6.0 * p + iGlobalTime, .4 );

    vec3 col = c.yzw;
	
    fragColor = vec4( col, 1.0 );
}
