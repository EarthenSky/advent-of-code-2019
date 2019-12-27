@echo off
echo run the following command first: $env:Path += ";C:\Program Files\Mono\bin"
echo --------------------------------------------------------------------------
echo compiling:
call csc day11.cs
echo --------------------------------------------------------------------------
echo running:
call mono day11.exe