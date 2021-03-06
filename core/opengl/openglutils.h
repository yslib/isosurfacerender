#ifndef _OPENGLUTILS_H_
#define _OPENGLUTILS_H_

#include "../utility/error.h"


inline
GLenum PrintGLErrorMsg(const char * file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		ysl::Warning("%s | %s (%d)", error.c_str(), file, line);
	}
	return errorCode;
}

#ifdef NDEBUG
#define GL_ERROR_REPORT void(0);
#define GL_ERROR_ASSERT	void(0);
#else
#define GL_ERROR_REPORT								PrintGLErrorMsg(__FILE__, __LINE__);
	//{												\
	//	GLenum err;									\
	//	while((err = glGetError()) != GL_NO_ERROR)	\
	//	{											\
	//		ysl::Warning("OpenGL Error Code:%d. File:%s, Line:%d. \n",err,__FILE__,__LINE__);\
	//	}											\
	//}
	

#define GL_ERROR_ASSERT							\
		assert(glGetError() == GL_NO_ERROR);	\


#endif /*NDBUG*/

#endif /*_OPENGLUTILS_H_*/