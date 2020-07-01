#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // Needed for OpenGL on windows

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <xsi_customprimitive.h>

#include "primitives/nrt_primitives.h"

using namespace XSI; 

extern GLUquadric* g_quadric = NULL;

SICALLBACK XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"Shekn");
	in_reg.PutName(L"Naive RT");
	in_reg.PutVersion(0, 1);
	//RegistrationInsertionPoint - do not remove this line
	in_reg.RegisterProperty(L"Naive RT Options");
	in_reg.RegisterRenderer(L"Naive RT");

	in_reg.RegisterEvent(L"NRT_OnObjectAdded", siOnObjectAdded);
	in_reg.RegisterEvent(L"NRT_OnObjectRemoved", siOnObjectRemoved);

	in_reg.RegisterPrimitive("nrtSphere");
	in_reg.RegisterMenu(siMenuTbGetPrimitiveID, L"NRTPrimitives", true, true);

	if (g_quadric != NULL)
	{
		::gluDeleteQuadric(g_quadric);
		g_quadric = NULL;
	}
	g_quadric = ::gluNewQuadric();
	if (!g_quadric)
	{
		return CStatus::Fail;
	}
	::gluQuadricDrawStyle(g_quadric, GLU_SILHOUETTE);

	return CStatus::OK;
}

SICALLBACK XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
	CString strPluginName;
	strPluginName = in_reg.GetName();
	Application().LogMessage(strPluginName + L" has been unloaded.", siVerboseMsg);
	return CStatus::OK;
}

SICALLBACK nrtSphere_Draw(const CRef& in_ref)
{
	Context in_ctxt(in_ref);
	CustomPrimitive in_prim(in_ctxt.GetSource());
	if (!in_prim.IsValid())
	{
		return CStatus::Fail;
	}
	CParameterRefArray& params = in_prim.GetParameters();
	double radius = params.GetValue("radius");
	draw_nrtSphere(radius, g_quadric);

	return CStatus::OK;
}