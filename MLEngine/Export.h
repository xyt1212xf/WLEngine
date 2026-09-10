#pragma once

#ifdef WLENGINE_EXPORTS 
#define WLENGINE_API __declspec(dllexport)
#else						/* }{ */
#define WLENGINE_API __declspec(dllimport)
#endif	

