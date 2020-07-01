#include "renderer.h"
#include "si_events.h"
#include "si_framebuffer.h"

#include "../tools/write_image.h"

#include <functional>

using namespace std;
using namespace std::placeholders;

RenderInstance::RenderInstance()
{
	force_recreate_scene = true;
	ready_to_render = true;
	prev_isolated_objects.Resize(0);
	scene_objects.clear();
	scene_materials.clear();
	nrt_engine.set_progress_callback(std::bind(&RenderInstance::progress_callback, this, _1));
	nrt_engine.set_update_tile_callback(std::bind(&RenderInstance::update_tile_callback, this, _1));
}

RenderInstance::~RenderInstance()
{
	
}

void RenderInstance::on_object_add(CRef& in_ctxt)
{
	force_recreate_scene = true;
}

void RenderInstance::on_object_remove(CRef& in_ctxt)
{
	force_recreate_scene = true;
}

CStatus RenderInstance::pre_render(RendererContext &r_ctxt)
{
	CStatus status;

	render_context = r_ctxt;
	eval_time = render_context.GetTime();

	output_paths = get_output_paths(render_context, eval_time);

	return CStatus::OK;
}

void clean_dirty_list(RendererContext &render_context, CValue &dirty_refs_value)
{
	bool empty_dirty_list = dirty_refs_value.IsEmpty();
	if (empty_dirty_list)
	{
		render_context.SetObjectClean(CRef());
	}
	else
	{
		CRefArray dirty_refs = dirty_refs_value;
		for (int i = 0; i < dirty_refs.GetCount(); i++)
		{
			render_context.SetObjectClean(dirty_refs[i]);
		}
	}
}

bool is_object_in_array(const CRefArray &array, const CRef &object)
{
	for (LONG i = 0; i < array.GetCount(); i++)
	{
		if (object == array[i])
		{
			return true;
		}
	}

	return false;
}

bool RenderInstance::is_recreate_isolated_view(CRefArray &visible_objects)
{
	ULONG current_isolated = visible_objects.GetCount();
	ULONG prev_isolated = prev_isolated_objects.GetCount();
	if (current_isolated != prev_isolated)
	{//switch from one mode to other
		if (current_isolated == 0)
		{//now the mode is non-isolated
			prev_isolated_objects.Clear();
		}
		else
		{//the mode is isolated, save links to objects
			prev_isolated_objects.Clear();
			for (LONG i = 0; i < visible_objects.GetCount(); i++)
			{
				prev_isolated_objects.Add(visible_objects[i]);
			}
		}

		return true;
	}

	//mode is the same as in the previous render session
	if (current_isolated == 0)
	{// no isolation
		prev_isolated_objects.Clear();
		return false;
	}
	else
	{//isolation, and previous is also isolation
		bool is_recreate = false;
		for (LONG i = 0; i < visible_objects.GetCount(); i++)
		{
			if (!is_object_in_array(prev_isolated_objects, visible_objects[i]))
			{
				i = visible_objects.GetCount();
				is_recreate = true;
			}
		}
		//recreate links
		prev_isolated_objects.Clear();
		for (LONG i = 0; i < visible_objects.GetCount(); i++)
		{
			prev_isolated_objects.Add(visible_objects[i]);
		}
		return is_recreate;
	}
}

void RenderInstance::update_scene()
{
	ready_to_render = false;

	render_type = render_context.GetAttribute("RenderType");
	CRefArray visible_objects = render_context.GetAttribute(L"ObjectList");
	//get object in the isolated view
	if (visible_objects.GetCount() > 0)
	{//add to objects in isolated view all lights
		CRefArray lights_array = render_context.GetAttribute(L"Lights");
		for (LONG i = 0; i < lights_array.GetCount(); i++)
		{
			CRef ref = lights_array.GetItem(i);
			if (!is_object_in_array(visible_objects, ref))
			{
				visible_objects.Add(ref);
			}
		}
	}

	//get active camera
	Primitive camera_prim = render_context.GetAttribute(L"Camera");
	X3DObject camera_obj = camera_prim.GetOwners()[0];
	Camera camera = camera_obj;

	//render property
	Property render_property = render_context.GetRendererProperty(eval_time);

	//create render buffer
	buffer = RenderBuffer(
		(ULONG)render_context.GetAttribute(L"ImageWidth"),
		(ULONG)render_context.GetAttribute(L"ImageHeight"),
		(ULONG)render_context.GetAttribute(L"CropLeft"),
		(ULONG)render_context.GetAttribute(L"CropBottom"),
		(ULONG)render_context.GetAttribute(L"CropWidth"),
		(ULONG)render_context.GetAttribute(L"CropHeight"),
		3);  // use 3 components for the image

	CValue dirty_refs_value = render_context.GetAttribute(L"DirtyList");
	bool empty_dirty_list = dirty_refs_value.IsEmpty();
	bool is_isolated_new = is_recreate_isolated_view(visible_objects);
	
	bool is_create_scene = false;
	if (!(force_recreate_scene || empty_dirty_list || is_isolated_new))
	{//try to update
		CRefArray dirty_refs = dirty_refs_value;
		CStatus update_status = update_scene(dirty_refs, render_property);
		clean_dirty_list(render_context, dirty_refs_value);
		if (update_status == CStatus::Fail)  // fail to update, recreate it from scratch
		{
			is_create_scene = true;
		}
	}
	else
	{
		is_create_scene = true;
	}

	if (is_create_scene)
	{
		clear(false);  // clear all data (here and on the engine) befor creating new scene

		if (visible_objects.GetCount() > 0)
		{// for isolated view
			create_scene(visible_objects, camera, render_property);
		}
		else
		{//for all scene view
			CRefArray& all_objects_list = Application().FindObjects(siX3DObjectID);
			create_scene(all_objects_list, camera, render_property);
		}
		//after creation scene, we should clear all objects from dirty list
		clean_dirty_list(render_context, dirty_refs_value);
	}

	force_recreate_scene = false;
}

CStatus RenderInstance::interrupt_update_scene()
{
	ready_to_render = true;
	return end_render_event(render_context, output_paths, true);
}

CStatus RenderInstance::render()
{
	//rendering process

	// Notify the renderer manager that a new frame is about to begin. This is necessary so
	// that any recipient tile sink can re-adjust its own size to accommodate.
	render_context.NewFrame(buffer.full_width, buffer.full_height);

	nrt_engine.render();

	return CStatus::OK;
}

CStatus RenderInstance::start_render()
{
	CStatus status;
	
	if (begin_render_event(render_context, output_paths) != CStatus::OK)
	{
		return CStatus::Fail;
	}

	render();

	if (end_render_event(render_context, output_paths, false) != CStatus::OK)
	{
		return CStatus::Fail;
	}
	ready_to_render = true;
	return status;
}

CStatus RenderInstance::post_render()
{
	if (output_paths.GetCount() > 0)
	{//save the buffer to the first path
		CString out_path = CString(output_paths[0]);

		std::vector<float> pixels = nrt_engine.get_buffer_pixels();
		write(out_path.GetAsciiString(), buffer.width, buffer.height, buffer.components, pixels);
	}

	return CStatus::OK;
}

void RenderInstance::abort()
{
	nrt_engine.abort();
}

void RenderInstance::clear(bool full_clear)
{
	if (full_clear)
	{
		prev_isolated_objects.Clear();
	}
	buffer.clean();
	ready_to_render = true;
	force_recreate_scene = true;
	scene_objects.clear();
	scene_materials.clear();

	nrt_engine.clear();
}