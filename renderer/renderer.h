#pragma once

//#include <xsi_renderer.h>
#include <xsi_renderercontext.h>
#include <xsi_application.h>
#include <xsi_primitive.h>
#include <xsi_x3dobject.h>
#include <xsi_camera.h>

#include <vector>
#include <map>

#include "fragments.h"
#include "render_buffer.h"
//#include "../color_core/color_engine.h"
#include "../nrt_core/nrt_engine.h"
#include "../nrt_core/tile.h"

using namespace XSI;

class RenderInstance
{
public:
	RenderInstance();
	~RenderInstance();

	CStatus interrupt_update_scene();  // call if render abort after updating scene, but before starting render process
	void update_scene();  // update scene data (or create the new scene, it it needed)
	CStatus start_render();  // start the render process
	void abort();  // interrupt the render process
	void clear(bool full_clear = true);  // clear all scene data, but doest not remove RenderInstance
	CStatus pre_render(RendererContext &r_ctxt);  // call before blocking and getting scene data
	CStatus post_render();  // call after render is finish

	void on_object_add(CRef& in_ctxt);
	void on_object_remove(CRef& in_ctxt);

	bool is_ready_to_render() { return ready_to_render; }

private:
	CStatus render();  // actual render command
	void create_scene(const CRefArray &objects, const Camera &camera, const Property &render_property);
	CStatus update_scene(const CRefArray &update_objects, const Property &render_property);
	bool is_recreate_isolated_view(CRefArray &visible_objects);

	RendererContext render_context;
	void progress_callback(double progress);
	void update_tile_callback(Tile &tile);

	RenderBuffer buffer;
	CStringArray output_paths;
	CString render_type;
	CTime eval_time;

	bool ready_to_render;
	bool force_recreate_scene;  // when true, then at the next render call the scene should be recteated, even if there are no any changes
								// active it after an object created or removed
	CRefArray prev_isolated_objects;  // here we store links to objects in the isolated view from the last render session
	std::map<ULONG, shared_ptr<sphere>> scene_objects;  // key - unique id, values - nrt_engine sphere object
	std::map<ULONG, shared_ptr<material>> scene_materials;  // key - unique id, values - nrt_engine material

	//render engine object
	//ColorEngine color_engine;
	NRTEngine nrt_engine;
};
