# Naive Ray Tracer

This is source code for Softimage addon with simple ray tracing render engine. This engine is based on [this](https://raytracing.github.io/books/RayTracingInOneWeekend.html) tutorial. The main goal of this project not to create a full-featured render engine (Cycles will be better in any case), but create a template for connecting other render engines to Softimage. 

## Features

1. Only one type of geometry primitives - spheres.

2. Only one type of light source - background light (gradient with two colors).

3. Two quality settings: samples count and maximum raytracing depth.

4. Three material nodes: Lambert, Metal and Dielectric.

5. Save output images to ppm ASCII format, png and jpg (by using [stb_image_write](https://github.com/nothings/stb) library).

## Structure

All files for the render engine is placed on nrt_core folder. rtweekend sub-folder contains files from the original tutorial (may be with some modifications). All interaction between Sofitmage and render engine implemented on the file /renderer/engine_connector.cpp and all files from loader folder. In fact, for other render engine, only engine_connector.cpp should be changed. Entry points are RenderInstance::create_scene and RenderInstance::update_scene methods.

## How to compile

Create empty plugin for Softimage. Add files to the project. Add opengl32.lib and glu32.lib to linker. It should works.

## Screen form Softimage

![Softimage window](screen_01.png?raw=true)