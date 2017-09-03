clear

clang src/platform/sdl_main.c -lSDL2 -o start_retro

if [[ -n $? ]]; then
  echo "build succeeded"
else
  echo "build failed"
fi
