Tetris Clone

**Main functions**



**Data Structure**

Main game field will be a grid that is 10 blocks wide and 20 blocks tall, where the blocks start in row 21 and 22.

Game field size -> 10x20 visible, 10x22 total.


each block can occupy several states: (keep track using a u8int)
Empty
Red block
Green Block
Blue Block
Orange Block
Purple Block


We need some kind of data structure to represent the currently falling block. This would include information such as:
the color of the block (in that u8int format probably)
the shape of the block (maybe start with single blocks falling for simplicity)
the current position
the finalization timer after touching another block. (oh yeah, and reset this timer if we go back in the air, so check every frame)

typedef struct {
	uint8_t color;
	uint8_t shape;
	int x;
	int y;
	int timer;

}FallingBlock;


control flow:
block spawns at top of screen

block starts falling until it touches another block, then wait one second (for sliding)and finalize the placement.
Every time we finalize the placement of a block, we should check the row(s) that it occupies to see if we made a line.

Spawn another block