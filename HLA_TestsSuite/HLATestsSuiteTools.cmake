ENABLE_TESTING()

# Here is following several macros from
# http://www.cmake.org/Wiki/CMakeMacroListOperations
MACRO(CAR var)
  SET(${var} ${ARGV1})
ENDMACRO(CAR)

MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

#http://www.cmake.org/Wiki/CMakeMacroParseArguments
MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})    
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})    
    LIST_CONTAINS(is_arg_name ${arg} ${arg_names})
    IF (is_arg_name)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name)
      LIST_CONTAINS(is_option ${arg} ${option_names})
      IF (is_option)
	     SET(${prefix}_${arg} TRUE)
      ELSE (is_option)
	     SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option)
    ENDIF (is_arg_name)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)

#
# HLATestsSuite_ADD_TEST may be used to ease the build and run of HLA Tests.
#
# Usage:
#  HLATestsSuite_ADD_TEST(<testname>
#                          FED_FILE <fed file list>
#                          SOURCE_FILES <source files list>
#                          DTEST_SCRIPT <script>
#                          COMMENTS "a comment string"
#                          MORELIBS "some libraries the test should be linked with"
#
#  All options but the <testname> are optional.
#  FED_FILE will be installed in share/federations
#  SOURCES_FILES will be compiled into an executable whose name is <testname>
#                the executable will be automatically linked with RTI libraries.
#  DTEST_SCRIPTS is the list of actual test drivers that will be passed to ctest
#                the script will be called with argument
#                        --federate=${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME}
#                There will be one ctest test for each provided script.
#  COMMENTS may be any helpful comment
#  MORELIBS may be used to specify additional libraries (besides RTI libs)
#
#  VERBOSE  the macro will print some infos when run.
#  HLA1516  use IEEE-1516 libs instead of default HLA1.3 libs.
#           
MACRO(HLATestsSuite_ADD_TEST)
    PARSE_ARGUMENTS(HLATSTools "FED_FILE;SOURCE_FILES;DTEST_SCRIPT;COMMENTS;MORELIBS" "VERBOSE;HLA1516" ${ARGN})          
    CAR(TESTNAME ${HLATSTools_DEFAULT_ARGS})
           
    MESSAGE(STATUS "Configuring HLA Test: ${TESTNAME}")
    IF (HLATSTools_VERBOSE)     
       MESSAGE(STATUS "       FED_FILE = ${HLATSTools_FED_FILE}")
       MESSAGE(STATUS "   SOURCE_FILES = ${HLATSTools_SOURCE_FILES}")
       MESSAGE(STATUS "   DTEST_SCRIPT = ${HLATSTools_DTEST_SCRIPT}")       
       MESSAGE(STATUS "       COMMENTS = ${HLATSTools_COMMENTS}")
       MESSAGE(STATUS "       MORELIBS = ${HLATSTools_MORELIBS}")
       IF(HLATSTools_HLA1516)
         MESSAGE(STATUS " -> Will be linked with IEEE-1516 libs ")
       ELSE(HLATSTools_HLA1516)
         MESSAGE(STATUS " -> Will be linked with HLA 1.3 libs   ")
       ENDIF(HLATSTools_HLA1516)
    ENDIF(HLATSTools_VERBOSE)
    
    
    IF (HLATSTools_SOURCE_FILES) 
        ADD_EXECUTABLE(${TESTNAME} ${HLATSTools_SOURCE_FILES})    
        IF(HLATSTools_HLA1516)
           TARGET_LINK_LIBRARIES (${TESTNAME} ${RTI1516_LIBRARIES} ${HLATSTools_MORELIBS})
        ELSE(HLATSTools_HLA1516)
           TARGET_LINK_LIBRARIES (${TESTNAME} ${RTI_LIBRARIES} ${HLATSTools_MORELIBS})
        ENDIF(HLATSTools_HLA1516)    
        INSTALL(TARGETS ${TESTNAME} RUNTIME DESTINATION bin)
    ENDIF(HLATSTools_SOURCE_FILES)
    
    IF (HLATSTools_FED_FILE)
        INSTALL(FILES ${HLATSTools_FED_FILE} DESTINATION share/federations)
    ENDIF(HLATSTools_FED_FILE)
    
    IF(HLATSTools_DTEST_SCRIPT)
        SET(NUM 0)
        foreach(DTEST_SCRIPT ${HLATSTools_DTEST_SCRIPT})
            MATH(EXPR NUM '${NUM}+1')
            IF (HLATSTools_VERBOSE)
                MESSAGE(STATUS "       Adding test ${TESTNAME}-${NUM} for test script ${DTEST_SCRIPT}")
            ENDIF(HLATSTools_VERBOSE)
            CONFIGURE_FILE(${DTEST_SCRIPT} ${DTEST_SCRIPT} COPYONLY)
            ADD_TEST(${TESTNAME}-${NUM} ${DTEST_SCRIPT} --federate=${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME})
        endforeach(DTEST_SCRIPT)  
    ENDIF(HLATSTools_DTEST_SCRIPT)
    
ENDMACRO(HLATestsSuite_ADD_TEST)
