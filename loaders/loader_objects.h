#pragma once

#include <map>
#include <memory>

#include <xsi_vector3.h>
#include <xsi_x3dobject.h>
#include <xsi_time.h>

#include "../nrt_core/rtweekend/sphere.h"
#include "../nrt_core/nrt_engine.h"

using namespace XSI;

CStatus update_sphere(ULONG id, const MATH::CVector3 &position, float radius, std::map<ULONG, shared_ptr<sphere>> &scene_objects);
CStatus add_sphere(NRTEngine &nrt_engine, X3DObject &object, CTime &eval_time, shared_ptr<material> mat, std::map<ULONG, shared_ptr<sphere>> &scene_objects);