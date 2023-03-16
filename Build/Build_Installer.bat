@ECHO OFF

SET MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
SET DEVENV="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe"
SET SETUPSLN=..\Src\Setup\Setup.sln

%DEVENV% %SETUPSLN% /Rebuild "ReleaseMT|x64" /Project Setup
