@echo off

set venv_path=%VENV_PATH%
set CMAKE_GENERATOR=Ninja
set CC=clang
set CXX=clang++
set CMAKE_BUILD_PARALLEL_LEVEL=4
set CMAKE_BUILD_TYPE=Debug

set CMAKE_ARGS=-DOBJECT_LAYER_BITS=16 ^
-DUSE_SSE4_1=ON ^
-DUSE_SSE4_2=ON ^
-DUSE_AVX=ON ^
-DUSE_AVX2=ON ^
-DUSE_F16C=ON ^
-DUSE_LZCNT=ON ^
-DUSE_TZCNT=ON ^
-DUSE_FMADD=ON ^
-DINTERPROCEDURAL_OPTIMIZATION=ON ^
-DENABLE_OBJECT_STREAM=ON ^
-DDEBUG_RENDERER_IN_DEBUG_AND_RELEASE=ON ^
-DPROFILER_IN_DISTRIBUTION=ON ^
-DOVERRIDE_CXX_FLAGS=ON ^
-DUSE_AVX512=OFF ^
-DCPP_EXCEPTIONS_ENABLED=OFF ^
-DCROSS_PLATFORM_DETERMINISTIC=OFF ^
-DUSE_STD_VECTOR=OFF ^
-DTRACK_BROADPHASE_STATS=OFF ^
-DTRACK_NARROWPHASE_STATS=OFF ^
-DDOUBLE_PRECISION=OFF ^
-DFLOATING_POINT_EXCEPTIONS_ENABLED=OFF ^
-DDISABLE_CUSTOM_ALLOCATOR=ON ^
-DCPP_RTTI_ENABLED=OFF

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

echo Virtual environment activated
python --version
