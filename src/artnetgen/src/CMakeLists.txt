############################################################################
##
## BSD 3-Clause License
##
## Copyright (c) 2019, James Cherry, Parallax Software, Inc.
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are met:
##
## * Redistributions of source code must retain the above copyright notice, this
##   list of conditions and the following disclaimer.
##
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer in the documentation
##   and/or other materials provided with the distribution.
##
## * Neither the name of the copyright holder nor the names of its
##   contributors may be used to endorse or promote products derived from
##   this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
## LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
## CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
##
############################################################################

include("openroad")

# already made in ppl
#add_subdirectory(munkres)

swig_lib(NAME      artnetgen
         NAMESPACE artnetgen
         I_FILE    artNetGen.i
         SCRIPTS   artNetGen.tcl
)

set(ARTNETGEN_WRAP ${CMAKE_CURRENT_BINARY_DIR}/artNetGen_wrap.cc)
set(ARTNETGEN_TCL_INIT ${CMAKE_CURRENT_BINARY_DIR}/artNetGenExtTclInitVar.cc)
set(LOGGER_HOME ${PROJECT_SOURCE_DIR}/src/utl)

add_custom_command(OUTPUT ${ARTNETGEN_WRAP}
    COMMAND ${SWIG_EXECUTABLE} -tcl8 -c++ -I${ARTNETGEN_HOME}/include -I${ODB_HOME}/src/swig/tcl -o
    ${ARTNETGEN_WRAP} ${ARTNETGEN_HOME}/src/artNetGen.i
    WORKING_DIRECTORY ${ARTNETGEN_HOME}/src
    DEPENDS ${ARTNETGEN_HOME}/src/artNetGen.i ${ARTNETGEN_HOME}/include/artnetgen/artNetGen.h
)

add_custom_command(OUTPUT ${ARTNETGEN_TCL_INIT}
    COMMAND ${OPENSTA_HOME}/etc/TclEncode.tcl ${ARTNETGEN_TCL_INIT} artnetgen_tcl_inits
    ${ARTNETGEN_HOME}/src/artNetGen.tcl
    WORKING_DIRECTORY ${ARTNETGEN_HOME}/src
    DEPENDS ${ARTNETGEN_HOME}/src/artNetGen.tcl ${OPENSTA_HOME}/etc/TclEncode.tcl
)

target_sources(artnetgen #빌드 최종 결과물로 생성할 라이브러리 추가
PRIVATE
    artNetGen.cpp
    MakeArtNetGen.cpp
    netlist.cpp
    distribution.cpp
    bin.cpp
    node.cpp
    techMapping.cpp
    distMatching.cpp
    writeSpec.cpp
    ${ARTNETGEN_WRAP}
    ${ARTNETGEN_TCL_INIT}
)


target_include_directories(artnetgen
  PUBLIC
    ../include
   ${Boost_INCLUDE_DIR}
)

target_link_libraries(artnetgen
  PRIVATE
    odb
    dbSta
    OpenSTA
    utl
)

messages(
  TARGET artnetgen
  OUTPUT_DIR ..
)
