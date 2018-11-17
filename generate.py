import os
import re
import sys
import collections
from clang import cindex

HEADER = """
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <limits>
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
    template_ImVector<char>(deargui, "Vector_char");
    template_ImVector<float>(deargui, "Vector_float");
    template_ImVector<unsigned char>(deargui, "Vector_unsignedchar");
    template_ImVector<unsigned short>(deargui, "Vector_unsignedshort");
    template_ImVector<ImDrawCmd>(deargui, "Vector_DrawCmd");
    template_ImVector<ImDrawVert>(deargui, "Vector_DrawVert");
    template_ImVector<ImFontGlyph>(deargui, "Vector_FontGlyph");
"""

FOOTER = """
    Style.def("set_color", [](ImGuiStyle& self, int item, ImVec4 color)
    {
        if (item < 0) throw py::index_error();
        if (item >= IM_ARRAYSIZE(self.Colors)) throw py::index_error();
        self.Colors[item] = color;
    }, py::arg("item"), py::arg("color"));
    IO.def("set_mouse_down", [](ImGuiIO& self, int button, bool down)
    {
        if (button < 0) throw py::index_error();
        if (button >= IM_ARRAYSIZE(self.MouseDown)) throw py::index_error();
        self.MouseDown[button] = down;
    }, py::arg("button"), py::arg("down"));
    IO.def("set_key_down", [](ImGuiIO& self, int key, bool down)
    {
        if (key < 0) throw py::index_error();
        if (key >= IM_ARRAYSIZE(self.KeysDown)) throw py::index_error();
        self.KeysDown[key] = down;
    }, py::arg("key"), py::arg("down"));
    IO.def("set_key_map", [](ImGuiIO& self, int key, int value)
    {
        if (key < 0) throw py::index_error();
        if (key >= IM_ARRAYSIZE(self.KeyMap)) throw py::index_error();
        self.KeyMap[key] = value;
    }, py::arg("key"), py::arg("value"));
    DrawData.def_property_readonly("cmd_lists", [](const ImDrawData& self)
    {
        py::list ret;
        for(int i = 0; i < self.CmdListsCount; i++)
        {
            ret.append(self.CmdLists[i]);
        }
        return ret;
    });
    DrawVert.def_property_readonly_static("pos_offset", [](py::object)
    {
        return IM_OFFSETOF(ImDrawVert, pos);
    });
    DrawVert.def_property_readonly_static("uv_offset", [](py::object)
    {
        return IM_OFFSETOF(ImDrawVert, uv);
    });
    DrawVert.def_property_readonly_static("col_offset", [](py::object)
    {
        return IM_OFFSETOF(ImDrawVert, col);
    });
    FontAtlas.def("get_tex_data_as_alpha8", [](ImFontAtlas& atlas)
    {
        unsigned char* pixels;
        int width, height, bytes_per_pixel;
        atlas.GetTexDataAsAlpha8(&pixels, &width, &height, &bytes_per_pixel);
        std::string data((char*)pixels, width * height * bytes_per_pixel);
        return std::make_tuple(width, height, py::bytes(data));
    });
    FontAtlas.def("get_tex_data_as_rgba32", [](ImFontAtlas& atlas)
    {
        unsigned char* pixels;
        int width, height, bytes_per_pixel;
        atlas.GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);
        std::string data((char*)pixels, width * height * bytes_per_pixel);
        return std::make_tuple(width, height, py::bytes(data));
    });
    deargui.def("init", []()
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(100.0, 100.0);
        unsigned char* pixels;
        int w, h;
        io.Fonts->GetTexDataAsAlpha8(&pixels, &w, &h, nullptr);
    });
    deargui.def("input_text", [](const char* label, char* data, size_t max_size, ImGuiInputTextFlags flags)
    {
        max_size++;
        char* text = (char*)malloc(max_size * sizeof(char));
        strncpy(text, data, max_size);
        auto ret = ImGui::InputText(label, text, max_size, flags, nullptr, NULL);
        std::string output(text);
        free(text);
        return std::make_tuple(ret, output);
    }
    , py::arg("label")
    , py::arg("data")
    , py::arg("max_size")
    , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("input_text_multiline", [](const char* label, char* data, size_t max_size, const ImVec2& size, ImGuiInputTextFlags flags)
    {
        max_size++;
        char* text = (char*)malloc(max_size * sizeof(char));
        strncpy(text, data, max_size);
        auto ret = ImGui::InputTextMultiline(label, text, max_size, size, flags, nullptr, NULL);
        std::string output(text);
        free(text);
        return std::make_tuple(ret, output);
    }
    , py::arg("label")
    , py::arg("data")
    , py::arg("max_size")
    , py::arg("size") = ImVec2(0,0)
    , py::arg("flags") = 0
    , py::return_value_policy::automatic_reference);
    deargui.def("combo", [](const char* label, int * current_item, std::vector<std::string> items, int popup_max_height_in_items)
    {
        std::vector<const char*> ptrs;
        for (const std::string& s : items)
        {
            ptrs.push_back(s.c_str());
        }
        auto ret = ImGui::Combo(label, current_item, ptrs.data(), ptrs.size(), popup_max_height_in_items);
        return std::make_tuple(ret, current_item);
    }
    , py::arg("label")
    , py::arg("current_item")
    , py::arg("items")
    , py::arg("popup_max_height_in_items") = -1
    , py::return_value_policy::automatic_reference);
    deargui.def("list_box", [](const char* label, int * current_item, std::vector<std::string> items, int height_in_items)
    {
        std::vector<const char*> ptrs;
        for (const std::string& s : items)
        {
            ptrs.push_back(s.c_str());
        }
        auto ret = ImGui::ListBox(label, current_item, ptrs.data(), ptrs.size(), height_in_items);
        return std::make_tuple(ret, current_item);
    }
    , py::arg("label")
    , py::arg("current_item")
    , py::arg("items")
    , py::arg("height_in_items") = -1
    , py::return_value_policy::automatic_reference);
    deargui.def("plot_lines", [](const char* label, std::vector<float> values, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
    {
        ImGui::PlotLines(label, values.data(), values.size(), values_offset, overlay_text, scale_min, scale_max, graph_size, sizeof(float));
    }
    , py::arg("label")
    , py::arg("values")
    , py::arg("values_offset") = 0
    , py::arg("overlay_text") = nullptr
    , py::arg("scale_min") = FLT_MAX
    , py::arg("scale_max") = FLT_MAX
    , py::arg("graph_size") = ImVec2(0,0)
    );
    deargui.def("plot_histogram", [](const char* label, std::vector<float> values, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
    {
        ImGui::PlotHistogram(label, values.data(), values.size(), values_offset, overlay_text, scale_min, scale_max, graph_size, sizeof(float));
    }
    , py::arg("label")
    , py::arg("values")
    , py::arg("values_offset") = 0
    , py::arg("overlay_text") = nullptr
    , py::arg("scale_min") = FLT_MAX
    , py::arg("scale_max") = FLT_MAX
    , py::arg("graph_size") = ImVec2(0,0)
    );
}
"""

EXCLUDES = set(
[
    # Wrapped
    'ImGui::Combo',
    'ImGui::ListBox',
    'ImGui::InputText',
    'ImGui::InputTextMultiline',
    'ImGui::PlotLines',
    'ImGui::PlotHistogram',
    'ImDrawData::CmdLists',
    'ImGuiIO::MouseDown',
    'ImGuiIO::KeysDown',
    'ImGuiIO::InputCharacters',
    'ImGuiIO::NavInputs',
    'ImFontAtlas::GetTexDataAsAlpha8',
    'ImFontAtlas::GetTexDataAsRGBA32',

    # Internal / Ignored
    'ImGuiTextFilter::Filters',
    'ImGuiStorage::Data',
    'ImFontAtlas::TexPixelsAlpha8',
    'ImFontAtlas::TexPixelsRGBA32',
    'ImFontAtlas::TexWidth',
    'ImFontAtlas::TexHeight',
    'ImFontAtlas::TexUvScale',
    'ImFontAtlas::TexUvWhitePixel',
    'ImFontAtlas::Fonts',
    'ImFontAtlas::CustomRects',
    'ImFontAtlas::ConfigData',
    'ImFontAtlas::CustomRectIds',
    'ImFontAtlas::AddCustomRectRegular',
    'ImFontAtlas::AddCustomRectFontGlyph',
    'ImFontAtlas::GetCustomRectByIndex',
    'ImFontAtlas::CalcCustomRectUV',
    'ImFontAtlas::GetMouseCursorTexData',
    'ImFont::CalcTextSizeA',
    'ImGui::SetNextWindowSizeConstraints',
    'ImGui::SetAllocatorFunctions',
    'ImGui::MemAlloc',
    'ImGui::MemFree',
    'ImGuiIO::GetClipboardTextFn',
    'ImGuiIO::SetClipboardTextFn',
    'ImGuiIO::ImeSetInputScreenPosFn',
    'ImDrawCmd::UserCallback',
    'ImColor::HSV',
    'ImNewDummy',
    'ImGuiTextBuffer',
    'CustomRect',
])

OVERLOADED = set([
    'ImGui::IsPopupOpen',
])

DEFAULTS = {
    'out_h' : '0',
    'out_s' : '0',
    'out_v' : '0',
    'out_r' : '0',
    'out_g' : '0',
    'out_g' : '0',
    'out_ini_size' : '0',
}

def snakecase(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def format_attribute(name):
    name = snakecase(name)
    name = name.rstrip('_')
    name = name.replace('__', '_')
    return name

def format_type(name):
    name = name.replace('ImGui', '')
    name = name.replace('Im', '')
    name = name.replace('<', '_')
    name = name.replace('>', '')
    name = name.replace(' ', '')
    name = name.rstrip('_')
    return name

def format_enum(name):
    name = name.replace('ImGui', '')
    name = name.replace('Im', '')
    name = snakecase(name).upper()
    name = name.replace('__', '_')
    name = name.rstrip('_')
    return name

def module(cursor):
    if cursor is None:
        return 'deargui'
    else:
        return format_type(cursor.spelling)

def is_excluded(cursor):
    if name(cursor) in EXCLUDES:
        return True
    if cursor.spelling.startswith('_'):
        return True
    return False

def name(cursor):
    if cursor is None:
        return ''
    elif cursor.kind == cindex.CursorKind.TRANSLATION_UNIT:
        return ''
    else:
        res = name(cursor.semantic_parent)
        if res != '':
            return res + '::' + cursor.spelling
    return cursor.spelling

def is_class_mappable(cursor):
    if not cursor.is_definition():
        return False
    if is_excluded(cursor):
        return False
    return True

def is_function_mappable(cursor):
    if 'operator' in cursor.spelling:
        return False
    if is_excluded(cursor):
        return False
    for argument in cursor.get_arguments():
        if argument.type.get_canonical().kind == cindex.TypeKind.POINTER:
            ptr = argument.type.get_canonical().get_pointee().kind
            if ptr == cindex.TypeKind.FUNCTIONPROTO:
                return False
        if argument.type.spelling == 'va_list':
            return False
    return True

def is_function_void_return(cursor):
    result = cursor.type.get_result()
    return result.kind == cindex.TypeKind.VOID

def is_property_mappable(cursor):
    if is_excluded(cursor):
        return False
    return True

def is_cursor_mappable(cursor):
    if cursor.location.file:
        return 'imgui.h' in cursor.location.file.name
    return False

def is_property_readonly(cursor):
    if cursor.type.kind == cindex.TypeKind.CONSTANTARRAY:
        return True
    return False

def is_overloaded(cursor):
    if not hasattr(is_overloaded, 'overloaded'):
        is_overloaded.overloaded = set(OVERLOADED)
    return name(cursor) in is_overloaded.overloaded

def arg_type(argument):
    if argument.type.kind == cindex.TypeKind.CONSTANTARRAY:
        return 'std::array<{}, {}>&'.format(
            argument.type.get_array_element_type().spelling,
            argument.type.get_array_size()
        )
    return argument.type.spelling

def arg_name(argument):
    if argument.type.kind == cindex.TypeKind.CONSTANTARRAY:
        return '&{}[0]'.format(argument.spelling)
    return argument.spelling

def arg_types(arguments):
    return ', '.join([arg_type(a) for a in arguments])

def arg_names(arguments):
    return ', '.join([arg_name(a) for a in arguments])

def arg_string(arguments):
    return ', '.join(['{} {}'.format(arg_type(a), a.spelling) for a in arguments])

def default_from_tokens(tokens):
    joined = ''.join([t.spelling for t in tokens])
    parts = joined.split('=')
    if len(parts) == 2:
        return parts[1]
    return ''

def write_pyargs(arguments):
    for argument in arguments:
        default = default_from_tokens(argument.get_tokens())
        for child in argument.get_children():
            if child.type.kind in [cindex.TypeKind.POINTER]:
                default = 'nullptr'
            elif not len(default):
                default = default_from_tokens(child.get_tokens())
        default = DEFAULTS.get(argument.spelling, default)
        if len(default):
            default = ' = ' + default
        out(', py::arg("{}"){}'.format(format_attribute(argument.spelling), default))

def parse_enum(cursor):
    out('py::enum_<{cname}>(deargui, "{pyname}", py::arithmetic())'.format(
        cname=name(cursor),
        pyname=format_type(cursor.spelling)
    ))
    out.indent += 1
    for value in cursor.get_children():
        out('.value("{pyname}", {cname})'.format(
            pyname=format_enum(value.spelling),
            cname=value.spelling
        ))
    out('.export_values();')
    out.indent -= 1
    out('')

def parse_constructor(cursor, cls):
    arguments = [a for a in cursor.get_arguments()]
    if len(arguments):
        out('{}.def(py::init<{}>()'.format(module(cls), arg_types(arguments)))
        write_pyargs(arguments)
        out(');')
    else:
        out('{}.def(py::init<>());'.format(module(cls)))

def parse_field(cursor, cls):
    pyname = format_attribute(cursor.spelling)
    cname = name(cursor)
    if is_property_mappable(cursor):
        if is_property_readonly(cursor):
            out('{}.def_readonly("{}", &{});'.format(module(cls), pyname, cname))
        else:
            out('{}.def_readwrite("{}", &{});'.format(module(cls), pyname, cname))

def should_wrap_function(cursor):
    if cursor.type.is_function_variadic():
        return True
    for arg in cursor.get_arguments():
        if arg.type.kind == cindex.TypeKind.CONSTANTARRAY:
            return True
        if should_return_argument(arg):
            return True
    return False

def should_return_argument(argument):
    argtype = argument.type.get_canonical()
    if argtype.kind == cindex.TypeKind.LVALUEREFERENCE:
        if not argtype.get_pointee().is_const_qualified():
            return True
    if argtype.kind == cindex.TypeKind.CONSTANTARRAY:
        return True
    if argtype.kind == cindex.TypeKind.POINTER:
        ptr = argtype.get_pointee()
        kinds = [
            cindex.TypeKind.BOOL,
            cindex.TypeKind.FLOAT,
            cindex.TypeKind.DOUBLE,
            cindex.TypeKind.INT,
            cindex.TypeKind.UINT,
            cindex.TypeKind.USHORT,
            cindex.TypeKind.ULONG,
            cindex.TypeKind.ULONGLONG,
        ]
        if not ptr.is_const_qualified() and ptr.kind in kinds:
            return True
    return False

def get_function_return(cursor):
    returned = [a.spelling for a in cursor.get_arguments() if should_return_argument(a)]
    if not is_function_void_return(cursor):
        returned.insert(0, 'ret')
    if len(returned) > 1:
        return 'std::make_tuple({})'.format(', '.join(returned))
    if len(returned) == 1:
        return returned[0]
    return ''

def get_return_policy(cursor):
    result = cursor.type.get_result()
    if result.kind == cindex.TypeKind.LVALUEREFERENCE:
        return 'py::return_value_policy::reference'
    else:
        return 'py::return_value_policy::automatic_reference'

def parse_function(cursor, cls=None):
    if is_function_mappable(cursor):
        mname = module(cls)
        arguments = [a for a in cursor.get_arguments()]
        cname = '&' + name(cursor)
        pyname = format_attribute(cursor.spelling)
        if is_overloaded(cursor):
            cname = 'py::overload_cast<{}>({})'.format(arg_types(arguments), cname)
        if should_wrap_function(cursor):
            out('{}.def("{}", []({})'.format(mname, pyname, arg_string(arguments)))
            out('{')
            ret = '' if is_function_void_return(cursor) else 'auto ret = '
            out('    {}{}({});'.format(ret, name(cursor), arg_names(arguments)))
            out('    return {};'.format(get_function_return(cursor)))
            out('}')
        else:
            out('{}.def("{}", {}'.format(mname, pyname, cname))
        write_pyargs(arguments)
        out(', {});'.format(get_return_policy(cursor)))

def parse_class(cursor):
    if is_class_mappable(cursor):
        clsname = format_type(cursor.spelling)
        out('py::class_<{}> {}(deargui, "{}");'.format(name(cursor), clsname, clsname))
        for child in cursor.get_children():
            if child.kind == cindex.CursorKind.CONSTRUCTOR:
                parse_constructor(child, cursor)
            elif child.kind == cindex.CursorKind.CXX_METHOD:
                parse_function(child, cursor)
            elif child.kind == cindex.CursorKind.FIELD_DECL:
                parse_field(child, cursor)

def parse_definitions(root):
    for cursor in root.get_children():
        if cursor.kind == cindex.CursorKind.ENUM_DECL:
            parse_enum(cursor)
        elif cursor.kind == cindex.CursorKind.STRUCT_DECL:
            parse_class(cursor)
        elif cursor.kind == cindex.CursorKind.FUNCTION_DECL:
            parse_function(cursor)
        elif cursor.kind == cindex.CursorKind.NAMESPACE:
            parse_definitions(cursor)

def parse_overloads(cursor):
    if not hasattr(parse_overloads, 'visited'):
        parse_overloads.visited = set()
    if not hasattr(is_overloaded, 'overloaded'):
        is_overloaded.overloaded = set(OVERLOADED)
    for child in cursor.get_children():
        if child.kind in [cindex.CursorKind.CXX_METHOD, cindex.CursorKind.FUNCTION_DECL]:
            key = name(child)
            if key in parse_overloads.visited:
                is_overloaded.overloaded.add(key)
            else:
                parse_overloads.visited.add(key)
        elif is_cursor_mappable(child):
            parse_overloads(child)

def out(line):
    if len(line):
        out.file.write(' ' * (getattr(out, 'indent', 0) * 4))
        out.file.write(line.replace('>>', '> >'))
    out.file.write('\n')

if __name__ == '__main__':
    if sys.platform == 'darwin':
        cindex.Config.set_library_path('/usr/local/opt/llvm@6/lib')
    base = os.path.dirname(os.path.realpath(__file__))
    path = os.path.join(base, 'imgui/imgui.h')
    tu = cindex.Index.create().parse(path, args=['-std=c++17', '-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1'])
    out.file = open(os.path.join(base, 'deargui/deargui.cpp'), 'w')
    out.indent = 0
    out(HEADER)
    out.indent = 1
    parse_overloads(tu.cursor)
    parse_definitions(tu.cursor)
    out.indent = 0
    out(FOOTER)
