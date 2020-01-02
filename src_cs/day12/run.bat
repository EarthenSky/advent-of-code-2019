@echo off
echo run the following command first: $env:Path += ";C:\Program Files\Mono\bin"
echo --------------------------------------------------------------------------
echo compiling:
call csc day12.cs alg.cs -optimize+
echo --------------------------------------------------------------------------
echo running:
call mono day12.exe