workspace "Negroni"
   configurations { "Debug", "Release" }
   architecture "x64"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Negroni"
   location "Negroni"
   kind "ConsoleApp"
   dpiawareness "HighPerMonitor"
   language "C++"
   objdir ("obj/" .. outputdir .. "/%{prj.name}")
   targetdir ("build/" .. outputdir .. "/%{prj.name}")

   files
   {
      "%{prj.name}/**.h",
      "%{prj.name}/**.hpp",
      "%{prj.name}/**.cpp"
   }

   filter "system:windows"
      cppdialect "C++20"
      buildoptions{"/utf-8"}
      includedirs { "%{prj.name}/ImGui/Vendor", "%{prj.name}/ImGui/Vendor/backends" }
      libdirs "$(DXSDK_DIR)/Lib/x64"
      links { "d3d11", "d3dcompiler", "dxgi" }

   filter "configurations:Debug"
      defines { "DEBUG", "_DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG", "_RELEASE", "RELEASE" }
      optimize "On"
