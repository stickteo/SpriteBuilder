# SpriteBuilder
Designed to extract OBJs from GBA ROMs and generate an ARMIPS patch from an edited BMP.

## Intro
SpriteBuilder constructs a BMP from a descriptor file and the ROM file.

Using the same descriptor file, an ARMIPS patch file is generated from the edited BMP file.

## Patching Process
- Create a descriptor file.
- Generate BMP using SpriteBuilder.
- Edit BMP. (e.g. using Usenti)
- Generate ARMIPS patch using SpriteBuilder.
- Profit.

## Command Line Options
```
extract bmp : spritebuilder -x dsc.txt img.bmp rom.bin (pal.pal)
compile asm : spritebuilder -a dsc.txt img.bmp patch.asm
```

## Descriptor File
A plaintext script. Meant to be light and easy to edit.

### Address Operators
```a <offset>``` : sets ROM address counter, 0 is start of file

### OBJ Operators
```s <w> <h>``` : outputs OBJ with <w> width and <h> height

```p <n>``` : set palette to index <n> (0-15), only valid for 4bpp

```h``` : half, set to 4bpp (default)

```f``` : full, set to 8bpp

### Image Movement Operators
```m <x> <y>``` : sets sprite output position at (x,y); (0,0) = top left

```x <x>``` : sets x coordinate

```y <y>``` : sets y coordinate

```r <dx> <dy>``` : adds relative offsets, ```(x,y) = (x,y) + (dx,dy)```

```rx <dx>``` : adds dx to x coordinate

```ry <dy>``` : adds dy to y coordinate

### Miscellaneous Operators
```;``` : line comment

## Thoughts and Comments
The design of SpriteBuilder is meant to improve upon gba2bmp. The idea with having a script based system is to reduce the amount of tedious work.

In gba2bmp, one has to manually write out the index for each tile...
In contrast SpriteBuilder, with the help of CrystalTile2, a script can be built up and eliminate more tedious tasks.
Some major tasks are calculating the address offset and tile placement.

## Changelog
- 2021-11-04 : alpha version
