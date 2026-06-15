g++ main.cpp FloodSim.cpp Grid.cpp MindustryLoader.cpp BinaryReader.cpp \
  -std=c++23 \
  -O0 -g \
  -fsanitize=address,undefined \
  -fno-omit-frame-pointer \
  -lsfml-graphics -lsfml-window -lsfml-system -lz
