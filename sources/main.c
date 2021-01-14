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


/* A quick main() for GEM apps */

#include <gem.h>
#include "portab.h"

static char *fail = "GEMAES not present in memory.\n";
/*static char *meme = "Unable to free memory.\n";*/
static char *exte = "Execution terminated.\n";

int freemem(void);


int main(int argc, char **argv)
{
	return GEMAIN(argc, argv);
}

