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

/* Derived from the 'Hello World' sample application / accessory, for
 * which the copyright is below */

/************************************************************************/
/*     File:     g2048.c                                   */
/************************************************************************/
/*                                             */
/*               GGGGG        EEEEEEEE     MM      MM          */
/*             GG             EE           MMMM  MMMM          */
/*             GG   GGG       EEEEE        MM  MM  MM          */
/*             GG   GG        EE           MM      MM          */
/*               GGGGG        EEEEEEEE     MM      MM          */
/*                                             */
/************************************************************************/
/*                                             */
/*                 +--------------------------+               */
/*                 | Digital Research, Inc.   |               */
/*                 | 60 Garden Court          |               */
/*                 | Monterey, CA.    93940   |               */
/*                 +--------------------------+               */
/*                                             */
/*   The  source code  contained in  this listing is a non-copyrighted     */
/*   work which  can be  freely used.  In applications of  this source     */
/*   code you  are requested to  acknowledge Digital Research, Inc. as     */
/*   the originator of this code.                         */
/*                                             */
/*   Author:     Tom Rolander                              */
/*   PRODUCT:     GEM Sample Desk Top Accessory                    */
/*   Module:     HELLO                                   */
/*   Version:     February 15, 1985                         */
/*                                             */
/************************************************************************/


#include <gem.h>

#include <string.h>         /* memcpy and memcmp */
#include <stdio.h>          /* sprintf */
#include <stdlib.h>         /* rand() */
#include <time.h>           /* time() */

#include "2048.h"
#include "2048game.h"

#include "portab.h"


#define WDW_STYLE NAME | INFO | CLOSER | MOVER


static WORD gl_wchar;               /* character width          */
static WORD gl_hchar;               /* character height          */
static WORD gl_wbox;               /* box (cell) width          */
static WORD gl_hbox;               /* box (cell) height          */
static WORD gem_handle;               /* GEM vdi handle          */
static WORD     vdi_handle;               /* g2048 vdi handle          */
static WORD     work_out[57];               /* open virt workstation values     */
static GRECT     work_area;               /* current window work area     */
static WORD     gl_rmsg[8];               /* message buffer          */

static WORD     gl_itemg2048 = 0;          /* g2048 menu item          */
static WORD     gl_xfull;               /* full window 'x'          */
static WORD     gl_yfull;               /* full window 'y'          */
static WORD     gl_wfull;               /* full window 'w' width     */
static WORD     gl_hfull;               /* full window 'h' height     */
static WORD     ev_which;               /* event message returned value     */
WORD     g2048_whndl = 0;          /* g2048 window handle          */
WORD     type_size;               /* system font cell size     */
#define     MESS_NLINES     20          /* maximum lines in message     */
#define     MESS_WIDTH     20          /* maximum width of message     */
BYTE     *wdw_title =     " 2048 ";
BYTE     status[32];     /* Score: line */
BYTE     scoret[80];     /* sprintf() template for "Score:" line */
// X_BUF_V2 gl_xbuf;     /* AES abilities from appl_init() */
WORD     st_textc[16];     /* Text 'contrast' colours */

GAMEBOARD gl_board, gl_oldboard;     /* Game current and previous state */
long     gl_hiscore;               /* High score (FreeGEM only) */

/*

Page*/
/************************************************************************/
/************************************************************************/
/****                                             ****/
/****                   Local Procedures                    ****/
/****                                             ****/
/************************************************************************/
/************************************************************************/


#define max(a, b) (a > b) ? a : b
#define min(a, b) (a < b) ? a : b


/* Simple wrapper for form_alert() with message box defined in RSC file */
WORD rsrc_alert(WORD rsrc, WORD defbtn)
{
    BYTE *str;

    rsrc_gaddr(R_STRING, rsrc, &str);

    return form_alert(defbtn, str);
}

/* Update the score line */
void update_score(void)
{
    if (!g2048_whndl)
    {
        return;     /* Window not open */
    }

#ifdef NOT_USED
    /* If the AES allows us to maintain a high score, show it */
    if (gl_xbuf.abilities & ABLE_PROP)
    {
        sprintf(status, scoret, gl_board.score, gl_hiscore);
    }
    else     /* Otherwise just show the score */
    {
        sprintf(status, scoret, gl_board.score);
    }
#endif
    wind_set_str(g2048_whndl, WF_INFO, status);

}


/* Start a new game. Clear the board and the 'previous turn' board, and
 * drop two new tiles onto the board. */
void new_game(void)
{
    gb_init(&gl_board);
    gb_init(&gl_oldboard);

    gb_newtile(&gl_board);
    gb_newtile(&gl_board);
}

/* Compute the union of two rectangles */
void rc_union(GRECT *p1, GRECT *p2)
{
    WORD            tx, ty, tw, th;

    tw = max(p1->g_x + p1->g_w, p2->g_x + p2->g_w);
    th = max(p1->g_y + p1->g_h, p2->g_y + p2->g_h);
    tx = min(p1->g_x, p2->g_x);
    ty = min(p1->g_y, p2->g_y);
    p2->g_x = tx;
    p2->g_y = ty;
    p2->g_w = tw - tx;
    p2->g_h = th - ty;
}



/*------------------------------*/
/*     do_open               */
/*------------------------------*/
WORD do_open(WORD wh, WORD org_x, WORD org_y, WORD x, WORD y, WORD w, WORD h)     /* grow and open specified wdw     */
{
    WORD     ret_code;

    graf_mouse(2,0x0L);
    graf_growbox(org_x, org_y, 21, 21, x, y, w, h);
    ret_code = wind_open(wh, x, y, w, h);
    graf_mouse(ARROW,0x0L);

    return(ret_code);
}


/*------------------------------*/
/*     do_close          */
/*------------------------------*/
void do_close(WORD wh,  WORD org_x, WORD org_y)     /* close and shrink specified window     */
{
    WORD     x, y, w, h;

    graf_mouse(2,0x0L);
    wind_get(wh, WF_WORKXYWH, &x, &y, &w, &h);
    wind_close(wh);
    graf_shrinkbox(org_x, org_y, 21, 21, x, y, w, h);
    graf_mouse(ARROW,0x0L);
}

/*------------------------------*/
/*     set_clip          */
/*------------------------------*/
void set_clip(WORD clip_flag, GRECT *s_area)     /* set clip to specified area          */
{
    WORD     pxy[4];

    grect_to_array(s_area, pxy);
    vs_clip(vdi_handle, clip_flag, pxy);
}

/*------------------------------*/
/*     align_x               */
/*------------------------------*/
WORD align_x(WORD x)          /* forces word alignment for column positon,     */
{
    return((x & 0xfff0) + ((x & 0x000c) ? 0x0010 : 0));
}

/*------------------------------*/
/*     wdw_size          */
/*------------------------------*/
void wdw_size(GRECT *box, WORD w, WORD h)     /* compute window size for given w * h chars     */
{
    WORD     pw, ph;

    vst_height(vdi_handle, type_size, &gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);

    pw = w * gl_wbox + 1;
    ph = h * gl_hbox + 1;
    wind_calc(WC_BORDER, WDW_STYLE, gl_wfull / 2 - pw / 2, gl_hfull / 2 - ph / 2, pw, ph, &box->g_x, &box->g_y, &box->g_w, &box->g_h);
    box->g_x = align_x(box->g_x) - 1;
}

/* Draw a tile. There are separate code paths for systems with 16 or more
 * colours (in which case all tiles are solid colour) and those with fewer
 * (tiles are patterned) */
void draw_tile(unsigned short tile, int x, int y)
{
    WORD     pxy[10];
    WORD      ink;
    WORD     linecol = G_BLACK;
    BYTE      caption[8];

    /* Draw the space at the edge of the tile */
    pxy[0] = x + 5 * gl_wbox - 1;
    pxy[1] = y;
    pxy[2] = pxy[0];
    pxy[3] = y + 5 * gl_hbox - 1;
    pxy[4] = x;
    pxy[5] = pxy[3];
    vsl_color(vdi_handle, G_WHITE);
    v_pline(vdi_handle, 3, pxy);

    /* Draw the tile border; if the tile has the MERGED flag, the border
     * will be red. */
    if (tile & MERGED)
    {
        linecol = G_RED;
        tile &= ~MERGED;
    }

    pxy[0] = x;
    pxy[1] = y;
    pxy[2] = x + 5 * gl_wbox - 2;
    pxy[3] = y + 5 * gl_hbox - 2;
    /* If the tile is an empty space, draw a 50% grey rectangle. */
    if (tile == 0)
    {
        vsf_color(vdi_handle, G_BLACK);
        vsf_interior(vdi_handle, 2);
        vsf_style(vdi_handle, 4);
        vr_recfl(vdi_handle, pxy);
        return;
    }
    sprintf(caption, "%d", tile);

    /* Compute the colour of the tile. The 2048 tile is red, and
     * lower-numbered tiles will be green, blue, etc. If there are
     * fewer than 16 colours, some or all tiles will appear in black. */
    ink = G_RED;
    while (tile < 2048)
    {
        ink++;
        tile *= 2;
    }
    vsf_color(vdi_handle, ink);
    if (work_out[13] < 16)
    {
        /* If there are fewer than 16 colours, select a pattern
         * as well as a colour for the tile. */
        vsf_interior(vdi_handle, 2);
        vsf_style(vdi_handle, ink - G_RED + 8);
    }
    else
    {
        vsf_interior(vdi_handle, 1);
    }
    /* Fill the tile */
    vr_recfl(vdi_handle, pxy);
    pxy[0] = x + ((5 - strlen(caption)) * gl_wbox) / 2 - 1;
    pxy[1] = y + 3 * gl_hbox - 1;

    /* Draw its value */
    if (work_out[13] < 16)
    {
        v_gtext(vdi_handle, pxy[0], pxy[1], caption);
    }
    else
    {
        vswr_mode(vdi_handle, MD_TRANS);
        vst_color(vdi_handle, st_textc[ink]);
        v_gtext(vdi_handle, pxy[0], pxy[1], caption);
        vswr_mode(vdi_handle, MD_REPLACE);
    }

    /* Draw its border */
    pxy[0] = x;
    pxy[1] = y;
    pxy[2] = x;
    pxy[3] = y + 5 * gl_hbox - 2;
    pxy[4] = x + 5 * gl_wbox - 2;
    pxy[5] = pxy[3];
    pxy[6] = pxy[4];
    pxy[7] = y;
    pxy[8] = x;
    pxy[9] = y;
    vsl_color(vdi_handle, linecol);
    v_pline(vdi_handle, 5, pxy);
}


/*------------------------------*/
/*     disp_message          */
/*------------------------------*/
void disp_board(GRECT *clip_area) /* display an area of the board     */
{
    WORD     pxy[4];
    WORD     xcurr, ycurr;
    int     row, col;

    set_clip(TRUE, clip_area);
    vsf_interior(vdi_handle, 1);
    vsf_color(vdi_handle, G_WHITE);
    grect_to_array(&work_area, pxy);
    graf_mouse(M_OFF, 0x0L);

    vsl_color(vdi_handle, G_BLACK);
    vswr_mode(vdi_handle, MD_REPLACE);
    vsl_type (vdi_handle, FIS_SOLID);
    vswr_mode(vdi_handle, 1);
    ycurr = work_area.g_y;

    for (row = 0; row < 4; row++)
    {
        xcurr = work_area.g_x;
        for (col = 0; col < 4; col++)
        {
            draw_tile(gl_board.tile[row+1][col+1],
                    xcurr, ycurr);
            xcurr += 5 * gl_wbox;
        }
        ycurr += 5 * gl_hbox;
    }

    graf_mouse(M_ON, 0x0L);
    set_clip(FALSE, clip_area);
}



/*------------------------------*/
/*     do_redraw          */
/*------------------------------*/
void do_redraw(WORD wh, GRECT *area)          /* redraw message applying area clip     */
{
    GRECT     box;

    graf_mouse(M_OFF, 0x0L);
    if (gl_board.score != gl_oldboard.score)
    {
        if (gl_board.score > gl_hiscore)
        {
            gl_hiscore = gl_board.score;
        }
        update_score();
        gl_oldboard.score = gl_board.score;
    }
    wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
    while ( box.g_w && box.g_h )
    {
        if (rc_intersect(area, &box))
        {
            if (wh == g2048_whndl)
            {
                disp_board(&box);
            }
        }
        wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
    }
    graf_mouse(M_ON, 0x0L);
}



/*

Page*/
/************************************************************************/
/************************************************************************/
/****                                             ****/
/****                   Message Handling                    ****/
/****                                             ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*     hndl_mesag          */
/*------------------------------*/
BOOLEAN     hndl_mesag()
{
    GRECT     box;
    BOOLEAN     done;
    WORD     wdw_hndl;

    done = FALSE;
    wdw_hndl = gl_rmsg[3];               /* wdw handle of mesag     */
    switch( gl_rmsg[0] )               /* switch on type of msg*/
    {
    case AC_OPEN:                    /* do accessory open     */
        if ( (gl_rmsg[4] == gl_itemg2048) &&
             (!g2048_whndl) )          /* unless already open     */
        {
            graf_mouse(BUSY_BEE, 0x0L);
            g2048_whndl = wind_create(WDW_STYLE, align_x(gl_xfull)-1, gl_yfull, gl_wfull, gl_hfull);
            if (g2048_whndl == -1)
            {
                graf_mouse(ARROW, 0x0L);
                rsrc_alert(NOWINDOW, 1);
                g2048_whndl = 0;
                return(TRUE);
            }
            new_game();
            update_score();
            if (g2048_whndl)
            {
                wind_set_str(g2048_whndl, WF_NAME, wdw_title);
                wind_set_str(g2048_whndl, WF_INFO, status);
            }
            wdw_size(&box, MESS_WIDTH, MESS_NLINES);
#if     DESKACC                         /* open from menu area     */
            do_open(g2048_whndl, gl_wbox*4, gl_hbox/2, box.g_x, box.g_y, box.g_w, box.g_h);
#else                              /* open from screen cntr*/
            do_open(g2048_whndl, gl_wfull/2, gl_hfull/2, box.g_x, box.g_y, box.g_w, box.g_h);
#endif
            wind_get(g2048_whndl, WF_WORKXYWH,     &work_area.g_x, &work_area.g_y, &work_area.g_w, &work_area.g_h);
            disp_board(&work_area);
            graf_mouse(ARROW,0x0L);
        }
        else
        {
            graf_mouse(ARROW, 0x0L);
            wind_set(g2048_whndl, WF_TOP, 0, 0, 0, 0);
        }
        break;

    case AC_CLOSE:                    /* do accessory close     */
        if ( (gl_rmsg[3] == gl_itemg2048) &&
             (g2048_whndl) )
        {
            g2048_whndl = 0;     /* reset window handle     */
        }
        break;

    case WM_REDRAW:                    /* do redraw wdw contnts*/
        do_redraw(wdw_hndl, (GRECT *) &gl_rmsg[4]);
        break;

    case WM_TOPPED:                    /* do window topped     */
        wind_set(wdw_hndl, WF_TOP, 0, 0, 0, 0);
        break;

    case WM_CLOSED:                    /* do window closed     */
#if     DESKACC                         /* close to menu bar     */
        do_close(g2048_whndl, gl_wbox*4, gl_hbox/2);
#else                              /* close to screen cntr     */
        do_close(g2048_whndl, gl_wfull/2, gl_hfull/2);
#endif
        wind_delete(g2048_whndl);
        g2048_whndl = 0;
        done = TRUE;
        break;

    case WM_MOVED:                    /* do window move     */
        wind_set(wdw_hndl, WF_CURRXYWH, align_x(gl_rmsg[4])-1, gl_rmsg[5], gl_rmsg[6], gl_rmsg[7]);
        wind_get(g2048_whndl, WF_WORKXYWH,     &work_area.g_x, &work_area.g_y, &work_area.g_w, &work_area.g_h);
        break;

    default:
        break;
    } /* switch */
    return(done);
} /* hndl_mesag */


void send_redraw(WORD wh, GRECT *p)
{
    WORD    msg[8];

    msg[0] = WM_REDRAW;             /* Defined in GEMBIND.H     */
    msg[1] = gl_apid;               /* As returned by appl_init */
    msg[2] = 0;
    msg[3] = wh;                    /* Handle of window to redraw */
    msg[4] = p->g_x;
    msg[5] = p->g_y;
    msg[6] = p->g_w;
    msg[7] = p->g_h;
    appl_write(gl_apid, 16, msg);     /* Use ADDR(msg) for portability */
}


/* Handle keyboard input. Up/down/left/right cause moves; Ctrl-Q quits.  */
BOOLEAN hndl_keybd(UWORD keycode)
{
    short result, row, col;
    GRECT rc, rct;
    int redraw = 0;

    switch (keycode)
    {
    case 0x4B00: case 'h': case 'H':
        result = gb_move(&gl_board, -1, 0);
        break;
    case 0x4D00: case 'l': case 'L':
        result = gb_move(&gl_board, 1, 0);
        break;
    case 0x4800: case 'k': case 'K':
        result = gb_move(&gl_board, 0, -1);
        break;
    case 0x5000: case 'j': case 'J':
        result = gb_move(&gl_board, 0, 1);
        break;
    case 0x1011:     // Ctrl-Q to quit
        gl_rmsg[0] = WM_CLOSED;
        return hndl_mesag();
    }
    /* In case of a victory, redraw the board to show it and then
     * pop up the victory message. */
    if (result == 2)
    {
        disp_board(&work_area);
        rsrc_alert(VICTORY, 1);
        gl_rmsg[0] = WM_CLOSED;
        return hndl_mesag();
    }
    /* Otherwise, if the move was successful add a new tile to the
     * board. */
    if (result == 1)
    {
        gb_newtile(&gl_board);
    }
    /* Invalidate updated area(s) of the board */
    for (row = 0; row < 4; row++)
    {
        for (col = 0; col < 4; col++)
        {
            /* If a tile has not been updated, add its rectangle to the update
 * rectangle. */
            if (gl_board.tile[row+1][col+1] !=
                    gl_oldboard.tile[row+1][col+1])
            {
                rct.g_x = work_area.g_x + row * 5 * gl_wbox;
                rct.g_y = work_area.g_y + col * 5 * gl_hbox;
                rct.g_w = 5 * gl_wbox - 2;
                rct.g_h = 5 * gl_hbox - 2;
                if (redraw) rc_union(&rct, &rc);
                else         rc = rct;
                gl_oldboard.tile[row+1][col+1] =
                        gl_board.tile[row+1][col+1];
                redraw = 1;
            }
        }
    }
    if (redraw)
    {
        /* This should be faster but I haven't got it working
         * properly: Only redraw updated areas *
        send_redraw(g2048_whndl, &rc);  */
        send_redraw(g2048_whndl, &work_area);
    }

    return FALSE;
}


/*------------------------------*/
/*     g2048               */
/*------------------------------*/
void g2048(void)
{
    BOOLEAN     done;
    WORD mx, my, mb, ks, kr, br;

    /**/                         /* loop handling user     */
    /**/                         /*   input until done     */
    done = FALSE;                    /*   -or- if DESKACC     */



    while( !done )                    /*   then forever     */
    {
        /* If the window is open and there are no more moves
         * possible, then tell the user they have lost and
         * close the window. */
        if (g2048_whndl != 0 && !gb_canmove(&gl_board))
        {
            disp_board(&work_area);
            rsrc_alert(DEFEAT, 1);
            gl_rmsg[0] = WM_CLOSED;
            ev_which = MU_MESAG;
        }
        else     /* Otherwise, get the user's input */
        {
            ev_which = evnt_multi(MU_MESAG | MU_KEYBD,
                                  0, 0, 0,     /* Mouse button flags */
                                  0, 0, 0, 0, 0,      /* Mouse rectangle 1 flags */
                                  0, 0, 0, 0, 0,      /* Mouse rectangle 2 flags */
                                  gl_rmsg,     /* Message buffer */
                                  0,               /* timer */
                                  &mx, &my, &mb, &ks, &kr, &br);
        }
        wind_update(BEG_UPDATE);     /* begin window update     */
        if (ev_which & MU_MESAG)
        {
            done = hndl_mesag();     /* handle event message     */
        }
        if (ev_which & MU_KEYBD)
        {
            done = hndl_keybd(kr);     /* handle keypress */
        }
        wind_update(END_UPDATE);     /* end window update     */
#if     DESKACC
        done = FALSE;     /* never exit loop for desk accessory     */
#endif
    }
}


/*

Page*/
/************************************************************************/
/************************************************************************/
/****                                             ****/
/****                   Termination                         ****/
/****                                             ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/* g2048_term                   */
/*------------------------------*/
void g2048_term()
{
#if     DESKACC
    return(FALSE);              /* Desk Accessory never ends     */
#else
    v_clsvwk( vdi_handle );     /* close virtual work station     */
    appl_exit();                /* application exit          */
#endif
}

/*------------------------------*/
/* g2048_init                   */
/*------------------------------*/
WORD g2048_init()
{
    WORD i;
    WORD work_in[11];
    WORD attributes[10];
    WORD rgb[3];
    BYTE buf[20];
    BYTE *sc;
    time_t t;

    /* Initialise PRNG */
    time(&t);
    srand(t);

    gl_apid = appl_init();          /* initialize libraries     */

#if     DESKACC
    wind_update(BEG_UPDATE);
#endif

    for (i = 0; i < 10; i++)
    {
        work_in[i] = 1;
    }
    work_in[10] = 2;
    gem_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
    vdi_handle = gem_handle;
    v_opnvwk(work_in, &vdi_handle, work_out);     /* open virtual work stn*/

    /* Load resources */
    if (!rsrc_load("2048.rsc"))
    {
        form_alert(1, "[3][Fatal Error|2048.RSC not found][ Abort ]");
#ifdef DESKACC     /* DAs can't terminate. Instead, just don't register our */
        /* menu option, so the drawing code will never be called. */
        type_size = 0;
#else
        return FALSE;
#endif

    }
    vqt_attributes(vdi_handle, attributes);
    type_size = attributes[7];          /* get system font hbox     */

    /* use the "Score: %ld" template */
    rsrc_gaddr(R_STRING, SCOREL, &sc);

    /* Copy the template to near memory so sprintf() can use it. */
    for (i = 0; sc[i]; i++)
    {
        scoret[i] = sc[i];
    }
    scoret[i] = 0;

    /* Calculate text contrast colours for each colour in the palette.
     * Do it by getting R/G/B values for each colour; if any is above
     * 500, use black, else white. */
    for (i = 0; i < 16; i++)
    {
        vq_color(vdi_handle, i, 1, rgb);
        if (rgb[0] > 500 || rgb[1] > 500 || rgb[2] > 500)
            st_textc[i] = G_BLACK;
        else     st_textc[i] = G_WHITE;
    }

#if     DESKACC          /* enter g2048 in menu     */
    if (type_size)     /* (only if initialisation succeeded) */
        gl_itemg2048 = menu_register(gl_apid, ADDR("  2048") );
#else
    if (vdi_handle == 0)
        return(FALSE);
#endif
    /* init. message address*/
    wind_get(DESK, WF_WORKXYWH, &gl_xfull, &gl_yfull, &gl_wfull, &gl_hfull);
    return(TRUE);
}

int main(int argc, char *argv[])
{
    if (g2048_init())               /* initialization     */
    {
#if     DESKACC
        wind_update(END_UPDATE);
        g2048();
#else                              /* simulate AC_OPEN     */
        gl_rmsg[0] = AC_OPEN;
        gl_rmsg[4] = gl_itemg2048;
        hndl_mesag();

        g2048();

        g2048_term();               /* termination          */
#endif
    }
    return 0;
}

