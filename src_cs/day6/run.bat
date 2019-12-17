@echo off
echo run the following command first: $env:Path += ";C:\Program Files\Mono\bin"
echo --------------------------------------------------------------------------
echo compiling:
call csc main.cs
echo --------------------------------------------------------------------------
echo running:
call mono main.exe