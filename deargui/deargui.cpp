
#include <pybind11/pybind11.h>
#include "imgui.h"
#include "imgui_internal.h"
namespace py = pybind11;

template<typename T>
void template_ImVector(py::module &module, const char* name)
{
    py::class_< ImVector<T> >(module, name)
        .def_property_readonly_static("stride", [](py::object)
        {
            return sizeof(T);
        })
        .def_property_readonly("data", [](const ImVector<T>& self)
        {
            return long((void*)self.Data);
        })
        .def("__len__", [](const ImVector<T>& self)
        {
            return self.size();
        })
        .def("__iter__", [](const ImVector<T>& self)
        {
            return py::make_iterator(self.begin(), self.end());
        })
        .def("__getitem__", [](const ImVector<T>& self, size_t i)
        {
            if ((int)i >= self.size()) throw py::index_error();
            return self[i];
        })
        ;
}

PYBIND11_MODULE(deargui, deargui)
{
    py::class_<ImGuiContext>(deargui, "Context");

    deargui.def("init", []()
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(100.0, 100.0);
        unsigned char* pixels;
        int w, h;
        io.Fonts->GetTexDataAsAlpha8(&pixels, &w, &h, nullptr);
    });
    deargui.def("get_display_size", []()
    {
        ImGuiIO& io = ImGui::GetIO();
        return io.DisplaySize;
    });
    deargui.def("set_display_size", [](ImVec2 size)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = size;
    }, py::arg("size"));
    deargui.def("set_mouse_pos", [](ImVec2 pos)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = pos;
    }, py::arg("pos"));
    deargui.def("set_mouse_down", [](int button, bool down)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (button < 0) throw py::index_error();
        if (button >= IM_ARRAYSIZE(io.MouseDown)) throw py::index_error();
        io.MouseDown[button] = down;
    }, py::arg("button"), py::arg("down"));
    deargui.def("set_key_down", [](int key, bool down)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (key < 0) throw py::index_error();
        if (key >= IM_ARRAYSIZE(io.KeysDown)) throw py::index_error();
        io.KeysDown[key] = down;
    }, py::arg("key"), py::arg("down"));

    py::class_<ImVec2>(deargui, "Vec2")
        .def_readwrite("x", &ImVec2::x)
        .def_readwrite("y", &ImVec2::y)
        .def(py::init<>())
        .def(py::init<float, float>()
           , py::arg("_x")
           , py::arg("_y")
        )

    ;

    py::class_<ImVec4>(deargui, "Vec4")
        .def_readwrite("x", &ImVec4::x)
        .def_readwrite("y", &ImVec4::y)
        .def_readwrite("z", &ImVec4::z)
        .def_readwrite("w", &ImVec4::w)
        .def(py::init<>())
        .def(py::init<float, float, float, float>()
           , py::arg("_x")
           , py::arg("_y")
           , py::arg("_z")
           , py::arg("_w")
        )

    ;

    deargui.def("create_context", &ImGui::CreateContext
       , py::arg("shared_font_atlas") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("destroy_context", &ImGui::DestroyContext
       , py::arg("ctx") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("get_current_context", &ImGui::GetCurrentContext, py::return_value_policy::automatic_reference);
    deargui.def("set_current_context", &ImGui::SetCurrentContext
       , py::arg("ctx")
    , py::return_value_policy::automatic_reference);
    deargui.def("debug_check_version_and_data_layout", &ImGui::DebugCheckVersionAndDataLayout
       , py::arg("version_str")
       , py::arg("sz_io")
       , py::arg("sz_style")
       , py::arg("sz_vec2")
       , py::arg("sz_vec4")
       , py::arg("sz_drawvert")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_io", &ImGui::GetIO, py::return_value_policy::automatic_reference);
    deargui.def("get_style", &ImGui::GetStyle, py::return_value_policy::automatic_reference);
    deargui.def("new_frame", &ImGui::NewFrame, py::return_value_policy::automatic_reference);
    deargui.def("end_frame", &ImGui::EndFrame, py::return_value_policy::automatic_reference);
    deargui.def("render", &ImGui::Render, py::return_value_policy::automatic_reference);
    deargui.def("get_draw_data", &ImGui::GetDrawData, py::return_value_policy::automatic_reference);
    deargui.def("show_demo_window", &ImGui::ShowDemoWindow
       , py::arg("p_open") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("show_metrics_window", &ImGui::ShowMetricsWindow
       , py::arg("p_open") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("show_style_editor", &ImGui::ShowStyleEditor
       , py::arg("ref") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("show_style_selector", &ImGui::ShowStyleSelector
       , py::arg("label")
    , py::return_value_policy::automatic_reference);
    deargui.def("show_font_selector", &ImGui::ShowFontSelector
       , py::arg("label")
    , py::return_value_policy::automatic_reference);
    deargui.def("show_user_guide", &ImGui::ShowUserGuide, py::return_value_policy::automatic_reference);
    deargui.def("get_version", &ImGui::GetVersion, py::return_value_policy::automatic_reference);
    deargui.def("style_colors_dark", &ImGui::StyleColorsDark
       , py::arg("dst") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("style_colors_classic", &ImGui::StyleColorsClassic
       , py::arg("dst") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("style_colors_light", &ImGui::StyleColorsLight
       , py::arg("dst") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("begin", &ImGui::Begin
       , py::arg("name")
       , py::arg("p_open") = nullptr
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("end", &ImGui::End, py::return_value_policy::automatic_reference);
    deargui.def("begin_child", py::overload_cast<const char *, const ImVec2 &, bool, ImGuiWindowFlags>(&ImGui::BeginChild)
       , py::arg("str_id")
       , py::arg("size") = ImVec2(0,0)
       , py::arg("border") = false
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_child", py::overload_cast<ImGuiID, const ImVec2 &, bool, ImGuiWindowFlags>(&ImGui::BeginChild)
       , py::arg("id")
       , py::arg("size") = ImVec2(0,0)
       , py::arg("border") = false
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("end_child", &ImGui::EndChild, py::return_value_policy::automatic_reference);
    deargui.def("is_window_appearing", &ImGui::IsWindowAppearing, py::return_value_policy::automatic_reference);
    deargui.def("is_window_collapsed", &ImGui::IsWindowCollapsed, py::return_value_policy::automatic_reference);
    deargui.def("is_window_focused", &ImGui::IsWindowFocused
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("is_window_hovered", &ImGui::IsWindowHovered
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("get_window_draw_list", &ImGui::GetWindowDrawList, py::return_value_policy::automatic_reference);
    deargui.def("get_window_pos", &ImGui::GetWindowPos, py::return_value_policy::automatic_reference);
    deargui.def("get_window_size", &ImGui::GetWindowSize, py::return_value_policy::automatic_reference);
    deargui.def("get_window_width", &ImGui::GetWindowWidth, py::return_value_policy::automatic_reference);
    deargui.def("get_window_height", &ImGui::GetWindowHeight, py::return_value_policy::automatic_reference);
    deargui.def("get_content_region_max", &ImGui::GetContentRegionMax, py::return_value_policy::automatic_reference);
    deargui.def("get_content_region_avail", &ImGui::GetContentRegionAvail, py::return_value_policy::automatic_reference);
    deargui.def("get_content_region_avail_width", &ImGui::GetContentRegionAvailWidth, py::return_value_policy::automatic_reference);
    deargui.def("get_window_content_region_min", &ImGui::GetWindowContentRegionMin, py::return_value_policy::automatic_reference);
    deargui.def("get_window_content_region_max", &ImGui::GetWindowContentRegionMax, py::return_value_policy::automatic_reference);
    deargui.def("get_window_content_region_width", &ImGui::GetWindowContentRegionWidth, py::return_value_policy::automatic_reference);
    deargui.def("set_next_window_pos", &ImGui::SetNextWindowPos
       , py::arg("pos")
       , py::arg("cond") = 0
       , py::arg("pivot") = ImVec2(0,0)
    , py::return_value_policy::automatic_reference);
    deargui.def("set_next_window_size", &ImGui::SetNextWindowSize
       , py::arg("size")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("set_next_window_content_size", &ImGui::SetNextWindowContentSize
       , py::arg("size")
    , py::return_value_policy::automatic_reference);
    deargui.def("set_next_window_collapsed", &ImGui::SetNextWindowCollapsed
       , py::arg("collapsed")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("set_next_window_focus", &ImGui::SetNextWindowFocus, py::return_value_policy::automatic_reference);
    deargui.def("set_next_window_bg_alpha", &ImGui::SetNextWindowBgAlpha
       , py::arg("alpha")
    , py::return_value_policy::automatic_reference);
    deargui.def("set_window_pos", py::overload_cast<const ImVec2 &, ImGuiCond>(&ImGui::SetWindowPos)
       , py::arg("pos")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("set_window_size", py::overload_cast<const ImVec2 &, ImGuiCond>(&ImGui::SetWindowSize)
       , py::arg("size")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("set_window_collapsed", py::overload_cast<bool, ImGuiCond>(&ImGui::SetWindowCollapsed)
       , py::arg("collapsed")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("set_window_focus", py::overload_cast<>(&ImGui::SetWindowFocus), py::return_value_policy::automatic_reference);
    deargui.def("set_window_font_scale", &ImGui::SetWindowFontScale
       , py::arg("scale")
    , py::return_value_policy::automatic_reference);
    deargui.def("set_window_pos", py::overload_cast<const char *, const ImVec2 &, ImGuiCond>(&ImGui::SetWindowPos)
       , py::arg("name")
       , py::arg("pos")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("set_window_size", py::overload_cast<const char *, const ImVec2 &, ImGuiCond>(&ImGui::SetWindowSize)
       , py::arg("name")
       , py::arg("size")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("set_window_collapsed", py::overload_cast<const char *, bool, ImGuiCond>(&ImGui::SetWindowCollapsed)
       , py::arg("name")
       , py::arg("collapsed")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("set_window_focus", py::overload_cast<const char *>(&ImGui::SetWindowFocus)
       , py::arg("name")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_scroll_x", &ImGui::GetScrollX, py::return_value_policy::automatic_reference);
    deargui.def("get_scroll_y", &ImGui::GetScrollY, py::return_value_policy::automatic_reference);
    deargui.def("get_scroll_max_x", &ImGui::GetScrollMaxX, py::return_value_policy::automatic_reference);
    deargui.def("get_scroll_max_y", &ImGui::GetScrollMaxY, py::return_value_policy::automatic_reference);
    deargui.def("set_scroll_x", &ImGui::SetScrollX
       , py::arg("scroll_x")
    , py::return_value_policy::automatic_reference);
    deargui.def("set_scroll_y", &ImGui::SetScrollY
       , py::arg("scroll_y")
    , py::return_value_policy::automatic_reference);
    deargui.def("set_scroll_here", &ImGui::SetScrollHere
       , py::arg("center_y_ratio") = 0.5f
    , py::return_value_policy::automatic_reference);
    deargui.def("set_scroll_from_pos_y", &ImGui::SetScrollFromPosY
       , py::arg("pos_y")
       , py::arg("center_y_ratio") = 0.5f
    , py::return_value_policy::automatic_reference);
    deargui.def("push_font", &ImGui::PushFont
       , py::arg("font")
    , py::return_value_policy::automatic_reference);
    deargui.def("pop_font", &ImGui::PopFont, py::return_value_policy::automatic_reference);
    deargui.def("push_style_color", py::overload_cast<ImGuiCol, ImU32>(&ImGui::PushStyleColor)
       , py::arg("idx")
       , py::arg("col")
    , py::return_value_policy::automatic_reference);
    deargui.def("push_style_color", py::overload_cast<ImGuiCol, const ImVec4 &>(&ImGui::PushStyleColor)
       , py::arg("idx")
       , py::arg("col")
    , py::return_value_policy::automatic_reference);
    deargui.def("pop_style_color", &ImGui::PopStyleColor
       , py::arg("count") = 1
    , py::return_value_policy::automatic_reference);
    deargui.def("push_style_var", py::overload_cast<ImGuiStyleVar, float>(&ImGui::PushStyleVar)
       , py::arg("idx")
       , py::arg("val")
    , py::return_value_policy::automatic_reference);
    deargui.def("push_style_var", py::overload_cast<ImGuiStyleVar, const ImVec2 &>(&ImGui::PushStyleVar)
       , py::arg("idx")
       , py::arg("val")
    , py::return_value_policy::automatic_reference);
    deargui.def("pop_style_var", &ImGui::PopStyleVar
       , py::arg("count") = 1
    , py::return_value_policy::automatic_reference);
    deargui.def("get_style_color_vec4", &ImGui::GetStyleColorVec4
       , py::arg("idx")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_font", &ImGui::GetFont, py::return_value_policy::automatic_reference);
    deargui.def("get_font_size", &ImGui::GetFontSize, py::return_value_policy::automatic_reference);
    deargui.def("get_font_tex_uv_white_pixel", &ImGui::GetFontTexUvWhitePixel, py::return_value_policy::automatic_reference);
    deargui.def("get_color_u32", py::overload_cast<ImGuiCol, float>(&ImGui::GetColorU32)
       , py::arg("idx")
       , py::arg("alpha_mul") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("get_color_u32", py::overload_cast<const ImVec4 &>(&ImGui::GetColorU32)
       , py::arg("col")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_color_u32", py::overload_cast<ImU32>(&ImGui::GetColorU32)
       , py::arg("col")
    , py::return_value_policy::automatic_reference);
    deargui.def("push_item_width", &ImGui::PushItemWidth
       , py::arg("item_width")
    , py::return_value_policy::automatic_reference);
    deargui.def("pop_item_width", &ImGui::PopItemWidth, py::return_value_policy::automatic_reference);
    deargui.def("calc_item_width", &ImGui::CalcItemWidth, py::return_value_policy::automatic_reference);
    deargui.def("push_text_wrap_pos", &ImGui::PushTextWrapPos
       , py::arg("wrap_pos_x") = 0.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("pop_text_wrap_pos", &ImGui::PopTextWrapPos, py::return_value_policy::automatic_reference);
    deargui.def("push_allow_keyboard_focus", &ImGui::PushAllowKeyboardFocus
       , py::arg("allow_keyboard_focus")
    , py::return_value_policy::automatic_reference);
    deargui.def("pop_allow_keyboard_focus", &ImGui::PopAllowKeyboardFocus, py::return_value_policy::automatic_reference);
    deargui.def("push_button_repeat", &ImGui::PushButtonRepeat
       , py::arg("repeat")
    , py::return_value_policy::automatic_reference);
    deargui.def("pop_button_repeat", &ImGui::PopButtonRepeat, py::return_value_policy::automatic_reference);
    deargui.def("separator", &ImGui::Separator, py::return_value_policy::automatic_reference);
    deargui.def("same_line", &ImGui::SameLine
       , py::arg("pos_x") = 0.0f
       , py::arg("spacing_w") = -1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("new_line", &ImGui::NewLine, py::return_value_policy::automatic_reference);
    deargui.def("spacing", &ImGui::Spacing, py::return_value_policy::automatic_reference);
    deargui.def("dummy", &ImGui::Dummy
       , py::arg("size")
    , py::return_value_policy::automatic_reference);
    deargui.def("indent", &ImGui::Indent
       , py::arg("indent_w") = 0.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("unindent", &ImGui::Unindent
       , py::arg("indent_w") = 0.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_group", &ImGui::BeginGroup, py::return_value_policy::automatic_reference);
    deargui.def("end_group", &ImGui::EndGroup, py::return_value_policy::automatic_reference);
    deargui.def("get_cursor_pos", &ImGui::GetCursorPos, py::return_value_policy::automatic_reference);
    deargui.def("get_cursor_pos_x", &ImGui::GetCursorPosX, py::return_value_policy::automatic_reference);
    deargui.def("get_cursor_pos_y", &ImGui::GetCursorPosY, py::return_value_policy::automatic_reference);
    deargui.def("set_cursor_pos", &ImGui::SetCursorPos
       , py::arg("local_pos")
    , py::return_value_policy::automatic_reference);
    deargui.def("set_cursor_pos_x", &ImGui::SetCursorPosX
       , py::arg("x")
    , py::return_value_policy::automatic_reference);
    deargui.def("set_cursor_pos_y", &ImGui::SetCursorPosY
       , py::arg("y")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_cursor_start_pos", &ImGui::GetCursorStartPos, py::return_value_policy::automatic_reference);
    deargui.def("get_cursor_screen_pos", &ImGui::GetCursorScreenPos, py::return_value_policy::automatic_reference);
    deargui.def("set_cursor_screen_pos", &ImGui::SetCursorScreenPos
       , py::arg("screen_pos")
    , py::return_value_policy::automatic_reference);
    deargui.def("align_text_to_frame_padding", &ImGui::AlignTextToFramePadding, py::return_value_policy::automatic_reference);
    deargui.def("get_text_line_height", &ImGui::GetTextLineHeight, py::return_value_policy::automatic_reference);
    deargui.def("get_text_line_height_with_spacing", &ImGui::GetTextLineHeightWithSpacing, py::return_value_policy::automatic_reference);
    deargui.def("get_frame_height", &ImGui::GetFrameHeight, py::return_value_policy::automatic_reference);
    deargui.def("get_frame_height_with_spacing", &ImGui::GetFrameHeightWithSpacing, py::return_value_policy::automatic_reference);
    deargui.def("push_id", py::overload_cast<const char *>(&ImGui::PushID)
       , py::arg("str_id")
    , py::return_value_policy::automatic_reference);
    deargui.def("push_id", py::overload_cast<const char *, const char *>(&ImGui::PushID)
       , py::arg("str_id_begin")
       , py::arg("str_id_end")
    , py::return_value_policy::automatic_reference);
    deargui.def("push_id", py::overload_cast<const void *>(&ImGui::PushID)
       , py::arg("ptr_id")
    , py::return_value_policy::automatic_reference);
    deargui.def("push_id", py::overload_cast<int>(&ImGui::PushID)
       , py::arg("int_id")
    , py::return_value_policy::automatic_reference);
    deargui.def("pop_id", &ImGui::PopID, py::return_value_policy::automatic_reference);
    deargui.def("get_id", py::overload_cast<const char *>(&ImGui::GetID)
       , py::arg("str_id")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_id", py::overload_cast<const char *, const char *>(&ImGui::GetID)
       , py::arg("str_id_begin")
       , py::arg("str_id_end")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_id", py::overload_cast<const void *>(&ImGui::GetID)
       , py::arg("ptr_id")
    , py::return_value_policy::automatic_reference);
    deargui.def("text_unformatted", &ImGui::TextUnformatted
       , py::arg("text")
       , py::arg("text_end") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("text_v", &ImGui::TextV
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("text_colored_v", &ImGui::TextColoredV
       , py::arg("col")
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("text_disabled_v", &ImGui::TextDisabledV
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("text_wrapped_v", &ImGui::TextWrappedV
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("label_text_v", &ImGui::LabelTextV
       , py::arg("label")
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("bullet_text_v", &ImGui::BulletTextV
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("button", &ImGui::Button
       , py::arg("label")
       , py::arg("size") = ImVec2(0,0)
    , py::return_value_policy::automatic_reference);
    deargui.def("small_button", &ImGui::SmallButton
       , py::arg("label")
    , py::return_value_policy::automatic_reference);
    deargui.def("invisible_button", &ImGui::InvisibleButton
       , py::arg("str_id")
       , py::arg("size")
    , py::return_value_policy::automatic_reference);
    deargui.def("arrow_button", &ImGui::ArrowButton
       , py::arg("str_id")
       , py::arg("dir")
    , py::return_value_policy::automatic_reference);
    deargui.def("image", &ImGui::Image
       , py::arg("user_texture_id")
       , py::arg("size")
       , py::arg("uv0") = ImVec2(0,0)
       , py::arg("uv1") = ImVec2(1,1)
       , py::arg("tint_col") = ImVec4(1,1,1,1)
       , py::arg("border_col") = ImVec4(0,0,0,0)
    , py::return_value_policy::automatic_reference);
    deargui.def("image_button", &ImGui::ImageButton
       , py::arg("user_texture_id")
       , py::arg("size")
       , py::arg("uv0") = ImVec2(0,0)
       , py::arg("uv1") = ImVec2(1,1)
       , py::arg("frame_padding") = -1
       , py::arg("bg_col") = ImVec4(0,0,0,0)
       , py::arg("tint_col") = ImVec4(1,1,1,1)
    , py::return_value_policy::automatic_reference);
    deargui.def("checkbox", &ImGui::Checkbox
       , py::arg("label")
       , py::arg("v")
    , py::return_value_policy::automatic_reference);
    deargui.def("checkbox_flags", &ImGui::CheckboxFlags
       , py::arg("label")
       , py::arg("flags")
       , py::arg("flags_value")
    , py::return_value_policy::automatic_reference);
    deargui.def("radio_button", py::overload_cast<const char *, bool>(&ImGui::RadioButton)
       , py::arg("label")
       , py::arg("active")
    , py::return_value_policy::automatic_reference);
    deargui.def("radio_button", py::overload_cast<const char *, int *, int>(&ImGui::RadioButton)
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_button")
    , py::return_value_policy::automatic_reference);
    deargui.def("progress_bar", &ImGui::ProgressBar
       , py::arg("fraction")
       , py::arg("size_arg") = ImVec2(-1,0)
       , py::arg("overlay") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("bullet", &ImGui::Bullet, py::return_value_policy::automatic_reference);
    deargui.def("begin_combo", &ImGui::BeginCombo
       , py::arg("label")
       , py::arg("preview_value")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("end_combo", &ImGui::EndCombo, py::return_value_policy::automatic_reference);
    deargui.def("drag_float2", &ImGui::DragFloat2
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_speed") = 1.0f
       , py::arg("v_min") = 0.0f
       , py::arg("v_max") = 0.0f
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_float3", &ImGui::DragFloat3
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_speed") = 1.0f
       , py::arg("v_min") = 0.0f
       , py::arg("v_max") = 0.0f
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_float4", &ImGui::DragFloat4
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_speed") = 1.0f
       , py::arg("v_min") = 0.0f
       , py::arg("v_max") = 0.0f
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_float_range2", &ImGui::DragFloatRange2
       , py::arg("label")
       , py::arg("v_current_min")
       , py::arg("v_current_max")
       , py::arg("v_speed") = 1.0f
       , py::arg("v_min") = 0.0f
       , py::arg("v_max") = 0.0f
       , py::arg("format") = nullptr
       , py::arg("format_max") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_int", &ImGui::DragInt
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_speed") = 1.0f
       , py::arg("v_min") = 0
       , py::arg("v_max") = 0
       , py::arg("format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_int2", &ImGui::DragInt2
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_speed") = 1.0f
       , py::arg("v_min") = 0
       , py::arg("v_max") = 0
       , py::arg("format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_int3", &ImGui::DragInt3
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_speed") = 1.0f
       , py::arg("v_min") = 0
       , py::arg("v_max") = 0
       , py::arg("format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_int4", &ImGui::DragInt4
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_speed") = 1.0f
       , py::arg("v_min") = 0
       , py::arg("v_max") = 0
       , py::arg("format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_int_range2", &ImGui::DragIntRange2
       , py::arg("label")
       , py::arg("v_current_min")
       , py::arg("v_current_max")
       , py::arg("v_speed") = 1.0f
       , py::arg("v_min") = 0
       , py::arg("v_max") = 0
       , py::arg("format") = nullptr
       , py::arg("format_max") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_scalar", &ImGui::DragScalar
       , py::arg("label")
       , py::arg("data_type")
       , py::arg("v")
       , py::arg("v_speed")
       , py::arg("v_min") = nullptr
       , py::arg("v_max") = nullptr
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("drag_scalar_n", &ImGui::DragScalarN
       , py::arg("label")
       , py::arg("data_type")
       , py::arg("v")
       , py::arg("components")
       , py::arg("v_speed")
       , py::arg("v_min") = nullptr
       , py::arg("v_max") = nullptr
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_float", &ImGui::SliderFloat
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_float2", &ImGui::SliderFloat2
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_float3", &ImGui::SliderFloat3
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_float4", &ImGui::SliderFloat4
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_angle", &ImGui::SliderAngle
       , py::arg("label")
       , py::arg("v_rad")
       , py::arg("v_degrees_min") = -360.0f
       , py::arg("v_degrees_max") = +360.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_int", &ImGui::SliderInt
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_int2", &ImGui::SliderInt2
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_int3", &ImGui::SliderInt3
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_int4", &ImGui::SliderInt4
       , py::arg("label")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_scalar", &ImGui::SliderScalar
       , py::arg("label")
       , py::arg("data_type")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("slider_scalar_n", &ImGui::SliderScalarN
       , py::arg("label")
       , py::arg("data_type")
       , py::arg("v")
       , py::arg("components")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("v_slider_float", &ImGui::VSliderFloat
       , py::arg("label")
       , py::arg("size")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("v_slider_int", &ImGui::VSliderInt
       , py::arg("label")
       , py::arg("size")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("v_slider_scalar", &ImGui::VSliderScalar
       , py::arg("label")
       , py::arg("size")
       , py::arg("data_type")
       , py::arg("v")
       , py::arg("v_min")
       , py::arg("v_max")
       , py::arg("format") = nullptr
       , py::arg("power") = 1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("input_float", &ImGui::InputFloat
       , py::arg("label")
       , py::arg("v")
       , py::arg("step") = 0.0f
       , py::arg("step_fast") = 0.0f
       , py::arg("format") = nullptr
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_float2", &ImGui::InputFloat2
       , py::arg("label")
       , py::arg("v")
       , py::arg("format") = nullptr
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_float3", &ImGui::InputFloat3
       , py::arg("label")
       , py::arg("v")
       , py::arg("format") = nullptr
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_float4", &ImGui::InputFloat4
       , py::arg("label")
       , py::arg("v")
       , py::arg("format") = nullptr
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_int", &ImGui::InputInt
       , py::arg("label")
       , py::arg("v")
       , py::arg("step") = 1
       , py::arg("step_fast") = 100
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_int2", &ImGui::InputInt2
       , py::arg("label")
       , py::arg("v")
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_int3", &ImGui::InputInt3
       , py::arg("label")
       , py::arg("v")
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_int4", &ImGui::InputInt4
       , py::arg("label")
       , py::arg("v")
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_double", &ImGui::InputDouble
       , py::arg("label")
       , py::arg("v")
       , py::arg("step") = 0.0f
       , py::arg("step_fast") = 0.0f
       , py::arg("format") = nullptr
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_scalar", &ImGui::InputScalar
       , py::arg("label")
       , py::arg("data_type")
       , py::arg("v")
       , py::arg("step") = nullptr
       , py::arg("step_fast") = nullptr
       , py::arg("format") = nullptr
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_scalar_n", &ImGui::InputScalarN
       , py::arg("label")
       , py::arg("data_type")
       , py::arg("v")
       , py::arg("components")
       , py::arg("step") = nullptr
       , py::arg("step_fast") = nullptr
       , py::arg("format") = nullptr
       , py::arg("extra_flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("color_edit3", &ImGui::ColorEdit3
       , py::arg("label")
       , py::arg("col")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("color_edit4", &ImGui::ColorEdit4
       , py::arg("label")
       , py::arg("col")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("color_picker3", &ImGui::ColorPicker3
       , py::arg("label")
       , py::arg("col")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("color_picker4", &ImGui::ColorPicker4
       , py::arg("label")
       , py::arg("col")
       , py::arg("flags") = 0
       , py::arg("ref_col") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("color_button", &ImGui::ColorButton
       , py::arg("desc_id")
       , py::arg("col")
       , py::arg("flags") = 0
       , py::arg("size") = ImVec2(0,0)
    , py::return_value_policy::automatic_reference);
    deargui.def("set_color_edit_options", &ImGui::SetColorEditOptions
       , py::arg("flags")
    , py::return_value_policy::automatic_reference);
    deargui.def("tree_node", py::overload_cast<const char *>(&ImGui::TreeNode)
       , py::arg("label")
    , py::return_value_policy::automatic_reference);
    deargui.def("tree_node_v", py::overload_cast<const char *, const char *, va_list>(&ImGui::TreeNodeV)
       , py::arg("str_id")
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("tree_node_v", py::overload_cast<const void *, const char *, va_list>(&ImGui::TreeNodeV)
       , py::arg("ptr_id")
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("tree_node_ex", py::overload_cast<const char *, ImGuiTreeNodeFlags>(&ImGui::TreeNodeEx)
       , py::arg("label")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("tree_node_ex_v", py::overload_cast<const char *, ImGuiTreeNodeFlags, const char *, va_list>(&ImGui::TreeNodeExV)
       , py::arg("str_id")
       , py::arg("flags")
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("tree_node_ex_v", py::overload_cast<const void *, ImGuiTreeNodeFlags, const char *, va_list>(&ImGui::TreeNodeExV)
       , py::arg("ptr_id")
       , py::arg("flags")
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("tree_push", py::overload_cast<const char *>(&ImGui::TreePush)
       , py::arg("str_id")
    , py::return_value_policy::automatic_reference);
    deargui.def("tree_push", py::overload_cast<const void *>(&ImGui::TreePush)
       , py::arg("ptr_id") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("tree_pop", &ImGui::TreePop, py::return_value_policy::automatic_reference);
    deargui.def("tree_advance_to_label_pos", &ImGui::TreeAdvanceToLabelPos, py::return_value_policy::automatic_reference);
    deargui.def("get_tree_node_to_label_spacing", &ImGui::GetTreeNodeToLabelSpacing, py::return_value_policy::automatic_reference);
    deargui.def("set_next_tree_node_open", &ImGui::SetNextTreeNodeOpen
       , py::arg("is_open")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("collapsing_header", py::overload_cast<const char *, ImGuiTreeNodeFlags>(&ImGui::CollapsingHeader)
       , py::arg("label")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("collapsing_header", py::overload_cast<const char *, bool *, ImGuiTreeNodeFlags>(&ImGui::CollapsingHeader)
       , py::arg("label")
       , py::arg("p_open")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("selectable", py::overload_cast<const char *, bool, ImGuiSelectableFlags, const ImVec2 &>(&ImGui::Selectable)
       , py::arg("label")
       , py::arg("selected") = false
       , py::arg("flags") = 0
       , py::arg("size") = ImVec2(0,0)
    , py::return_value_policy::automatic_reference);
    deargui.def("selectable", py::overload_cast<const char *, bool *, ImGuiSelectableFlags, const ImVec2 &>(&ImGui::Selectable)
       , py::arg("label")
       , py::arg("p_selected")
       , py::arg("flags") = 0
       , py::arg("size") = ImVec2(0,0)
    , py::return_value_policy::automatic_reference);
    deargui.def("list_box_header", py::overload_cast<const char *, const ImVec2 &>(&ImGui::ListBoxHeader)
       , py::arg("label")
       , py::arg("size") = ImVec2(0,0)
    , py::return_value_policy::automatic_reference);
    deargui.def("list_box_header", py::overload_cast<const char *, int, int>(&ImGui::ListBoxHeader)
       , py::arg("label")
       , py::arg("items_count")
       , py::arg("height_in_items") = -1
    , py::return_value_policy::automatic_reference);
    deargui.def("list_box_footer", &ImGui::ListBoxFooter, py::return_value_policy::automatic_reference);
    deargui.def("value", py::overload_cast<const char *, bool>(&ImGui::Value)
       , py::arg("prefix")
       , py::arg("b")
    , py::return_value_policy::automatic_reference);
    deargui.def("value", py::overload_cast<const char *, int>(&ImGui::Value)
       , py::arg("prefix")
       , py::arg("v")
    , py::return_value_policy::automatic_reference);
    deargui.def("value", py::overload_cast<const char *, unsigned int>(&ImGui::Value)
       , py::arg("prefix")
       , py::arg("v")
    , py::return_value_policy::automatic_reference);
    deargui.def("value", py::overload_cast<const char *, float, const char *>(&ImGui::Value)
       , py::arg("prefix")
       , py::arg("v")
       , py::arg("float_format") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_main_menu_bar", &ImGui::BeginMainMenuBar, py::return_value_policy::automatic_reference);
    deargui.def("end_main_menu_bar", &ImGui::EndMainMenuBar, py::return_value_policy::automatic_reference);
    deargui.def("begin_menu_bar", &ImGui::BeginMenuBar, py::return_value_policy::automatic_reference);
    deargui.def("end_menu_bar", &ImGui::EndMenuBar, py::return_value_policy::automatic_reference);
    deargui.def("begin_menu", &ImGui::BeginMenu
       , py::arg("label")
       , py::arg("enabled") = true
    , py::return_value_policy::automatic_reference);
    deargui.def("end_menu", &ImGui::EndMenu, py::return_value_policy::automatic_reference);
    deargui.def("menu_item", py::overload_cast<const char *, const char *, bool, bool>(&ImGui::MenuItem)
       , py::arg("label")
       , py::arg("shortcut") = nullptr
       , py::arg("selected") = false
       , py::arg("enabled") = true
    , py::return_value_policy::automatic_reference);
    deargui.def("menu_item", py::overload_cast<const char *, const char *, bool *, bool>(&ImGui::MenuItem)
       , py::arg("label")
       , py::arg("shortcut")
       , py::arg("p_selected")
       , py::arg("enabled") = true
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_tooltip", &ImGui::BeginTooltip, py::return_value_policy::automatic_reference);
    deargui.def("end_tooltip", &ImGui::EndTooltip, py::return_value_policy::automatic_reference);
    deargui.def("set_tooltip_v", &ImGui::SetTooltipV
       , py::arg("fmt")
       , py::arg("args")
    , py::return_value_policy::automatic_reference);
    deargui.def("open_popup", &ImGui::OpenPopup
       , py::arg("str_id")
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_popup", &ImGui::BeginPopup
       , py::arg("str_id")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_popup_context_item", &ImGui::BeginPopupContextItem
       , py::arg("str_id") = nullptr
       , py::arg("mouse_button") = 1
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_popup_context_window", &ImGui::BeginPopupContextWindow
       , py::arg("str_id") = nullptr
       , py::arg("mouse_button") = 1
       , py::arg("also_over_items") = true
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_popup_context_void", &ImGui::BeginPopupContextVoid
       , py::arg("str_id") = nullptr
       , py::arg("mouse_button") = 1
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_popup_modal", &ImGui::BeginPopupModal
       , py::arg("name")
       , py::arg("p_open") = nullptr
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("end_popup", &ImGui::EndPopup, py::return_value_policy::automatic_reference);
    deargui.def("open_popup_on_item_click", &ImGui::OpenPopupOnItemClick
       , py::arg("str_id") = nullptr
       , py::arg("mouse_button") = 1
    , py::return_value_policy::automatic_reference);
    deargui.def("close_current_popup", &ImGui::CloseCurrentPopup, py::return_value_policy::automatic_reference);
    deargui.def("columns", &ImGui::Columns
       , py::arg("count") = 1
       , py::arg("id") = nullptr
       , py::arg("border") = true
    , py::return_value_policy::automatic_reference);
    deargui.def("next_column", &ImGui::NextColumn, py::return_value_policy::automatic_reference);
    deargui.def("get_column_index", &ImGui::GetColumnIndex, py::return_value_policy::automatic_reference);
    deargui.def("get_column_width", &ImGui::GetColumnWidth
       , py::arg("column_index") = -1
    , py::return_value_policy::automatic_reference);
    deargui.def("set_column_width", &ImGui::SetColumnWidth
       , py::arg("column_index")
       , py::arg("width")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_column_offset", &ImGui::GetColumnOffset
       , py::arg("column_index") = -1
    , py::return_value_policy::automatic_reference);
    deargui.def("set_column_offset", &ImGui::SetColumnOffset
       , py::arg("column_index")
       , py::arg("offset_x")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_columns_count", &ImGui::GetColumnsCount, py::return_value_policy::automatic_reference);
    deargui.def("log_to_tty", &ImGui::LogToTTY
       , py::arg("max_depth") = -1
    , py::return_value_policy::automatic_reference);
    deargui.def("log_to_file", &ImGui::LogToFile
       , py::arg("max_depth") = -1
       , py::arg("filename") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("log_to_clipboard", &ImGui::LogToClipboard
       , py::arg("max_depth") = -1
    , py::return_value_policy::automatic_reference);
    deargui.def("log_finish", &ImGui::LogFinish, py::return_value_policy::automatic_reference);
    deargui.def("log_buttons", &ImGui::LogButtons, py::return_value_policy::automatic_reference);
    deargui.def("begin_drag_drop_source", &ImGui::BeginDragDropSource
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("set_drag_drop_payload", &ImGui::SetDragDropPayload
       , py::arg("type")
       , py::arg("data")
       , py::arg("size")
       , py::arg("cond") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("end_drag_drop_source", &ImGui::EndDragDropSource, py::return_value_policy::automatic_reference);
    deargui.def("begin_drag_drop_target", &ImGui::BeginDragDropTarget, py::return_value_policy::automatic_reference);
    deargui.def("accept_drag_drop_payload", &ImGui::AcceptDragDropPayload
       , py::arg("type")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("end_drag_drop_target", &ImGui::EndDragDropTarget, py::return_value_policy::automatic_reference);
    deargui.def("push_clip_rect", &ImGui::PushClipRect
       , py::arg("clip_rect_min")
       , py::arg("clip_rect_max")
       , py::arg("intersect_with_current_clip_rect")
    , py::return_value_policy::automatic_reference);
    deargui.def("pop_clip_rect", &ImGui::PopClipRect, py::return_value_policy::automatic_reference);
    deargui.def("set_item_default_focus", &ImGui::SetItemDefaultFocus, py::return_value_policy::automatic_reference);
    deargui.def("set_keyboard_focus_here", &ImGui::SetKeyboardFocusHere
       , py::arg("offset") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("is_item_hovered", &ImGui::IsItemHovered
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("is_item_active", &ImGui::IsItemActive, py::return_value_policy::automatic_reference);
    deargui.def("is_item_focused", &ImGui::IsItemFocused, py::return_value_policy::automatic_reference);
    deargui.def("is_item_clicked", &ImGui::IsItemClicked
       , py::arg("mouse_button") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("is_item_visible", &ImGui::IsItemVisible, py::return_value_policy::automatic_reference);
    deargui.def("is_item_edited", &ImGui::IsItemEdited, py::return_value_policy::automatic_reference);
    deargui.def("is_item_deactivated", &ImGui::IsItemDeactivated, py::return_value_policy::automatic_reference);
    deargui.def("is_item_deactivated_after_edit", &ImGui::IsItemDeactivatedAfterEdit, py::return_value_policy::automatic_reference);
    deargui.def("is_any_item_hovered", &ImGui::IsAnyItemHovered, py::return_value_policy::automatic_reference);
    deargui.def("is_any_item_active", &ImGui::IsAnyItemActive, py::return_value_policy::automatic_reference);
    deargui.def("is_any_item_focused", &ImGui::IsAnyItemFocused, py::return_value_policy::automatic_reference);
    deargui.def("get_item_rect_min", &ImGui::GetItemRectMin, py::return_value_policy::automatic_reference);
    deargui.def("get_item_rect_max", &ImGui::GetItemRectMax, py::return_value_policy::automatic_reference);
    deargui.def("get_item_rect_size", &ImGui::GetItemRectSize, py::return_value_policy::automatic_reference);
    deargui.def("set_item_allow_overlap", &ImGui::SetItemAllowOverlap, py::return_value_policy::automatic_reference);
    deargui.def("is_rect_visible", py::overload_cast<const ImVec2 &>(&ImGui::IsRectVisible)
       , py::arg("size")
    , py::return_value_policy::automatic_reference);
    deargui.def("is_rect_visible", py::overload_cast<const ImVec2 &, const ImVec2 &>(&ImGui::IsRectVisible)
       , py::arg("rect_min")
       , py::arg("rect_max")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_time", &ImGui::GetTime, py::return_value_policy::automatic_reference);
    deargui.def("get_frame_count", &ImGui::GetFrameCount, py::return_value_policy::automatic_reference);
    deargui.def("get_overlay_draw_list", &ImGui::GetOverlayDrawList, py::return_value_policy::automatic_reference);
    deargui.def("get_draw_list_shared_data", &ImGui::GetDrawListSharedData, py::return_value_policy::automatic_reference);
    deargui.def("get_style_color_name", &ImGui::GetStyleColorName
       , py::arg("idx")
    , py::return_value_policy::automatic_reference);
    deargui.def("set_state_storage", &ImGui::SetStateStorage
       , py::arg("storage")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_state_storage", &ImGui::GetStateStorage, py::return_value_policy::automatic_reference);
    deargui.def("calc_text_size", &ImGui::CalcTextSize
       , py::arg("text")
       , py::arg("text_end") = nullptr
       , py::arg("hide_text_after_double_hash") = false
       , py::arg("wrap_width") = -1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("calc_list_clipping", &ImGui::CalcListClipping
       , py::arg("items_count")
       , py::arg("items_height")
       , py::arg("out_items_display_start")
       , py::arg("out_items_display_end")
    , py::return_value_policy::automatic_reference);
    deargui.def("begin_child_frame", &ImGui::BeginChildFrame
       , py::arg("id")
       , py::arg("size")
       , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("end_child_frame", &ImGui::EndChildFrame, py::return_value_policy::automatic_reference);
    deargui.def("color_convert_u32_to_float4", &ImGui::ColorConvertU32ToFloat4
       , py::arg("in")
    , py::return_value_policy::automatic_reference);
    deargui.def("color_convert_float4_to_u32", &ImGui::ColorConvertFloat4ToU32
       , py::arg("in")
    , py::return_value_policy::automatic_reference);
    deargui.def("color_convert_rg_bto_hsv", &ImGui::ColorConvertRGBtoHSV
       , py::arg("r")
       , py::arg("g")
       , py::arg("b")
       , py::arg("out_h")
       , py::arg("out_s")
       , py::arg("out_v")
    , py::return_value_policy::automatic_reference);
    deargui.def("color_convert_hs_vto_rgb", &ImGui::ColorConvertHSVtoRGB
       , py::arg("h")
       , py::arg("s")
       , py::arg("v")
       , py::arg("out_r")
       , py::arg("out_g")
       , py::arg("out_b")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_key_index", &ImGui::GetKeyIndex
       , py::arg("imgui_key")
    , py::return_value_policy::automatic_reference);
    deargui.def("is_key_down", &ImGui::IsKeyDown
       , py::arg("user_key_index")
    , py::return_value_policy::automatic_reference);
    deargui.def("is_key_pressed", &ImGui::IsKeyPressed
       , py::arg("user_key_index")
       , py::arg("repeat") = true
    , py::return_value_policy::automatic_reference);
    deargui.def("is_key_released", &ImGui::IsKeyReleased
       , py::arg("user_key_index")
    , py::return_value_policy::automatic_reference);
    deargui.def("get_key_pressed_amount", &ImGui::GetKeyPressedAmount
       , py::arg("key_index")
       , py::arg("repeat_delay")
       , py::arg("rate")
    , py::return_value_policy::automatic_reference);
    deargui.def("is_mouse_down", &ImGui::IsMouseDown
       , py::arg("button")
    , py::return_value_policy::automatic_reference);
    deargui.def("is_any_mouse_down", &ImGui::IsAnyMouseDown, py::return_value_policy::automatic_reference);
    deargui.def("is_mouse_clicked", &ImGui::IsMouseClicked
       , py::arg("button")
       , py::arg("repeat") = false
    , py::return_value_policy::automatic_reference);
    deargui.def("is_mouse_double_clicked", &ImGui::IsMouseDoubleClicked
       , py::arg("button")
    , py::return_value_policy::automatic_reference);
    deargui.def("is_mouse_released", &ImGui::IsMouseReleased
       , py::arg("button")
    , py::return_value_policy::automatic_reference);
    deargui.def("is_mouse_dragging", &ImGui::IsMouseDragging
       , py::arg("button") = 0
       , py::arg("lock_threshold") = -1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("is_mouse_hovering_rect", &ImGui::IsMouseHoveringRect
       , py::arg("r_min")
       , py::arg("r_max")
       , py::arg("clip") = true
    , py::return_value_policy::automatic_reference);
    deargui.def("is_mouse_pos_valid", &ImGui::IsMousePosValid
       , py::arg("mouse_pos") = nullptr
    , py::return_value_policy::automatic_reference);
    deargui.def("get_mouse_pos", &ImGui::GetMousePos, py::return_value_policy::automatic_reference);
    deargui.def("get_mouse_pos_on_opening_current_popup", &ImGui::GetMousePosOnOpeningCurrentPopup, py::return_value_policy::automatic_reference);
    deargui.def("get_mouse_drag_delta", &ImGui::GetMouseDragDelta
       , py::arg("button") = 0
       , py::arg("lock_threshold") = -1.0f
    , py::return_value_policy::automatic_reference);
    deargui.def("reset_mouse_drag_delta", &ImGui::ResetMouseDragDelta
       , py::arg("button") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("get_mouse_cursor", &ImGui::GetMouseCursor, py::return_value_policy::automatic_reference);
    deargui.def("set_mouse_cursor", &ImGui::SetMouseCursor
       , py::arg("type")
    , py::return_value_policy::automatic_reference);
    deargui.def("capture_keyboard_from_app", &ImGui::CaptureKeyboardFromApp
       , py::arg("capture") = true
    , py::return_value_policy::automatic_reference);
    deargui.def("capture_mouse_from_app", &ImGui::CaptureMouseFromApp
       , py::arg("capture") = true
    , py::return_value_policy::automatic_reference);
    deargui.def("get_clipboard_text", &ImGui::GetClipboardText, py::return_value_policy::automatic_reference);
    deargui.def("set_clipboard_text", &ImGui::SetClipboardText
       , py::arg("text")
    , py::return_value_policy::automatic_reference);
    deargui.def("load_ini_settings_from_disk", &ImGui::LoadIniSettingsFromDisk
       , py::arg("ini_filename")
    , py::return_value_policy::automatic_reference);
    deargui.def("load_ini_settings_from_memory", &ImGui::LoadIniSettingsFromMemory
       , py::arg("ini_data")
       , py::arg("ini_size") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("save_ini_settings_to_disk", &ImGui::SaveIniSettingsToDisk
       , py::arg("ini_filename")
    , py::return_value_policy::automatic_reference);
    deargui.def("save_ini_settings_to_memory", &ImGui::SaveIniSettingsToMemory
       , py::arg("out_ini_size") = nullptr
    , py::return_value_policy::automatic_reference);
    py::enum_<ImGuiWindowFlags_>(deargui, "WindowFlags")
        .value("WINDOW_FLAGS__NONE", ImGuiWindowFlags_None)
        .value("WINDOW_FLAGS__NO_TITLE_BAR", ImGuiWindowFlags_NoTitleBar)
        .value("WINDOW_FLAGS__NO_RESIZE", ImGuiWindowFlags_NoResize)
        .value("WINDOW_FLAGS__NO_MOVE", ImGuiWindowFlags_NoMove)
        .value("WINDOW_FLAGS__NO_SCROLLBAR", ImGuiWindowFlags_NoScrollbar)
        .value("WINDOW_FLAGS__NO_SCROLL_WITH_MOUSE", ImGuiWindowFlags_NoScrollWithMouse)
        .value("WINDOW_FLAGS__NO_COLLAPSE", ImGuiWindowFlags_NoCollapse)
        .value("WINDOW_FLAGS__ALWAYS_AUTO_RESIZE", ImGuiWindowFlags_AlwaysAutoResize)
        .value("WINDOW_FLAGS__NO_SAVED_SETTINGS", ImGuiWindowFlags_NoSavedSettings)
        .value("WINDOW_FLAGS__NO_INPUTS", ImGuiWindowFlags_NoInputs)
        .value("WINDOW_FLAGS__MENU_BAR", ImGuiWindowFlags_MenuBar)
        .value("WINDOW_FLAGS__HORIZONTAL_SCROLLBAR", ImGuiWindowFlags_HorizontalScrollbar)
        .value("WINDOW_FLAGS__NO_FOCUS_ON_APPEARING", ImGuiWindowFlags_NoFocusOnAppearing)
        .value("WINDOW_FLAGS__NO_BRING_TO_FRONT_ON_FOCUS", ImGuiWindowFlags_NoBringToFrontOnFocus)
        .value("WINDOW_FLAGS__ALWAYS_VERTICAL_SCROLLBAR", ImGuiWindowFlags_AlwaysVerticalScrollbar)
        .value("WINDOW_FLAGS__ALWAYS_HORIZONTAL_SCROLLBAR", ImGuiWindowFlags_AlwaysHorizontalScrollbar)
        .value("WINDOW_FLAGS__ALWAYS_USE_WINDOW_PADDING", ImGuiWindowFlags_AlwaysUseWindowPadding)
        .value("WINDOW_FLAGS__NO_NAV_INPUTS", ImGuiWindowFlags_NoNavInputs)
        .value("WINDOW_FLAGS__NO_NAV_FOCUS", ImGuiWindowFlags_NoNavFocus)
        .value("WINDOW_FLAGS__NO_NAV", ImGuiWindowFlags_NoNav)
        .value("WINDOW_FLAGS__NAV_FLATTENED", ImGuiWindowFlags_NavFlattened)
        .value("WINDOW_FLAGS__CHILD_WINDOW", ImGuiWindowFlags_ChildWindow)
        .value("WINDOW_FLAGS__TOOLTIP", ImGuiWindowFlags_Tooltip)
        .value("WINDOW_FLAGS__POPUP", ImGuiWindowFlags_Popup)
        .value("WINDOW_FLAGS__MODAL", ImGuiWindowFlags_Modal)
        .value("WINDOW_FLAGS__CHILD_MENU", ImGuiWindowFlags_ChildMenu)
        .export_values();

    py::enum_<ImGuiInputTextFlags_>(deargui, "InputTextFlags")
        .value("INPUT_TEXT_FLAGS__NONE", ImGuiInputTextFlags_None)
        .value("INPUT_TEXT_FLAGS__CHARS_DECIMAL", ImGuiInputTextFlags_CharsDecimal)
        .value("INPUT_TEXT_FLAGS__CHARS_HEXADECIMAL", ImGuiInputTextFlags_CharsHexadecimal)
        .value("INPUT_TEXT_FLAGS__CHARS_UPPERCASE", ImGuiInputTextFlags_CharsUppercase)
        .value("INPUT_TEXT_FLAGS__CHARS_NO_BLANK", ImGuiInputTextFlags_CharsNoBlank)
        .value("INPUT_TEXT_FLAGS__AUTO_SELECT_ALL", ImGuiInputTextFlags_AutoSelectAll)
        .value("INPUT_TEXT_FLAGS__ENTER_RETURNS_TRUE", ImGuiInputTextFlags_EnterReturnsTrue)
        .value("INPUT_TEXT_FLAGS__CALLBACK_COMPLETION", ImGuiInputTextFlags_CallbackCompletion)
        .value("INPUT_TEXT_FLAGS__CALLBACK_HISTORY", ImGuiInputTextFlags_CallbackHistory)
        .value("INPUT_TEXT_FLAGS__CALLBACK_ALWAYS", ImGuiInputTextFlags_CallbackAlways)
        .value("INPUT_TEXT_FLAGS__CALLBACK_CHAR_FILTER", ImGuiInputTextFlags_CallbackCharFilter)
        .value("INPUT_TEXT_FLAGS__ALLOW_TAB_INPUT", ImGuiInputTextFlags_AllowTabInput)
        .value("INPUT_TEXT_FLAGS__CTRL_ENTER_FOR_NEW_LINE", ImGuiInputTextFlags_CtrlEnterForNewLine)
        .value("INPUT_TEXT_FLAGS__NO_HORIZONTAL_SCROLL", ImGuiInputTextFlags_NoHorizontalScroll)
        .value("INPUT_TEXT_FLAGS__ALWAYS_INSERT_MODE", ImGuiInputTextFlags_AlwaysInsertMode)
        .value("INPUT_TEXT_FLAGS__READ_ONLY", ImGuiInputTextFlags_ReadOnly)
        .value("INPUT_TEXT_FLAGS__PASSWORD", ImGuiInputTextFlags_Password)
        .value("INPUT_TEXT_FLAGS__NO_UNDO_REDO", ImGuiInputTextFlags_NoUndoRedo)
        .value("INPUT_TEXT_FLAGS__CHARS_SCIENTIFIC", ImGuiInputTextFlags_CharsScientific)
        .value("INPUT_TEXT_FLAGS__CALLBACK_RESIZE", ImGuiInputTextFlags_CallbackResize)
        .value("INPUT_TEXT_FLAGS__MULTILINE", ImGuiInputTextFlags_Multiline)
        .export_values();

    py::enum_<ImGuiTreeNodeFlags_>(deargui, "TreeNodeFlags")
        .value("TREE_NODE_FLAGS__NONE", ImGuiTreeNodeFlags_None)
        .value("TREE_NODE_FLAGS__SELECTED", ImGuiTreeNodeFlags_Selected)
        .value("TREE_NODE_FLAGS__FRAMED", ImGuiTreeNodeFlags_Framed)
        .value("TREE_NODE_FLAGS__ALLOW_ITEM_OVERLAP", ImGuiTreeNodeFlags_AllowItemOverlap)
        .value("TREE_NODE_FLAGS__NO_TREE_PUSH_ON_OPEN", ImGuiTreeNodeFlags_NoTreePushOnOpen)
        .value("TREE_NODE_FLAGS__NO_AUTO_OPEN_ON_LOG", ImGuiTreeNodeFlags_NoAutoOpenOnLog)
        .value("TREE_NODE_FLAGS__DEFAULT_OPEN", ImGuiTreeNodeFlags_DefaultOpen)
        .value("TREE_NODE_FLAGS__OPEN_ON_DOUBLE_CLICK", ImGuiTreeNodeFlags_OpenOnDoubleClick)
        .value("TREE_NODE_FLAGS__OPEN_ON_ARROW", ImGuiTreeNodeFlags_OpenOnArrow)
        .value("TREE_NODE_FLAGS__LEAF", ImGuiTreeNodeFlags_Leaf)
        .value("TREE_NODE_FLAGS__BULLET", ImGuiTreeNodeFlags_Bullet)
        .value("TREE_NODE_FLAGS__FRAME_PADDING", ImGuiTreeNodeFlags_FramePadding)
        .value("TREE_NODE_FLAGS__NAV_LEFT_JUMPS_BACK_HERE", ImGuiTreeNodeFlags_NavLeftJumpsBackHere)
        .value("TREE_NODE_FLAGS__COLLAPSING_HEADER", ImGuiTreeNodeFlags_CollapsingHeader)
        .export_values();

    py::enum_<ImGuiSelectableFlags_>(deargui, "SelectableFlags")
        .value("SELECTABLE_FLAGS__NONE", ImGuiSelectableFlags_None)
        .value("SELECTABLE_FLAGS__DONT_CLOSE_POPUPS", ImGuiSelectableFlags_DontClosePopups)
        .value("SELECTABLE_FLAGS__SPAN_ALL_COLUMNS", ImGuiSelectableFlags_SpanAllColumns)
        .value("SELECTABLE_FLAGS__ALLOW_DOUBLE_CLICK", ImGuiSelectableFlags_AllowDoubleClick)
        .value("SELECTABLE_FLAGS__DISABLED", ImGuiSelectableFlags_Disabled)
        .export_values();

    py::enum_<ImGuiComboFlags_>(deargui, "ComboFlags")
        .value("COMBO_FLAGS__NONE", ImGuiComboFlags_None)
        .value("COMBO_FLAGS__POPUP_ALIGN_LEFT", ImGuiComboFlags_PopupAlignLeft)
        .value("COMBO_FLAGS__HEIGHT_SMALL", ImGuiComboFlags_HeightSmall)
        .value("COMBO_FLAGS__HEIGHT_REGULAR", ImGuiComboFlags_HeightRegular)
        .value("COMBO_FLAGS__HEIGHT_LARGE", ImGuiComboFlags_HeightLarge)
        .value("COMBO_FLAGS__HEIGHT_LARGEST", ImGuiComboFlags_HeightLargest)
        .value("COMBO_FLAGS__NO_ARROW_BUTTON", ImGuiComboFlags_NoArrowButton)
        .value("COMBO_FLAGS__NO_PREVIEW", ImGuiComboFlags_NoPreview)
        .value("COMBO_FLAGS__HEIGHT_MASK", ImGuiComboFlags_HeightMask_)
        .export_values();

    py::enum_<ImGuiFocusedFlags_>(deargui, "FocusedFlags")
        .value("FOCUSED_FLAGS__NONE", ImGuiFocusedFlags_None)
        .value("FOCUSED_FLAGS__CHILD_WINDOWS", ImGuiFocusedFlags_ChildWindows)
        .value("FOCUSED_FLAGS__ROOT_WINDOW", ImGuiFocusedFlags_RootWindow)
        .value("FOCUSED_FLAGS__ANY_WINDOW", ImGuiFocusedFlags_AnyWindow)
        .value("FOCUSED_FLAGS__ROOT_AND_CHILD_WINDOWS", ImGuiFocusedFlags_RootAndChildWindows)
        .export_values();

    py::enum_<ImGuiHoveredFlags_>(deargui, "HoveredFlags")
        .value("HOVERED_FLAGS__NONE", ImGuiHoveredFlags_None)
        .value("HOVERED_FLAGS__CHILD_WINDOWS", ImGuiHoveredFlags_ChildWindows)
        .value("HOVERED_FLAGS__ROOT_WINDOW", ImGuiHoveredFlags_RootWindow)
        .value("HOVERED_FLAGS__ANY_WINDOW", ImGuiHoveredFlags_AnyWindow)
        .value("HOVERED_FLAGS__ALLOW_WHEN_BLOCKED_BY_POPUP", ImGuiHoveredFlags_AllowWhenBlockedByPopup)
        .value("HOVERED_FLAGS__ALLOW_WHEN_BLOCKED_BY_ACTIVE_ITEM", ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)
        .value("HOVERED_FLAGS__ALLOW_WHEN_OVERLAPPED", ImGuiHoveredFlags_AllowWhenOverlapped)
        .value("HOVERED_FLAGS__ALLOW_WHEN_DISABLED", ImGuiHoveredFlags_AllowWhenDisabled)
        .value("HOVERED_FLAGS__RECT_ONLY", ImGuiHoveredFlags_RectOnly)
        .value("HOVERED_FLAGS__ROOT_AND_CHILD_WINDOWS", ImGuiHoveredFlags_RootAndChildWindows)
        .export_values();

    py::enum_<ImGuiDragDropFlags_>(deargui, "DragDropFlags")
        .value("DRAG_DROP_FLAGS__NONE", ImGuiDragDropFlags_None)
        .value("DRAG_DROP_FLAGS__SOURCE_NO_PREVIEW_TOOLTIP", ImGuiDragDropFlags_SourceNoPreviewTooltip)
        .value("DRAG_DROP_FLAGS__SOURCE_NO_DISABLE_HOVER", ImGuiDragDropFlags_SourceNoDisableHover)
        .value("DRAG_DROP_FLAGS__SOURCE_NO_HOLD_TO_OPEN_OTHERS", ImGuiDragDropFlags_SourceNoHoldToOpenOthers)
        .value("DRAG_DROP_FLAGS__SOURCE_ALLOW_NULL_ID", ImGuiDragDropFlags_SourceAllowNullID)
        .value("DRAG_DROP_FLAGS__SOURCE_EXTERN", ImGuiDragDropFlags_SourceExtern)
        .value("DRAG_DROP_FLAGS__SOURCE_AUTO_EXPIRE_PAYLOAD", ImGuiDragDropFlags_SourceAutoExpirePayload)
        .value("DRAG_DROP_FLAGS__ACCEPT_BEFORE_DELIVERY", ImGuiDragDropFlags_AcceptBeforeDelivery)
        .value("DRAG_DROP_FLAGS__ACCEPT_NO_DRAW_DEFAULT_RECT", ImGuiDragDropFlags_AcceptNoDrawDefaultRect)
        .value("DRAG_DROP_FLAGS__ACCEPT_NO_PREVIEW_TOOLTIP", ImGuiDragDropFlags_AcceptNoPreviewTooltip)
        .value("DRAG_DROP_FLAGS__ACCEPT_PEEK_ONLY", ImGuiDragDropFlags_AcceptPeekOnly)
        .export_values();

    py::enum_<ImGuiDataType_>(deargui, "DataType")
        .value("DATA_TYPE_S32", ImGuiDataType_S32)
        .value("DATA_TYPE_U32", ImGuiDataType_U32)
        .value("DATA_TYPE_S64", ImGuiDataType_S64)
        .value("DATA_TYPE_U64", ImGuiDataType_U64)
        .value("DATA_TYPE__FLOAT", ImGuiDataType_Float)
        .value("DATA_TYPE__DOUBLE", ImGuiDataType_Double)
        .value("DATA_TYPE_COUNT", ImGuiDataType_COUNT)
        .export_values();

    py::enum_<ImGuiDir_>(deargui, "Dir")
        .value("DIR__NONE", ImGuiDir_None)
        .value("DIR__LEFT", ImGuiDir_Left)
        .value("DIR__RIGHT", ImGuiDir_Right)
        .value("DIR__UP", ImGuiDir_Up)
        .value("DIR__DOWN", ImGuiDir_Down)
        .value("DIR_COUNT", ImGuiDir_COUNT)
        .export_values();

    py::enum_<ImGuiKey_>(deargui, "Key")
        .value("KEY__TAB", ImGuiKey_Tab)
        .value("KEY__LEFT_ARROW", ImGuiKey_LeftArrow)
        .value("KEY__RIGHT_ARROW", ImGuiKey_RightArrow)
        .value("KEY__UP_ARROW", ImGuiKey_UpArrow)
        .value("KEY__DOWN_ARROW", ImGuiKey_DownArrow)
        .value("KEY__PAGE_UP", ImGuiKey_PageUp)
        .value("KEY__PAGE_DOWN", ImGuiKey_PageDown)
        .value("KEY__HOME", ImGuiKey_Home)
        .value("KEY__END", ImGuiKey_End)
        .value("KEY__INSERT", ImGuiKey_Insert)
        .value("KEY__DELETE", ImGuiKey_Delete)
        .value("KEY__BACKSPACE", ImGuiKey_Backspace)
        .value("KEY__SPACE", ImGuiKey_Space)
        .value("KEY__ENTER", ImGuiKey_Enter)
        .value("KEY__ESCAPE", ImGuiKey_Escape)
        .value("KEY_A", ImGuiKey_A)
        .value("KEY_C", ImGuiKey_C)
        .value("KEY_V", ImGuiKey_V)
        .value("KEY_X", ImGuiKey_X)
        .value("KEY_Y", ImGuiKey_Y)
        .value("KEY_Z", ImGuiKey_Z)
        .value("KEY_COUNT", ImGuiKey_COUNT)
        .export_values();

    py::enum_<ImGuiNavInput_>(deargui, "NavInput")
        .value("NAV_INPUT__ACTIVATE", ImGuiNavInput_Activate)
        .value("NAV_INPUT__CANCEL", ImGuiNavInput_Cancel)
        .value("NAV_INPUT__INPUT", ImGuiNavInput_Input)
        .value("NAV_INPUT__MENU", ImGuiNavInput_Menu)
        .value("NAV_INPUT__DPAD_LEFT", ImGuiNavInput_DpadLeft)
        .value("NAV_INPUT__DPAD_RIGHT", ImGuiNavInput_DpadRight)
        .value("NAV_INPUT__DPAD_UP", ImGuiNavInput_DpadUp)
        .value("NAV_INPUT__DPAD_DOWN", ImGuiNavInput_DpadDown)
        .value("NAV_INPUT_L_STICK_LEFT", ImGuiNavInput_LStickLeft)
        .value("NAV_INPUT_L_STICK_RIGHT", ImGuiNavInput_LStickRight)
        .value("NAV_INPUT_L_STICK_UP", ImGuiNavInput_LStickUp)
        .value("NAV_INPUT_L_STICK_DOWN", ImGuiNavInput_LStickDown)
        .value("NAV_INPUT__FOCUS_PREV", ImGuiNavInput_FocusPrev)
        .value("NAV_INPUT__FOCUS_NEXT", ImGuiNavInput_FocusNext)
        .value("NAV_INPUT__TWEAK_SLOW", ImGuiNavInput_TweakSlow)
        .value("NAV_INPUT__TWEAK_FAST", ImGuiNavInput_TweakFast)
        .value("NAV_INPUT__KEY_MENU", ImGuiNavInput_KeyMenu_)
        .value("NAV_INPUT__KEY_LEFT", ImGuiNavInput_KeyLeft_)
        .value("NAV_INPUT__KEY_RIGHT", ImGuiNavInput_KeyRight_)
        .value("NAV_INPUT__KEY_UP", ImGuiNavInput_KeyUp_)
        .value("NAV_INPUT__KEY_DOWN", ImGuiNavInput_KeyDown_)
        .value("NAV_INPUT_COUNT", ImGuiNavInput_COUNT)
        .value("NAV_INPUT__INTERNAL_START", ImGuiNavInput_InternalStart_)
        .export_values();

    py::enum_<ImGuiConfigFlags_>(deargui, "ConfigFlags")
        .value("CONFIG_FLAGS__NAV_ENABLE_KEYBOARD", ImGuiConfigFlags_NavEnableKeyboard)
        .value("CONFIG_FLAGS__NAV_ENABLE_GAMEPAD", ImGuiConfigFlags_NavEnableGamepad)
        .value("CONFIG_FLAGS__NAV_ENABLE_SET_MOUSE_POS", ImGuiConfigFlags_NavEnableSetMousePos)
        .value("CONFIG_FLAGS__NAV_NO_CAPTURE_KEYBOARD", ImGuiConfigFlags_NavNoCaptureKeyboard)
        .value("CONFIG_FLAGS__NO_MOUSE", ImGuiConfigFlags_NoMouse)
        .value("CONFIG_FLAGS__NO_MOUSE_CURSOR_CHANGE", ImGuiConfigFlags_NoMouseCursorChange)
        .value("CONFIG_FLAGS__IS_SRGB", ImGuiConfigFlags_IsSRGB)
        .value("CONFIG_FLAGS__IS_TOUCH_SCREEN", ImGuiConfigFlags_IsTouchScreen)
        .export_values();

    py::enum_<ImGuiBackendFlags_>(deargui, "BackendFlags")
        .value("BACKEND_FLAGS__HAS_GAMEPAD", ImGuiBackendFlags_HasGamepad)
        .value("BACKEND_FLAGS__HAS_MOUSE_CURSORS", ImGuiBackendFlags_HasMouseCursors)
        .value("BACKEND_FLAGS__HAS_SET_MOUSE_POS", ImGuiBackendFlags_HasSetMousePos)
        .export_values();

    py::enum_<ImGuiCol_>(deargui, "Col")
        .value("COL__TEXT", ImGuiCol_Text)
        .value("COL__TEXT_DISABLED", ImGuiCol_TextDisabled)
        .value("COL__WINDOW_BG", ImGuiCol_WindowBg)
        .value("COL__CHILD_BG", ImGuiCol_ChildBg)
        .value("COL__POPUP_BG", ImGuiCol_PopupBg)
        .value("COL__BORDER", ImGuiCol_Border)
        .value("COL__BORDER_SHADOW", ImGuiCol_BorderShadow)
        .value("COL__FRAME_BG", ImGuiCol_FrameBg)
        .value("COL__FRAME_BG_HOVERED", ImGuiCol_FrameBgHovered)
        .value("COL__FRAME_BG_ACTIVE", ImGuiCol_FrameBgActive)
        .value("COL__TITLE_BG", ImGuiCol_TitleBg)
        .value("COL__TITLE_BG_ACTIVE", ImGuiCol_TitleBgActive)
        .value("COL__TITLE_BG_COLLAPSED", ImGuiCol_TitleBgCollapsed)
        .value("COL__MENU_BAR_BG", ImGuiCol_MenuBarBg)
        .value("COL__SCROLLBAR_BG", ImGuiCol_ScrollbarBg)
        .value("COL__SCROLLBAR_GRAB", ImGuiCol_ScrollbarGrab)
        .value("COL__SCROLLBAR_GRAB_HOVERED", ImGuiCol_ScrollbarGrabHovered)
        .value("COL__SCROLLBAR_GRAB_ACTIVE", ImGuiCol_ScrollbarGrabActive)
        .value("COL__CHECK_MARK", ImGuiCol_CheckMark)
        .value("COL__SLIDER_GRAB", ImGuiCol_SliderGrab)
        .value("COL__SLIDER_GRAB_ACTIVE", ImGuiCol_SliderGrabActive)
        .value("COL__BUTTON", ImGuiCol_Button)
        .value("COL__BUTTON_HOVERED", ImGuiCol_ButtonHovered)
        .value("COL__BUTTON_ACTIVE", ImGuiCol_ButtonActive)
        .value("COL__HEADER", ImGuiCol_Header)
        .value("COL__HEADER_HOVERED", ImGuiCol_HeaderHovered)
        .value("COL__HEADER_ACTIVE", ImGuiCol_HeaderActive)
        .value("COL__SEPARATOR", ImGuiCol_Separator)
        .value("COL__SEPARATOR_HOVERED", ImGuiCol_SeparatorHovered)
        .value("COL__SEPARATOR_ACTIVE", ImGuiCol_SeparatorActive)
        .value("COL__RESIZE_GRIP", ImGuiCol_ResizeGrip)
        .value("COL__RESIZE_GRIP_HOVERED", ImGuiCol_ResizeGripHovered)
        .value("COL__RESIZE_GRIP_ACTIVE", ImGuiCol_ResizeGripActive)
        .value("COL__PLOT_LINES", ImGuiCol_PlotLines)
        .value("COL__PLOT_LINES_HOVERED", ImGuiCol_PlotLinesHovered)
        .value("COL__PLOT_HISTOGRAM", ImGuiCol_PlotHistogram)
        .value("COL__PLOT_HISTOGRAM_HOVERED", ImGuiCol_PlotHistogramHovered)
        .value("COL__TEXT_SELECTED_BG", ImGuiCol_TextSelectedBg)
        .value("COL__DRAG_DROP_TARGET", ImGuiCol_DragDropTarget)
        .value("COL__NAV_HIGHLIGHT", ImGuiCol_NavHighlight)
        .value("COL__NAV_WINDOWING_HIGHLIGHT", ImGuiCol_NavWindowingHighlight)
        .value("COL__NAV_WINDOWING_DIM_BG", ImGuiCol_NavWindowingDimBg)
        .value("COL__MODAL_WINDOW_DIM_BG", ImGuiCol_ModalWindowDimBg)
        .value("COL_COUNT", ImGuiCol_COUNT)
        .export_values();

    py::enum_<ImGuiStyleVar_>(deargui, "StyleVar")
        .value("STYLE_VAR__ALPHA", ImGuiStyleVar_Alpha)
        .value("STYLE_VAR__WINDOW_PADDING", ImGuiStyleVar_WindowPadding)
        .value("STYLE_VAR__WINDOW_ROUNDING", ImGuiStyleVar_WindowRounding)
        .value("STYLE_VAR__WINDOW_BORDER_SIZE", ImGuiStyleVar_WindowBorderSize)
        .value("STYLE_VAR__WINDOW_MIN_SIZE", ImGuiStyleVar_WindowMinSize)
        .value("STYLE_VAR__WINDOW_TITLE_ALIGN", ImGuiStyleVar_WindowTitleAlign)
        .value("STYLE_VAR__CHILD_ROUNDING", ImGuiStyleVar_ChildRounding)
        .value("STYLE_VAR__CHILD_BORDER_SIZE", ImGuiStyleVar_ChildBorderSize)
        .value("STYLE_VAR__POPUP_ROUNDING", ImGuiStyleVar_PopupRounding)
        .value("STYLE_VAR__POPUP_BORDER_SIZE", ImGuiStyleVar_PopupBorderSize)
        .value("STYLE_VAR__FRAME_PADDING", ImGuiStyleVar_FramePadding)
        .value("STYLE_VAR__FRAME_ROUNDING", ImGuiStyleVar_FrameRounding)
        .value("STYLE_VAR__FRAME_BORDER_SIZE", ImGuiStyleVar_FrameBorderSize)
        .value("STYLE_VAR__ITEM_SPACING", ImGuiStyleVar_ItemSpacing)
        .value("STYLE_VAR__ITEM_INNER_SPACING", ImGuiStyleVar_ItemInnerSpacing)
        .value("STYLE_VAR__INDENT_SPACING", ImGuiStyleVar_IndentSpacing)
        .value("STYLE_VAR__SCROLLBAR_SIZE", ImGuiStyleVar_ScrollbarSize)
        .value("STYLE_VAR__SCROLLBAR_ROUNDING", ImGuiStyleVar_ScrollbarRounding)
        .value("STYLE_VAR__GRAB_MIN_SIZE", ImGuiStyleVar_GrabMinSize)
        .value("STYLE_VAR__GRAB_ROUNDING", ImGuiStyleVar_GrabRounding)
        .value("STYLE_VAR__BUTTON_TEXT_ALIGN", ImGuiStyleVar_ButtonTextAlign)
        .value("STYLE_VAR_COUNT", ImGuiStyleVar_COUNT)
        .export_values();

    py::enum_<ImGuiColorEditFlags_>(deargui, "ColorEditFlags")
        .value("COLOR_EDIT_FLAGS__NONE", ImGuiColorEditFlags_None)
        .value("COLOR_EDIT_FLAGS__NO_ALPHA", ImGuiColorEditFlags_NoAlpha)
        .value("COLOR_EDIT_FLAGS__NO_PICKER", ImGuiColorEditFlags_NoPicker)
        .value("COLOR_EDIT_FLAGS__NO_OPTIONS", ImGuiColorEditFlags_NoOptions)
        .value("COLOR_EDIT_FLAGS__NO_SMALL_PREVIEW", ImGuiColorEditFlags_NoSmallPreview)
        .value("COLOR_EDIT_FLAGS__NO_INPUTS", ImGuiColorEditFlags_NoInputs)
        .value("COLOR_EDIT_FLAGS__NO_TOOLTIP", ImGuiColorEditFlags_NoTooltip)
        .value("COLOR_EDIT_FLAGS__NO_LABEL", ImGuiColorEditFlags_NoLabel)
        .value("COLOR_EDIT_FLAGS__NO_SIDE_PREVIEW", ImGuiColorEditFlags_NoSidePreview)
        .value("COLOR_EDIT_FLAGS__NO_DRAG_DROP", ImGuiColorEditFlags_NoDragDrop)
        .value("COLOR_EDIT_FLAGS__ALPHA_BAR", ImGuiColorEditFlags_AlphaBar)
        .value("COLOR_EDIT_FLAGS__ALPHA_PREVIEW", ImGuiColorEditFlags_AlphaPreview)
        .value("COLOR_EDIT_FLAGS__ALPHA_PREVIEW_HALF", ImGuiColorEditFlags_AlphaPreviewHalf)
        .value("COLOR_EDIT_FLAGS_HDR", ImGuiColorEditFlags_HDR)
        .value("COLOR_EDIT_FLAGS_RGB", ImGuiColorEditFlags_RGB)
        .value("COLOR_EDIT_FLAGS_HSV", ImGuiColorEditFlags_HSV)
        .value("COLOR_EDIT_FLAGS_HEX", ImGuiColorEditFlags_HEX)
        .value("COLOR_EDIT_FLAGS__UINT8", ImGuiColorEditFlags_Uint8)
        .value("COLOR_EDIT_FLAGS__FLOAT", ImGuiColorEditFlags_Float)
        .value("COLOR_EDIT_FLAGS__PICKER_HUE_BAR", ImGuiColorEditFlags_PickerHueBar)
        .value("COLOR_EDIT_FLAGS__PICKER_HUE_WHEEL", ImGuiColorEditFlags_PickerHueWheel)
        .value("COLOR_EDIT_FLAGS___INPUTS_MASK", ImGuiColorEditFlags__InputsMask)
        .value("COLOR_EDIT_FLAGS___DATA_TYPE_MASK", ImGuiColorEditFlags__DataTypeMask)
        .value("COLOR_EDIT_FLAGS___PICKER_MASK", ImGuiColorEditFlags__PickerMask)
        .value("COLOR_EDIT_FLAGS___OPTIONS_DEFAULT", ImGuiColorEditFlags__OptionsDefault)
        .export_values();

    py::enum_<ImGuiMouseCursor_>(deargui, "MouseCursor")
        .value("MOUSE_CURSOR__NONE", ImGuiMouseCursor_None)
        .value("MOUSE_CURSOR__ARROW", ImGuiMouseCursor_Arrow)
        .value("MOUSE_CURSOR__TEXT_INPUT", ImGuiMouseCursor_TextInput)
        .value("MOUSE_CURSOR__RESIZE_ALL", ImGuiMouseCursor_ResizeAll)
        .value("MOUSE_CURSOR__RESIZE_NS", ImGuiMouseCursor_ResizeNS)
        .value("MOUSE_CURSOR__RESIZE_EW", ImGuiMouseCursor_ResizeEW)
        .value("MOUSE_CURSOR__RESIZE_NESW", ImGuiMouseCursor_ResizeNESW)
        .value("MOUSE_CURSOR__RESIZE_NWSE", ImGuiMouseCursor_ResizeNWSE)
        .value("MOUSE_CURSOR__HAND", ImGuiMouseCursor_Hand)
        .value("MOUSE_CURSOR_COUNT", ImGuiMouseCursor_COUNT)
        .export_values();

    py::enum_<ImGuiCond_>(deargui, "Cond")
        .value("COND__ALWAYS", ImGuiCond_Always)
        .value("COND__ONCE", ImGuiCond_Once)
        .value("COND__FIRST_USE_EVER", ImGuiCond_FirstUseEver)
        .value("COND__APPEARING", ImGuiCond_Appearing)
        .export_values();

    py::class_<ImGuiStyle>(deargui, "Style")
        .def_readwrite("alpha", &ImGuiStyle::Alpha)
        .def_readwrite("window_padding", &ImGuiStyle::WindowPadding)
        .def_readwrite("window_rounding", &ImGuiStyle::WindowRounding)
        .def_readwrite("window_border_size", &ImGuiStyle::WindowBorderSize)
        .def_readwrite("window_min_size", &ImGuiStyle::WindowMinSize)
        .def_readwrite("window_title_align", &ImGuiStyle::WindowTitleAlign)
        .def_readwrite("child_rounding", &ImGuiStyle::ChildRounding)
        .def_readwrite("child_border_size", &ImGuiStyle::ChildBorderSize)
        .def_readwrite("popup_rounding", &ImGuiStyle::PopupRounding)
        .def_readwrite("popup_border_size", &ImGuiStyle::PopupBorderSize)
        .def_readwrite("frame_padding", &ImGuiStyle::FramePadding)
        .def_readwrite("frame_rounding", &ImGuiStyle::FrameRounding)
        .def_readwrite("frame_border_size", &ImGuiStyle::FrameBorderSize)
        .def_readwrite("item_spacing", &ImGuiStyle::ItemSpacing)
        .def_readwrite("item_inner_spacing", &ImGuiStyle::ItemInnerSpacing)
        .def_readwrite("touch_extra_padding", &ImGuiStyle::TouchExtraPadding)
        .def_readwrite("indent_spacing", &ImGuiStyle::IndentSpacing)
        .def_readwrite("columns_min_spacing", &ImGuiStyle::ColumnsMinSpacing)
        .def_readwrite("scrollbar_size", &ImGuiStyle::ScrollbarSize)
        .def_readwrite("scrollbar_rounding", &ImGuiStyle::ScrollbarRounding)
        .def_readwrite("grab_min_size", &ImGuiStyle::GrabMinSize)
        .def_readwrite("grab_rounding", &ImGuiStyle::GrabRounding)
        .def_readwrite("button_text_align", &ImGuiStyle::ButtonTextAlign)
        .def_readwrite("display_window_padding", &ImGuiStyle::DisplayWindowPadding)
        .def_readwrite("display_safe_area_padding", &ImGuiStyle::DisplaySafeAreaPadding)
        .def_readwrite("mouse_cursor_scale", &ImGuiStyle::MouseCursorScale)
        .def_readwrite("anti_aliased_lines", &ImGuiStyle::AntiAliasedLines)
        .def_readwrite("anti_aliased_fill", &ImGuiStyle::AntiAliasedFill)
        .def_readwrite("curve_tessellation_tol", &ImGuiStyle::CurveTessellationTol)
        .def_readonly("colors", &ImGuiStyle::Colors)
        .def(py::init<>())
        .def("scale_all_sizes", &ImGuiStyle::ScaleAllSizes
           , py::arg("scale_factor")
        )

    ;

    py::class_<ImGuiIO>(deargui, "IO")
        .def_readwrite("config_flags", &ImGuiIO::ConfigFlags)
        .def_readwrite("backend_flags", &ImGuiIO::BackendFlags)
        .def_readwrite("display_size", &ImGuiIO::DisplaySize)
        .def_readwrite("delta_time", &ImGuiIO::DeltaTime)
        .def_readwrite("ini_saving_rate", &ImGuiIO::IniSavingRate)
        .def_readwrite("ini_filename", &ImGuiIO::IniFilename)
        .def_readwrite("log_filename", &ImGuiIO::LogFilename)
        .def_readwrite("mouse_double_click_time", &ImGuiIO::MouseDoubleClickTime)
        .def_readwrite("mouse_double_click_max_dist", &ImGuiIO::MouseDoubleClickMaxDist)
        .def_readwrite("mouse_drag_threshold", &ImGuiIO::MouseDragThreshold)
        .def_readonly("key_map", &ImGuiIO::KeyMap)
        .def_readwrite("key_repeat_delay", &ImGuiIO::KeyRepeatDelay)
        .def_readwrite("key_repeat_rate", &ImGuiIO::KeyRepeatRate)
        .def_readwrite("user_data", &ImGuiIO::UserData)
        .def_readwrite("fonts", &ImGuiIO::Fonts)
        .def_readwrite("font_global_scale", &ImGuiIO::FontGlobalScale)
        .def_readwrite("font_allow_user_scaling", &ImGuiIO::FontAllowUserScaling)
        .def_readwrite("font_default", &ImGuiIO::FontDefault)
        .def_readwrite("display_framebuffer_scale", &ImGuiIO::DisplayFramebufferScale)
        .def_readwrite("mouse_draw_cursor", &ImGuiIO::MouseDrawCursor)
        .def_readwrite("config_mac_osx_behaviors", &ImGuiIO::ConfigMacOSXBehaviors)
        .def_readwrite("config_input_text_cursor_blink", &ImGuiIO::ConfigInputTextCursorBlink)
        .def_readwrite("config_resize_windows_from_edges", &ImGuiIO::ConfigResizeWindowsFromEdges)
        .def_readwrite("clipboard_user_data", &ImGuiIO::ClipboardUserData)
        .def_readwrite("ime_window_handle", &ImGuiIO::ImeWindowHandle)
        .def_readwrite("render_draw_lists_fn_unused", &ImGuiIO::RenderDrawListsFnUnused)
        .def_readwrite("mouse_pos", &ImGuiIO::MousePos)
        .def_readwrite("mouse_wheel", &ImGuiIO::MouseWheel)
        .def_readwrite("mouse_wheel_h", &ImGuiIO::MouseWheelH)
        .def_readwrite("key_ctrl", &ImGuiIO::KeyCtrl)
        .def_readwrite("key_shift", &ImGuiIO::KeyShift)
        .def_readwrite("key_alt", &ImGuiIO::KeyAlt)
        .def_readwrite("key_super", &ImGuiIO::KeySuper)
        .def("add_input_character", &ImGuiIO::AddInputCharacter
           , py::arg("c")
        )
        .def("add_input_characters_utf8", &ImGuiIO::AddInputCharactersUTF8
           , py::arg("utf8_chars")
        )
        .def("clear_input_characters", &ImGuiIO::ClearInputCharacters)
        .def_readwrite("want_capture_mouse", &ImGuiIO::WantCaptureMouse)
        .def_readwrite("want_capture_keyboard", &ImGuiIO::WantCaptureKeyboard)
        .def_readwrite("want_text_input", &ImGuiIO::WantTextInput)
        .def_readwrite("want_set_mouse_pos", &ImGuiIO::WantSetMousePos)
        .def_readwrite("want_save_ini_settings", &ImGuiIO::WantSaveIniSettings)
        .def_readwrite("nav_active", &ImGuiIO::NavActive)
        .def_readwrite("nav_visible", &ImGuiIO::NavVisible)
        .def_readwrite("framerate", &ImGuiIO::Framerate)
        .def_readwrite("metrics_render_vertices", &ImGuiIO::MetricsRenderVertices)
        .def_readwrite("metrics_render_indices", &ImGuiIO::MetricsRenderIndices)
        .def_readwrite("metrics_render_windows", &ImGuiIO::MetricsRenderWindows)
        .def_readwrite("metrics_active_windows", &ImGuiIO::MetricsActiveWindows)
        .def_readwrite("metrics_active_allocations", &ImGuiIO::MetricsActiveAllocations)
        .def_readwrite("mouse_delta", &ImGuiIO::MouseDelta)
        .def_readwrite("mouse_pos_prev", &ImGuiIO::MousePosPrev)
        .def_readonly("mouse_clicked_pos", &ImGuiIO::MouseClickedPos)
        .def_readonly("mouse_clicked_time", &ImGuiIO::MouseClickedTime)
        .def_readonly("mouse_clicked", &ImGuiIO::MouseClicked)
        .def_readonly("mouse_double_clicked", &ImGuiIO::MouseDoubleClicked)
        .def_readonly("mouse_released", &ImGuiIO::MouseReleased)
        .def_readonly("mouse_down_owned", &ImGuiIO::MouseDownOwned)
        .def_readonly("mouse_down_duration", &ImGuiIO::MouseDownDuration)
        .def_readonly("mouse_down_duration_prev", &ImGuiIO::MouseDownDurationPrev)
        .def_readonly("mouse_drag_max_distance_abs", &ImGuiIO::MouseDragMaxDistanceAbs)
        .def_readonly("mouse_drag_max_distance_sqr", &ImGuiIO::MouseDragMaxDistanceSqr)
        .def_readonly("keys_down_duration", &ImGuiIO::KeysDownDuration)
        .def_readonly("keys_down_duration_prev", &ImGuiIO::KeysDownDurationPrev)
        .def_readonly("nav_inputs_down_duration", &ImGuiIO::NavInputsDownDuration)
        .def_readonly("nav_inputs_down_duration_prev", &ImGuiIO::NavInputsDownDurationPrev)
        .def(py::init<>())

    ;

    py::class_<ImGuiOnceUponAFrame>(deargui, "OnceUponAFrame")
        .def(py::init<>())
        .def_readwrite("ref_frame", &ImGuiOnceUponAFrame::RefFrame)

    ;

    py::class_<ImGuiTextFilter>(deargui, "TextFilter")
        .def(py::init<const char *>()
           , py::arg("default_filter") = nullptr
        )
        .def("draw", &ImGuiTextFilter::Draw
           , py::arg("label") = nullptr
           , py::arg("width") = 0.0f
        )
        .def("pass_filter", &ImGuiTextFilter::PassFilter
           , py::arg("text")
           , py::arg("text_end") = nullptr
        )
        .def("build", &ImGuiTextFilter::Build)
        .def("clear", &ImGuiTextFilter::Clear)
        .def("is_active", &ImGuiTextFilter::IsActive)
        .def_readonly("input_buf", &ImGuiTextFilter::InputBuf)
        .def_readwrite("count_grep", &ImGuiTextFilter::CountGrep)

    ;

    py::class_<ImGuiTextBuffer>(deargui, "TextBuffer")
        .def_readwrite("buf", &ImGuiTextBuffer::Buf)
        .def(py::init<>())
        .def("begin", &ImGuiTextBuffer::begin)
        .def("end", &ImGuiTextBuffer::end)
        .def("size", &ImGuiTextBuffer::size)
        .def("empty", &ImGuiTextBuffer::empty)
        .def("clear", &ImGuiTextBuffer::clear)
        .def("reserve", &ImGuiTextBuffer::reserve
           , py::arg("capacity")
        )
        .def("c_str", &ImGuiTextBuffer::c_str)
        .def("appendfv", &ImGuiTextBuffer::appendfv
           , py::arg("fmt")
           , py::arg("args")
        )

    ;

    py::class_<ImGuiStorage>(deargui, "Storage")
        .def("clear", &ImGuiStorage::Clear)
        .def("get_int", &ImGuiStorage::GetInt
           , py::arg("key")
           , py::arg("default_val") = 0
        )
        .def("set_int", &ImGuiStorage::SetInt
           , py::arg("key")
           , py::arg("val")
        )
        .def("get_bool", &ImGuiStorage::GetBool
           , py::arg("key")
           , py::arg("default_val") = false
        )
        .def("set_bool", &ImGuiStorage::SetBool
           , py::arg("key")
           , py::arg("val")
        )
        .def("get_float", &ImGuiStorage::GetFloat
           , py::arg("key")
           , py::arg("default_val") = 0.0f
        )
        .def("set_float", &ImGuiStorage::SetFloat
           , py::arg("key")
           , py::arg("val")
        )
        .def("get_void_ptr", &ImGuiStorage::GetVoidPtr
           , py::arg("key")
        )
        .def("set_void_ptr", &ImGuiStorage::SetVoidPtr
           , py::arg("key")
           , py::arg("val")
        )
        .def("get_int_ref", &ImGuiStorage::GetIntRef
           , py::arg("key")
           , py::arg("default_val") = 0
        )
        .def("get_bool_ref", &ImGuiStorage::GetBoolRef
           , py::arg("key")
           , py::arg("default_val") = false
        )
        .def("get_float_ref", &ImGuiStorage::GetFloatRef
           , py::arg("key")
           , py::arg("default_val") = 0.0f
        )
        .def("get_void_ptr_ref", &ImGuiStorage::GetVoidPtrRef
           , py::arg("key")
           , py::arg("default_val") = nullptr
        )
        .def("set_all_int", &ImGuiStorage::SetAllInt
           , py::arg("val")
        )
        .def("build_sort_by_key", &ImGuiStorage::BuildSortByKey)

    ;

    py::class_<ImGuiInputTextCallbackData>(deargui, "InputTextCallbackData")
        .def_readwrite("event_flag", &ImGuiInputTextCallbackData::EventFlag)
        .def_readwrite("flags", &ImGuiInputTextCallbackData::Flags)
        .def_readwrite("user_data", &ImGuiInputTextCallbackData::UserData)
        .def_readwrite("event_char", &ImGuiInputTextCallbackData::EventChar)
        .def_readwrite("event_key", &ImGuiInputTextCallbackData::EventKey)
        .def_readwrite("buf", &ImGuiInputTextCallbackData::Buf)
        .def_readwrite("buf_text_len", &ImGuiInputTextCallbackData::BufTextLen)
        .def_readwrite("buf_size", &ImGuiInputTextCallbackData::BufSize)
        .def_readwrite("buf_dirty", &ImGuiInputTextCallbackData::BufDirty)
        .def_readwrite("cursor_pos", &ImGuiInputTextCallbackData::CursorPos)
        .def_readwrite("selection_start", &ImGuiInputTextCallbackData::SelectionStart)
        .def_readwrite("selection_end", &ImGuiInputTextCallbackData::SelectionEnd)
        .def(py::init<>())
        .def("delete_chars", &ImGuiInputTextCallbackData::DeleteChars
           , py::arg("pos")
           , py::arg("bytes_count")
        )
        .def("insert_chars", &ImGuiInputTextCallbackData::InsertChars
           , py::arg("pos")
           , py::arg("text")
           , py::arg("text_end") = nullptr
        )
        .def("has_selection", &ImGuiInputTextCallbackData::HasSelection)

    ;

    py::class_<ImGuiSizeCallbackData>(deargui, "SizeCallbackData")
        .def_readwrite("user_data", &ImGuiSizeCallbackData::UserData)
        .def_readwrite("pos", &ImGuiSizeCallbackData::Pos)
        .def_readwrite("current_size", &ImGuiSizeCallbackData::CurrentSize)
        .def_readwrite("desired_size", &ImGuiSizeCallbackData::DesiredSize)

    ;

    py::class_<ImGuiPayload>(deargui, "Payload")
        .def_readwrite("data", &ImGuiPayload::Data)
        .def_readwrite("data_size", &ImGuiPayload::DataSize)
        .def_readwrite("source_id", &ImGuiPayload::SourceId)
        .def_readwrite("source_parent_id", &ImGuiPayload::SourceParentId)
        .def_readwrite("data_frame_count", &ImGuiPayload::DataFrameCount)
        .def_readonly("data_type", &ImGuiPayload::DataType)
        .def_readwrite("preview", &ImGuiPayload::Preview)
        .def_readwrite("delivery", &ImGuiPayload::Delivery)
        .def(py::init<>())
        .def("clear", &ImGuiPayload::Clear)
        .def("is_data_type", &ImGuiPayload::IsDataType
           , py::arg("type")
        )
        .def("is_preview", &ImGuiPayload::IsPreview)
        .def("is_delivery", &ImGuiPayload::IsDelivery)

    ;

    py::class_<ImGuiListClipper>(deargui, "ListClipper")
        .def_readwrite("start_pos_y", &ImGuiListClipper::StartPosY)
        .def_readwrite("items_height", &ImGuiListClipper::ItemsHeight)
        .def_readwrite("items_count", &ImGuiListClipper::ItemsCount)
        .def_readwrite("step_no", &ImGuiListClipper::StepNo)
        .def_readwrite("display_start", &ImGuiListClipper::DisplayStart)
        .def_readwrite("display_end", &ImGuiListClipper::DisplayEnd)
        .def(py::init<int, float>()
           , py::arg("items_count") = -1
           , py::arg("items_height") = -1.0f
        )
        .def("step", &ImGuiListClipper::Step)
        .def("begin", &ImGuiListClipper::Begin
           , py::arg("items_count")
           , py::arg("items_height") = -1.0f
        )
        .def("end", &ImGuiListClipper::End)

    ;

    py::class_<ImDrawCmd>(deargui, "DrawCmd")
        .def_readwrite("elem_count", &ImDrawCmd::ElemCount)
        .def_readwrite("clip_rect", &ImDrawCmd::ClipRect)
        .def_readwrite("texture_id", &ImDrawCmd::TextureId)
        .def_readwrite("user_callback_data", &ImDrawCmd::UserCallbackData)
        .def(py::init<>())

    ;

    py::class_<ImDrawVert>(deargui, "DrawVert")
        .def_readwrite("pos", &ImDrawVert::pos)
        .def_readwrite("uv", &ImDrawVert::uv)
        .def_readwrite("col", &ImDrawVert::col)
        .def_property_readonly_static("pos_offset", [](py::object)
        {
            return IM_OFFSETOF(ImDrawVert, pos);
        })
        .def_property_readonly_static("uv_offset", [](py::object)
        {
            return IM_OFFSETOF(ImDrawVert, uv);
        })
        .def_property_readonly_static("col_offset", [](py::object)
        {
            return IM_OFFSETOF(ImDrawVert, col);
        })
    ;

    py::class_<ImDrawChannel>(deargui, "DrawChannel")
        .def_readwrite("cmd_buffer", &ImDrawChannel::CmdBuffer)
        .def_readwrite("idx_buffer", &ImDrawChannel::IdxBuffer)

    ;

    py::enum_<ImDrawCornerFlags_>(deargui, "DrawCornerFlags")
        .value("DRAW_CORNER_FLAGS__TOP_LEFT", ImDrawCornerFlags_TopLeft)
        .value("DRAW_CORNER_FLAGS__TOP_RIGHT", ImDrawCornerFlags_TopRight)
        .value("DRAW_CORNER_FLAGS__BOT_LEFT", ImDrawCornerFlags_BotLeft)
        .value("DRAW_CORNER_FLAGS__BOT_RIGHT", ImDrawCornerFlags_BotRight)
        .value("DRAW_CORNER_FLAGS__TOP", ImDrawCornerFlags_Top)
        .value("DRAW_CORNER_FLAGS__BOT", ImDrawCornerFlags_Bot)
        .value("DRAW_CORNER_FLAGS__LEFT", ImDrawCornerFlags_Left)
        .value("DRAW_CORNER_FLAGS__RIGHT", ImDrawCornerFlags_Right)
        .value("DRAW_CORNER_FLAGS__ALL", ImDrawCornerFlags_All)
        .export_values();

    py::enum_<ImDrawListFlags_>(deargui, "DrawListFlags")
        .value("DRAW_LIST_FLAGS__ANTI_ALIASED_LINES", ImDrawListFlags_AntiAliasedLines)
        .value("DRAW_LIST_FLAGS__ANTI_ALIASED_FILL", ImDrawListFlags_AntiAliasedFill)
        .export_values();

    py::class_<ImDrawList>(deargui, "DrawList")
        .def_readwrite("cmd_buffer", &ImDrawList::CmdBuffer)
        .def_readwrite("idx_buffer", &ImDrawList::IdxBuffer)
        .def_readwrite("vtx_buffer", &ImDrawList::VtxBuffer)
        .def_readwrite("flags", &ImDrawList::Flags)
        .def(py::init<const ImDrawListSharedData *>()
           , py::arg("shared_data")
        )
        .def("push_clip_rect", &ImDrawList::PushClipRect
           , py::arg("clip_rect_min")
           , py::arg("clip_rect_max")
           , py::arg("intersect_with_current_clip_rect") = false
        )
        .def("push_clip_rect_full_screen", &ImDrawList::PushClipRectFullScreen)
        .def("pop_clip_rect", &ImDrawList::PopClipRect)
        .def("push_texture_id", &ImDrawList::PushTextureID
           , py::arg("texture_id")
        )
        .def("pop_texture_id", &ImDrawList::PopTextureID)
        .def("get_clip_rect_min", &ImDrawList::GetClipRectMin)
        .def("get_clip_rect_max", &ImDrawList::GetClipRectMax)
        .def("add_line", &ImDrawList::AddLine
           , py::arg("a")
           , py::arg("b")
           , py::arg("col")
           , py::arg("thickness") = 1.0f
        )
        .def("add_rect", &ImDrawList::AddRect
           , py::arg("a")
           , py::arg("b")
           , py::arg("col")
           , py::arg("rounding") = 0.0f
           , py::arg("rounding_corners_flags") = ImDrawCornerFlags_All
           , py::arg("thickness") = 1.0f
        )
        .def("add_rect_filled", &ImDrawList::AddRectFilled
           , py::arg("a")
           , py::arg("b")
           , py::arg("col")
           , py::arg("rounding") = 0.0f
           , py::arg("rounding_corners_flags") = ImDrawCornerFlags_All
        )
        .def("add_rect_filled_multi_color", &ImDrawList::AddRectFilledMultiColor
           , py::arg("a")
           , py::arg("b")
           , py::arg("col_upr_left")
           , py::arg("col_upr_right")
           , py::arg("col_bot_right")
           , py::arg("col_bot_left")
        )
        .def("add_quad", &ImDrawList::AddQuad
           , py::arg("a")
           , py::arg("b")
           , py::arg("c")
           , py::arg("d")
           , py::arg("col")
           , py::arg("thickness") = 1.0f
        )
        .def("add_quad_filled", &ImDrawList::AddQuadFilled
           , py::arg("a")
           , py::arg("b")
           , py::arg("c")
           , py::arg("d")
           , py::arg("col")
        )
        .def("add_triangle", &ImDrawList::AddTriangle
           , py::arg("a")
           , py::arg("b")
           , py::arg("c")
           , py::arg("col")
           , py::arg("thickness") = 1.0f
        )
        .def("add_triangle_filled", &ImDrawList::AddTriangleFilled
           , py::arg("a")
           , py::arg("b")
           , py::arg("c")
           , py::arg("col")
        )
        .def("add_circle", &ImDrawList::AddCircle
           , py::arg("centre")
           , py::arg("radius")
           , py::arg("col")
           , py::arg("num_segments") = 12
           , py::arg("thickness") = 1.0f
        )
        .def("add_circle_filled", &ImDrawList::AddCircleFilled
           , py::arg("centre")
           , py::arg("radius")
           , py::arg("col")
           , py::arg("num_segments") = 12
        )
        .def("add_text", py::overload_cast<const ImVec2 &, ImU32, const char *, const char *>(&ImDrawList::AddText)
           , py::arg("pos")
           , py::arg("col")
           , py::arg("text_begin")
           , py::arg("text_end") = nullptr
        )
        .def("add_text", py::overload_cast<const ImFont *, float, const ImVec2 &, ImU32, const char *, const char *, float, const ImVec4 *>(&ImDrawList::AddText)
           , py::arg("font")
           , py::arg("font_size")
           , py::arg("pos")
           , py::arg("col")
           , py::arg("text_begin")
           , py::arg("text_end") = nullptr
           , py::arg("wrap_width") = 0.0f
           , py::arg("cpu_fine_clip_rect") = nullptr
        )
        .def("add_image", &ImDrawList::AddImage
           , py::arg("user_texture_id")
           , py::arg("a")
           , py::arg("b")
           , py::arg("uv_a") = ImVec2(0,0)
           , py::arg("uv_b") = ImVec2(1,1)
           , py::arg("col") = 0xFFFFFFFF
        )
        .def("add_image_quad", &ImDrawList::AddImageQuad
           , py::arg("user_texture_id")
           , py::arg("a")
           , py::arg("b")
           , py::arg("c")
           , py::arg("d")
           , py::arg("uv_a") = ImVec2(0,0)
           , py::arg("uv_b") = ImVec2(1,0)
           , py::arg("uv_c") = ImVec2(1,1)
           , py::arg("uv_d") = ImVec2(0,1)
           , py::arg("col") = 0xFFFFFFFF
        )
        .def("add_image_rounded", &ImDrawList::AddImageRounded
           , py::arg("user_texture_id")
           , py::arg("a")
           , py::arg("b")
           , py::arg("uv_a")
           , py::arg("uv_b")
           , py::arg("col")
           , py::arg("rounding")
           , py::arg("rounding_corners") = ImDrawCornerFlags_All
        )
        .def("add_polyline", &ImDrawList::AddPolyline
           , py::arg("points")
           , py::arg("num_points")
           , py::arg("col")
           , py::arg("closed")
           , py::arg("thickness")
        )
        .def("add_convex_poly_filled", &ImDrawList::AddConvexPolyFilled
           , py::arg("points")
           , py::arg("num_points")
           , py::arg("col")
        )
        .def("add_bezier_curve", &ImDrawList::AddBezierCurve
           , py::arg("pos0")
           , py::arg("cp0")
           , py::arg("cp1")
           , py::arg("pos1")
           , py::arg("col")
           , py::arg("thickness")
           , py::arg("num_segments") = 0
        )
        .def("path_clear", &ImDrawList::PathClear)
        .def("path_line_to", &ImDrawList::PathLineTo
           , py::arg("pos")
        )
        .def("path_line_to_merge_duplicate", &ImDrawList::PathLineToMergeDuplicate
           , py::arg("pos")
        )
        .def("path_fill_convex", &ImDrawList::PathFillConvex
           , py::arg("col")
        )
        .def("path_stroke", &ImDrawList::PathStroke
           , py::arg("col")
           , py::arg("closed")
           , py::arg("thickness") = 1.0f
        )
        .def("path_arc_to", &ImDrawList::PathArcTo
           , py::arg("centre")
           , py::arg("radius")
           , py::arg("a_min")
           , py::arg("a_max")
           , py::arg("num_segments") = 10
        )
        .def("path_arc_to_fast", &ImDrawList::PathArcToFast
           , py::arg("centre")
           , py::arg("radius")
           , py::arg("a_min_of_12")
           , py::arg("a_max_of_12")
        )
        .def("path_bezier_curve_to", &ImDrawList::PathBezierCurveTo
           , py::arg("p1")
           , py::arg("p2")
           , py::arg("p3")
           , py::arg("num_segments") = 0
        )
        .def("path_rect", &ImDrawList::PathRect
           , py::arg("rect_min")
           , py::arg("rect_max")
           , py::arg("rounding") = 0.0f
           , py::arg("rounding_corners_flags") = ImDrawCornerFlags_All
        )
        .def("channels_split", &ImDrawList::ChannelsSplit
           , py::arg("channels_count")
        )
        .def("channels_merge", &ImDrawList::ChannelsMerge)
        .def("channels_set_current", &ImDrawList::ChannelsSetCurrent
           , py::arg("channel_index")
        )
        .def("add_callback", &ImDrawList::AddCallback
           , py::arg("callback")
           , py::arg("callback_data")
        )
        .def("add_draw_cmd", &ImDrawList::AddDrawCmd)
        .def("clone_output", &ImDrawList::CloneOutput)
        .def("clear", &ImDrawList::Clear)
        .def("clear_free_memory", &ImDrawList::ClearFreeMemory)
        .def("prim_reserve", &ImDrawList::PrimReserve
           , py::arg("idx_count")
           , py::arg("vtx_count")
        )
        .def("prim_rect", &ImDrawList::PrimRect
           , py::arg("a")
           , py::arg("b")
           , py::arg("col")
        )
        .def("prim_rect_uv", &ImDrawList::PrimRectUV
           , py::arg("a")
           , py::arg("b")
           , py::arg("uv_a")
           , py::arg("uv_b")
           , py::arg("col")
        )
        .def("prim_quad_uv", &ImDrawList::PrimQuadUV
           , py::arg("a")
           , py::arg("b")
           , py::arg("c")
           , py::arg("d")
           , py::arg("uv_a")
           , py::arg("uv_b")
           , py::arg("uv_c")
           , py::arg("uv_d")
           , py::arg("col")
        )
        .def("prim_write_vtx", &ImDrawList::PrimWriteVtx
           , py::arg("pos")
           , py::arg("uv")
           , py::arg("col")
        )
        .def("prim_write_idx", &ImDrawList::PrimWriteIdx
           , py::arg("idx")
        )
        .def("prim_vtx", &ImDrawList::PrimVtx
           , py::arg("pos")
           , py::arg("uv")
           , py::arg("col")
        )
        .def("update_clip_rect", &ImDrawList::UpdateClipRect)
        .def("update_texture_id", &ImDrawList::UpdateTextureID)

    ;

    py::class_<ImDrawData>(deargui, "DrawData")
        .def_readwrite("valid", &ImDrawData::Valid)
        .def_readwrite("cmd_lists_count", &ImDrawData::CmdListsCount)
        .def_readwrite("total_idx_count", &ImDrawData::TotalIdxCount)
        .def_readwrite("total_vtx_count", &ImDrawData::TotalVtxCount)
        .def_readwrite("display_pos", &ImDrawData::DisplayPos)
        .def_readwrite("display_size", &ImDrawData::DisplaySize)
        .def(py::init<>())
        .def("clear", &ImDrawData::Clear)
        .def("de_index_all_buffers", &ImDrawData::DeIndexAllBuffers)
        .def("scale_clip_rects", &ImDrawData::ScaleClipRects
           , py::arg("sc")
        )
        .def_property_readonly("cmd_lists", [](const ImDrawData& self)
        {
            py::list ret;
            for(int i = 0; i < self.CmdListsCount; i++)
            {
                ret.append(self.CmdLists[i]);
            }
            return ret;
        })
    ;

    py::class_<ImFontConfig>(deargui, "FontConfig")
        .def_readwrite("font_data", &ImFontConfig::FontData)
        .def_readwrite("font_data_size", &ImFontConfig::FontDataSize)
        .def_readwrite("font_data_owned_by_atlas", &ImFontConfig::FontDataOwnedByAtlas)
        .def_readwrite("font_no", &ImFontConfig::FontNo)
        .def_readwrite("size_pixels", &ImFontConfig::SizePixels)
        .def_readwrite("oversample_h", &ImFontConfig::OversampleH)
        .def_readwrite("oversample_v", &ImFontConfig::OversampleV)
        .def_readwrite("pixel_snap_h", &ImFontConfig::PixelSnapH)
        .def_readwrite("glyph_extra_spacing", &ImFontConfig::GlyphExtraSpacing)
        .def_readwrite("glyph_offset", &ImFontConfig::GlyphOffset)
        .def_readwrite("glyph_ranges", &ImFontConfig::GlyphRanges)
        .def_readwrite("glyph_min_advance_x", &ImFontConfig::GlyphMinAdvanceX)
        .def_readwrite("glyph_max_advance_x", &ImFontConfig::GlyphMaxAdvanceX)
        .def_readwrite("merge_mode", &ImFontConfig::MergeMode)
        .def_readwrite("rasterizer_flags", &ImFontConfig::RasterizerFlags)
        .def_readwrite("rasterizer_multiply", &ImFontConfig::RasterizerMultiply)
        .def_readonly("name", &ImFontConfig::Name)
        .def_readwrite("dst_font", &ImFontConfig::DstFont)
        .def(py::init<>())

    ;

    py::class_<ImFontGlyph>(deargui, "FontGlyph")
        .def_readwrite("codepoint", &ImFontGlyph::Codepoint)
        .def_readwrite("advance_x", &ImFontGlyph::AdvanceX)
        .def_readwrite("x0", &ImFontGlyph::X0)
        .def_readwrite("y0", &ImFontGlyph::Y0)
        .def_readwrite("x1", &ImFontGlyph::X1)
        .def_readwrite("y1", &ImFontGlyph::Y1)
        .def_readwrite("u0", &ImFontGlyph::U0)
        .def_readwrite("v0", &ImFontGlyph::V0)
        .def_readwrite("u1", &ImFontGlyph::U1)
        .def_readwrite("v1", &ImFontGlyph::V1)

    ;

    py::enum_<ImFontAtlasFlags_>(deargui, "FontAtlasFlags")
        .value("FONT_ATLAS_FLAGS__NONE", ImFontAtlasFlags_None)
        .value("FONT_ATLAS_FLAGS__NO_POWER_OF_TWO_HEIGHT", ImFontAtlasFlags_NoPowerOfTwoHeight)
        .value("FONT_ATLAS_FLAGS__NO_MOUSE_CURSORS", ImFontAtlasFlags_NoMouseCursors)
        .export_values();

    py::class_<ImFontAtlas>(deargui, "FontAtlas")
        .def(py::init<>())
        .def("add_font", &ImFontAtlas::AddFont
           , py::arg("font_cfg")
        )
        .def("add_font_default", &ImFontAtlas::AddFontDefault
           , py::arg("font_cfg") = nullptr
        )
        .def("add_font_from_file_ttf", &ImFontAtlas::AddFontFromFileTTF
           , py::arg("filename")
           , py::arg("size_pixels")
           , py::arg("font_cfg") = nullptr
           , py::arg("glyph_ranges") = nullptr
        )
        .def("add_font_from_memory_ttf", &ImFontAtlas::AddFontFromMemoryTTF
           , py::arg("font_data")
           , py::arg("font_size")
           , py::arg("size_pixels")
           , py::arg("font_cfg") = nullptr
           , py::arg("glyph_ranges") = nullptr
        )
        .def("add_font_from_memory_compressed_ttf", &ImFontAtlas::AddFontFromMemoryCompressedTTF
           , py::arg("compressed_font_data")
           , py::arg("compressed_font_size")
           , py::arg("size_pixels")
           , py::arg("font_cfg") = nullptr
           , py::arg("glyph_ranges") = nullptr
        )
        .def("add_font_from_memory_compressed_base85_ttf", &ImFontAtlas::AddFontFromMemoryCompressedBase85TTF
           , py::arg("compressed_font_data_base85")
           , py::arg("size_pixels")
           , py::arg("font_cfg") = nullptr
           , py::arg("glyph_ranges") = nullptr
        )
        .def("clear_input_data", &ImFontAtlas::ClearInputData)
        .def("clear_tex_data", &ImFontAtlas::ClearTexData)
        .def("clear_fonts", &ImFontAtlas::ClearFonts)
        .def("clear", &ImFontAtlas::Clear)
        .def("build", &ImFontAtlas::Build)
        .def("is_built", &ImFontAtlas::IsBuilt)
        .def("set_tex_id", &ImFontAtlas::SetTexID
           , py::arg("id")
        )
        .def("get_glyph_ranges_default", &ImFontAtlas::GetGlyphRangesDefault)
        .def("get_glyph_ranges_korean", &ImFontAtlas::GetGlyphRangesKorean)
        .def("get_glyph_ranges_japanese", &ImFontAtlas::GetGlyphRangesJapanese)
        .def("get_glyph_ranges_chinese_full", &ImFontAtlas::GetGlyphRangesChineseFull)
        .def("get_glyph_ranges_chinese_simplified_common", &ImFontAtlas::GetGlyphRangesChineseSimplifiedCommon)
        .def("get_glyph_ranges_cyrillic", &ImFontAtlas::GetGlyphRangesCyrillic)
        .def("get_glyph_ranges_thai", &ImFontAtlas::GetGlyphRangesThai)
        .def("add_custom_rect_regular", &ImFontAtlas::AddCustomRectRegular
           , py::arg("id")
           , py::arg("width")
           , py::arg("height")
        )
        .def("add_custom_rect_font_glyph", &ImFontAtlas::AddCustomRectFontGlyph
           , py::arg("font")
           , py::arg("id")
           , py::arg("width")
           , py::arg("height")
           , py::arg("advance_x")
           , py::arg("offset") = ImVec2(0,0)
        )
        .def("get_custom_rect_by_index", &ImFontAtlas::GetCustomRectByIndex
           , py::arg("index")
        )
        .def("calc_custom_rect_uv", &ImFontAtlas::CalcCustomRectUV
           , py::arg("rect")
           , py::arg("out_uv_min")
           , py::arg("out_uv_max")
        )
        .def("get_mouse_cursor_tex_data", &ImFontAtlas::GetMouseCursorTexData
           , py::arg("cursor")
           , py::arg("out_offset")
           , py::arg("out_size")
           , py::arg("out_uv_border")
           , py::arg("out_uv_fill")
        )
        .def_readwrite("locked", &ImFontAtlas::Locked)
        .def_readwrite("flags", &ImFontAtlas::Flags)
        .def_readwrite("tex_id", &ImFontAtlas::TexID)
        .def_readwrite("tex_desired_width", &ImFontAtlas::TexDesiredWidth)
        .def_readwrite("tex_glyph_padding", &ImFontAtlas::TexGlyphPadding)
        .def("get_tex_data_as_alpha8", [](ImFontAtlas& atlas)
        {
            unsigned char* pixels;
            int width, height, bytes_per_pixel;
            atlas.GetTexDataAsAlpha8(&pixels, &width, &height, &bytes_per_pixel);
            std::string data((char*)pixels, width * height * bytes_per_pixel);
            return std::make_tuple(width, height, py::bytes(data));
        })
        .def("get_tex_data_as_rgba32", [](ImFontAtlas& atlas)
        {
            unsigned char* pixels;
            int width, height, bytes_per_pixel;
            atlas.GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);
            std::string data((char*)pixels, width * height * bytes_per_pixel);
            return std::make_tuple(width, height, py::bytes(data));
        })
    ;

    py::class_<ImFont>(deargui, "Font")
        .def_readwrite("font_size", &ImFont::FontSize)
        .def_readwrite("scale", &ImFont::Scale)
        .def_readwrite("display_offset", &ImFont::DisplayOffset)
        .def_readwrite("glyphs", &ImFont::Glyphs)
        .def_readwrite("index_advance_x", &ImFont::IndexAdvanceX)
        .def_readwrite("index_lookup", &ImFont::IndexLookup)
        .def_readwrite("fallback_glyph", &ImFont::FallbackGlyph)
        .def_readwrite("fallback_advance_x", &ImFont::FallbackAdvanceX)
        .def_readwrite("fallback_char", &ImFont::FallbackChar)
        .def_readwrite("config_data_count", &ImFont::ConfigDataCount)
        .def_readwrite("config_data", &ImFont::ConfigData)
        .def_readwrite("container_atlas", &ImFont::ContainerAtlas)
        .def_readwrite("ascent", &ImFont::Ascent)
        .def_readwrite("descent", &ImFont::Descent)
        .def_readwrite("dirty_lookup_tables", &ImFont::DirtyLookupTables)
        .def_readwrite("metrics_total_surface", &ImFont::MetricsTotalSurface)
        .def(py::init<>())
        .def("clear_output_data", &ImFont::ClearOutputData)
        .def("build_lookup_table", &ImFont::BuildLookupTable)
        .def("find_glyph", &ImFont::FindGlyph
           , py::arg("c")
        )
        .def("find_glyph_no_fallback", &ImFont::FindGlyphNoFallback
           , py::arg("c")
        )
        .def("set_fallback_char", &ImFont::SetFallbackChar
           , py::arg("c")
        )
        .def("get_char_advance", &ImFont::GetCharAdvance
           , py::arg("c")
        )
        .def("is_loaded", &ImFont::IsLoaded)
        .def("get_debug_name", &ImFont::GetDebugName)
        .def("calc_word_wrap_position_a", &ImFont::CalcWordWrapPositionA
           , py::arg("scale")
           , py::arg("text")
           , py::arg("text_end")
           , py::arg("wrap_width")
        )
        .def("render_char", &ImFont::RenderChar
           , py::arg("draw_list")
           , py::arg("size")
           , py::arg("pos")
           , py::arg("col")
           , py::arg("c")
        )
        .def("render_text", &ImFont::RenderText
           , py::arg("draw_list")
           , py::arg("size")
           , py::arg("pos")
           , py::arg("col")
           , py::arg("clip_rect")
           , py::arg("text_begin")
           , py::arg("text_end")
           , py::arg("wrap_width") = 0.0f
           , py::arg("cpu_fine_clip") = false
        )
        .def("grow_index", &ImFont::GrowIndex
           , py::arg("new_size")
        )
        .def("add_glyph", &ImFont::AddGlyph
           , py::arg("c")
           , py::arg("x0")
           , py::arg("y0")
           , py::arg("x1")
           , py::arg("y1")
           , py::arg("u0")
           , py::arg("v0")
           , py::arg("u1")
           , py::arg("v1")
           , py::arg("advance_x")
        )
        .def("add_remap_char", &ImFont::AddRemapChar
           , py::arg("dst")
           , py::arg("src")
           , py::arg("overwrite_dst") = true
        )

    ;


    template_ImVector<char>(deargui, "Vector_char");
    template_ImVector<float>(deargui, "Vector_float");
    template_ImVector<unsigned char>(deargui, "Vector_unsignedchar");
    template_ImVector<unsigned short>(deargui, "Vector_unsignedshort");
    template_ImVector<ImDrawCmd>(deargui, "Vector_DrawCmd");
    template_ImVector<ImDrawVert>(deargui, "Vector_DrawVert");
    template_ImVector<ImFontGlyph>(deargui, "Vector_FontGlyph");
}


