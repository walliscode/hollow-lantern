# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

if(EXISTS "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/CMakeFiles/fc-stamp/glm/glm-gitclone-lastrun.txt" AND EXISTS "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/CMakeFiles/fc-stamp/glm/glm-gitinfo.txt" AND
  "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/CMakeFiles/fc-stamp/glm/glm-gitclone-lastrun.txt" IS_NEWER_THAN "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/CMakeFiles/fc-stamp/glm/glm-gitinfo.txt")
  message(VERBOSE
    "Avoiding repeated git clone, stamp file is up to date: "
    "'/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/CMakeFiles/fc-stamp/glm/glm-gitclone-lastrun.txt'"
  )
  return()
endif()

# Even at VERBOSE level, we don't want to see the commands executed, but
# enabling them to be shown for DEBUG may be useful to help diagnose problems.
cmake_language(GET_MESSAGE_LOG_LEVEL active_log_level)
if(active_log_level MATCHES "DEBUG|TRACE")
  set(maybe_show_command COMMAND_ECHO STDOUT)
else()
  set(maybe_show_command "")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/_deps/glm-src"
  RESULT_VARIABLE error_code
  ${maybe_show_command}
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/_deps/glm-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"
            clone --no-checkout --config "advice.detachedHead=false" "https://github.com/g-truc/glm.git" "glm-src"
    WORKING_DIRECTORY "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/_deps"
    RESULT_VARIABLE error_code
    ${maybe_show_command}
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(NOTICE "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/g-truc/glm.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"
          checkout "0af55ccecd98d4e5a8d1fad7de25ba429d60e863" --
  WORKING_DIRECTORY "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/_deps/glm-src"
  RESULT_VARIABLE error_code
  ${maybe_show_command}
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: '0af55ccecd98d4e5a8d1fad7de25ba429d60e863'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/_deps/glm-src"
    RESULT_VARIABLE error_code
    ${maybe_show_command}
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/_deps/glm-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/CMakeFiles/fc-stamp/glm/glm-gitinfo.txt" "/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/CMakeFiles/fc-stamp/glm/glm-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  ${maybe_show_command}
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/bwallis/projects/walliscode/hollow-lantern/build/Debug/CMakeFiles/fc-stamp/glm/glm-gitclone-lastrun.txt'")
endif()
