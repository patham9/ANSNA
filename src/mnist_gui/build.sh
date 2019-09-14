mv ../main.c ../main.ignored
Str=`ls *.c ../*.c | xargs`
echo $Str
echo "Compilation started: Unused code will be printed and removed from the binary:"
rm -f prog
gcc -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--print-gc-sections -DSDR_BLOCK_TYPE=__uint128_t -D_POSIX_C_SOURCE=199506L -pedantic -std=c99 -g3 -O3 -Wall -Wextra -Wformat-security $Str -lXinerama -lXrender -lXrandr -lm -lGL -lGLU -lglut -lGLEW -lglfw -lm -lpthread -oprog
mv ../main.ignored ../main.c
echo "Done."
