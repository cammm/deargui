import os
import re
import sys
import collections
from clang import cindex

HEADER = """
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
"""

FOOTER = """
    template_ImVector<char>(deargui, "Vector_char");
    template_ImVector<float>(deargui, "Vector_float");
    template_ImVector<unsigned char>(deargui, "Vector_unsignedchar");
    template_ImVector<unsigned short>(deargui, "Vector_unsignedshort");
    template_ImVector<ImDrawCmd>(deargui, "Vector_DrawCmd");
    template_ImVector<ImDrawVert>(deargui, "Vector_DrawVert");
    template_ImVector<ImFontGlyph>(deargui, "Vector_FontGlyph");
}

"""


EXTENSIONS = {}
EXTENSIONS["ImDrawData"] = """
        .def_property_readonly("cmd_lists", [](const ImDrawData& self)
        {
            py::list ret;
            for(int i = 0; i < self.CmdListsCount; i++)
            {
                ret.append(self.CmdLists[i]);
            }
            return ret;
        })
"""
EXTENSIONS["ImDrawVert"] = """
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
"""
EXTENSIONS["ImFontAtlas"] = """
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
"""

EXCLUDES = set(
[
    # Fixme
    'ImGui::SetNextWindowSizeConstraints',
    'ImGui::IsPopupOpen',
    'ImGui::DragFloat',
    'ImGui::Combo',
    'ImGui::InputText',
    'ImGui::InputTextMultiline',
    'ImGui::ListBox',
    'ImGui::PlotLines',
    "ImGui::PlotHistogram",
    'ImFont::CalcTextSizeA',
    "ImGuiTextFilter::Filters",
    "ImGuiIO::GetClipboardTextFn",
    "ImGuiIO::SetClipboardTextFn",
    "ImGuiIO::ImeSetInputScreenPosFn",
    "ImDrawCmd::UserCallback",

    # Wrapped
    'ImDrawData::CmdLists',
    'ImGuiIO::MouseDown',
    'ImGuiIO::KeysDown',
    'ImGuiIO::InputCharacters',
    'ImGuiIO::NavInputs',
    'ImFontAtlas::GetTexDataAsAlpha8',
    'ImFontAtlas::GetTexDataAsRGBA32',

    # Deprecated
    'ImColor',
    'ImGuiIO::DisplayVisibleMin',
    'ImGuiIO::DisplayVisibleMax',

    # Internal
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
    'ImGui::SetAllocatorFunctions',
    'ImGui::MemAlloc',
    'ImGui::MemFree',
    'ImNewDummy',

])

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
    name = name.rstrip('_')
    name = snakecase(name).upper()
    return name

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
    if cursor.type.is_function_variadic():
        return False
    if is_excluded(cursor):
        return False
    # for argument in cursor.get_arguments():
    #     if argument.type.get_canonical().kind == cindex.TypeKind.INCOMPLETEARRAY:
    #         return False
    #     if argument.type.get_canonical().kind == cindex.TypeKind.POINTER:
    #         ptr = argument.type.get_canonical().get_pointee().kind
    #         if ptr in [cindex.TypeKind.UNEXPOSED, cindex.TypeKind.POINTER, cindex.TypeKind.FUNCTIONPROTO]:
    #             return False
    return True

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
        is_overloaded.overloaded = set()
    return name(cursor) in is_overloaded.overloaded

def arg_types(arguments):
    return ', '.join([a.type.spelling for a in arguments])

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
        if len(default):
            default = ' = ' + default
        out('   , py::arg("{}"){}'.format(format_attribute(argument.spelling), default))

def parse_enum(cursor):
    out('py::enum_<{cname}>(deargui, "{pyname}")'.format(
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

def parse_function(cursor):
    if is_function_mappable(cursor):
        arguments = [a for a in cursor.get_arguments()]
        cname = '&' + name(cursor)
        pyname = format_attribute(cursor.spelling)
        if is_overloaded(cursor):
            cname = 'py::overload_cast<{}>({})'.format(arg_types(arguments), cname)
        if len(arguments):
            out('deargui.def("{}", {}'.format(pyname, cname))
            write_pyargs(arguments)
            out(', py::return_value_policy::automatic_reference);')
        else:
            out('deargui.def("{}", {}, py::return_value_policy::automatic_reference);'.format(pyname, cname))

def parse_class(cursor):
    if is_class_mappable(cursor):
        out('py::class_<{cname}>(deargui, "{pyname}")'.format(
            cname = name(cursor),
            pyname = format_type(cursor.spelling)
        ))
        out.indent += 1
        for child in cursor.get_children():
            if child.kind == cindex.CursorKind.CONSTRUCTOR:
                arguments = [a for a in child.get_arguments()]
                if len(arguments):
                    out('.def(py::init<{}>()'.format(arg_types(arguments)))
                    write_pyargs(arguments)
                    out(')')
                else:
                    out('.def(py::init<>())')
            elif child.kind == cindex.CursorKind.CXX_METHOD:
                if is_function_mappable(child):
                    arguments = [a for a in child.get_arguments()]
                    pyname = format_attribute(child.spelling)
                    cname = '&' + name(child)
                    if is_overloaded(child):
                        cname = 'py::overload_cast<{}>({})'.format(arg_types(arguments), cname)
                    if len(arguments):
                        out('.def("{}", {}'.format(pyname, cname))
                        write_pyargs(arguments)
                        out(')')
                    else:
                        out('.def("{}", {})'.format(pyname, cname))
            elif child.kind == cindex.CursorKind.FIELD_DECL:
                pyname = format_attribute(child.spelling)
                cname = name(child)
                if is_property_mappable(child):
                    if is_property_readonly(child):
                        out('.def_readonly("{}", &{})'.format(pyname, cname))
                    else:
                        out('.def_readwrite("{}", &{})'.format(pyname, cname))

        # Write any manual extensions for this class
        out(EXTENSIONS.get(cursor.spelling, '').strip())
        out.indent -= 1
        out(';')
        out('')

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
        is_overloaded.overloaded = set()
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
    path = os.path.join(base, "imgui/imgui.h")
    tu = cindex.Index.create().parse(path, args=['-std=c++17', '-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1'])
    out.file = open(os.path.join(base, "deargui/deargui.cpp"), 'w')
    out.indent = 0
    out(HEADER)
    out.indent = 1
    parse_overloads(tu.cursor)
    parse_definitions(tu.cursor)
    out.indent = 0
    out(FOOTER)
