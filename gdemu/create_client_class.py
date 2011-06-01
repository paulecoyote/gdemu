#!/usr/bin/python
# 
# \file create_client_class.py
# \brief This script creates .cpp and .h files for your class
# \date 2008
# \author Jan Boon (Kaetemi)
# This script creates .cpp and .h files for your class
# 
# Copyright (C) 2008-2009  by authors
# 
# This file is part of NEVRAX NEL.
# NEVRAX NEL is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 2 of the License,
# or (at your option) any later version.
# 
# NEVRAX NEL is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with NEVRAX NEL; see the file COPYING.  If not, see
# <http://www.gnu.org/licenses/>.
# 
# $Id: create_client_class.py 979 2011-05-29 22:24:40Z kaetemi $
# 

import time, sys
from textwrap import TextWrapper

wrapper = TextWrapper()
indent = " * "
wrapper.initial_indent = indent
wrapper.subsequent_indent = indent
gmtime = time.gmtime(time.time())
filename = "NEEDED_FOR_buildLine"
newline = "\n"

print ""

print "This script will create .cpp and .h files for your class."
print "To use the defaults, simply hit ENTER, else type in the new value."

print ""

def askVar(name, default):
	sys.stdout.write(name + " (" + default + "): ")
	line = sys.stdin.readline()
	linestrip = line.strip()
	if linestrip == "":
		return default
	else:
		return linestrip

file = askVar("File", "namespace_your_class_name").replace(" ", "_")
classname = askVar("Class", "YourClassName")
author = askVar("Author", "Jan Boon")
username = askVar("Username", "Kaetemi")
cauth = askVar("Authors", "False")
nevrax = "False"
snowballs2 = "False"
linkexception = askVar("Linkexception", "")
namespace = askVar("Namespace", "GDEMU")
product = askVar("Product", "GAMEDUINO EMULATOR")
dir = askVar("Directory", "./")
year = askVar("Year", time.strftime("%Y", gmtime))
date = askVar("Date", time.strftime("%Y-%m-%d %H:%MGMT", gmtime))
#hdefine = askVar("Define", namespace + "_" + file.upper() + "_H")
hdefine = askVar("Define", file.upper() + "_H")

print ""

filedoc = [ ]
filedoc += [ "{CLASS}" ]
filedoc += [ "$I" + "d$" ]
filedoc += [ "\\file {FILE}" ]
filedoc += [ "\\brief {CLASS}" ]
filedoc += [ "\\date {DATE}" ]
filedoc += [ "\\author {AUTHOR} ({USERNAME})" ]

copyright = [ ]
if cauth == "True":
	copyright += [ "Copyright (C) {YEAR}  by authors" ]
else:
	copyright += [ "Copyright (C) {YEAR}  {AUTHOR} ({USERNAME})" ]
copyright += [ "" ]
copyright += [ "This file is part of {PRODUCT}." ]
copyright += [ "{PRODUCT} is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version." ]
copyright += [ "" ]
copyright += [ "{PRODUCT} is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details." ]
copyright += [ "" ]
copyright += [ "You should have received a copy of the GNU General Public License along with {PRODUCT}; see the file COPYING.  If not, see <http://www.gnu.org/licenses/>." ]
if linkexception != "":
	copyright += [ "" ]
	copyright += [ "Linking this library statically or dynamically with other modules is making a combined work based on this library.  Thus, the terms and conditions of the GNU General Public License cover the whole combination." ]
	copyright += [ "" ]
	copyright += [ "As a special exception, the copyright holders of this library give you permission to link this library with {LINKEXCEPTION} to produce an executable, regardless of the license terms of {LINKEXCEPTION}, and distribute linked combinations including the two, provided that you also meet the terms and conditions of the license of {LINKEXCEPTION}.  You must obey the GNU General Public License in all respects for all of the code used other than {LINKEXCEPTION}.  If you modify this file, you may extend this exception to your version of the file, but you are not obligated to do so.  If you do not wish to do so, delete this exception statement from your version." ]

hcode1 = [ ]
hcode1 += [ "#ifndef {HDEFINE}" ]
hcode1 += [ "#define {HDEFINE}" ]
hcode1 += [ "// #include <...>" ]
hcode1 += [ "" ]
hcode1 += [ "// System includes" ]
hcode1 += [ "" ]
hcode1 += [ "// Project includes" ]
hcode1 += [ "" ]
hcode1 += [ "namespace {NAMESPACE} {" ]
hcode1 += [ "" ]

hcomment = [ ]
hcomment += [ "{CLASS}" ]
hcomment += [ "\\brief {CLASS}" ]
hcomment += [ "\\date {DATE}" ]
hcomment += [ "\\author {AUTHOR} ({USERNAME})" ]

hcode2 = [ ]
hcode2 += [ "class {CLASS}"]
hcode2 += [ "{"]
hcode2 += [ "protected:"]
hcode2 += [ "	// pointers"]
hcode2 += [ "	// ..."]
hcode2 += [ "	" ]
hcode2 += [ "	// instances"]
hcode2 += [ "	// ..."]
hcode2 += [ "public:"]
hcode2 += [ "	{CLASS}();"]
hcode2 += [ "	virtual ~{CLASS}();"]
hcode2 += [ "" ]
hcode2 += [ "private:"]
hcode2 += [ "	{CLASS}(const {CLASS} &);"]
hcode2 += [ "	{CLASS} &operator=(const {CLASS} &);"]
hcode2 += [ "	" ]
hcode2 += [ "}; /* class {CLASS} */"]
hcode2 += [ "" ]
hcode2 += [ "} /* namespace {NAMESPACE} */"]
hcode2 += [ "" ]
hcode2 += [ "#endif /* #ifndef {HDEFINE} */"]
hcode2 += [ "" ]
hcode2 += [ "/* end of file */" ]

cppcode = [ ]
cppcode += [ "// #include <...>"]
cppcode += [ "#include \"{FILENOEXT}.h\""]
cppcode += [ "" ]
cppcode += [ "// System includes" ]
cppcode += [ "" ]
cppcode += [ "// Project includes" ]
cppcode += [ "" ]
cppcode += [ "// using namespace ...;" ]
cppcode += [ "" ]
cppcode += [ "namespace {NAMESPACE} {" ]
cppcode += [ "" ]
cppcode += [ "{CLASS}::{CLASS}()" ]
cppcode += [ "{" ]
cppcode += [ "	" ]
cppcode += [ "}" ]
cppcode += [ "" ]
cppcode += [ "{CLASS}::~{CLASS}()" ]
cppcode += [ "{" ]
cppcode += [ "	" ]
cppcode += [ "}" ]
cppcode += [ "" ]
cppcode += [ "} /* namespace {NAMESPACE} */" ]
cppcode += [ "" ]
cppcode += [ "/* end of file */" ]

def buildLine(line):
	newline = line.replace("{PRODUCT}", product)
	newline = newline.replace("{YEAR}", year)
	newline = newline.replace("{DATE}", date)
	newline = newline.replace("{AUTHOR}", author)
	newline = newline.replace("{USERNAME}", username)
	newline = newline.replace("{CLASS}", classname)
	newline = newline.replace("{FILENOEXT}", file)
	newline = newline.replace("{FILE}", filename)
	newline = newline.replace("{HDEFINE}", hdefine)
	newline = newline.replace("{NAMESPACE}", namespace)
	newline = newline.replace("{LINKEXCEPTION}", linkexception)
	return newline

def printComment(f, lines):
	for line in lines:
		if line == "":
			f.write(indent + newline)
		else:
			for subline in wrapper.wrap(buildLine(line)):
				f.write(subline + newline)

def writeHeader(f):
	f.write("/**" + newline)
	printComment(f, filedoc)
	f.write(" */" + newline)
	f.write(newline)
	f.write("/* " + newline)
	printComment(f, copyright)
	f.write(" */" + newline)
	f.write(newline)
def writeCode(f, code):
	for line in code:
		f.write(buildLine(line) + newline)

#note: need filename for buildLine
filename = file + ".cpp"
filepath = dir + filename
f = open(filepath, 'w')
writeHeader(f)
writeCode(f, cppcode)
f.close()

filename = file + ".h"
filepath = dir + filename
f = open(filepath, 'w')
writeHeader(f)
writeCode(f, hcode1)
f.write("/**" + newline)
printComment(f, hcomment)
f.write(" */" + newline)
writeCode(f, hcode2)
f.close()

print "Done."
