#pragma once

#include "../Core/Gui.h"
#include "imgui.h"

#include <format>
#include <functional>

using namespace Core;

namespace ImGui
{
    class ImGuiObjectEditor : public GuiLayer
    {
    public:
        ImGuiObjectEditor();
        ~ImGuiObjectEditor();

        virtual void Draw() override;

        void SetObject(const ObjectRef& object);

    private:
        ObjectRef object = nullptr;
    };
}

ImGui::ImGuiObjectEditor::ImGuiObjectEditor()
{
}

ImGui::ImGuiObjectEditor::~ImGuiObjectEditor()
{
}

void ImGui::ImGuiObjectEditor::Draw()
{
    ImGuiWindowFlags windowFlags = 0;
    //windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
    windowFlags |= ImGuiWindowFlags_NoNavInputs;
    bool open = true;

    if (!object) return;

    str id = std::format("{:08}", (u32)object->id);
    str formattedId = std::format("{}-{}", id.substr(0, 3), id.substr(3, 5));
    str title = std::format("Object #{}", formattedId);

    ImGui::Begin(title.c_str(), NULL /*&open*/, windowFlags);
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
                ImGui::Text("Location");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                static const char* LocationText = "X:  %.2f\nY:  %.2f\nZ:  %.2f";
                ImGui::Text(LocationText, object->transform.location.x, object->transform.location.y, object->transform.location.z);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Rotation");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                static const char* RotationText = "P:  %.1f\nY:  %.1f\nR:  %.1f";
                ImGui::Text(RotationText, object->transform.rotation.pitch, object->transform.rotation.yaw, object->transform.rotation.roll);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Scale");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                static const char* ScaleText = "X:  %.1f\nY:  %.1f\nZ:  %.1f\n\n";
                ImGui::Text(ScaleText, object->transform.scale.x, object->transform.scale.y, object->transform.scale.z);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Use Tint Color  ");
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::Checkbox("##use_tint_color", &object->useTintColor);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Tint Color");
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::ColorEdit3("Tint Color", object->tintColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Mesh");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text(object->mesh ? object->mesh->name.c_str() : "<NULL>");
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Script");
                ImGui::SameLine(0, 50);
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text(object->script->Name().c_str());
            }

            ImGui::EndTable();
        }

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        ImVec2 newPos = ImVec2(viewportSize.x - windowSize.x - 10.0f, 10.0f);
        ImGui::SetWindowPos(newPos);
    }
    ImGui::End();
}

void ImGui::ImGuiObjectEditor::SetObject(const ObjectRef& object)
{
    this->object = object;
}
