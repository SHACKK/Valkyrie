@ECHO OFF

SET MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
SET DEVENV="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe"
SET PROJECTSLN=..\Src\Valkyrie\Valkyrie.sln 

RD /S /Q x64ReleaseMT

%MSBUILD% %PROJECTSLN% /m /t:Rebuild /p:Configuration=ReleaseMT;Platform=x64
