cmake_minimum_required(VERSION 3.19)

project(asbear
	VERSION 3.0.0
	DESCRIPTION "A Simple But Efficient Audio Recorder"
	LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_VERSION_MAJOR 6)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../build/bin)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(SFML REQUIRED COMPONENTS system audio)


include_directories(include)

set(TS_FILES translations/asbear-fr.ts)

set_source_files_properties(${TS_FILES}
	PROPERTIES OUTPUT_LOCATION "../../assets/lang")

set(PROJECT_SOURCES
	src/main.cpp
	src/application.cpp
	src/application/converter_widget.cpp
	src/application/recorder_widget.cpp
	src/application/recording_manager_widget.cpp
	src/application/settings_widget.cpp
	src/custom_widgets/audio_level_bar.cpp
	src/custom_widgets/devices_combo_box.cpp
	src/custom_widgets/direct_jump_slider.cpp
	src/custom_widgets/level_timeline.cpp
	src/custom_widgets/playback_bar.cpp
	src/tools/audio_recorder.cpp
	src/tools/converter.cpp
	src/tools/player.cpp
	src/main.cpp
	include/application.hpp
	include/application/converter_widget.hpp
	include/application/recorder_widget.hpp
	include/application/recording_manager_widget.hpp
	include/application/settings_widget.hpp
	include/custom_widgets/audio_level_bar.hpp
	include/custom_widgets/devices_combo_box.hpp
	include/custom_widgets/direct_jump_slider.hpp
	include/custom_widgets/level_timeline.hpp
	include/custom_widgets/playback_bar.hpp
	include/tools/audio_recorder.hpp
	include/tools/converter.hpp
	include/tools/player.hpp
	${TS_FILES}
)

qt_add_executable(asbear
	MANUAL_FINALIZATION
	${PROJECT_SOURCES}
)


target_link_libraries(asbear PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(asbear PRIVATE sfml-audio sfml-system)

set_target_properties(asbear PROPERTIES
	${BUNDLE_ID_OPTION}
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS asbear
BUNDLE DESTINATION
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(asbear)

qt_add_lupdate(asbear TS_FILES ${TS_FILES})
qt_add_lrelease(TS_FILES ${TS_FILES})

# install assets to release folder
ADD_CUSTOM_TARGET(link_target ALL COMMAND ${CMAKE_COMMAND} -E create_symlink "../../assets" "../../build/release/data")

