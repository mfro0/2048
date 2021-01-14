/************************************************************************

    2048: Sliding-block puzzle game

    Copyright (C) 2014  John Elliott <jce@seasip.demon.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*************************************************************************/

/* The external interface of the game engine. Start with the game state. */

typedef struct
{
    /*
     * The tiles. The board is 4x4 but the memory array is 6x6, to
     * provide a boundary all around the game area
     */
    unsigned short tile[6][6];
    /* The current score. */
    long score;
} GAMEBOARD;

/*
 * A tile score is 1-2048 (the existence of a 2048 tile indicating victory).
 * Score of zero indicates an empty space.
 * The top two bits of a tile score are flags:
 */

#define BORDER 0x8000   /* This flag is set on all tiles round the edge */
#define MERGED 0x4000   /* This flag is set on any tiles created in the */
                        /* most recent turn */


/* Initialise the board to empty */
void gb_init(GAMEBOARD *g);

/* Drop a new tile at random on the board. This will either be a '1' or
 * a '2'. */
void gb_newtile(GAMEBOARD *g);

/* Clear the MERGED flags on all tiles */
void gb_clearmerge(GAMEBOARD *g);

/* See if any moves are possible. Returns 1 if yes, 0 if the board is
 * unwinnable. */
int gb_canmove(GAMEBOARD *g);

/* Attempt a move. dx = -1, dy = 0: Left. dx = 1, dy = 0: Right etc. */
int gb_move(GAMEBOARD *g, int dx, int dy);
