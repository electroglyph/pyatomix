project(
    'atomix_base',
    'cpp',
    version: '0.0.1',
    default_options: [
        'cpp_std=c++20',
        'buildtype=release',
        'debug=false',
        'optimization=3',
        'vsenv=true',
    ],
)

py = import('python').find_installation(pure: false)
pybind11_dep = dependency('pybind11')

py.extension_module('atomix_base',
    'src/main.cpp',
    install: true,
    dependencies: [pybind11_dep],
)