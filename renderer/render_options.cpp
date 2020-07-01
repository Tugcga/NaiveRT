#include "render_options.h"

using namespace XSI;

SICALLBACK NaiveRTOptions_Define(CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);
	Parameter param;
	CValue dft;
	const int caps = siPersistable | siAnimatable;
	CustomProperty prop = ctxt.GetSource();

	prop.AddParameter(L"ground_R", CValue::siDouble, caps, "Color", "", 0.5, param);
	prop.AddParameter(L"ground_G", CValue::siDouble, caps, "", "", 0.7, param);
	prop.AddParameter(L"ground_B", CValue::siDouble, caps, "", "", 0.88, param);

	prop.AddParameter(L"sky_R", CValue::siDouble, caps, "Color", "", 1.0, param);
	prop.AddParameter(L"sky_G", CValue::siDouble, caps, "", "", 1.0, param);
	prop.AddParameter(L"sky_B", CValue::siDouble, caps, "", "", 1.0, param);

	prop.AddParameter("aa_samples", CValue::siInt4, caps, "", "", 4, 1, 4096*8, 1, 128, param);
	prop.AddParameter("max_depth", CValue::siInt4, caps, "", "", 5, 1, 1024, 1, 16, param);

	prop.AddParameter(L"aperture", CValue::siDouble, caps, "", "", 0.0, 0.0, 1024.0, 0.0, 1.0, param);

	return CStatus::OK;
}

SICALLBACK NaiveRTOptions_DefineLayout(CRef& in_ctxt)
{
	Context ctxt(in_ctxt);
	PPGLayout oLayout;
	PPGItem oItem;
	oLayout = ctxt.GetSource();
	oLayout.Clear();

	oLayout.AddGroup("Gradient");
	oLayout.AddColor(L"sky_R", L"Sky");
	oLayout.AddColor(L"ground_R", L"Ground");
	oLayout.EndGroup();

	oLayout.AddGroup("Quality");
	oLayout.AddItem("aa_samples", "Samples");
	oLayout.AddItem("max_depth", "Max Depth");
	oLayout.EndGroup();

	oLayout.AddGroup("Camera");
	oLayout.AddItem("aperture", "Camera Aperture");
	oLayout.EndGroup();
	return CStatus::OK;
}