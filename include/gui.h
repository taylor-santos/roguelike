//
// Created by taylor-santos on 5/8/2021 at 15:41.
//

#ifndef ROGUELIKE_INCLUDE_GUI_H
#define ROGUELIKE_INCLUDE_GUI_H

struct ImGuiContext;
struct ImFontAtlas;

namespace GUI {

class Context {
public:
    Context();

    ~Context();

    void
    makeCurrent() const;

    void
    updatePlatformWindows() const;

    [[nodiscard]] bool
    wantCaptureMouse() const;

    [[nodiscard]] bool
    wantCaptureKeyboard() const;

    Context(const Context &) = delete;
    Context &
    operator=(const Context &) = delete;

private:
    ImGuiContext *ctx_;
};

} // namespace GUI

#endif // ROGUELIKE_INCLUDE_GUI_H
