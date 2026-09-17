#pragma once

#include "core/core.hpp"

// GLAD must precede GLFW and any other OpenGL headers.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <miniaudio.h>

#include "objet/color.hpp"
#include "objet/fleche.hpp"
#include "log/log.hpp"
#include "shader/shader.h"

#include "Events/event.hpp"
#include "Events/applicationEvent.hpp"
#include "Events/keyEvent.hpp"
#include "Events/mouseEvent.hpp"


#include "application/application.hpp"
