#pragma once

#include "precompiled.h"

#include "Core/Base.h"
#include "Core/Clock.h"
#include "Core/SwapChain.h"
#include "Core/Logger.h"
#include "Core/Input.h"
#include "Core/Keyboard.h"
#include "Core/Mouse.h"
#include "Core/KeyValueFile.h"
#include "Core/RenderThread.h"
#include "Core/GameThread.h"
#include "ImGui/ImGuiDemo.h"
#include "ImGui/ImGuiObjectEditor.h"
#include "ImGui/ImGuiProfiler.h"
#include "ImGui/LmdImGui.h"
#include "Assimp/AssimpMeshLoader.h"
#include "Game.h"

#if defined(OS_WINDOWS)
#include "DirectX/D3D11Renderer.h"
#include "ImGui/ImGuiWin32Platform.h"
#include "ImGui/ImGuiDX11Renderer.h"
#include "Windows/Win32Window.h"
#elif defined(OS_MACOS)
#include "Metal/MTLRenderer.h"
#include "ImGui/ImGuiOSXPlatform.h"
#include "ImGui/ImGuiMetalRenderer.h"
#include "macOS/CocoaWindow.h"
#elif defined(OS_LINUX)
#include "Vulkan/VkRenderer.h"
#include "ImGui/ImGuiGLFWPlatform.h"
#include "ImGui/ImGuiVulkanRenderer.h"
#include "GLFW/GLFWWindow.h"
#endif

using namespace Core;
using namespace std::placeholders;

namespace LMD
{
	class Lemonade
	{
	public:
		Lemonade(u32 width, u32 height, const wstr& name) : width(width), height(height), name(name) {};

		int exec()
		{
            //startLogger();

#if defined(OS_WINDOWS)
            Windows::Win32Window window(width, height, name);
            DirectX::D3D11Renderer renderer(window);
            ImGui::ImGuiWin32Platform imGuiPlatform(window);
            ImGui::ImGuiDX11Renderer imGuiRenderer(renderer.Device(), renderer.DeviceContext());

            window.OnResized = std::bind(&DirectX::D3D11Renderer::Resize, &renderer, _1, _2);
            window.OnWndProc = std::bind(&ImGui::ImGuiWin32Platform::WndProc, &imGuiPlatform, _1, _2, _3, _4);

            //Assimp::AssimpMeshLoader::readFlag = Assimp::ReadFlag::DirectX;
#elif defined(OS_MACOS)
            macOS::CocoaWindow window(width, height, name);
            Metal::MTLRenderer renderer(window);
            ImGui::ImGuiOSXPlatform imGuiPlatform(window);
            ImGui::ImGuiMetalRenderer imGuiRenderer(renderer.Device(), renderer.DeviceContext());
#elif defined(OS_LINUX)
            GLFW::GLFWWindow window(width, height, name);
            //OpenGL::OGL3Renderer renderer(window);
            Vulkan::VkRenderer renderer(window);
            ImGui::ImGuiGLFWPlatform imGuiPlatform(window);
            ImGui::ImGuiVulkanRenderer imGuiRenderer(renderer.Device(), renderer.DeviceContext());
#endif

            ImGui::LmdImGui gui(imGuiPlatform, imGuiRenderer);

            bool running = true;

            ActionKeyMap keyBindings = KeyValueFile::Read("KeyBindings.kvl");
            Input::UseKeyBindings(keyBindings);
            Keyboard::OnPress("Quit") = [&running]() { running = false; };
            //Keyboard::Listen("Jump", [](const str& key) { log_info("new Jump key: {}", key); });
            Mouse::UseKeyBindings(keyBindings);

#if defined(DEVELOPER)
            ImGui::ImGuiDemo imGuiDemo(renderer);
            ImGui::ImGuiObjectEditor objectEditor;
            ImGui::ImGuiProfiler profiler;

            gui.Add(&imGuiDemo);
            gui.Add(&objectEditor);
            gui.Add(&profiler);

            Keyboard::OnPress("ToggleDemoUI") = [&imGuiDemo, &objectEditor]() {
                imGuiDemo.visible    = !imGuiDemo.visible;
                objectEditor.visible = !objectEditor.visible;
            };
#endif

            Negroni::Game game;

            objectEditor.SetObject(game.state.objects[0]);

            for (auto& object : game.state.objects)
            {
                if (object->mesh)
                    renderer.LoadMesh(object->mesh);
            }
            renderer.SetVSync(true);
            window.Show();

            //RenderThread renderThread(renderer, gui);
            //GameThread gameThread(game, std::bind(&RenderThread::Draw, &renderThread, _1));

            //const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
            constexpr float HEX_0C = 12.0f / 255.0f;
            const float color[4] = { HEX_0C, HEX_0C, HEX_0C, 1.0f };

            while (running)
            {
                if (window.Closed()) break;

                auto input = window.ReadInput();
                auto dt = Lemonade::DeltaTime();
                auto& state = game.Update(input, dt);

                //gameThread.Update(input);
                //renderThread.Draw(state);

                renderer.Clear((rgba)color);
                renderer.Draw(state);
                gui.Draw();
                renderer.Present();

                ProfileClear();
            }

            renderer.Cleanup();
            for (auto& object : game.state.objects)
            {
                if (object->mesh)
                    renderer.UnloadMesh(object->mesh);
            }
            window.Cleanup();

            //gameThread.Stop();
            //renderThread.Stop();

            //stopLogger();

            return 0;
		}

	private:
        static float DeltaTime()
        {
            static u64 timeStart = Clock::now();
            static u64 timeCur = Clock::now();
            static float t = (timeCur - timeStart) / 1000.0f;
            static float prev_t = t;

            timeCur = Clock::now();
            t = (timeCur - timeStart) / 1000.0f;
            float dt = t - prev_t;
            prev_t = t;

            return dt;
        }

		u32 width;
		u32 height;
		wstr name;
	};
}
