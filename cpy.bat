::cpy.bat

::Please run this file after running build.bat.
::이 파일을 실행하기 전 build.bat를 먼저 실행해 주십시오.
chcp 65001 > nul
@echo off

set "TARGET_DIR=tests\Kccim.Gui\bin\Debug\net8.0-windows"
set "DLL_FILE=kccim.dll"

if not exist "%DLL_FILE%" (
    echo [ERROR] %DLL_FILE% not found. Please run build.bat first.
    echo [오류] %DLL_FILE% 파일이 없습니다. build.bat를 먼저 실행해주십시오.
    goto end
)

if not exist "%TARGET_DIR%" (
    echo [WARNING] Target directory does not exist. Creating directory...
    echo [경고] 대상 폴더가 없습니다. 폴더를 생성합니다...
    mkdir "%TARGET_DIR%"
)

echo Copying %DLL_FILE% to %TARGET_DIR%...
echo 복사 중...
copy /Y "%DLL_FILE%" "%TARGET_DIR%\"

if %errorlevel% equ 0 (
    echo [SUCCESS] %DLL_FILE% successfully copied to Test Project.
    echo [성공] %DLL_FILE% 파일이 테스트 프로젝트로 복사되었습니다.
    echo To run the test: dotnet run --project tests/Kccim.Gui/Kccim.Gui.csproj
    ) else (
    echo [FAILED] Copy operation failed. Please check folder permissions.
    echo [실패] 복사에 실패했습니다. 폴더 권한을 확인해주세요.
)

:end
pause