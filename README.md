# Dwarf Quest

### A ECS RPG Game Built from Scratch

### How to compile and run

To compile this we need SDL, SDL image and Entt.

```
cd src
```

```
g++ -std=c++17 $(pkg-config --cflags sdl2) \
-o ../dwarf-quest \
main.cpp \
$(pkg-config --libs sdl2 sdl2_image)
```


