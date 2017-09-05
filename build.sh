clear

clang src/platform/sdl_main.c -lSDL2 -g -o start_retro

if [[ -n $? ]]; then
  echo "build succeeded"
else
  echo "build failed"
fi
