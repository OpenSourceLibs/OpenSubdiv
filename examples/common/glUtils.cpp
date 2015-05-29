//
//   Copyright 2015 Pixar
//
//   Licensed under the Apache License, Version 2.0 (the "Apache License")
//   with the following modification; you may not use this file except in
//   compliance with the Apache License and the following modification to it:
//   Section 6. Trademarks. is deleted and replaced with:
//
//   6. Trademarks. This License does not grant permission to use the trade
//      names, trademarks, service marks, or product names of the Licensor
//      and its affiliates, except as required to comply with Section 4(c) of
//      the License and to reproduce the content of the NOTICE file.
//
//   You may obtain a copy of the Apache License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the Apache License with the above modification is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//   KIND, either express or implied. See the Apache License for the specific
//   language governing permissions and limitations under the Apache License.
//

#include <sstream>
#include <string>
#include "glUtils.h"

namespace GLUtils {

// Note that glewIsSupported is required here for Core profile, glewGetExtension
// and GLEW_extension_name will not work on all drivers.
#ifdef OSD_USES_GLEW
#define IS_SUPPORTED(x) \
   (glewIsSupported(x) == GL_TRUE)
#endif

void
CheckGLErrors(std::string const & where) {
    GLuint err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "GL error: "
                  << (where.empty() ? "" : where + " ")
                  << err << "\n";
    }
}

GLuint
CompileShader(GLenum shaderType, const char *source) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar emsg[40960];
        glGetShaderInfoLog(shader, sizeof(emsg), 0, emsg);
        fprintf(stderr, "Error compiling GLSL shader: %s\n", emsg);
        return 0;
    }

    return shader;
}

bool
SupportsAdaptiveTessellation() {
#ifdef OSD_USES_GLEW
    return IS_SUPPORTED("GL_ARB_tessellation_shader");
#else
#if defined(GL_ARB_tessellation_shader) || defined(GL_VERSION_4_0)
    return true;
#else
    return false;
#endif
#endif
}

///Helper function that parses the open gl version string, retrieving the major 
///and minor version from it.
void GetMajorMinorVersion(int *major, int *minor){
    const GLubyte *ver = glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (!ver){
        *major = -1;
        *minor = -1;
    }
    else{
        std::stringstream ss;
        ss << std::string(ver, ver + 1) << " " << std::string(ver + 2, ver + 3);
        ss >> *major;
        ss >> *minor;
    }
}

/** Gets the shader version based on the current opengl version and returns 
 * it in a string form */

std::string
GetShaderVersion(){
    std::string shader_version;
    int major, minor;
    GetMajorMinorVersion(&major, &minor);
    int version_number = major * 10 + minor;
    switch (version_number){
    case 20:
        shader_version = "110";
        break;
    case 21:
        shader_version = "120";
        break;
    case 30:
        shader_version = "130";
        break;
    case 31:
        shader_version = "140";
        break;
    case 32:
        shader_version = "150";
        break;
    default:
        std::stringstream ss;
        ss << version_number;
        shader_version = ss.str() + "0";
        break;
    }
    return shader_version;
}

/* Generates the version defintion needed by the glsl shaders based on the 
 * opengl string
*/
std::string GetShaderVersionInclude(){
    return "#version " + GetShaderVersion() + "\n";
}

bool GL_ARBSeparateShaderObjectsOrGL_VERSION_4_1(){
#if defined(OSD_USES_GLEW)
    return IS_SUPPORTED("GL_ARB_separate_shader_objects") ||
            (GLEW_VERSION_4_1 && IS_SUPPORTED("GL_ARB_tessellation_shader"));
#else
#if defined(GL_ARB_separate_shader_objects) || defined(GL_VERSION_4_1)
    return true;
#else
    return false;
#endif
#endif
}

bool GL_ARBComputeShaderOrGL_VERSION_4_3() {
#if defined(OSD_USES_GLEW)
    return IS_SUPPORTED("GL_ARB_compute_shader") ||
           (GLEW_VERSION_4_3);
#else
#if defined(GL_ARB_compute_shader) || defined(GL_VERSION_4_3)
    return true;
#else
    return false;
#endif
#endif
}

#undef IS_SUPPORTED

}   // namesapce GLUtils
