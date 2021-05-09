//
// Created by taylor-santos on 5/8/2021 at 15:41.
//

#include <gui.h>

#include "imgui.h"

namespace GUI {

Context::Context()
    : ctx_{ImGui::CreateContext()} {
    IMGUI_CHECKVERSION();
    ImGui::SetCurrentContext(ctx_);
}

Context::~Context() {
    ImGui::DestroyContext(ctx_);
}

void
Context::makeCurrent() const {
    ImGui::SetCurrentContext(ctx_);
}

} // namespace GUI
