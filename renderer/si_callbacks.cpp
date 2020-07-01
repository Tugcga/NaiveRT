#include "si_callbacks.h"

RenderInstance g_RenderRegionPass;

RenderInstance* render;
RenderInstance* get_render_instance() { return render; }

SICALLBACK NaiveRT_Init(CRef &in_ctxt)
{
	Context		ctxt(in_ctxt);
	Renderer	renderer = ctxt.GetSource();

	CLongArray	process;
	process.Add(siRenderProcessRender);
	renderer.PutProcessTypes(process);

	renderer.AddProperty(siRenderPropertyOptions, L"Naive RT.Naive RT Options");

	renderer.AddOutputImageFormat(L"Portable Anymap (PPM)", L"ppm");
	renderer.AddOutputImageFormatSubType(siRenderChannelColorType, L"RGB", siImageBitDepthInteger8);

	renderer.AddOutputImageFormat(L"Joint Photographic Experts Group (JPG)", L"jpg");
	renderer.AddOutputImageFormatSubType(siRenderChannelColorType, L"RGB", siImageBitDepthInteger8);

	renderer.AddOutputImageFormat("Portable Network Graphics (PNG)", "png");
	renderer.AddOutputImageFormatSubType(siRenderChannelColorType, "RGB", siImageBitDepthInteger8);

	render = &g_RenderRegionPass;

	g_bAborted = false;
	::InitializeCriticalSection(&g_barrierAbort);
	g_hAbort = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	return(CStatus::OK);
}

SICALLBACK NaiveRT_Term(CRef &in_ctxt)
{
	::DeleteObject(g_hAbort);
	::DeleteCriticalSection(&g_barrierAbort);

	g_hAbort = NULL;
	::ZeroMemory(&g_barrierAbort, sizeof(g_barrierAbort));

	return(CStatus::OK);
}

SICALLBACK NaiveRT_Cleanup(CRef &in_ctxt)
{
	render->clear();

	return(CStatus::OK);
}

SICALLBACK NaiveRT_Abort(CRef &in_ctxt)
{
	render->abort();

	set_abort(true);

	return(CStatus::OK);
}

SICALLBACK NaiveRT_Quality(CRef &in_ctxt)
{
	return CStatus::OK;
}

SICALLBACK NaiveRT_Query(CRef &in_ctxt)
{
	RendererContext	ctxt(in_ctxt);
	const int type = ctxt.GetAttribute(L"QueryType");
	switch (type)
	{
	case siRenderQueryArchiveIsValid:
	{
		break;
	}
	case siRenderQueryWantDirtyList:
	{
		ctxt.PutAttribute(L"WantDirtyList", true);
		break;
	}
	case siRenderQueryDisplayBitDepths:
	{
		CLongArray bitDepths;
		bitDepths.Add(siImageBitDepthInteger8);
		bitDepths.Add(siImageBitDepthInteger16);

		CString softimage_version_string = Application().GetVersion();
		CStringArray softimageVersion = softimage_version_string.Split(L".");
		if (atoi(softimageVersion[0].GetAsciiString()) >= 10)
		{
			bitDepths.Add(siImageBitDepthFloat32);
		}

		ctxt.PutAttribute(L"BitDepths", bitDepths);
		break;
	}
	default:;
#if XSISDK_VERSION > 11000
	case siRenderQueryHasPreMulAlphaOutput:
	{
		ctxt.PutAttribute(L"HasPreMulAlphaOutput", false);
		break;
	}
#endif      
	}

	return CStatus::OK;
}


SICALLBACK NaiveRT_Process(CRef &in_ctxt)
{
	set_abort(false);
	CStatus status;

	RendererContext	ctxt(in_ctxt);
	Renderer renderer = ctxt.GetSource();

	if (!render->is_ready_to_render())
	{
		return CStatus::Abort;
	}

	status = render->pre_render(ctxt);  // here we actualy set the ref to context, calculate time and so on
	if (status != CStatus::OK)
	{
		return CStatus::Abort;
	}

	LockRendererData locker = LockRendererData(renderer);
	if (locker.lock() != CStatus::OK)
	{
		return CStatus::Abort;
	}

	render->update_scene();  // here we transfer data from XSI scene to the render engine scene

	if (locker.unlock() != CStatus::OK)
	{
		render->interrupt_update_scene();
		return CStatus::Abort;
	}

	if (is_aborted())
	{
		return CStatus::Abort;
	}

	status = render->start_render();  // command to start rendering

	if (status == CStatus::OK)
	{
		status = render->post_render();
	}
	
	return status;
}

SICALLBACK NRT_OnObjectAdded_OnEvent(CRef& in_ctxt)
{
	get_render_instance()->on_object_add(in_ctxt);
	return CStatus::OK;
}


SICALLBACK NRT_OnObjectRemoved_OnEvent(CRef& in_ctxt)
{
	get_render_instance()->on_object_remove(in_ctxt);
	return CStatus::OK;
}
