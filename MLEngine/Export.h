#pragma once

#ifdef MLENGINE_EXPORTS 
#define MLENGINE_API __declspec(dllexport)
#else						/* }{ */
#define MLENGINE_API __declspec(dllimport)
#endif	

