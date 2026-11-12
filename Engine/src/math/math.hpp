#pragma once
#include <glm/glm.hpp>
#include "core/core.hpp"

namespace scivibe{  namespace Math{
    SCIVIBE_API bool DecomposeTransform( const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);
}}