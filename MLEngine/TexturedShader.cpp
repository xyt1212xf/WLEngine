#include "TexturedShader.h"

namespace ML
{
	IMPLEMENT_GLOBAL_SHADER(CTexturedVS, "Textured.hlsl", "TexturedVS", "vs_5_1")
	IMPLEMENT_GLOBAL_SHADER(CTexturedPS, "Textured.hlsl", "TexturedPS", "ps_5_1")
}
