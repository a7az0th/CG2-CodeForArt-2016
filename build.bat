@echo off
setlocal
@set GENERATOR=0
@set RUNTIME=0

@set SOURCE_DIR="D:\cg2\CG2-CodeForArt-2016_2"


@if %GENERATOR%==0 (
	if not "%VS140COMNTOOLS%"=="" (
		set GENERATOR="Visual Studio 14 2015 Win64"
		set RUNTIME=v140
	)
)


@if %GENERATOR%==0 (
	if not "%VS120COMNTOOLS%"=="" (
		set GENERATOR="Visual Studio 12 2013 Win64"
		set RUNTIME=v120
	)
)


@if %GENERATOR%==0 (
	if not "%VS110COMNTOOLS%"=="" (
		set GENERATOR="Visual Studio 11 2012 Win64"
		rem set GENERATOR="Visual Studio 11 2012"
		set RUNTIME=v110
	)
)

@set BUILD_DIR=CG2demo_Build
@mkdir %BUILD_DIR%
@del /f /s /q %BUILD_DIR%
@cd %BUILD_DIR%

cmake %SOURCE_DIR% -G %GENERATOR% -T %RUNTIME% 
endlocal
@pause
