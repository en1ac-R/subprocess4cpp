#!/usr/bin/env python3

"""Check C++ format"""

import difflib
import os
import shutil
import subprocess
import sys
import tempfile
import time

from argparse import ArgumentParser
from pathlib import Path

def run(args):
  """Print arguments and execute"""
  one_line = ' '.join(args)
  print(f'Executing: {one_line}')
  result = subprocess.run(args, text=True, check=False, capture_output=True)
  if result.returncode != 0:
    output = f'\n{result.stderr}\n{result.stdout}'
    print(f'Exit with code {result.returncode}:{output}')
    sys.exit(1)
  assert result.stderr == ''
  return result.stdout

def skip_dir(relative_dir):
  """Directory should be skipped from checking"""
  exclude_dirs = ['_build', '_builds', 'third_party', '_deps', '_venv']
  if not relative_dir.parts:
    return False
  for excluded in exclude_dirs:
    if excluded in relative_dir.parts:
      return True
  return False

def parse_args():
  """Parse arguments"""
  parser = ArgumentParser(description=__doc__)
  group = parser.add_mutually_exclusive_group()
  a_arg = group.add_argument('-a', '--apply', action='store_true',
      help='apply formatting for a single file')
  f_arg = group.add_argument('-f', '--force', action='store_true',
      help='force formatting for all files')
  arg_apply_hints = ''
  for arg in [a_arg, f_arg]:
    arg_apply_hints += f'\n  {arg.option_strings} {arg.help}'
  args = parser.parse_args()
  return args.apply, args.force, arg_apply_hints

def check_configuration(script_dir):
  """Check configuration file"""
  cfg = script_dir / '.clang-format'
  print(f'Configuration: {cfg}')
  assert cfg.exists()

def get_script_dir():
  """This script directory"""
  this_script = Path(__file__)
  assert this_script.exists()

  script_dir = this_script.parent
  assert script_dir.is_dir()
  return script_dir

def run_main():
  """Wrapper for main"""
  script_dir = get_script_dir()
  assert script_dir.is_dir()

  apply, force, arg_apply_hints = parse_args()
  assert not apply is None
  assert not force is None
  assert not arg_apply_hints is None
  assert arg_apply_hints != ''

  check_configuration(script_dir)

  for root, _, files in os.walk(script_dir):
    relative = Path(root).relative_to(script_dir)
    if skip_dir(relative):
      continue
    for i in files:
      if Path(i).suffix not in ['.cpp', '.h', '.hpp']:
        continue
      to_check = Path(root) / i
      with open(to_check, 'r', encoding='utf-8') as file:
        original = file.read()
      formatted = run(['clang-format', '-style=file', str(to_check)])
      if original == formatted:
        continue
      with tempfile.NamedTemporaryFile(mode='w+t', delete=False) as tmp:
        tmp.write(formatted)
      if not apply and not force:
        if original.rstrip('\n') == formatted.rstrip('\n'):
          print('\nInconsistent newline(s) at end of file')
        else:
          diff = difflib.unified_diff(original.splitlines(),
              formatted.splitlines(), fromfile=str(to_check), tofile=tmp.name)
          print('\n'.join(diff))
        print(f'\nApply hint:\n\n  mv {tmp.name} {to_check}')
        print(f'\nAlso:\n{arg_apply_hints}')
        sys.exit(1)

      print(f'Applying formatting: {tmp.name} -> {to_check}')
      # Pathlib.replace (possible) error: Invalid cross-device link
      shutil.move(tmp.name, to_check)

      # '--apply' will only be performed once
      apply = False

if __name__ == '__main__':
  start = time.time()
  run_main()
  elapsed = time.time() - start
  print(f'Done in {elapsed:.3f} sec')
