# Nuclear

> Only compiles on and for Linux right now

## Compiler:
### Build:
```bash
make Nuclear
```
For testing you can run
```bash
make Nuclear run
```

### Compile
```bash
./Nuclear input.(n | nuke | nuclear) output
```
When using the compiler ensure to make the extension of the source as `n`, `nuke`, or `nuclear` and the output extension as nothing or `exe`
```bash
./Nuclear test.nuke out -t -r
```
The option `-t` will show how long it took to compile

The option `-r` will run it after compilation

```bash
./Nuclear test.nuke out -S
```
The option `-S` will only output as assembly and not a binary.

## Nuclear
For an example of how to code in Nuke check `syntax.nuke`