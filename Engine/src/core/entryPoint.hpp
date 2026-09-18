#pragma once
#include "pch/pch.hpp"
#include "application/application.hpp"
#include "log/log.hpp"


//extern scivibe::Application* scivibe::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	scivibe::Log::Init();
	auto app = scivibe::CreateApplication();
	app->Run();
	delete app;
}
