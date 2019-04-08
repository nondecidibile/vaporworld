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
	expansion(PFNGLGETERRORPROC, glGetError) \
	expansion(PFNGLGETINTEGERVPROC, glGetIntegerv) \
	expansion(PFNGLGETINTEGERI_VPROC, glGetIntegeri_v) \
	expansion(PFNGLCLEARPROC, glClear) \
	expansion(PFNGLCLEARCOLORPROC, glClearColor) \
	expansion(PFNGLFLUSHPROC, glFlush) \
	expansion(PFNGLFINISHPROC, glFinish) \
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
	expansion(PFNGLUNIFORM1FPROC, glUniform1f) \
	expansion(PFNGLUNIFORM1IPROC, glUniform1i) \
	expansion(PFNGLUNIFORM1UIPROC, glUniform1ui) \
	expansion(PFNGLUNIFORM1FVPROC, glUniform1fv) \
	expansion(PFNGLUNIFORM2FVPROC, glUniform2fv) \
	expansion(PFNGLUNIFORM3FVPROC, glUniform3fv) \
	expansion(PFNGLUNIFORM4FVPROC, glUniform4fv) \
	expansion(PFNGLUNIFORM1IVPROC, glUniform1iv) \
	expansion(PFNGLUNIFORM2IVPROC, glUniform2iv) \
	expansion(PFNGLUNIFORM1UIVPROC, glUniform1uiv) \
	expansion(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv) \
	expansion(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers) \
	expansion(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers) \
	expansion(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer) \
	expansion(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D) \
	expansion(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays) \
	expansion(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray) \
	expansion(PFNGLGENBUFFERSPROC, glGenBuffers) \
	expansion(PFNGLDELETEBUFFERSPROC, glDeleteBuffers) \
	expansion(PFNGLISBUFFERPROC, glIsBuffer) \
	expansion(PFNGLBINDBUFFERPROC, glBindBuffer) \
	expansion(PFNGLBINDBUFFERBASEPROC, glBindBufferBase) \
	expansion(PFNGLBINDVERTEXBUFFERPROC, glBindVertexBuffer) \
	expansion(PFNGLBUFFERDATAPROC, glBufferData) \
	expansion(PFNGLNAMEDBUFFERDATAPROC, glNamedBufferData) \
	expansion(PFNGLBUFFERSUBDATAPROC, glBufferSubData) \
	expansion(PFNGLNAMEDBUFFERSUBDATAPROC, glNamedBufferSubData) \
	expansion(PFNGLBUFFERSTORAGEPROC, glBufferStorage) \
	expansion(PFNGLNAMEDBUFFERSTORAGEPROC, glNamedBufferStorage) \
	expansion(PFNGLGETBUFFERSUBDATAPROC, glGetBufferSubData) \
	expansion(PFNGLGETNAMEDBUFFERSUBDATAPROC, glGetNamedBufferSubData) \
	expansion(PFNGLMAPBUFFERRANGEPROC, glMapBufferRange) \
	expansion(PFNGLMAPNAMEDBUFFERRANGEPROC, glMapNamedBufferRange) \
	expansion(PFNGLUNMAPBUFFERPROC, glUnmapBuffer) \
	expansion(PFNGLUNMAPNAMEDBUFFERPROC, glUnmapNamedBuffer) \
	expansion(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer) \
	expansion(PFNGLVERTEXATTRIBIPOINTERPROC, glVertexAttribIPointer) \
	expansion(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
	expansion(PFNGLVERTEXATTRIBFORMATPROC, glVertexAttribFormat) \
	expansion(PFNGLVERTEXATTRIBBINDINGPROC, glVertexAttribBinding) \
	expansion(PFNGLGENTEXTURESPROC, glGenTextures) \
	expansion(PFNGLDELETETEXTURESPROC, glDeleteTextures) \
	expansion(PFNGLACTIVETEXTUREPROC, glActiveTexture) \
	expansion(PFNGLBINDTEXTUREPROC, glBindTexture) \
	expansion(PFNGLBINDTEXTURESPROC, glBindTextures) \
	expansion(PFNGLBINDIMAGETEXTUREPROC, glBindImageTexture) \
	expansion(PFNGLBINDIMAGETEXTURESPROC, glBindImageTextures) \
	expansion(PFNGLTEXIMAGE2DPROC, glTexImage2D) \
	expansion(PFNGLTEXIMAGE3DPROC, glTexImage3D) \
	expansion(PFNGLTEXPARAMETERIPROC, glTexParameteri) \
	expansion(PFNGLPIXELSTOREIPROC, glPixelStorei) \
	expansion(PFNGLPIXELSTOREFPROC, glPixelStoref) \
	expansion(PFNGLGENERATEMIPMAPPROC, glGenerateMipMap) \
	expansion(PFNGLDRAWARRAYSPROC, glDrawArrays) \
	expansion(PFNGLDRAWELEMENTSPROC, glDrawElements) \
	expansion(PFNGLPOLYGONMODEPROC, glPolygonMode) \
	expansion(PFNGLPOINTSIZEPROC, glPointSize) \
	expansion(PFNGLDISPATCHCOMPUTEPROC, glDispatchCompute) \
	expansion(PFNGLMEMORYBARRIERPROC, glMemoryBarrier) \
	expansion(PFNGLFENCESYNCPROC, glFenceSync) \
	expansion(PFNGLREADBUFFERPROC, glReadBuffer) \
	expansion(PFNGLDRAWBUFFERPROC, glDrawBuffer) \
	expansion(PFNGLDRAWBUFFERSPROC, glDrawBuffers) \
	expansion(PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer) \

#define GL_DECLARE_ENTRYPOINTS(type, func) extern type func;

/// @brief Namespace for OpenGL functions

namespace GLFuncPointers
{
	GL_ENTRYPOINTS(GL_DECLARE_ENTRYPOINTS)
}
using namespace GLFuncPointers;

#undef GL_DECLARE_ENTRYPOINTS


