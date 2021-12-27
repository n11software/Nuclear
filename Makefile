rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CPPSourceCode = $(call rwildcard,Source,*.cpp)
Objects = $(patsubst Source/%.cpp, Build/%.o, $(CPPSourceCode))
Directories = $(wildcard Source/*)

Build/%.o: Source/%.cpp
	@-mkdir -p $(@D)
	@-g++ -c $^ -std=c++2a -o $@

all: Nuclear run

Nuclear: $(Objects)
	@g++ $(Objects) -std=c++2a -o Nuclear

run:
	@./Nuclear test.nuke Nuked

compile:
	@./Nuclear test.nuke Nuked > test.S
	@-fasm test.S
	@./test