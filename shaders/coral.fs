//Original formula from quick-dudley at fractalforums.com
//http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/

#define time iGlobalTime
#define size iResolution

vec3 C;

float DE(in vec3 p){
	float dr=1.0,r=length(p);
	for (int n = 0; n < 10; n++) if(r<2000.0){
		dr = dr * r * 2.0; 
		p=vec3(p.x*p.x-2.0*p.y*p.z,2.0*p.x*p.y+p.z*p.z,-2.0*p.x*p.z+p.y*p.y)+C;
		r = length(p);
	}
	return min(log(r) * (r-1.0) / dr,.8);
}

float rnd(vec2 c){return fract(sin(dot(vec2(1.317,19.753),c))*413.7972);}
float rndStart(vec2 fragCoord){
	return 0.5+0.5*rnd(fragCoord.xy+vec2(time*217.0));
}
float shadao(vec3 ro, vec3 rd, float px, vec2 fragCoord){//pretty much IQ's SoftShadow
	float res=1.0,d,t=2.0*px*rndStart(fragCoord);
	for(int i=0;i<4;i++){
		d=max(px,DE(ro+rd*t)*1.5);
		t+=d;
		res=min(res,d/t+t*0.1);
	}
	return res;
}
vec3 Sky(vec3 rd){//what sky??
	return vec3(0.5+0.5*rd.y);
}
vec3 L;
vec3 Color(vec3 ro, vec3 rd, float t, float px, vec3 col, bool bFill, vec2 fragCoord){
	ro+=rd*t;
	float d=DE(ro);
	vec2 e=vec2(px*t,0.0);
	vec3 dn=vec3(DE(ro-e.xyy),DE(ro-e.yxy),DE(ro-e.yyx));
	vec3 dp=vec3(DE(ro+e.xyy),DE(ro+e.yxy),DE(ro+e.yyx));
	vec3 N=(dp-dn)/(length(dp-vec3(d))+length(vec3(d)-dn));
	vec3 R=reflect(rd,N);
	vec3 lc=vec3(1.0,0.9,0.8),sc=vec3(0.9,0.5,0.4),rc=Sky(R);
	float sh=clamp(shadao(ro,L,px*t,fragCoord)+0.2,0.0,1.0);
	sh=sh*(0.5+0.5*dot(N,L))*exp(-t*0.125);
	vec3 scol=sh*lc*(sc+rc*pow(max(0.0,dot(R,L)),4.0));
	if(bFill)d*=0.2;
	col=mix(scol,col,clamp(d/(px*t),0.0,1.0));
	return col;
}
mat3 lookat(vec3 fw){
	fw=normalize(fw);vec3 rt=normalize(cross(fw,vec3(0.0,1.0,0.0)));return mat3(rt,cross(rt,fw),fw);
}
vec3 Julia(float t){
	t=mod(t,7.0);
	if(t<1.0)return vec3(-2.0,0.0,0.0);
	if(t<2.0)return vec3(0.0,-.64,-1.52);
	if(t<3.0)return vec3(-0.72);
	if(t<4.0)return vec3(-0.96,0.16,-0.6);
	if(t<5.0)return vec3(0.4,-1.06,-2.0);
	if(t<6.0)return vec3(-0.2,-0.48,-1.52);
	return vec3(-1.54,0.06,0.22);
}
void mainImage(in vec2 fragCoord ) {
	float px=0.5/size.y;
	L=normalize(vec3(0.4,0.8,-0.6));
	float tim=time*0.5;
	
	vec3 ro=vec3(cos(tim*1.3),sin(tim*0.7),sin(tim))*3.0;
	vec3 rd=lookat(vec3(-0.1)-ro)*normalize(vec3((2.0*fragCoord.xy-size.xy)/size.y,3.0));
	//ro=eye;rd=normalize(dir);
	
	C=mix(Julia(tim-1.0),Julia(tim),smoothstep(0.0,1.0,fract(tim)*5.0));
	
	float t=DE(ro)*rndStart(fragCoord),d=0.0,od=10.0;
	vec3 edge=vec3(-1.0);
	bool bGrab=false;
	vec3 col=Sky(rd);
	for(int i=0;i<78;i++){
		t+=d*0.2;
		d=DE(ro+rd*t);
		if(d>od){
			if(bGrab && od<px*t && edge.x<0.0){
				edge=vec3(edge.yz,t-od);
				bGrab=false;
			}
		}else bGrab=true;
		od=d;
		if(t>1000.0 || d<0.00001)break;
	}
	bool bFill=false;
	d*=0.2;
	if(d<px*t && t<1000.0){
		if(edge.x>0.0)edge=edge.zxy;
		edge=vec3(edge.yz,t);
		bFill=true;
	}
	for(int i=0;i<3;i++){
		if(edge.z>0.0)col=Color(ro,rd,edge.z,px,col,bFill,fragCoord);
		edge=edge.zxy;
		bFill=false;
	}
	fragColor = vec4(2.0*col,1.0);
}
