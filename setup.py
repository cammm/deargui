import setuptools

class get_pybind_include(object):
    def __str__(self):
        import pybind11
        return pybind11.get_include()

module = setuptools.Extension("deargui", [
    'imgui/imgui.cpp',
    'imgui/imgui_demo.cpp',
    'imgui/imgui_draw.cpp',
    'imgui/imgui_widgets.cpp',
    'deargui/deargui.cpp',
  ],
  include_dirs = [get_pybind_include(), 'imgui'],
  extra_compile_args = ['/bigobj', '/DImTextureID=int', '/DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1'],
  language = 'c++',
)

setuptools.setup(
  name             = 'deargui',
  description      = 'Python bindings for Dear ImGui',
  version          = '0.1.4',
  url              = 'http://github.com/cammm/deargui',
  license          = 'MIT',
  author           = 'cammm',
  packages         = setuptools.find_packages(),
  ext_modules      = [module],
  install_requires = ['pybind11'],
)
