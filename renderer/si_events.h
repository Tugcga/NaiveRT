#pragma once
#include <xsi_status.h>
#include <xsi_renderercontext.h>
#include <xsi_application.h>

using namespace XSI;


CStatus begin_render_event(RendererContext& ctxt, CStringArray& output_paths)
{
	CStatus status;

	const siRenderingType render_type = (ctxt.GetSequenceLength() > 1) ? siRenderSequence : siRenderFramePreview;
	if (ctxt.GetSequenceIndex() == 0)
	{
		status = ctxt.TriggerEvent(siOnBeginSequence, render_type, ctxt.GetTime(), output_paths, siRenderFieldNone);
	}

	if (status != CStatus::OK)
	{
		return status;
	}
	status = ctxt.TriggerEvent(siOnBeginFrame, render_type, ctxt.GetTime(), output_paths, siRenderFieldNone);

	return status;
}

CStatus end_render_event(RendererContext& ctxt, CStringArray& output_paths, bool in_skipped)
{
	CStatus status;
	const siRenderingType render_type = (ctxt.GetSequenceLength() > 1) ? siRenderSequence : siRenderFramePreview;
	if (!in_skipped)
	{
		status = ctxt.TriggerEvent(siOnEndFrame, render_type, ctxt.GetTime(), output_paths, siRenderFieldNone);

		if (status != CStatus::OK)
		{
			return status;
		}
	}
	if (ctxt.GetSequenceIndex() + 1 == ctxt.GetSequenceLength())
	{
		status = ctxt.TriggerEvent(siOnEndSequence, render_type, ctxt.GetTime(), output_paths, siRenderFieldNone);
	}

	return status;
}