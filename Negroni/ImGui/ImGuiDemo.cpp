#include "ImGuiDemo.h"

#include "imgui_internal.h"

static u64 allocated = 0;

void* operator new(u64 size)
{
    allocated += size;
    return std::malloc(size);
}

void operator delete(void* ptr, u64 size) noexcept
{
    allocated -= size;
    std::free(ptr);
}

ImGui::ImGuiDemo::ImGuiDemo(Renderer& renderer) : renderer(renderer)
{
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    SetupStyle();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\CascadiaMono.ttf", 20.0f);
    //io.Fonts->AddFontFromFileTTF("..\\Fonts\\RobotoMono-Regular.ttf", 40.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);
}

ImGui::ImGuiDemo::~ImGuiDemo()
{
}

void ImGui::ImGuiDemo::SetupStyle()
{
    ImGui::StyleColorsDark();

    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_TitleBg]           = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive]     = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]  = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBg]           = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]    = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_FrameBgActive]     = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_CheckMark]         = ImVec4(1.00f, 0.85f, 0.20f, 1.00f);
    colors[ImGuiCol_Button]            = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered]     = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_ButtonActive]      = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_SliderGrab]        = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]  = ImVec4(1.00f, 0.85f, 0.20f, 1.00f);
    colors[ImGuiCol_Header]            = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_HeaderHovered]     = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_HeaderActive]      = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ResizeGrip]        = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]  = colors[ImGuiCol_ResizeGripHovered];
    colors[ImGuiCol_SeparatorActive]   = colors[ImGuiCol_ResizeGripActive];

    style->WindowPadding    = ImVec2(12, 12);
    style->FramePadding     = ImVec2(12, 6);
    style->ItemSpacing      = ImVec2(8, 8);
    style->ItemInnerSpacing = ImVec2(8, 8);

    style->WindowRounding    = 4;
    style->FrameRounding     = 2;
    style->ScrollbarRounding = 1;
    style->GrabRounding      = 1;
}

void ImGui::ImGuiDemo::Draw()
{
    ImGuiIO& io = ImGui::GetIO();

    //bool showDemoWindow = true;
    //ImGui::ShowDemoWindow(&showDemoWindow);

    ImGuiWindowFlags windowFlags = 0;
    //windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
    windowFlags |= ImGuiWindowFlags_NoNavInputs;
    bool open = true;

    const float deltaTime = 1000.0f / io.Framerate;
    static float minFrameTime = deltaTime;
    static float maxFrameTime = deltaTime;
    static float displayedMinFrameTime = deltaTime;
    static float displayedMaxFrameTime = deltaTime;
    static float frameTime = deltaTime;
    static float framerate = io.Framerate;
    static float minFramerate = framerate;
    static float maxFramerate = framerate;
    static float displayedMinFramerate = framerate;
    static float displayedMaxFramerate = framerate;
    static float cumulativeDeltaTime = 0.0f;
    static int frameCount = 0;

    cumulativeDeltaTime += deltaTime;
    frameCount++;

    if (deltaTime < minFrameTime) minFrameTime = deltaTime;
    if (deltaTime > maxFrameTime) maxFrameTime = deltaTime;

    if (io.Framerate < minFramerate) minFramerate = io.Framerate;
    if (io.Framerate > maxFramerate) maxFramerate = io.Framerate;

    if (cumulativeDeltaTime >= 1000.0f)
    {
        frameTime = cumulativeDeltaTime / frameCount;
        framerate = 1000.0f / frameTime;
        cumulativeDeltaTime = 0.0f;
        frameCount = 0;

        displayedMinFrameTime = minFrameTime;
        displayedMaxFrameTime = maxFrameTime;
        minFrameTime = deltaTime;
        maxFrameTime = deltaTime;

        displayedMinFramerate = minFramerate;
        displayedMaxFramerate = maxFramerate;
        minFramerate = io.Framerate;
        maxFramerate = io.Framerate;
    }

    ImGui::Begin("Statistics", NULL /*&open*/, windowFlags);
    {
        if (ImGui::BeginTable("table", 2, /*ImGuiTableFlags_RowBg |*/ ImGuiTableFlags_PadOuterX))
        {
            ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
            //ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("VSync");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::Checkbox("##vsync", &newVSync);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Frame Time  ");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                static const char* FrameTimeText = "Avg:  %.3f ms\nMin:  %.3f ms\nMax:  %.3f ms\n";
                ImGui::Text(FrameTimeText, frameTime, displayedMinFrameTime, displayedMaxFrameTime);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("FPS  ");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                static const char* FPSText = "Avg:  %.0f\nMin:  %.0f\nMax:  %.0f\n";
                ImGui::Text(FPSText, framerate, displayedMinFramerate, displayedMaxFramerate);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Heap");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%.2f KB", allocated / 1024.0f);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Rendered");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%d", renderer.rendered);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Culled");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%d", renderer.culled);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("FXAA");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::Checkbox("##fxaa", &newFXAA);
            }

            ImGui::EndTable();

            static char str0[128] = "Hello, world!";
            ImGui::InputText("##inputtext", str0, IM_ARRAYSIZE(str0));

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            ImGui::InputFloat3("##inputfloat3", vec4a);
        }

        ImGui::SetWindowPos(ImVec2(10.0f, 10.0f));
    }
    ImGui::End();

    if (vSync != newVSync)
    {
        vSync = newVSync;
        renderer.SetVSync(vSync);
    }

    if (fxaa != newFXAA)
    {
        fxaa = newFXAA;
        renderer.SetFXAA(fxaa);
    }
}
