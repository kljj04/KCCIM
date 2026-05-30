::build.bat

::Needs GCC to build.
:: Check if you have the GCC registered in path.
::빌드하려면 GCC가 필요합니다.
::빌드가 안 된다면 GCC가 path에 등록되어있는지 확인하십시오.
@echo off
chcp 65001 > nul

echo Building kccim.dll...
echo 빌드 중...
gcc -Wall -Wextra -O2 -shared -DKCCIM_EXPORTS -Iinclude -Iinternal -o kccim.dll src/automata.c src/table.c src/kccim.c

if %errorlevel% equ 0 (
    echo [SUCCESS] kccim.dll built successfully.
    echo To copy the DLL to test projects, run cpy.bat.
    echo [성공] 성공적으로 빌드되었습니다.
    echo DLL을 테스트 프로젝트로 복사하기 위해 cpy.bat을 실행하십시오.
) else (
    echo [FAILED] Compilation failed. Please retry.
    echo [실패] 빌드가 실패하였습니다. 다시 시도해주세요.
)
pause