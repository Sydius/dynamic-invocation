VERSION='1.0.0'
APPNAME='dynamic-invocation'

srcdir='.'
blddir='build'

def options(opt):
    opt.load('compiler_cxx')

def configure(conf):
    conf.load('compiler_cxx')

def build(bld):
    bld.program(
        source=['main.cpp'],
        includes=['../call-with-tuple', '../serialize-tuple'],
        target='dynamic-invocation',
        lib=['boost_serialization', 'boost_unit_test_framework'],
        cxxflags='-O3 --std=c++0x --pedantic -Wall'
    )
