import sys
import setuptools

class get_pybind_include(object):
    def __str__(self):
        import pybind11
        print('get_pybind_include', pybind11.get_include())
        return pybind11.get_include()

class get_pybind_user_include(object):
    def __str__(self):
        import pybind11
        print('get_pybind_user_include', pybind11.get_include(True))
        return pybind11.get_include(True)

if sys.platform == 'win32':
  compile_args = [
    '/bigobj',
    '/DImTextureID=int',
    '/DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1',
  ]
else:
  compile_args = [
    '-std=c++14',
    '-Wno-error',
    '-Wno-null-conversion',
    '-DImTextureID=int',
    '-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1',
  ]

module = setuptools.Extension("deargui", [
    'imgui/imgui.cpp',
    'imgui/imgui_demo.cpp',
    'imgui/imgui_draw.cpp',
    'imgui/imgui_widgets.cpp',
    'deargui/deargui.cpp',
  ],
  include_dirs = [get_pybind_include(), get_pybind_user_include(), 'imgui'],
  extra_compile_args = compile_args,
  language = 'c++',
)

setuptools.setup(
  name             = 'deargui',
  description      = 'Python bindings for Dear ImGui',
  version          = '0.1.5',
  url              = 'http://github.com/cammm/deargui',
  license          = 'MIT',
  author           = 'cammm',
  packages         = setuptools.find_packages(),
  ext_modules      = [module],
  setup_requires   = ['pybind11'],
)
