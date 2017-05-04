void mainImage( in vec2 fragCoord )
{
	vec2 pix = vec2((iFractalWindow.z - iFractalWindow.x) / iResolution.x, (iFractalWindow.w - iFractalWindow.y) / iResolution.y);
	vec2 p = vec2(iFractalWindow.x + fragCoord.x * pix.x, iFractalWindow.y + fragCoord.y * pix.y);

	p *= 2.;
    p -= 1.;	
	p += vec2(0.274,0.482);
	
	vec2 zc = vec2(0, 0);
	vec2 zp = zc;
	
	float l = 0.95;
	
	float d = 1e20;
	float d2 = 1e20;
	float d3 = 1e20;
	for(int i = 0; i < 100; i++)
	{	
		zp = zc;
		zc = p + vec2(zc.x * zc.x - zc.y * zc.y, 2. * zc.x * zc.y);
		
		vec2 z = zp + (zc - zp) * sin((iGlobalTime - 30.) * 0.1) * 5;
		
		float dist1 = abs(z.x * z.x * z.x - z.y * z.y);
		float dist2 = abs(z.x * z.x * z.x - z.y * z.y * z.x);
		
		float dist3 = min(dist1, dist2); //dist1 + (dist2 - dist1) * l;
		
		d = min(d, dist3);
		
		float dist4 = abs(z.x * z.x + z.y * z.y);
		float dist5 = abs(z.x * z.x * z.y * z.y);
		
		float dist6 = dist4 + (dist5 - dist4) * l;
		
		d2 = min(d2, dist6);	
		
		float dist7 = abs(z.x * z.x - z.y * z.y);
		float dist8 = abs(z.x * z.x - z.y * z.y);
		
		float dist9 = dist7 + (dist8 - dist7) * l;
		
		d3 = min(d3, dist9);
	}
	
	d = pow(abs(1. - sqrt(abs(d))), (sin(iGlobalTime) + 2) * 10);
	d2 = pow(abs(1. - sqrt(abs(d2))), (cos(iGlobalTime) + 2) * 10);
	d3 = pow(abs(1. - sqrt(abs(d3))), 10);
	
	fragColor = vec4( d, d2, d3, 1.0 );
}
