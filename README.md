# VortexEngine
"VortexEngine v0.0.1" OpenGL Game Engine written in C/C++ and good for beginner-friendly and supports 3d only and extended libraries. This OpenGL Game Engine called "VortexEngine" its good for PC platforms and supports x86 for 32-bit applications,for example you can make your own game using VortexEngine you need only Editing Game Data and Using High Map Level Programming Language (HMLPL) or Level Programming Language (LPL) thats mean Map Level Editor its using Scripting not an application Thats Editing Maps compile to binary thats so hard work to implement feature because this engine is complex files of C/C++ each 6000 lines or 5000 lines in LibGL.cpp and its main Engine.

Finally,This Engine is Good for beginners and easy to use and nice.

Supports: Platforms:Windows, Linux, MacOS. External Libraries: GLFW, GLAD OpenGL Generator (3.3 core profile full extensions), Assimp (for full models formats like gltf,fbx,obj,dae,etc..), STB Library for (stb_image.h for load all images formats like png,jpg,etc... and stb_truetype.h for true type font (TTF))


NOTE: this engine is not full working in github but open source or pre-releases are in Google Drive you can download open Sources
HERE:
Open Source:

Pre-Release:

Tutorial to Build Engine:
In The Open Source "VortexEngineOpenSource-VisualStudio.zip" 
1.Extract it and open extracted folder.
2.delete these files (LibGLRemake.vcxproj,LibGLRemake.vcxproj.filters,LibGLRemake.vcxproj.user,imgui.ini).
3.delete folder "Release" because its in builded by visual studio 2022.
4.If you already delete it, you need select all files and external folder in "LibGLRemake" then copy all files in "LibGLRemake".
5.Paste into your new project Visual studio 2022 using like this Way "yourprojectname/yourprojectname/ ** paste here **.
6.If you already pasted you will back to your project visual studio 2022 application then Set Target Configuration into Release x86 because this engine was builded in x86 release.
7.Then Go to "Source Files" in visual studio 2022 and right click to "Source Files" next goto "Add" and Click To "Existing Files..." and in upload files dialog and go back to extracted folder of "VortexEngineOpenSource-VisualStudio.zip" and go to "src" folder and select all using Control + A and click Ok button to upload files into your project.
8.Then Include all external Libraries into your project in project options includes and libraries.
9.finally go to testC.c its example usage you can read from testC.c and go to LibGL.h for more helps, and build solution and if builded go to output file and run it.
