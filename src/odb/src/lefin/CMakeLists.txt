add_library(lefin
    lefin.cpp
    reader.cpp
    lefTechLayerSpacingEolParser.cpp
    lefTechLayerWrongDirSpacingParser.cpp
    lefTechLayerMinStepParser.cpp
    lefTechLayerCornerSpacingParser.cpp
    lefTechLayerSpacingTablePrlParser.cpp
    lefTechLayerRightWayOnGridOnlyParser.cpp
    lefTechLayerRectOnlyParser.cpp
    lefTechLayerCutClassParser.cpp
    lefTechLayerCutSpacingParser.cpp
    lefTechLayerCutSpacingTableParser.cpp
    lefTechLayerCutEnclosureParser.cpp
    lefTechLayerTypeParser.cpp
    lefTechLayerEolExtensionParser.cpp
    lefTechLayerEolKeepOutRuleParser.cpp
    lefTechLayerAreaRuleParser.cpp
    lefTechLayerPitchRuleParser.cpp
    lefTechLayerForbiddenSpacingRuleParser.cpp
    lefMacroClassParser.cpp
    ArraySpacingParser.cpp
    WidthTableParser.cpp
    MinCutParser.cpp
    MetalWidthViaMapParser.cpp
    KeepOutZoneParser.cpp
)

target_include_directories(lefin
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${TCL_INCLUDE_PATH}
)

set_target_properties(lefin
  PROPERTIES
    # python requirement
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(lefin
    PUBLIC
        db
        zutil
        lef
        lefzlib
        utl_lib
        Boost::boost
)
