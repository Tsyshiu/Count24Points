@echo off
REM setlocal ENABLEDELAYEDEXPANSION 
REM setlocal效果等同使用call及%%VARIABLE%%，但本次不能使用setlocal，会使所有的set变量退出后不可用
if "%HasBeenInVSCLEnv%" == "" ( 
    REM 设置cl环境
    set "CurPath=%cd%"
    call "E:\Program Files (x86)\Visual Studio 2017\VC\Auxiliary\Build\vcvars64.bat"
    call cd /d "%%CurPath%%"
    set HasBeenInVSCLEnv=true
    echo;
    echo see USAGE at: "C:\Users\acer\Documents\WindowsPowerShell\zxCompiling.bat"
    REM clcp^(Compile^)-- cl /EHsc /Fe[param2:outfileName] [param1:srcFile/srcPath]
    REM ------- clcp [path] [outfileName]
    REM ------- clcp /f [param1:file] [outfileName]
    REM cldb^(Debug^)-- cl /EHsc /Fe[param2:outfileName] [param1:srcFilePath/srcPath] /Zi
    REM ------- cldb [path] [outfileName]
    REM ------- cldb /f [file] [outfileName]    
    REM more-- https://msdn.microsoft.com/zh-cn/library/19z1t1wy.aspx   
)

for %%a in (clcpFun,cldbFun) do (
if "%~1"=="%%a" call:%%a %2 %3 %4 
)

GOTO :EOF

::--------------------------------------------------------  
::-- Function section 
::--------------------------------------------------------  
:clcpFun
if "%~1" == "/f" (
    if "%~2" == "" (
        echo 参数错误
        GOTO :EOF
    ) else (
        set srcFiles=%~2
        set outFile=%~3
    )
) else (
    if "%~1" == "" (
        set srcFiles=*.c*
        set outFile=%~2        
    ) else (
        set srcFiles=%~1\*.c*
        set outFile=%~2
    )
)
cl /EHsc /W3 /sdl /Fe%outFile% %srcFiles%
GOTO :EOF

:cldbFun
if "%~1" == "/f" (
    if "%~2" == "" (
        echo 参数错误
        GOTO :EOF
    ) else (
        set srcFiles=%~2
        set outFile=%~3
    )
) else (
    if "%~1" == "" (
        set srcFiles=*.c*
        set outFile=%~2        
    ) else (
        set srcFiles=%~1\*.c*
        set outFile=%~2
    )
)
cl /EHsc /W3 /sdl /Fe%outFile% %srcFiles% /Zi
GOTO :EOF