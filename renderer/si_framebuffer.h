#pragma once
#include <xsi_renderercontext.h>
#include <xsi_framebuffer.h>

using namespace XSI;

CStringArray get_output_paths(RendererContext& ctxt, CTime& time)
{
	CStringArray to_return;
	CRefArray frame_buffers = ctxt.GetFramebuffers();
	const LONG fb_count = frame_buffers.GetCount();
	for (LONG i = 0; i < fb_count; ++i)
	{
		Framebuffer fb(frame_buffers[i]);
		CValue enbable_val = fb.GetParameterValue("Enabled", time.GetTime());
		if (enbable_val)
		{
			to_return.Add(fb.GetResolvedPath(time));
		}
	}
	return to_return;
}