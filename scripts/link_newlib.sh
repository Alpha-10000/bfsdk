#!/bin/bash -e
#
# Bareflank Hypervisor
#
# Copyright (C) 2015 Assured Information Security, Inc.
# Author: Rian Quinn        <quinnr@ainfosec.com>
# Author: Brendan Kerrigan  <kerriganb@ainfosec.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# Newlib refuses to build a shared library unless your OS is Linux, so we
# build the shared library ourselves here.

# TODO
#
# We need to compile newlib in it's own build dir and copy the results in
# place instead of giving it a prefix like we do now. This way, we don't end
# up with a bunch of files that we don't need / use in the prefix that makes
# it hard to navigate
#

eval $1 -shared `find $2/newlib/src/newlib-build/x86_64-vmm-elf/newlib/libc -name "*.o" | xargs echo` -o libc.so  1>/dev/null 2>/dev/null
mv libc.so $3/lib/ 1>/dev/null 2>/dev/null
