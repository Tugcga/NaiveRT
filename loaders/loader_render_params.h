#pragma once
#include <xsi_time.h>
#include <xsi_property.h>

#include "../nrt_core/nrt_engine.h"
#include "../renderer/render_buffer.h"

using namespace XSI;

void set_render_parameters(NRTEngine &nrt_engine, const RenderBuffer &buffer, CTime &eval_time, const Property &render_property);