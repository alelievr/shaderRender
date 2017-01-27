//fawlty fountain by eiffie
//I was wondering how to fake raytraced metaballs and then I saw this by ztri...
//https://www.shadertoy.com/view/4dfGzM
//So I stole the main idea of finding the average of closest points but I don't
//even attempt a depth check so pretty lame. Anyways <3 <3 <3

#define time iGlobalTime*0.5
#define size iResolution

vec3 getBackground( in vec3 rd ){return texture(iChannel0,rd.xy).rgb;}

vec3 scene(vec3 ro, vec3 rd) 
{// find color of scene
	float d=-ro.y/rd.y,BR=0.01;//this BR is the square of the blob radius
	vec3 P=vec3(0.0),cp=vec3(0.0,-2.0,0.0),NP=vec3(0.0,1.0,0.0);
	bool bPool=false;
	if(d>0.0){P=ro+rd*d;bPool=(length(P.xz)<6.0);}
	vec3 col=getBackground(rd);
	for(float i=0.0;i<63.0;i+=1.0){
		float r=fract(time+i*0.01),q=1.25+sin(i*27.39)*0.1;
		float a=i*0.4;
		vec2 p=vec2(abs(sin(a*2.0))*0.5,sin(a));
		if(a>1.57 && a<4.71){//making a <3
			float f=sin(a);
			f*=f*sign(f);
			p=vec2(cos(a),f);
		}
		p.x*=1.33;
		vec3 sc=vec3(p.x*r,sin(r*3.1416),p.y*r)*q;//sphere center
		vec3 tc=ro+rd*dot(sc-ro,rd)-sc;			// the idea of finding the average closest point
		float w=smoothstep(BR*2.0,0.0,dot(tc,tc));//came from https://www.shadertoy.com/view/4dfGzM
		cp=mix(cp,sc,w);						// me maths are even crazier :) but my output isn't as pretty :(
		if(bPool){//add up offsets to the normal if the ray hits the pool
			p=P.xz-p*q;
			a=r;r=length(p);
			p=p/(1.0+r)*sin((pow(r,0.75)-a)*40.0)*max(0.0,(1.0-a))*smoothstep(0.0,1.0,(pow(a,0.25)-r));
			NP+=vec3(p.x,0.0,p.y);
		}
	}
	vec3 L=normalize(vec3(0.1,0.8,0.3));//light it up
	if(bPool){//draw the pool of pink goo
		vec3 N=normalize(NP);
		vec3 refl=reflect(rd,N);
		vec3 scol=getBackground(refl)*max(0.0,dot(N,L));
		scol+=vec3(1.0)*pow(max(0.0,dot(refl,L)),32.0);
		col=mix(vec3(1.0,0.7,0.8)*scol,col,smoothstep(5.8,6.0,length(P.xz)));
	}
	vec3 tc=cp-ro;//pretend we have a single sphere centered at the average of closest
	float b=dot(tc,rd);
	float h=b*b-dot(tc,tc)+BR;
	if(h>0.001){
		float t=b-sqrt(h);
		P=ro+rd*t;
		vec3 N=normalize(P-cp);
		vec3 refl=reflect(rd,N);
		vec3 scol=2.0*getBackground(refl)*(0.5+0.5*dot(N,L));
		scol+=vec3(1.0)*pow(max(0.0,dot(refl,L)),32.0);
		col=mix(col,vec3(1.0,0.7,0.8)*scol,smoothstep(0.0,0.4*BR,h)*smoothstep(0.0,0.1,P.y));
	}
	return col;
}	

mat3 lookat(vec3 fw,vec3 up){
	fw=normalize(fw);vec3 rt=normalize(cross(fw,up));return mat3(rt,cross(rt,fw),fw);
}

void mainImage( in vec2 fragCoord ) {
	vec3 ro=vec3(2.0*sin(time*0.25),1.0,2.0*cos(time*0.25));
	mat3 rotCam=lookat(vec3(0.0,0.25,0.0)-ro,vec3(0.0,1.0,0.0));
	vec3 rd=vec3((2.0*fragCoord.xy-size.xy)/size.y,1.25);
	vec3 clr=scene(ro,normalize(rotCam*rd));

	fragColor=vec4(clamp(clr,0.0,1.0),1.0);
}
