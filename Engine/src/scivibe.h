#pragma once

#include "core/core.hpp"
#include "core/timeStep.hpp"

// GLAD must precede GLFW and any other OpenGL headers.
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <miniaudio.h>

#include "objet/color.hpp"
#include "core/log.hpp"
#include "shader/shader.hpp"

#include "Events/event.hpp"
#include "Events/applicationEvent.hpp"
#include "Events/keyEvent.hpp"
#include "Events/mouseEvent.hpp"

#include "layer/layer.hpp"

#include "core/application.hpp"


#include "gui/imGuiLayer.hpp"

#include "constants/keyCode.hpp"
#include "constants/mouseCode.hpp"

#include "renderer/renderer.hpp"
#include "renderer/renderCommand.hpp"
#include "renderer/buffer.hpp"
#include "renderer/vertexArray.hpp"
#include "renderer/texture.hpp"
#include "renderer/camera.hpp"
#include "renderer/orthographicCameraControler.hpp"
#include "renderer/renderer2D.hpp"
#include "renderer/frameBuffer.hpp"
#include "renderer/rendererCurve2D.hpp"

#include "objet/color.hpp"
#include "objet/objet2D.hpp"
#include "objet/line.hpp"

#include "pseudoLatex/font.hpp"
#include "pseudoLatex/pseudoLatex.hpp"
#include "pseudoLatex/psFile.hpp"

#include "animator/animator.hpp"

#include "input/input.hpp"

#include "shader/shader.hpp"



