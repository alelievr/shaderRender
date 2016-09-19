vec4 fold1(vec4 z) {
    vec3 p = z.xyz;
    p = p - 2.0 * clamp(p, -1.0, 1.0);
    return vec4(p, z.w);
}

vec4 fold2(vec4 z) {
    vec3 p = z.xyz;
    p = p - 2.0 * clamp(p, -1.0, 1.0);
    return vec4(p * 2.0, 2.0 * z.w);
}

vec4 invertRadius(vec4 z, float radius2, float limit) {
  float r2 = dot(z.xyz, z.xyz);
  float f = clamp(radius2 / r2, 1., limit);
  return z * f;
}

vec4 affine(vec4 z, float factor, vec3 offset) {
  z.xyz *= factor;
  z.xyz += offset;
  z.w *= abs(factor);
  return z;
}

vec4 mandel(vec4 z, vec3 offset) {
  float x = z.x;
  float y = z.y;

  z.w = 2. * length(z.xy) * z.w + 1.;

  z.x = x*x - y*y + offset.x;
  z.y = 2.*x*y + offset.y;

  return z;
}

vec4 invert(vec4 z, float factor) {
  float r2 = dot(z.xyz, z.xyz);
  float f = factor / r2;
  return z * f;
}

vec4 rotateXY(vec4 z, float angle) {
  float c = cos(angle);
  float s = sin(angle);
  mat2 m = mat2(c, s, -s, c);
  return vec4(m * z.xy, z.zw);
}

vec4 rotateXZ(vec4 z, float angle) {
  float c = cos(angle);
  float s = sin(angle);
  mat2 m = mat2(c, s, -s, c);
  vec2 r = m * z.xz;
  return vec4(r.x, z.y, r.y, z.w);
}

vec4 shiftXY(vec4 z, float angle, float radius) {
  float c = cos(angle);
  float s = sin(angle);
  return vec4(vec2(c, s) * radius + z.xy, z.zw);
}

float surface = 0.0;
float sdf(vec3 p) {
    //vec3 pmod = mod(p + 2.0, 4.0) - 2.0;
    vec4 z = vec4(p, 1.0);
    
    float t = iGlobalTime * .2;

    vec3 vo1 = vec3(sin(t * .1), cos(t * .0961), sin(t * .017)) * 1.1;
    vec3 vo2 = vec3(cos(t * .07), sin(t * .0533), sin(t * .138)) * 1.1;
    vec3 vo3 = vec3(sin(t * .031), sin(t * .0449), cos(t * .201)) * 1.1;
    
    z = invertRadius(z, 10.0, 1.5);
    z = invertRadius(z, 10.0*10.0, 2.0);
    z = rotateXY(z, t);
    z = fold1(z);
    z = rotateXZ(z, t * 1.112);
    z.xyz += vo3;
    z = fold2(z);
    z.xyz += vo1;
    z = affine(z, -1.5, p);
    z = invertRadius(z, 4.0*4.0, 2.0);
    z = affine(z, -1.5, p);
    z = rotateXY(z, t * .881);
    z = fold1(z);
    z = rotateXZ(z, t * .783);
    z = fold1(z);
    z = affine(z, -1.5, p);
    z = invertRadius(z, 10.0*10.0, 3.0);
    z = fold1(z);
    z = fold1(z);
    z = affine(z, -1.5, p);
    z = invertRadius(z, 10.0*10.0, 2.0);


    vec3 po = vec3(0.0, 0.0, 0.0);

    vec3 box = abs(z.xyz);
    float d1 = (max(box.x - 2.0, max(box.y - 2.0, box.z - 10.0))) / z.w;
    float d2 = (max(box.x - 20.0, max(box.y - .5, box.z - .5))) / z.w;
	
    float d3 = min(d1, d2);
    if (d2 == d3) {
      surface = 1.0;
    }
    else {
      surface = 0.0;
    }
    return d3;
}

vec3 normal(vec3 p) {
	vec2 e = vec2(.0001, 0.0);
    float f = sdf(p);
    return normalize(vec3(
        sdf(p + e.xyy) - f,
        sdf(p + e.yxy) - f,
        sdf(p + e.yyx) - f
    ));
}

void mainImage( in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 xy = (uv * 2.0 - 1.0) * vec2(iResolution.x / iResolution.y, 1.0);

    vec3 color = vec3(.03, .02, .02) * (1.5 - uv.y);

    vec3 light = vec3(.577);
    float angle = 1.0-iMouse.x * .005;   
    float c = cos(angle);
    float s = sin(angle);
    mat3 camera = mat3(
        c,  0.0, -s,
         0.0,  1.0, 0.0,
        s,  0.0, c//,
	);
    float fov = .5;

    /*vec3 rd = camera * normalize(vec3(xy * fov,-1.0));
    vec3 ro = vec3(s,0.0,c)*6.0;*/
	vec3 rd = iForward;
	vec3 ro = iMoveAmount.xyz;

    
    float tmax = 100.0;
    float dmin = .001;
    float t = 0.0;
    float d = sdf(ro);
    if (d < 0.0) rd = -rd;
    for (int i = 0; i < 96; ++i) {
        vec3 p = ro + rd * t;
        d = sdf(p);
        t += d;
        if (t > tmax || d < dmin) break;
    }

    vec3 p = ro + rd * t;
    vec3 n = normal(p);
    if (t < tmax && d < dmin) {
      vec3 albedo = mix(vec3(.02, .1, 1.0), vec3(1.0), surface);
  	  float diffuse = clamp(dot(n, light), 0.0, 1.0);
      
      float ao = 0.0;
      float am = 0.0;
      float step = .05;
      vec3 pa = p;
      for (int i = 0; i < 8; ++i) {
          pa += n * step;
          am += step;
          ao += max(0.0, sdf(pa) / am);
          step += .05;
      }
      
      t = .1;
      float shadow = 1.0;
      if (diffuse > 0.0) {
        for (int i = 0; i < 24; ++i) {
          vec3 pb = p + light * t;
          d = sdf(pb);
          t += d;
          if (d < 0.0) break;
        }
        
        shadow = clamp(d, 0.0, 1.0);
        diffuse *= shadow;
      }
        
      ao = ao / 8.0;
      float ambient = 0.08;

      vec3 sun = vec3(1.0, 1.0, .85);
      vec3 sky = vec3(1.0, .5, .5);
      color = 6.0 * vec3(mix(sun * diffuse, sky, ambient) * ao * ao * albedo * 5.0);
      color = color/(1.0+color);
    }
    
	fragColor = vec4(sqrt(color),1.0);
}
