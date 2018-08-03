add_custom_target(pytest COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=$<TARGET_FILE_DIR:parselmouth> ${PYTHON_EXECUTABLE} -m pytest DEPENDS parselmouth WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(tests DEPENDS pytest)
