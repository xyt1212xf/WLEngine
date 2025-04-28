
material = {}
material =
{
	AmbientLight = "0.1,0.1,0.1,1";
	DiffuseLight = "0.6,0.6,0.6,1";
	LightDirection = "0.577,0.577,0.577,1";
	psConstBuffer = 
	{
		[1] = 
		{
			name = "AmbientLight",
			size = 16,
			registerIndex = 7,
		},
		[2] = 
		{
			name = "DiffuseLight",
			size = 16,
			registerIndex = 7,
		},
		[3] = 
		{
			name = "LightDirection",
			size = 16,
			registerIndex = 7,
		},
		[4] = 
		{
			name = "LightView",
			size = 64,
			registerIndex = 7,
		},
		[5] = 
		{
			name = "LightProjection",
			size = 64,
			registerIndex = 7,
		},
	},
	
}
