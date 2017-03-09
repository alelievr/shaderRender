// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

void mainImage( in vec2 fragCoord )
{
	vec2 pix = vec2((iFractalWindow.z - iFractalWindow.x) / iResolution.x, (iFractalWindow.w - iFractalWindow.y) / iResolution.y);
	vec2 p = vec2(iFractalWindow.x + fragCoord.x * pix.x, iFractalWindow.y + fragCoord.y * pix.y);

    vec2 cc = 1.1*sin(0.002 * (iMouse.xy - iResolution / 2));

	vec4 dmin = vec4(1000.0);
    vec2 z = p*vec2(1.7,1.0);
    for( int i=0; i<64; i++ )
    {
        z = cc + vec2( z.x*z.x - z.y*z.y, 2.0*z.x*z.y );

		dmin=min(dmin, vec4(abs(0.0+z.y + 0.5*sin(z.x)), 
							abs(1.0+z.x + 0.5*sin(z.y)), 
							dot(z,z),
						    length( fract(z)-0.5) ) );
    }
    
    vec3 color = vec3( dmin.w );
	color = mix( color, vec3(1.00,0.80,0.60),     min(1.0,pow(dmin.x*0.25,0.20)) );
    color = mix( color, vec3(0.72,0.70,0.60),     min(1.0,pow(dmin.y*0.50,0.50)) );
	color = mix( color, vec3(1.00,1.00,1.00), 1.0-min(1.0,pow(dmin.z*1.00,0.15) ));

	color = 1.25*color*color;
	
	color *= 0.5 + 0.5*pow(16.0*p.x*(1.0-p.x)*p.y*(1.0-p.y),0.15);

	fragColor = vec4(color,1.0);
}
