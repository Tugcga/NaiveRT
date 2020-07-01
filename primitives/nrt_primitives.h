#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // Needed for OpenGL on windows

#include <GL/gl.h>
#include <GL/glu.h>

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>

#include <xsi_customprimitive.h>
#include <xsi_factory.h>
#include <xsi_parameter.h>
#include <xsi_ppglayout.h>

#include <xsi_menu.h>

void draw_nrtSphere(double radius, GLUquadric* g_quadric);