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

#include <stdlib.h>
#include <string.h>
#include "2048game.h"


/* Initialise the board for a new game. Border tiles round the edge,
 * empty spaces in the middle, and score = 0. */
void gb_init(GAMEBOARD *g)
{
    int r, c;

    for (r = 0; r < 6; r++)
    {
        for (c = 0; c < 6; c++)
        {
            if (r == 0 || c == 0 || r == 5 || c == 5)
                g->tile[r][c] = BORDER;
            else
                g->tile[r][c] = 0;
        }
    }
    g->score = 0L;

}


/* Create a new tile. Uses brute force: Picks cells at random until it finds
 * an empty one, and drops the new tile into it. Will hang if the board is
 * full. */
void gb_newtile(GAMEBOARD *g)
{
    while (1)
    {
        int r = 1 + (rand() % 4);
        int c = 1 + (rand() % 4);

        if (g->tile[r][c] == 0)
        {
            g->tile[r][c] = (rand() % 4) ? 2 : 4;

            return;
        }
    }
}

/* Attempt to move a tile to the left or right, until it hits either
 * the border or another tile. */
static void gb_movelr(GAMEBOARD *g, int row, int dx)
{
    int col, c2;

    for (col = (dx < 0) ? 1 : 4;
         g->tile[row][col] != BORDER;
         col -= dx)
    {
        c2 = col;
        /* No tile to move. */
        if (g->tile[row][c2] == 0) continue;
        /* Slide tile as far as it will go */
        while (g->tile[row][c2 + dx] == 0)
        {
            g->tile[row][c2 + dx] = g->tile[row][c2];
            g->tile[row][c2] = 0;
            c2 += dx;
        }
        /* Has it hit another of the same type? */
        if (g->tile[row][c2 + dx] == g->tile[row][c2])
        {
            g->score += 2 * g->tile[row][c2];
            g->tile[row][c2 + dx] *= 2;
            /* Set the 'newly merged' flag. Any further tile that
             * hits this one will not have the merged flag, and so
             * will not merge with it. */
            g->tile[row][c2 + dx] |= MERGED;
            g->tile[row][c2] = 0;
        }
    }
}



/* As gb_movelr, for vertical movement */
static void gb_moveud(GAMEBOARD *g, int col, int dy)
{
    int row, r2;

    for (row = (dy < 0) ? 1 : 4;
         g->tile[row][col] != BORDER;
         row -= dy)
    {
        r2 = row;
        if (g->tile[r2][col] == 0) continue;
        /* Slide tile as far as it will go */
        while (g->tile[r2 + dy][col] == 0)
        {
            g->tile[r2 + dy][col] = g->tile[r2][col];
            g->tile[r2][col] = 0;
            r2 += dy;
        }
        /* Has it hit another of the same type? */
        if (g->tile[r2 + dy][col] == g->tile[r2][col])
        {
            g->score += 2 * g->tile[r2][col];
            g->tile[r2 + dy][col] *= 2;
            /* Set the 'newly merged' flag so that nothing can be
             * merged with this tile. */
            g->tile[r2 + dy][col] |= MERGED;
            g->tile[r2][col] = 0;
        }
    }
}

/* Attempt a move */
int gb_move(GAMEBOARD *g, int dx, int dy)
{
    int row, col;
    GAMEBOARD g2;

    gb_clearmerge(g);	/* Clear any 'new tile' flags from previous move */

    /* Remember state of game before the move */
    g2 = *g;

    if (dy == 0)
    {
        /* Move each row horizontally */
        for (row = 1; row <= 4; row++)
        {
            gb_movelr(g, row, dx);
        }

    }
    else
    {
        /* Move each row vertically */
        for (row = 1; row <= 4; row++)
        {
            gb_moveud(g, row, dy);
        }

    }

    /* If the move did not change game state, it is ignored */
    if (!memcmp(&g2, g, sizeof(g2)))
    {
        return 0;	/* No tile moved */
    }

    /* Check for any 2048 tiles (victory condition) */
    for (row = 1; row <= 4; row++)
    {
        for (col = 1; col <= 4; col++)
        {
            if ((g->tile[row][col] & ~MERGED) == 2048)
            {
                return 2;
            }
        }
    }
    return 1;
}

/* Remove the 'merged' flags from all tiles */
void gb_clearmerge(GAMEBOARD *g)
{
    int row, col;

    for (row = 1; row <= 4; row++)
    {
        for (col = 1; col <= 4; col++)
        {
            g->tile[row][col] &= ~MERGED;
        }
    }
}


/* Check if a move is possible. */
int gb_canmove(GAMEBOARD *g)
{
    GAMEBOARD g2;

    /*
     * Brute-force approach: Try all four of the user's possible moves and see if
     * any of them changed the board state. If gb_move() returned zero the board
     * was unchanged, so we don't need to restore it before each attempt.
     */

    g2 = *g;
    gb_clearmerge(&g2);

    if (gb_move(&g2, -1,  0) ||
            gb_move(&g2,  1,  0) ||
            gb_move(&g2,  0, -1) ||
            gb_move(&g2,  0,  1) )
    {
        return 1;
    }
    return 0;
}

