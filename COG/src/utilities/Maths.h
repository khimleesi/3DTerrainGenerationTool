#pragma once

/*******************************************************************************************************************
	Maths.h, Maths.cpp
	Created by Kim Kane
	Last updated: 28/02/2018

	A simple header file that stores maths helper functions/variables.

*******************************************************************************************************************/

#include <pretty_glm/glm.hpp>
#include <vector>
#include <math.h>

namespace maths {

	float Barycentric(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec2& P);
}