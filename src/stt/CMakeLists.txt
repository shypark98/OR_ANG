###############################################################################
##
## BSD 3-Clause License
##
## Copyright (c) 2019, The Regents of the University of California
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
##   and#or other materials provided with the distribution.
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
###############################################################################

include("openroad")

find_package(LEMON NAMES LEMON lemon REQUIRED)

set(FLUTE_HOME ${PROJECT_SOURCE_DIR}/src/stt/src/flt)
set(PDR_HOME ${PROJECT_SOURCE_DIR}/src/stt/src/pdr)

set(POWV9_DAT ${FLUTE_HOME}/etc/POWV9.dat)
set(POST9_DAT ${FLUTE_HOME}/etc/POST9.dat)
set(POWV9_CPP ${CMAKE_CURRENT_BINARY_DIR}/POWV9.cpp)
set(POST9_CPP ${CMAKE_CURRENT_BINARY_DIR}/POST9.cpp)

add_custom_command(OUTPUT ${POWV9_CPP}
  COMMAND ${FLUTE_HOME}/etc/MakeDatVar.tcl powv9 ${POWV9_CPP} ${POWV9_DAT}
  DEPENDS ${POWV9_DAT} ${FLUTE_HOME}/etc/MakeDatVar.tcl
)

add_custom_command(OUTPUT ${POST9_CPP}
  COMMAND ${FLUTE_HOME}/etc/MakeDatVar.tcl post9 ${POST9_CPP} ${POST9_DAT}
  DEPENDS ${POST9_DAT} ${FLUTE_HOME}/etc/MakeDatVar.tcl
  )

swig_lib(NAME      stt
         NAMESPACE stt
         I_FILE    src/SteinerTreeBuilder.i
         SCRIPTS   src/SteinerTreeBuilder.tcl
)

add_library(stt_lib
    src/SteinerTreeBuilder.cpp
    ${FLUTE_HOME}/flute.cpp
    ${POWV9_CPP}
    ${POST9_CPP}
    ${PDR_HOME}/src/pd.cpp
)

target_sources(stt
  PRIVATE
    src/MakeSteinerTreeBuilder.cpp
    src/LinesRenderer.cpp
)

target_include_directories(stt_lib
  PUBLIC
    include
  PRIVATE
    ${LEMON_INCLUDE_DIRS}
)

target_include_directories(stt
  PUBLIC
    include
  PRIVATE
    src
)

target_link_libraries(stt_lib
    utl_lib
    OpenSTA
    odb
)

target_link_libraries(stt
    stt_lib
    utl
    gui
    OpenSTA
)

messages(
  TARGET stt
)

if (Python3_FOUND AND BUILD_PYTHON)
  swig_lib(NAME          stt_py
           NAMESPACE     stt
           LANGUAGE      python
           I_FILE        src/SteinerTreeBuilder-py.i
           SWIG_INCLUDES ${PROJECT_SOURCE_DIR}/include/stt
           SCRIPTS       ${CMAKE_CURRENT_BINARY_DIR}/stt_py.py
  )

  target_include_directories(stt_py
    PUBLIC
      include
  )

  target_link_libraries(stt_py
    PUBLIC
      stt
  )

endif()
