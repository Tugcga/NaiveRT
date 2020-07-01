#pragma once
#include <xsi_camera.h>
#include <xsi_time.h>

#include "../nrt_core/nrt_engine.h"

using namespace XSI;

void set_camera(NRTEngine &nrt_engine, const Camera &camera, CTime &eval_time, double aperture);