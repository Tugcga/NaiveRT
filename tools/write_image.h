#pragma once
#include <string>
#include <vector>
#include <fstream>

float clamp_float(float value, float min, float max);
void write_ppm(std::string path, int width, int height, std::vector<float> &pixels);
void write(std::string path, int width, int height, int components, std::vector<float> &pixels);