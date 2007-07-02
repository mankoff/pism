#!/usr/bin/ruby -w

# Copyright (C) 2004-2007 Jed Brown and Ed Bueler
#
# This file is part of Pism.
#
# Pism is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# Pism is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License
# along with Pism; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

cdl_input = 'pism_state.cdl'
c_source = 'ncgen.c'
c_ncvars = 'ncvars.h'
c_attributes = 'write_attributes.c'

autogen_warning = ["// This file was automatically generated by `ncgen.rb' from",
                   "// `pism_state.cdl'.  If you edit it, your changes will be overwritten",
                   "// on the next invocation of `ncgen.rb'.\n"]

system("ncgen -v2 -c #{cdl_input} > #{c_source}")
# use   system("ncgen -c #{cdl_input} > #{c_source}")  to avoid CF-compliance checker problem "can't read"?

s = File.new(c_source)
s.gets until $_ =~ /^main\(\)/

ov = File.new(c_ncvars, 'w')
ov.puts autogen_warning;

oa = File.new(c_attributes, 'w')
oa.puts autogen_warning

s.each do |l|
  break if l =~ /attribute vectors/
  ov << l;
  l.sub!(/91/, 'grid.p->Mx')
  l.sub!(/92/, 'grid.p->My')
  l.sub!(/93/, 'grid.p->Mz')
  l.sub!(/94/, 'grid.p->Mbz')
  oa << l
end
ov.close

s.each do |l|
  break if l =~ /leave define mode/
  l.sub!(/"pism_state.nc"/, 'fname')
  oa << l
  if l =~ /enter define mode/ then oa << "if (grid.rank == 0) {\n" end
end
oa << "} // end if (grid.rank == 0)\n"
oa.close
s.close

