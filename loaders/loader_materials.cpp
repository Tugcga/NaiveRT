#include "loader_materials.h"
#include "../tools/strings.h"

#include <xsi_materiallibrary.h>
#include <xsi_shader.h>
#include <xsi_color4f.h>
#include <xsi_parameter.h>
#include <xsi_application.h>

CString get_shader_type(const Shader &shader)
{
	CString type = "";
	CString progId = shader.GetProgID();
	//extract type from prog_id
	std::vector<ULONG> pos = get_symbol_positions(progId, '.');
	if (pos.size() >= 2)
	{
		//get part to the first point
		CString app_name = progId.GetSubString(0, pos[0]);
		if (app_name == "NRTShadersPlugin")
		{
			CString sh_name = progId.GetSubString(pos[0] + 1, pos[1] - pos[0] - 1);
			if (sh_name.Length() > 3)
			{
				type = sh_name.GetSubString(3, sh_name.Length());
			}
		}
	}
	return type;
}

shared_ptr<material> convert_material(const Material &xsi_mat, bool &is_convert)
{
	CRefArray shaders = xsi_mat.GetShaders();  // only the first level shaders
	for (LONG k = 0; k < shaders.GetCount(); k++)
	{
		Shader shader(shaders[k]);
		CRefArray subshaders = shader.GetAllShaders();
		if (subshaders.GetCount() == 0)
		{
			CString shader_type = get_shader_type(shader);
			if (shader_type == "Lambertian")
			{
				CParameterRefArray params = shader.GetParameters();
				MATH::CColor4f albedo = ((Parameter)params.GetItem("albedo")).GetValue();

				is_convert = true;
				return make_shared<lambertian>(color(albedo.GetR(), albedo.GetG(), albedo.GetB()));
				break;
			}
			else if (shader_type == "Metal")
			{
				CParameterRefArray params = shader.GetParameters();
				MATH::CColor4f albedo = ((Parameter)params.GetItem("albedo")).GetValue();
				float fuzz = ((Parameter)params.GetItem("fuzz")).GetValue();
				shared_ptr<metal> met = make_shared<metal>(color(albedo.GetR(), albedo.GetG(), albedo.GetB()), fuzz);

				is_convert = true;
				return make_shared<metal>(color(albedo.GetR(), albedo.GetG(), albedo.GetB()), fuzz);
				break;
			}
			else if (shader_type == "Dielectric")
			{
				CParameterRefArray params = shader.GetParameters();
				float ref_idx = ((Parameter)params.GetItem("ref_idx")).GetValue();
				shared_ptr<dielectric> met = make_shared<dielectric>(ref_idx);

				is_convert = true;
				return make_shared<dielectric>(ref_idx);
				break;
			}
		}
	}
	is_convert = false;
	return nullptr;
}

bool update_material(const Material &xsi_material, std::map<ULONG, shared_ptr<material>> &scene_materials)
{
	ULONG mat_id = xsi_material.GetObjectID();
	auto it = scene_materials.find(mat_id);
	if (it != scene_materials.end())
	{
		shared_ptr<material> mat = it->second;
		bool is_convert = false;
		shared_ptr<material> new_mat = convert_material(xsi_material, is_convert);
		if (is_convert && mat->type == new_mat->type)
		{
			mat->update(new_mat);  // but here mat and new mat can be different templates
		}
		else
		{
			//for different types, we should recteate the scene from scratch
			return false;
		}
	}
	else
	{
		log("No material with id " + CString(mat_id) + ", it is impossible");
	}
	return true;
}

void recognize_materials(std::map<ULONG, shared_ptr<material>> &scene_materials, const Scene &xsi_scene, NRTEngine &nrt_engine)
{
	CRefArray mat_libs = xsi_scene.GetMaterialLibraries();
	for (LONG i = 0; i < mat_libs.GetCount(); i++)
	{
		MaterialLibrary lib(mat_libs[i]);
		CRefArray materials = lib.GetItems();
		for (LONG j = 0; j < materials.GetCount(); j++)
		{
			Material xsi_mat(materials[j]);
			ULONG mat_id = xsi_mat.GetObjectID();

			bool is_convert = false;
			shared_ptr<material> mat = convert_material(xsi_mat, is_convert);
			if (is_convert)
			{
				scene_materials.insert(std::pair<ULONG, shared_ptr<material>>(mat_id, mat));
			}
			else
			{
				scene_materials.insert(std::pair<ULONG, shared_ptr<material>>(mat_id, nrt_engine.make_default_material()));
			}
		}
	}
}