#pragma once

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_renderer.h>
#include <xsi_status.h>

#include "renderer.h"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace XSI;

RenderInstance *get_render_instance();

class LockRendererData
{
protected:
	Renderer& renderer_;
	bool locked_;

public:
	LockRendererData(Renderer& renderer) : renderer_(renderer), locked_(false)
	{

	}

	CStatus lock()
	{
		if (!locked_)
		{
			const CStatus res = renderer_.LockSceneData();
			if (res == CStatus::OK)
			{
				locked_ = true;
			}
			return res;
		}
		return CStatus::OK;
	}

	CStatus unlock()
	{
		if (locked_)
		{
			const CStatus res = renderer_.UnlockSceneData();
			if (res == CStatus::OK)
			{
				locked_ = false;
			}
			return res;
		}
		return CStatus::OK;
	}

	~LockRendererData()
	{
		unlock(); // ensure unlocked happens when this object goes out of scope
	}
};

//! Abort handling.
static bool				g_bAborted;
HANDLE					g_hAbort;
CRITICAL_SECTION		g_barrierAbort;

void set_abort(bool in_bAbort)
{
	::EnterCriticalSection(&g_barrierAbort);
	g_bAborted = in_bAbort;
	if (in_bAbort)
	{
		::SetEvent(g_hAbort);
	}
	else
	{
		::ResetEvent(g_hAbort);
	}
	::LeaveCriticalSection(&g_barrierAbort);
}

bool is_aborted()
{
	::EnterCriticalSection(&g_barrierAbort);
	const bool b_abort = g_bAborted;
	::LeaveCriticalSection(&g_barrierAbort);

	return(b_abort);
}