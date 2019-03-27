#pragma once

#include "coremin.h"

/// @brief OpenGL libraries includes
#include <GL/glcorearb.h>
#include <GL/glext.h>

/////////////////////////////////////////////////
// OpenGL functions entry points               //
/////////////////////////////////////////////////

#define GL_ENTRYPOINTS(expansion) \
	expansion(PFNGLENABLEPROC, glEnable) \
	expansion(PFNGLDISABLEPROC, glDisable) \
	expansion(PFNGLCREATEPROGRAMPROC, glCreateProgram) \
	expansion(PFNGLGETPROGRAMIVPROC, glGetProgramiv) \
	expansion(PFNGLLINKPROGRAMPROC, glLinkProgram) \
	expansion(PFNGLUSEPROGRAMPROC, glUseProgram) \
	expansion(PFNGLCREATESHADERPROC, glCreateShader) \
	expansion(PFNGLGETSHADERIVPROC, glGetShaderiv) \
	expansion(PFNGLSHADERSOURCEPROC, glShaderSource) \
	expansion(PFNGLCOMPILESHADERPROC, glCompileShader) \
	expansion(PFNGLATTACHSHADERPROC, glAttachShader) \
	expansion(PFNGLGETUNIFORMLOCATIONARBPROC, glGetUniformLocation) \
	expansion(PFNGLUNIFORM1UIPROC, glUniform1ui) \
	expansion(PFNGLUNIFORM1FVPROC, glUniform1fv) \
	expansion(PFNGLUNIFORM2FVPROC, glUniform2fv) \
	expansion(PFNGLUNIFORM3FVPROC, glUniform3fv) \
	expansion(PFNGLUNIFORM4FVPROC, glUniform4fv) \
	expansion(PFNGLUNIFORM1IVPROC, glUniform1iv) \
	expansion(PFNGLUNIFORM1UIVPROC, glUniform1uiv) \
	expansion(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv) \
	expansion(PFNGLCLEARCOLORPROC, glClearColor) \
	expansion(PFNGLCLEARPROC, glClear) \
	expansion(PFNGLFLUSHPROC, glFlush) \
	expansion(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays) \
	expansion(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray) \
	expansion(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer) \
	expansion(PFNGLGENBUFFERSPROC, glGenBuffers) \
	expansion(PFNGLDELETEBUFFERSPROC, glDeleteBuffers) \
	expansion(PFNGLBINDBUFFERPROC, glBindBuffer) \
	expansion(PFNGLBUFFERDATAPROC, glBufferData) \
	expansion(PFNGLBUFFERSUBDATAPROC, glBufferSubData) \
	expansion(PFNGLNAMEDBUFFERDATAPROC, glNamedBufferData) \
	expansion(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer) \
	expansion(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
	expansion(PFNGLDRAWARRAYSPROC, glDrawArrays) \
	expansion(PFNGLDRAWELEMENTSPROC, glDrawElements) \
	expansion(PFNGLREADBUFFERPROC, glReadBuffer) \
	expansion(PFNGLDRAWBUFFERPROC, glDrawBuffer) \
	expansion(PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer) \
	expansion(PFNGLFINISHPROC, glFinish) \
	expansion(PFNGLBINDTEXTUREPROC, glBindTexture)

#define GL_DECLARE_ENTRYPOINTS(type, func) extern type func;

/// @brief Namespace for OpenGL functions

namespace GLFuncPointers
{
	GL_ENTRYPOINTS(GL_DECLARE_ENTRYPOINTS)
}
using namespace GLFuncPointers;

#undef GL_DECLARE_ENTRYPOINTS


