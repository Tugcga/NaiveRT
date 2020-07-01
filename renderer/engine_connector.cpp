#include "renderer.h"
#include <xsi_vector3.h>
#include <xsi_kinematics.h>
#include <xsi_material.h>
#include <xsi_scene.h>
#include <xsi_materiallibrary.h>
#include <xsi_shader.h>
#include <xsi_project.h>
#include <xsi_color4f.h>

#include "..\loaders\loader_materials.h"
#include "..\loaders\loader_objects.h"
#include "..\loaders\loader_cameras.h"
#include "..\loaders\loader_render_params.h"
#include "..\tools\logger.h"

enum UpdateType
{
	updateType_camera,
	updateType_transform,
	updateType_material,
	updateType_primitive,  // for custom primitive parameters
	updateType_undefined,
	updateType_render,
	updateType_visibility,
};

void RenderInstance::progress_callback(double progress)
{
	render_context.ProgressUpdate("Rendering...", "Rendering...", int(progress * 100));
}

void RenderInstance::update_tile_callback(Tile &tile)
{
	FragmentFloat fragment(tile.in_offX, tile.in_offY, tile.in_width, tile.in_height, tile.pixels, tile.comps, false);
	render_context.NewFragment(fragment);
}

void add_object_to_scene(X3DObject &xsi_object, NRTEngine &nrt_engine, CTime &eval_time, std::map<ULONG, shared_ptr<material>> &scene_materials, std::map<ULONG, shared_ptr<sphere>> &scene_objects)
{
	//try to get material
	Material xsi_mat = xsi_object.GetMaterial();
	ULONG mat_id = xsi_mat.GetObjectID();
	auto it = scene_materials.find(mat_id);
	shared_ptr<sphere> sph;
	if (it != scene_materials.end())
	{
		shared_ptr<material> mat = it->second;
		//add sphere to the engine
		CStatus is_add = add_sphere(nrt_engine, xsi_object, eval_time, mat, scene_objects);
	}
	else
	{
		log("No material for " + xsi_object.GetName() + ", this is impossible");
		//no material in the library
		//in fact it is impossible, because we recognize all materials in the scene
	}
}

//create scene from scratch
void RenderInstance::create_scene(const CRefArray &objects, const Camera &camera, const Property &render_property)
{
	set_render_parameters(nrt_engine, buffer, eval_time, render_property);
	set_camera(nrt_engine, camera, eval_time, render_property.GetParameterValue("aperture", eval_time));
	recognize_materials(scene_materials, Application().GetActiveProject().GetActiveScene(), nrt_engine);

	//filter objects by type
	CStringArray families;
	families.Add(siImplicitGeometryFamily);
	CRefArray spheres;
	objects.Filter("nrtSphere", families, "", spheres);
	for (LONG i = 0; i < spheres.GetCount(); i++)
	{
		X3DObject xsi_object(spheres[i]);
		
		//add sphere to the list of references
		add_object_to_scene(xsi_object, nrt_engine, eval_time, scene_materials, scene_objects);
	}
}

UpdateType get_update_type(CRef &in_ref)
{
	UpdateType to_return = UpdateType::updateType_undefined;
	SIObject xsi_obj = SIObject(in_ref);
	siClassID class_id = in_ref.GetClassID();
	switch (class_id)
	{
	case siStaticKinematicStateID:
	case siKinematicStateID:
	{
		if (strstr(in_ref.GetAsText().GetAsciiString(), ".global"))
		{
			xsi_obj = X3DObject(SIObject(SIObject(in_ref).GetParent()).GetParent());
			if (xsi_obj.IsA(siCameraID))
			{
				to_return = updateType_camera;
			}
			else if (xsi_obj.IsA(siX3DObjectID))
			{
				to_return = updateType_transform;
			}
		}
		break;
	}
	case siCustomPrimitiveID:
	{
		to_return = updateType_primitive;
		break;
	}
	case siMaterialID:
	{
		to_return = updateType_material;
		break;
	}
	case siShaderParameterID:
	{
		log("shader param: " + in_ref.GetAsText());
		break;
	}
	case siParameterID:
	{
		log("parameter: " + in_ref.GetAsText());
		break;
	}
	case siPropertyID:
	{
		if (xsi_obj.GetType() == "visibility")
		{
			to_return = updateType_visibility;
		}
		else
		{
			log("property: " + in_ref.GetAsText() + " " + xsi_obj.GetType());
		}
		break;
	}
	case siCustomPropertyID:
	{
		CString prop_type = xsi_obj.GetType();
		if (prop_type == "Naive_RT_Options")
		{//change render parameter
			to_return = updateType_render;
		}
		break;
	}
	
	}

	return to_return;
}

CStatus RenderInstance::update_scene(const CRefArray &update_objects, const Property &render_property)
{
	//here we should implement interaction with external render engine
	nrt_engine.set_buffer(buffer.full_width, buffer.full_height, buffer.corner_x, buffer.corner_y, buffer.width, buffer.height);

	bool update_render_props = false;
	for (LONG i = 0; i < update_objects.GetCount(); i++)
	{
		CRef obj(update_objects[i]);
		UpdateType update_type = get_update_type(obj);
		
		if (update_type != updateType_undefined)
		{
			SIObject xsi_obj = SIObject(obj);
			if (update_type == updateType_camera)
			{
				Camera camera = X3DObject(SIObject(xsi_obj.GetParent()).GetParent());
				set_camera(nrt_engine, camera, eval_time, render_property.GetParameterValue("aperture", eval_time));
			}
			else if (update_type == updateType_transform)
			{
				X3DObject xsi_object = X3DObject(SIObject(xsi_obj.GetParent()).GetParent());
				if (xsi_object.GetType() == "nrtSphere")
				{
					CStatus is_update = update_sphere(xsi_object.GetObjectID(), xsi_object.GetKinematics().GetGlobal().GetTransform().GetTranslation(), xsi_object.GetParameterValue("radius", eval_time), scene_objects);
					//if fail to update, add it to the scene
					add_object_to_scene(xsi_object, nrt_engine, eval_time, scene_materials, scene_objects);
				}
			}
			else if (update_type == updateType_visibility)
			{
				//no methods on the nrt_engine to delete objects from scene, so, in this case we should recreate the scene
				return CStatus::Fail;
			}
			else if (update_type == updateType_material)
			{
				Material mat = Material(xsi_obj);
				bool is_update = update_material(mat, scene_materials);
				// if return false, then the material update is fail
				if (!is_update)
				{
					return CStatus::Fail;
				}
			}
			else if (update_type == updateType_primitive)
			{
				CString primitive_type = xsi_obj.GetType();
				if (primitive_type == "nrtSphere")
				{
					X3DObject prim_parent = xsi_obj.GetParent();
					update_sphere(prim_parent.GetObjectID(), prim_parent.GetKinematics().GetGlobal().GetTransform().GetTranslation(), prim_parent.GetParameterValue("radius", eval_time), scene_objects);
				}
			}
			else if (update_type == updateType_render)
			{
				update_render_props = true;
			}
		}	
	}

	if (update_render_props)
	{
		set_render_parameters(nrt_engine, buffer, eval_time, render_property);
		nrt_engine.set_camera_aperture(render_property.GetParameterValue("aperture", eval_time));
	}

	return CStatus::OK;
}