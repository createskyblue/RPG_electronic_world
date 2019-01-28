@echo off 2>nul 3>nul
title 地图编辑器
mode con cols=30 lines=15
::image载入内存
%1start "" "tools\cmd.exe" "/c %~fs0 :"&exit
color 79
setlocal enabledelayedexpansion
set /p a_bmp=请输入bmp：
set /p map=请输入要编辑的地图:
if not exist %map%.dat echo.>>%map%.dat
set b_bmp=data\image\%a_bmp%.bmp

set image=load %b_bmp% %a_bmp%
set 角度=0
::set image=stretch mainbmp 16 16
set x=36
set y=33
set 方块id=0
::批量执行指令
set save_load=f
for /f "tokens=* delims= " %%a in (data/boot.dat) do (
%%a
echo %%a
)
for /l %%a in (0,1,255) do (
set 方块_%%a=0
)

  
:run
set image=cls
set image=stretch bg 256 256
set image=bg 36 33 trans
::加载动画和批量执行image指令
set save_load=f
for /f "tokens=* delims= " %%a in (%map%.dat) do (
%%a
)
set image=%a_bmp% %x% %y% 
title %map% x: %x% y: %y% id:%方块id%
choice /c wsadeqrpo /n >nul

if %errorlevel%==1 (
set /a y-=16
set /a 方块id-=16
)
if %errorlevel%==2 (
set /a y+=16
set /a 方块id+=16
)
if %errorlevel%==3 (
set /a x-=16
set /a 方块id-=1
)
if %errorlevel%==4 (
set /a x+=16
set /a 方块id+=1
)
if %errorlevel%==5 (
call :制作传送门
)
if %errorlevel%==7 (
set /p 角度=请输入旋转角度：
set image=rotate %a_bmp% %角度%
)
if %errorlevel%==6 (
call :reload
)
if %errorlevel%==9 (
call :输出地图
)
goto run
:reload
set a_bmp=
set /p a_bmp=请输入bmp：
set b_bmp=data\image\%a_bmp%.bmp
set image=load %b_bmp% %a_bmp%
Goto :Eof
:制作传送门
set 方块_%方块id%=%a_bmp%
echo set 方块_%方块id%=%a_bmp% >>%map%.dat
echo set image=%a_bmp% %x% %y% >>%map%.dat
goto :Eof

:输出地图
if exist "MAP_%map%.dat" del MAP_%map%.dat /s/f/q
for /l %%a in (0,1,255) do (
echo.!方块_%%a!,>>MAP_%map%.dat
)
for /f "tokens=*" %%i in (MAP_%map%.dat) do set n=!n! %%i
del MAP_%map%.dat
echo ^/^/LEVEL %map% >>MAP_%map%.dat
echo %n%>>MAP_%map%.dat
start "" MAP_%map%.dat
goto :Eof