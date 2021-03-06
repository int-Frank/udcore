project ("udCore" .. (projectSuffix or ""))
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"
	flags { "StaticRuntime", "OmitDefaultLibrary", "FatalWarnings" }

	files { "Source/**", "Include/**", "Docs/**" }

	files { "3rdParty/libdeflate/libdeflate.h", "3rdParty/libdeflate/lib/**" }
	includedirs { "3rdParty/libdeflate", "3rdParty/libdeflate/common" }

	files { "project.lua" }

	includedirs { "3rdParty" }
	includedirs { "3rdParty/mbedtls/include" }
	includedirs { "Include" }

	-- include common stuff
	dofile "common-proj.lua"

	files { "3rdParty/mbedtls/library/*.c", "3rdParty/mbedtls/include/mbedtls/*.h" }

	filter { "files:3rdParty/**" }
		warnings "Off"

	-- XCode4 can't set per-file configurations, so FatalWarnings needs to be off to build
	filter { "action:xcode4" }
		removeflags { "FatalWarnings" }

	filter { "configurations:Release", "system:Windows" }
		symbols "On"

	filter { "system:linux", "toolset:gcc" }
		buildoptions { "-pedantic" }
		enablewarnings { "undef" }

	-- for windows, make the output name and location identical to that of udbin
	filter { "system:Windows" }
		targetdir "Lib/%{cfg.system}_%{cfg.shortname}"
		symbolspath "$(TargetDir)/$(ProjectName).pdb"

	-- for nacl, make the output name and location identical to that of udbin
	filter { "system:nacl" }
		targetdir "Lib/windows_%{cfg.shortname}"

	filter {}
