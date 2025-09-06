@echo off

set venv_path=%VENV_PATH%

set CMAKE_GENERATOR=Ninja

set CC=clang

set CXX=clang++

REM Store the original directory
set "original_dir=%CD%"

REM Check if virtual environment path exists
if not exist "%venv_path%" (
    echo Virtual environment not found at %venv_path%
    exit /b 1
)

REM Check if activate script exists
if not exist "%venv_path%\Scripts\activate.bat" (
    echo Activation script not found at %venv_path%\Scripts\activate.bat
    exit /b 1
)

REM Activate virtual environment
call "%venv_path%\Scripts\activate.bat"

REM Return to the original directory
cd /d "%original_dir%"

REM Optional: Confirm virtual environment is active
echo Virtual environment activated
python --version