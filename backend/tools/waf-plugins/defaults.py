#! /usr/bin/env python
# encoding: utf-8

def options(opt):
  opt.add_option('-d', '--debug-level',
    action  = 'store',
    default = 'release',
    help    = 'Specify the debugging level [debug, release]',
    choices = [ 'debug', 'release' ],
    dest    = 'debug_level')

  #opt.load('compiler_cxx unittest_gtest lint')
  opt.load('compiler_cxx lint')# unittest_gtest lint')

def configure(conf):
  #conf.load('compiler_cxx local_rpath unittest_gtest lint')
  conf.load('compiler_cxx local_rpath lint')

  conf.env.CXXFLAGS         = [
    '-Werror',
    '-Wall',
    '-Woverloaded-virtual',
    '-Wextra',
    '-Wfloat-equal',
    '-Wno-unused-parameter',
    '-Wno-unused-function' ]

  if conf.options.debug_level == 'release':
    conf.env.CXXFLAGS += [ '-O2' ]
  else:
    conf.env.CXXFLAGS += [ '-g' ]
    conf.env.DEFINES += [ 'DEBUG' ]

#######################################
# Tag to disable program installation
#######################################
from waflib.TaskGen import before, feature
@feature('noinst')
@before('apply_link')
def no_inst(self):
  self.install_path = None
