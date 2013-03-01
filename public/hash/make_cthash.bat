echo off
echo // This file is auto-generated, any edits will be lost > _cthash.h
echo // Run make_cthash.bat [max] to generate this file >> _cthash.h

rem // The hash algo

set x=0
set i=1
:loop_hash
echo #define CTHASH%i%(S) CTHASHOP(CTHASH%x%(S),S,%x%) >> _cthash.h
set /A x=i
set /A i=i+1
if not %i%==%1 goto loop_hash

rem // Impl for char

echo #define EXT(C) ((unsigned char)C) >> _cthash.h
set x=0
set i=1
:loop_char
echo CTHASHIMPL(%i%,char,CTHASH%x%) >> _cthash.h
set /A x=i
set /A i=i+1
if not %i%==%1 goto loop_char
echo #undef EXT >> _cthash.h

rem // Impl for wchar_t

echo #define EXT(C) C >> _cthash.h
set x=0
set i=1
:loop_wchar_t
echo CTHASHIMPL(%i%,wchar_t,CTHASH%x%) >> _cthash.h
set /A x=i
set /A i=i+1
if not %i%==%1 goto loop_wchar_t
echo #undef EXT >> _cthash.h

rem // Undef macros

set i=1
:loop_undef
echo #undef CTHASH%i% >> _cthash.h
set /A i=i+1
if not %i%==%1 goto loop_undef

