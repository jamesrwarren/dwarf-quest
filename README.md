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

### Explanation of folder structure

Components that can be added to an entity are arranged in the components folder. Components are structs that can be emplaced on entities to give them some sort of behaviour.

Systems are operations performed on entities with the correct component selection for the system to run on and run on every frame cycle.

World is a folder for initialising the game state.

Config is for global configuration of the game parameters.

Assets is a folder that includes all images, maps and other non code assets that the game needs.

