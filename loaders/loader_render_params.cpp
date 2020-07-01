#include "loader_render_params.h"

void set_render_parameters(NRTEngine &nrt_engine, const RenderBuffer &buffer, CTime &eval_time, const Property &render_property)
{
	nrt_engine.set_buffer(buffer.full_width, buffer.full_height, buffer.corner_x, buffer.corner_y, buffer.width, buffer.height);

	nrt_engine.set_params(
		render_property.GetParameterValue("aa_samples", eval_time),
		render_property.GetParameterValue("max_depth", eval_time),
		color(render_property.GetParameterValue(L"ground_R", eval_time), render_property.GetParameterValue(L"ground_G", eval_time), render_property.GetParameterValue(L"ground_B", eval_time)),
		color(render_property.GetParameterValue(L"sky_R", eval_time), render_property.GetParameterValue(L"sky_G", eval_time), render_property.GetParameterValue(L"sky_B", eval_time)));
}