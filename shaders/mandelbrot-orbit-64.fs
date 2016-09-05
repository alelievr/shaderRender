
void mainImage( in vec2 fragCoord )
{
	//fractal pixel calcul block
	dvec2 uvMouse = (iMouse.xy / (iResolution / 2.)) / (iResolution / 2.) / 10.;

	dvec2 pix = dvec2((iFractalWindow.z - iFractalWindow.x) / iResolution.x, (iFractalWindow.w - iFractalWindow.y) / iResolution.y);
	dvec2 p = dvec2(iFractalWindow.x + fragCoord.x * pix.x, iFractalWindow.y + fragCoord.y * pix.y);

	p *= 2.;
    p -= 1.;	
	p += dvec2(0.274,0.482);
	
	dvec2 zc = dvec2(0, 0);
	dvec2 zp = zc;
	
	double l = 0.95;
	
	double d = 1e20;
	double d2 = 1e20;
	double d3 = 1e20;
	for(int i = 0; i < 50; i++)
	{	
		zp = zc;
		zc = p + dvec2(zc.x * zc.x - zc.y * zc.y, 2. * zc.x * zc.y);
		
		dvec2 z = zp + (zc - zp) * sin((iGlobalTime - 30.) * 0.1) * 5.;
		
		double dist1 = abs(z.x * z.x * z.x - z.y * z.y);
		double dist2 = abs(z.x * z.x * z.x - z.y * z.y * z.x);
		
		double dist3 = min(dist1, dist2); //dist1 + (dist2 - dist1) * l;
		
		d = min(d, dist3);
		
		double dist4 = abs(z.x * z.x + z.y * z.y);
		double dist5 = abs(z.x * z.x * z.y * z.y);
		
		double dist6 = dist4 + (dist5 - dist4) * l;
		
		d2 = min(d2, dist6);	
		
		double dist7 = abs(z.x * z.x - z.y * z.y);
		double dist8 = abs(z.x * z.x - z.y * z.y);
		
		double dist9 = dist7 + (dist8 - dist7) * l;
		
		d3 = min(d3, dist9);		
	}
	
	d = pow(float(abs(1. - sqrt(abs(d)))), (sin(iGlobalTime) + 2) * 10);
	d2 = pow(float(abs(1. - sqrt(abs(d2)))), (cos(iGlobalTime) + 2) * 10);
	d3 = pow(float(abs(1. - sqrt(abs(d3)))), 10);
	
	fragColor = vec4( d, d2, d3, 1.0 );
}
