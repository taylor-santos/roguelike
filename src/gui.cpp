//
// Created by taylor-santos on 5/8/2021 at 15:41.
//

#include "gui.h"

#include "imgui.h"

namespace GUI {

Context::Context()
    : ctx_{ImGui::CreateContext()} {
    IMGUI_CHECKVERSION();
    ImGui::SetCurrentContext(ctx_);
    ImGuiIO &io = ImGui::GetIO();
    // Enable Multi-Viewport / Platform Windows
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
}

Context::~Context() {
    ImGui::DestroyContext(ctx_);
}

void
Context::makeCurrent() const {
    ImGui::SetCurrentContext(ctx_);
}

void
Context::updatePlatformWindows() const {
    makeCurrent();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}
bool
Context::wantCaptureMouse() const {
    makeCurrent();
    ImGuiIO &io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool
Context::wantCaptureKeyboard() const {
    makeCurrent();
    ImGuiIO &io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

} // namespace GUI
