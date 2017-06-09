/*
Created by soma_arc, Kazushi Ahara - 2015
This work is licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported.
*/

// from Syntopia http://blog.hvidtfeldts.net/index.php/2015/01/path-tracing-3d-fractals/
vec2 rand2n(vec2 co, float sampleIndex) {
    vec2 seed = co * (sampleIndex + 1.0);
	seed+=vec2(-1,1);
    // implementation based on: lumina.sourceforge.net/Tutorials/Noise.html
    return vec2(fract(sin(dot(seed.xy ,vec2(12.9898,78.233))) * 43758.5453),
                fract(cos(dot(seed.xy ,vec2(4.898,7.23))) * 23421.631));
}

const vec2 c1Pos = vec2(0, 1);
const vec2 c2Pos = vec2(4, 4);
const vec2 c3Pos = vec2(-4, 4);
const float c1R = 1.;
const float c2R = 4.;
const float c3R = 4.;

vec2 circleInverse(vec2 pos, vec2 circlePos, float circleR){
	return ((pos - circlePos) * circleR * circleR)/(length(pos - circlePos) * length(pos - circlePos) ) + circlePos;
}

const int ITERATIONS = 13;
float IIS(vec2 pos){
    float loopNum = 0.;
	bool cont = false;
	for(int i = 0 ; i < ITERATIONS ; i++){
		cont = false;
		if(distance(pos,c1Pos) < c1R){
			pos = circleInverse(pos, c1Pos, c1R);
			cont = true;
            loopNum++;
		}else if(distance(pos, c2Pos) < c2R){
			pos = circleInverse(pos, c2Pos, c2R);
			cont = true;
            loopNum++;
		}else if(distance(pos, c3Pos) < c3R){
			pos = circleInverse(pos, c3Pos, c3R);
			cont = true;
            loopNum++;
		}else if(pos.y < 0.){
			pos = vec2(pos.x, -pos.y);
            cont = true;
            loopNum++;
		}
		if(cont == false) break;
	}

	return loopNum;
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

const float SAMPLE_NUM = 8.;
void mainImage( in vec2 fragCoord ){
    vec3 sum = vec3(0);
	float ratio = iResolution.x / iResolution.y / 2.0;
    for(float i = 0. ; i < SAMPLE_NUM ; i++){
        vec2 position = ( (fragCoord.xy + rand2n(fragCoord.xy, i)) / iResolution.yy ) - vec2(ratio, 0.5);
		position = position * iMoveAmount.w * 4 + iMoveAmount.xy;

        float loopNum = IIS(position);

        if(loopNum != 0.){
            sum += hsv2rgb(vec3(iGlobalTime + 0.04 * loopNum,1.0,1.0));
        }else{
            sum += vec3(0.,0.,0.);
        }
    }
    fragColor = vec4(sum/SAMPLE_NUM, 1.);
}
