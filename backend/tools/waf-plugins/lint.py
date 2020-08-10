#! /usr/bin/env python
# encoding: utf-8

import os
from future.utils import raise_from

def configure(conf):
  print('***', os.path.dirname(Context.waf_dir))
  conf.find_program('cpplint.py', var='LINT', path_list=[os.path.dirname(Context.waf_dir)])

#########################################
# Lint
#########################################
from waflib.TaskGen import before, feature, after_method
from waflib import Task, Context

lint_ignore_patterns = set(['unittest-gtest'])
def add_lint_ignore(pattern):
  lint_ignore_patterns.add(pattern)

def lint_should_ignore(path):
  for pattern in lint_ignore_patterns:
    if pattern in path:
      return True
  return False

files_linted = set()

@feature('cxx')
@after_method('process_source')
def add_files_to_lint(self):
  if self.target != 'testprog':
    for task in self.compiled_tasks:
      if not task.inputs[0] in files_linted:
        files_linted.add(task.inputs[0])
        lint = self.create_task('lint', task.inputs[0])
        lint.source_task = task
        lint.lint_done = False

to_lint = set()

class lint(Task.Task):
  after = [ 'cxx' ]

  def __init__(self, *k, **kw):
    Task.Task.__init__(self, *k, **kw)
    self.lint_done = True

  def runnable_status(self):
    if not self.lint_done:
      for t in self.run_after:
        if not t.hasrun:
          return Task.ASK_LATER;
      self.add_lint_tasks()

    return Task.Task.runnable_status(self)

  def add_lint_tasks(self):
    bld = self.generator.bld
    deps = bld.node_deps.get(self.source_task.uid())
    for dep in deps:
      if not dep in to_lint and not lint_should_ignore(dep.abspath()):
        to_lint.add(dep)

        task = Task.classes['lint'](env=self.env, generator=self.generator)
        task.set_inputs(dep)

        gen = bld.producer
        gen.outstanding.appendleft(task)# = [task] + gen.outstanding#.append(0, task)
        gen.total += 1

    self.lint_done = True

  def run(self):
    bld = self.generator.bld;
    path = self.inputs[0].path_from(self.generator.bld.bldnode)
    #print('path', path)
    if lint_should_ignore(path):
      return 0

    # Execute lint
    #print('env is', self.env['LINT'])
    cmd = '%s %s' % (self.env['LINT'][0], path)
    #print('cmd is', cmd)
    try:
      #print('var_dir is', bld.variant_dir)
      bld.cmd_and_log(cmd, quiet=Context.BOTH, cwd=bld.variant_dir)
    except Exception as e:
      print(e.stderr)
      if e.stderr =='':
        return 0
      return 1
    return 0
