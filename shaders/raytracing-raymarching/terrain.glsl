// Created by EvilRyu 2015
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

float hash(vec2 p)
{
    p=fract(p*vec2(5.3983,5.4472));
   	p+=dot(p.yx,p.xy+vec2(21.5351,14.3137));
    return fract(p.x*p.y*95.4337);
}

float noise(vec2 p)
{
    vec2 f;
    f=fract(p);
    p=floor(p);
    f=f*f*(3.0-2.0*f);
    return mix(mix(hash(p),hash(p+vec2(1.0,0.0)),f.x),
               mix(hash(p+vec2(0.0,1.0)),hash(p+vec2(1.0,1.0)),f.x),f.y);
}

float fbm(vec2 p)
{
    p*=0.09;
    float f=0.;
    float freq=4.0;
    float amp=0.8;
    for(int i=0;i<10;++i)
    {
        f+=noise(p*freq)*amp;
        amp*=0.5;
        freq*=1.79;
    }

    return f;
}

float f(vec3 p)
{
    float h=fbm(p.xz);
    h+=smoothstep(-.5,1.5,h);
    h=p.y-h;
    return h;
}

vec3 getnormal(vec3 p,float t)
{
    vec3 eps=vec3(0.001*t,0.0,0.0);
    vec3 n=vec3(f(p-eps.xyy)-f(p+eps.xyy),
                2.0*eps.x,
                f(p-eps.yyx)-f(p+eps.yyx));
  
    return normalize(n);
}

float tmax = 20.0;

float intersect(vec3 ro,vec3 rd )
{
    float h=1.0;
    float t=0.01;
    for(int i=0;i<150;++i)
    {
        h=f(ro+t*rd);
        if(h<0.001*t||t>tmax)
  			break;
        t+=h*0.5;
    }
    return t;
}
// from iq's Terrian Tubes https://www.shadertoy.com/view/4sjXzG
vec3 dome(vec3 rd, vec3 sun_dir)
{
    float sun=dot(rd,sun_dir);
    float sda=clamp(0.5+0.5*sun,0.0,1.0);
    float cho=max(rd.y,0.0);
    vec3 bgcol=mix(mix(vec3(0.00,0.40,0.80)*0.7, 
                       vec3(0.70,0.70,0.40),
                       pow(1.0-cho,1.0 + 3.0-3.0*sda)), 
                   vec3(0.53+0.2*sda,0.45-0.1*sda,0.4-0.25*sda),
                   pow(0.95-cho,10.0+8.0-8.0*sda));
    bgcol*=0.8+0.2*sda;
    return bgcol*0.4;
}

vec3 l1dir;
vec3 lighting(vec3 n,vec3 p,vec3 rd)
{
    l1dir.z*=-1.0;
    float dif=max(0.0,dot(l1dir,n));
    float bac=max(0.0,dot(normalize(vec3(-l1dir.x,0.0,l1dir.z)),n));
    float sky=max(0.0,dot(vec3(0.0,1.0,0.0),n));
    float sha=smoothstep(0.0,0.1,p.y-1.13);
    float spe=pow(max(0.0,dot(reflect(rd,n),l1dir)),2.0);
   	float amb=0.5*smoothstep(0.0,2.0,p.y);
    vec3 col=2.3*dif*vec3(1.32,1.12,0.45)*sha;
    col+=0.8*bac*vec3(1.32,1.10,0.4);
    col+=0.3*sky*vec3(0.0,0.6,1.0);
    col+=vec3(0.5)*spe;
    col+=0.3*amb*vec3(1.0);
    if(p.y-1.1<0.18)col*=vec3(0.4,0.6,0.7);
    return col;
}
    
vec3 camerapath(float t)
{
    vec3 p=vec3(-13.0+3.5*cos(t),3.3,-1.1+2.4*cos(2.4*t+2.0));
	return p;
} 
    
void mainImage( in vec2 fragCoord ) 
{ 
    vec2 q=fragCoord.xy/iResolution.xy; 

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


	vec3 ro = iMoveAmount.xyz / 2 + vec3(0, 1, 0);

    l1dir=normalize(vec3(0., 0.15, -1.1));
   
    vec3 sun_dir=l1dir;
    float sun=clamp(dot(sun_dir,rd),0.0,1.0);
    
    vec3 col=dome(rd,l1dir);
   
    float t=intersect(ro,rd);
	if(t>tmax) // sky
    { 
        col += 0.8*vec3(1.0,0.8,0.7)*pow(sun,512.0);//sun
		col += 0.1*vec3(1.0,0.4,0.2)*pow(sun,4.0);
		vec2 cuv=ro.xz+rd.xz*(10.0-ro.y)/rd.y;//cloud
        vec3 cloud=mix(vec3(1.0,0.7,0.1),vec3(1.0),1.0-sun);
		col=mix(col, cloud, 0.5*pow(texture2D(iChannel0,0.001*cuv).x,1.6));
    }
    else
    {
        vec3 p=ro+t*rd;
        vec3 n=getnormal(p,t);
        col=0.2*vec3(1.0,1.0,1.0)*lighting(n,p,rd);
        col=mix(col, 0.25*vec3(0.4,0.75,1.0), 1.0-exp(-0.006*t*t) );
    }
   	col+=vec3(1.0,0.6,0.2)*0.2*pow(sun,2.0);


    // post
    col=pow(clamp(col,0.0,1.0),vec3(0.45)); 
  
    col = col*1.3*vec3(1.0,1.0,1.02) + vec3(0.0,0.0,0.11);
    col = clamp(col,0.0,1.0);
    col = col*col*(3.0-2.0*col);
    col = mix( col, vec3(dot(col,vec3(0.333))), 0.25 );
    col*=0.5+0.5*pow(16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.7);  // vigneting
 	fragColor = vec4(col.xyz, 1.0); 
 }
