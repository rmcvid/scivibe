#pragma once

// GLAD must precede GLFW and any other OpenGL headers.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <miniaudio.h>

#include "objet/color.hpp"
#include "objet/fleche.hpp"
#include "objet/test.hpp"
#include "log/log.hpp"
#include "shader/shader.h"

#include "events/event.hpp"
#include "events/applicationEvent.hpp"
#include "events/keyEvent.hpp"
#include "events/mouseEvent.hpp"


#include "application/application.hpp"