#include <config.h>
#include <command.h>
#include <watchdog.h>

#include "mvTypes.h"
#include "menu.h"
#include "hwd_config.h"
#include "mvUartLog.h"

#include "private_diag_if.h"

/*extern int eth_rx(void);*/
extern char console_buffer[CFG_CBSIZE];        /* console I/O buffer    */
char erase_seq[] = "\b \b";                    /* erase sequence    */
char tab_seq[] = "        ";                /* used to expand TABs    */


void display_menu(int parent);
int cmd_menu(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

extern volatile MV_U32 *mvUTMemAlloc_PTR;


char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
    char *s;

    if (*np == 0)
    {
        return (p);
    }

    if (*(--p) == '\t')
    {            /* will retype the whole line */
        while (*colp > plen)
        {
            puts (erase_seq);
            (*colp)--;
        }
        for (s=buffer; s<p; ++s)
        {
            if (*s == '\t')
            {
                puts (tab_seq+((*colp) & 07));
                *colp += 8 - ((*colp) & 07);
            } else
            {
                ++(*colp);
                putc (*s);
            }
        }
    } else
    {
        puts (erase_seq);
        (*colp)--;
    }
    (*np)--;
    return (p);
}

/****************************************************************************/

/*
 * Prompt for input and read a line.
 * If  CONFIG_BOOT_RETRY_TIME is defined and retry_time >= 0,
 * time out when time goes past endtime (timebase time in ticks).
 * Return:    number of read characters
 *        -1 if break
 *        -2 if timed out
 */
int readline_ex (const char *const prompt)
{
    char   *p = console_buffer;
    int    n = 0;                /* buffer index        */
    int    plen = 0;            /* prompt length    */
    int    col;                /* output column cnt    */
    char    c;

    /* print prompt */
    if (prompt)
    {
        plen = strlen (prompt);
        puts (prompt);
    }
    col = plen;

    for (;;)
    {

#ifdef CONFIG_SHOW_ACTIVITY
        while (!tstc())
        {
            extern void show_activity(int arg);
            show_activity(0);
        }
#endif
        c = getc();
        
        /*
         * Special character handling
         */
        switch (c)
        {
            case '\r':                /* Enter        */
            case '\n':
                *p = '\0';
                puts ("\r\n\n");
                return (p - console_buffer);

            case '\0':                /* nul            */
                continue;

            case 0x03:                /* ^C - break        */
            case 0x1B:                /* ESC - break        */
                console_buffer[0] = '\0';    /* discard input */
                return (-1);

            case 0x15:                /* ^U - erase line    */
                while (col > plen)
                {
                    puts (erase_seq);
                    --col;
                }
                p = console_buffer;
                n = 0;
                continue;

            case 0x17:                /* ^W - erase word     */
                p=delete_char(console_buffer, p, &col, &n, plen);
                while ((n > 0) && (*p != ' '))
                {
                    p=delete_char(console_buffer, p, &col, &n, plen);
                }
                continue;

            case 0x08:                /* ^H  - backspace    */
            case 0x7F:                /* DEL - backspace    */
                p=delete_char(console_buffer, p, &col, &n, plen);
                continue;

            default:
                /*
                 * Must be a normal character then
                 */
                if (n < CFG_CBSIZE-2)
                {
                    ++col;        /* echo input        */
                    putc (c);
                    *p++ = c;
                    ++n;
                } else
                {            /* Buffer full        */
                    putc ('\a');
                }
        }
    }
}

void display_menu(int parent)
{
    /* init product diagnostic parameters and HW */
    diagIfInit();

    for (;;)
    {
        int i;
        int nItem = 1;
        int len;
        MENUITEM item;

        for (i=0; i<ARRAY_SIZE(HWD_MENU); ++i)
        {
            item = HWD_MENU[i];
            if (item.parentid == parent)
            {
                printf("[%d]\t%s\n", nItem, item.name);
                ++nItem;
            }
        }
        if (1==nItem) return;
        if (parent > 0)
        {
            printf("[ESC]\tReturn to previous menu\n\n");
        } else
        {
            printf("[ESC]\tExit diagnostic\n\n");
        }
        len = readline_ex(DIAG_PROMPT);
        if (len > 0)
        {
            int nCommand = simple_strtoul(console_buffer, NULL, 10);
            int nItem = 1;
            MV_STATUS status;

            for (i=0; i<ARRAY_SIZE(HWD_MENU); ++i)
            {
                item = HWD_MENU[i];
                if (item.parentid == parent)
                {
                    if (nItem == nCommand)
                    {
                        if ((NULL != item.cmd)&&(0x100 != item.cmd))
                        {
                            disable_interrupts();
                        #if (CONFIG_COMMANDS & CFG_CMD_CACHE)
                            icache_enable();
                            /* dchache_disable();*/
                        #endif
                            WATCHDOG_RESET();
                            status = item.cmd();
                        #if (CONFIG_COMMANDS & CFG_CMD_CACHE)
                            icache_enable();
                            /* dchache_enable();*/
                        #endif
                            enable_interrupts();

                            if (status != MV_OK)
                                status = MV_FAIL;
                            printf("[%d](%d)-%s\n\n", nItem, status, ((status!=MV_OK) ? "FAILED" : "PASSED"));
                            break;
                        } 
                        else
                        {
                            display_menu(item.id);
                            break;
                        }                        
                    } else
                    {
                        ++nItem;
                    }
                }
            }
        } else
        {
            if (-1==len)
            {
                printf("\n");
                return;
            }
        }
    }
}

int cmd_menu(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    /* hwIfPrintBoardInfo(); */
    display_menu(-1);
    
    /* Free the allocated memory before exit. */
    mvBspServicesFreeMem((MV_U8 *)mvUTMemAlloc_PTR);

    /* Un-Register the Uart Logger function. */    
    mvUartLogUnreg(0);
    
    return 1;
}

U_BOOT_CMD(mv_diag, 1, 1, cmd_menu, "mv_diag - Marvell Hardware diagnostic\n", 
       "\tMarvell Hardware diagnostic menu\n");

