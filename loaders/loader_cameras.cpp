#include "loader_cameras.h"

#include <xsi_kinematics.h>

void set_camera(NRTEngine &nrt_engine, const Camera &camera, CTime &eval_time, double aperture)
{
	MATH::CVector3 position = camera.GetKinematics().GetGlobal().GetTransform().GetTranslation();
	MATH::CVector3 interest = camera.GetInterest().GetKinematics().GetGlobal().GetTransform().GetTranslation();
	vec3 origin = vec3(position.GetX(), position.GetY(), position.GetZ());
	vec3 target = vec3(interest.GetX(), interest.GetY(), interest.GetZ());
	double distance = (target - origin).length();
	double vfov;
	int fov_type = camera.GetParameterValue("fovtype", eval_time);
	if (fov_type == 0)
	{
		vfov = camera.GetParameterValue("fov", eval_time);
	}
	else
	{
		double hfov = camera.GetParameterValue("fov", eval_time);
		double hfov_rad = hfov * pi / 180.0;
		double vfov_rad = 2.0 * atan(tan(hfov_rad / 2.0) / (double)(camera.GetParameterValue("aspect", eval_time)));
		vfov = vfov_rad * 180.0 / pi;
	}
	nrt_engine.set_camera(origin, target, vfov, distance, aperture);
}