cmake_minimum_required(VERSION 3.7)

project(renderer)

set(SOURCE_FILES
	Renderer.c
	Renderer.h
	EdgeData.h
	EdgeEquation.h
	Rasterizer.c
	Rasterizer.h
	LineClipper.c
	LineClipper.h
	ParameterEquation.h
	PixelData.h
	PixelShader.c
	PixelShader.h
	PolyClipper.c
	PolyClipper.h
	Rasterizer.h
	TriangleEquations.h
	VertexCache.h
	VertexProcessor.c
	VertexProcessor.h
	VertexShader.c
	VertexShader.h
	Vector.c
	Vector.h)

find_package(OpenMP)
if (OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

if (CMAKE_COMPILER_IS_GNUCXX)
	add_definitions("-Wall")
endif ()

add_library(renderer ${SOURCE_FILES})