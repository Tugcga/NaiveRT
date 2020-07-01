#include "nrt_primitives.h"

using namespace XSI;

#define kParamCaps	(siAnimatable | siPersistable | siKeyable)

SICALLBACK NRTPrimitives_Init(const CRef& in_ref)
{
	// retrieve the menu object to build
	Menu menu = Context(in_ref).GetSource();

	// set the menu caption
	menu.PutName(L"&NaiveRT Primitives");

	// adds other menu items and attach a function callback
	MenuItem menuItem;
	menu.AddCallbackItem(L"Sphere", L"OnSphereItem", menuItem);

	return CStatus::OK;
}

SICALLBACK OnSphereItem(CRef& in_ref)
{
	Context ctxt(in_ref);
	MenuItem menuItem = ctxt.GetSource();

	Application app;
	CValue out_arg;
	CValueArray in_args;
	in_args.Add(L"nrtSphere");
	app.ExecuteCommand("GetPrim", in_args, out_arg);

	return CStatus::OK;
}

SICALLBACK nrtSphere_Define(const CRef& in_ref)
{
	Context in_ctxt(in_ref);
	CustomPrimitive in_prim(in_ctxt.GetSource());
	if (in_prim.IsValid())
	{
		Factory fact = Application().GetFactory();
		CRef radiusDef = fact.CreateParamDef("radius", CValue::siDouble, kParamCaps, "radius", "", 1.0, 0.0, DBL_MAX, 0.0, 12.0);
		Parameter radius;
		in_prim.AddParameter(radiusDef, radius);
	}
	return CStatus::OK;
}

SICALLBACK nrtSphere_DefineLayout(CRef& in_ctxt)
{
	Context ctxt(in_ctxt);
	PPGLayout oLayout;
	PPGItem oItem;
	oLayout = ctxt.GetSource();
	oLayout.Clear();

	oLayout.AddGroup("Parameters");
	oLayout.AddItem("radius", "Radius");
	oLayout.EndGroup();

	return CStatus::OK;
}


SICALLBACK nrtSphere_BoundingBox(const CRef& in_ref)
{
	Context in_ctxt(in_ref);
	CustomPrimitive in_prim(in_ctxt.GetSource());
	if (!in_prim.IsValid())
	{
		return CStatus::Fail;
	}

	CParameterRefArray& params = in_prim.GetParameters();
	double radius = params.GetValue("radius");

	in_ctxt.PutAttribute("LowerBoundX", -radius);
	in_ctxt.PutAttribute("LowerBoundY", -radius);
	in_ctxt.PutAttribute("LowerBoundZ", -radius);
	in_ctxt.PutAttribute("UpperBoundX", radius);
	in_ctxt.PutAttribute("UpperBoundY", radius);
	in_ctxt.PutAttribute("UpperBoundZ", radius);

	return CStatus::OK;
}

void draw_nrtSphere(double radius, GLUquadric* g_quadric)
{
	::gluSphere(g_quadric, radius, 12, 12);
}