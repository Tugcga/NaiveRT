#pragma once
#include <map>

#include <xsi_scene.h>
#include <xsi_material.h>

#include "../nrt_core/nrt_engine.h"
#include "../tools/logger.h"

using namespace XSI;

void recognize_materials(std::map<ULONG, shared_ptr<material>> &scene_materials, const Scene &xsi_scene, NRTEngine &nrt_engine);
bool update_material(const Material &xsi_material, std::map<ULONG, shared_ptr<material>> &scene_materials);