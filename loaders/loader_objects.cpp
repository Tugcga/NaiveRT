#include "loader_objects.h"
#include "../tools/converter.h"

#include <xsi_application.h>
#include <xsi_kinematics.h>

CStatus update_sphere(ULONG id, const MATH::CVector3 &position, float radius, std::map<ULONG, shared_ptr<sphere>> &scene_objects)
{
	auto it = scene_objects.find(id);
	if (it != scene_objects.end())
	{
		shared_ptr<sphere> sph = it->second;
		sph->set_center(to_vec3(position));
		sph->set_radius(radius);
		return CStatus::OK;
	}
	else
	{
		return CStatus::Fail;
	}
}

CStatus add_sphere(NRTEngine &nrt_engine, X3DObject &object, CTime &eval_time, shared_ptr<material> mat, std::map<ULONG, shared_ptr<sphere>> &scene_objects)
{
	CRefArray properties = object.GetProperties();
	Property vis_prop = properties.GetItem(L"Visibility");
	bool is_visible = vis_prop.GetParameterValue("rendvis", eval_time);
	if (is_visible)
	{
		MATH::CVector3 position = object.GetKinematics().GetGlobal().GetTransform().GetTranslation();
		float radius = object.GetParameterValue("radius", eval_time);
		shared_ptr<sphere> sph = make_shared<sphere>(to_vec3(position), radius, mat);
		nrt_engine.add_sphere(sph);

		scene_objects.insert(std::pair<ULONG, shared_ptr<sphere>>(object.GetObjectID(), sph));
		return CStatus::OK;
	}
	return CStatus::Fail;
}