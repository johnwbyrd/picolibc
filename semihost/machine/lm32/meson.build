#
# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright © 2024 Keith Packard
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#

lib_semihost_srcs = [
  'lm32_close.c',
  'lm32_errno.c',
  'lm32_exit.c',
  'lm32_lseek.c',
  'lm32_open.c',
  'lm32_read.c',
  'lm32_scall.c',
  'lm32_stub.c',
  'lm32_unlink.c',
  'lm32_write.c',
]

has_semihost = true

foreach params : targets
  target = params['name']
  target_dir = params['dir']
  target_c_args = params['c_args']
  target_lib_prefix = params['lib_prefix']

  instdir = join_paths(lib_dir, target_dir)

  libsemihost_name = 'semihost'

  local_lib_semihost_target = static_library(join_paths(target_dir, target_lib_prefix + libsemihost_name),
				       lib_semihost_srcs,
				       install : really_install,
				       install_dir : instdir,
				       include_directories : inc,
				       c_args : target_c_args + c_args,
				       pic: false)

  set_variable('lib_semihost' + target, local_lib_semihost_target)

endforeach
