#pragma iChannel0 textures/nyancat.png nearest
#pragma iChannel1 textures/nyancat.png nearest
// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// Instead of using a pont, circle, line or any mathematical shape for traping the orbit
// of fc(z), one can use any arbitrary shape. For example, a NyanCat :)
//
// I invented this technique more than 10 years ago (can have a look to those experiments 
// here http://www.iquilezles.org/www/articles/ftrapsbitmap/ftrapsbitmap.htm).

vec4 getNyanCatColor( vec2 p, float time )
{
	p = clamp(p,0.0,1.0);
	p.x = p.x*40.0/256.0;
	p.y = 0.5 + 1.2*(0.5-p.y);
	p = clamp(p,0.0,1.0);
	float fr = floor( mod( 20.0*time, 6.0 ) );
	p.x += fr*40.0/256.0;
	return texture2D( iChannel0, p );
}

void mainImage( in vec2 fragCoord )
{
	//	vec2 p = -1.0+2.0*fragCoord.xy / iResolution.xy;
	//	p.x *= iResolution.x/iResolution.y;

    float time = iGlobalTime;

    // zoom	
	//	p = vec2(0.5,-0.05)  + p*0.75 * pow( 0.9, 20.0*(0.5+0.5*cos(0.25*time)) );
	vec2 pix = vec2((iFractalWindow.z - iFractalWindow.x) / iResolution.x, (iFractalWindow.w - iFractalWindow.y) / iResolution.y);
	vec2 p = vec2(iFractalWindow.x + fragCoord.x * pix.x, iFractalWindow.y + fragCoord.y * pix.y);

    vec4 col = vec4(0.0);
	vec3 s = mix( vec3( 0.2,0.2, 1.0 ), vec3( 0.5,-0.2,0.5), 0.5+0.5*sin(0.5*time) );

    // iterate Jc	
	vec2 c = vec2(-0.76, 0.15);
	float f = 0.0;
	vec2 z = p;
	for( int i=0; i< 150; i++ )
	{
		if( (dot(z,z)>4.0) || (col.w>0.1) ) continue; // break;

        // fc(z) = zÂ² + c		
		z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;

		col = getNyanCatColor( s.xy + s.z*z, time );
		f += 1.0;
	}

	vec3 bg = 0.5*vec3(1.0,0.5,0.5) * sqrt(f/100.0);

	col.xyz = mix( bg, col.xyz, col.w );


	fragColor = vec4( col.xyz,1.0);
}
