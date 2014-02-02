# Find and configure Miles Sound System
find_package(Miles QUIET)
#config_package(RAD_MSS "Miles Sound System")
package_option(RAD_MSS)

# Find and configure FMOD Ex
find_package(FMODEx QUIET)
#config_package(FMODEX "FMOD Ex sound library")
package_option(FMODEX)

# Find and configure OpenAL
find_package(OpenAL QUIET)
#config_package(OPENAL "OpenAL sound library")
package_option(OPENAL)


# Find and configure Freetype
find_package(Freetype QUIET)
#config_package(FREETYPE "Freetype")
package_option(FREETYPE)

# Find and configure GTK
set(Freetype_FIND_QUIETLY TRUE) # Fix for builtin FindGTK2
set(GTK2_GTK_FIND_QUIETLY TRUE) # Fix for builtin FindGTK2
find_package(GTK2 QUIET COMPONENTS gtk)
#config_package(GTK2 "gtk+-2")
package_option(GTK2)

# Find and configure WxWidgets
find_package(wxWidgets QUIET)
if(WXWIDGETS_FOUND)
  set(WX_FOUND TRUE) # Mangle for convenience
endif()
# Cleanup after builtin FindWx
mark_as_advanced(wxWidgets_CONFIG_EXECUTABLE)
mark_as_advanced(wxWidgets_wxrc_EXECUTABLE)
#config_package(WX "WxWidgets")
package_option(WX)

# Find and configure FLTK
set(OpenGL_FIND_QUIETLY TRUE) # Fix for builtin FindFLTK
find_package(FLTK QUIET)
mark_as_advanced(FLTK_BASE_LIBRARY) # Cleanup after builtin FLTK
mark_as_advanced(FLTK_CONFIG_SCRIPT) # Cleanup after builtin FLTK
mark_as_advanced(FLTK_FLUID_EXECUTABLE) # Cleanup after builtin FLTK
mark_as_advanced(FLTK_FORMS_LIBRARY) # Cleanup after builtin FLTK
mark_as_advanced(FLTK_GL_LIBRARY) # Cleanup after builtin FLTK
mark_as_advanced(FLTK_IMAGES_LIBRARY) # Cleanup after builtin FLTK
mark_as_advanced(FLTK_INCLUDE_DIR) # Cleanup after builtin FLTK
#config_package(FLTK)
package_option(FLTK)

# Cleanup after builtin FindFLTK
mark_as_advanced(FLTK_DIR)
mark_as_advanced(FLTK_MATH_LIBRARY)


########
# TODO #
########

# Find and configure PhysX
#find_package(PhysX)
#config_package(PHYSX "Aegia PhysX")

# Find and configure SpeedTree
#find_package(SpeedTree)
#config_package(SPEEDTREE "SpeedTree")

# Find and configure OpenGL ES 1
#find_package(GLES)
#config_package(GLES COMMENT "OpenGL ES 1")

# Find and configure OpenGL ES 2
#find_package(GLES)
#config_package(GLES COMMENT "OpenGL ES 2")

# Find and configure DirectX 8
#find_package(DX8)
#config_package(DX8 COMMENT "DirectX8")

# Find and configure DirectX 9
#find_package(DX9)
#config_package(DX9 COMMENT "DirectX9")

# Find and configure DirectX 11
#find_package(DX11)
#config_package(DX11 COMMENT "DirectX11")

# Find and configure Mesa
#find_package(Mesa)
#config_package(MESA COMMENT "Mesa")

# Find and configure OpenCV
#find_package(OpenCV)
#config_package(OPENCV COMMENT "OpenCV")

# Find and configure FFMPEG
#find_package(FFMPEG)
#config_package(FFMPEG)

# Find and configure ODE
#find_package(ODE)
#config_package(ODE)

# Find and configure Awesomium
#find_package(Awesomium)
#config_package(AWESOMIUM COMMENT "Awesomium")

# Find and configure OpenMaya
#find_package(OpenMaya)
#config_package(MAYA COMMENT "OpenMaya")

# Find and configure FCollada
#find_package(FCollada)
#config_package(FCOLLADA COMMENT "FCollada")
#if(FOUND_COLLADA14DOM OR FOUND_COLLADA15DOM)
# set(USE_COLLADA TRUE CACHE BOOL "If true, compile Panda3D with COLLADA DOM")
# if(USE_COLLADA)
#   if(FOUND_COLLADA15DOM)
#     set(HAVE_COLLADA15DOM TRUE)
#   else()
#     set(HAVE_COLLADA14DOM TRUE)
#   endif()
# endif()
#endif()

# Find and configure Assimp
#find_package(Assimp)
#config_package(ASSIMP COMMENT "Assimp")

# Find and configure ARToolKit
#find_package(ARToolKit)
#config_package(ARTOOLKIT COMMENT "ARToolKit")

# Find and configure libRocket
#find_package(Rocket)
#config_package(ROCKET COMMENT "libRocket")
#if(HAVE_ROCKET AND HAVE_PYTHON)
# # Check for rocket python bindings
# if(FOUND_ROCKET_PYTHON)
#   option(USE_ROCKET_PYTHON "If on, compile Panda3D with python bindings for libRocket" ON)
#   if(USE_ROCKET_PYTHON)
#     set(HAVE_ROCKET_PYTHON TRUE)
#   endif()
#   else()
#       unset(USE_ROCKET_PYTHON CACHE)
# endif()
# if(HAVE_ROCKET_PYTHON)
#   message(STATUS "+ libRocket with Python bindings")
# else()
#   message(STATUS "+ libRocket without Python bindings")
# endif()
#else()
# unset(USE_ROCKET_PYTHON CACHE)
#endif()

# Find and configure Bullet
#find_package(Bullet)
#config_package(BULLET COMMENT "Bullet Physics")

# Find and configure Vorbis
#find_package(Vorbis)
#config_package(VORBIS COMMENT "Vorbis Ogg decoder")
