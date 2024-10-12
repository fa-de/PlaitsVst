cmake_minimum_required(VERSION 3.14.0)

option(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES "Enable VST 3 Plug-in Examples" OFF)
option(SMTG_ENABLE_VST3_HOSTING_EXAMPLES "Enable VST 3 Hosting Examples" OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "")

set(vst3sdk_SOURCE_DIR "D:/Music/VST/VST_SDK/vst3sdk")
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(PlaitsVst
    # This is your plug-in version number. Change it here only.
    # Version number symbols usable in C++ can be found in
    # source/version.h and ${PROJECT_BINARY_DIR}/projectversion.h.
    VERSION 0.5.0.0 
    DESCRIPTION "PlaitsVst VST 3 Plug-in"
)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(PlaitsVst
    source/version.h
    source/myplugincids.h
    source/mypluginparameters.h
    source/mypluginprocessor.h
    source/mypluginprocessor.cpp
    source/myplugincontroller.h
    source/myplugincontroller.cpp
    source/mypluginentry.cpp
    source/PlaitsWrapper.h
    source/PlaitsWrapper.cpp
    source/EventQueue.h
    source/MonoVoiceHandler.h
    source/VstParameterInterpolator.h
)

#- VSTGUI Wanted ----
if(SMTG_ENABLE_VSTGUI_SUPPORT)
    target_sources(PlaitsVst
        PRIVATE
            resource/myplugineditor.uidesc
    )
    target_link_libraries(PlaitsVst
        PRIVATE
            vstgui_support
    )
    smtg_target_add_plugin_resources(PlaitsVst
        RESOURCES
            "resource/myplugineditor.uidesc"
    )
endif(SMTG_ENABLE_VSTGUI_SUPPORT)
# -------------------

smtg_target_add_plugin_snapshots (PlaitsVst
    RESOURCES
        resource/6DC72358B50551988A2ACCF08FA9E65F_snapshot.png
        resource/6DC72358B50551988A2ACCF08FA9E65F_snapshot_2.0x.png
)

target_link_libraries(PlaitsVst
    PRIVATE
        sdk
)

smtg_target_configure_version_file(PlaitsVst)

if(SMTG_MAC)
    smtg_target_set_bundle(PlaitsVst
        BUNDLE_IDENTIFIER de.phobon.PlaitsVst
        COMPANY_NAME "Fabian Deutschmann"
    )
    smtg_target_set_debug_executable(PlaitsVst
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(PlaitsVst PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PlaitsVst)

        smtg_target_set_debug_executable(PlaitsVst
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)

#add_subdirectory("./eurorack/")
include(EurorackCMakeLists.txt)


target_link_libraries(PlaitsVst PRIVATE PLAITS)
target_include_directories(PlaitsVst PRIVATE ./eurorack/)

if(MSVC)
    target_compile_definitions(PlaitsVst PRIVATE _USE_MATH_DEFINES)
endif()