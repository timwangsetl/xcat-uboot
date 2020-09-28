/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 *
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <zlib.h>
#include <asm/byteorder.h>
#ifdef CONFIG_HAS_DATAFLASH
#include <dataflash.h>
#endif

/*cmd_boot.c*/
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern void mvEgigaStrToMac( char *source , char *dest );

typedef void (*LINUX_KERNEL_PTR)(int zero, int arch, uint params);

void boot_linux_kernel(LINUX_KERNEL_PTR theKernel, ulong bi_arch_number, 
                       ulong bi_boot_params, int kernel_is_big_endian);

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_LCD) ||	\
    defined (CONFIG_MARVELL_TAG)


static void setup_start_tag (bd_t *bd);

# ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd);
# endif
static void setup_commandline_tag (bd_t *bd, char *commandline);

# ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start,
			      ulong initrd_end);
# endif
static void setup_end_tag (bd_t *bd);

# if defined (CONFIG_VFD) || defined (CONFIG_LCD)
static void setup_videolfb_tag (gd_t *gd);
# endif

#if defined (CONFIG_MARVELL_TAG)
static void setup_marvell_tag(void);
#endif

static struct tag *params;
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

#ifdef CONFIG_SHOW_BOOT_PROGRESS
# include <status_led.h>
# define SHOW_BOOT_PROGRESS(arg)	show_boot_progress(arg)
#else
# define SHOW_BOOT_PROGRESS(arg)
#endif

extern image_header_t header;	/* from cmd_bootm.c */

/* indicator showing that uboot is booting Linux with other endianness */
static int s_changeEndian = 0;

#define mv_tag_next(t) _mv_tag_next(t, __FUNCTION__)
#define mv_swab32(t) t = swab32(t)
#define mv_swab16(t) t = swab16(t)

static struct tag *_mv_tag_next(struct tag *params, const unsigned char *func_name )
{
  //
  // perform the necessary swab32 and advance the params.
  //
  struct tag *next = ((struct tag *)((u32 *)(params) + (params)->hdr.size));
	int i;

  if (s_changeEndian)
  {
    //
    // perform the necessary swaps
    //
    if (strcmp(func_name, "setup_start_tag") == 0)
    {
      mv_swab32(params->hdr.tag);
      mv_swab32(params->hdr.size);
      mv_swab32(params->u.core.flags);
      mv_swab32(params->u.core.pagesize);
      mv_swab32(params->u.core.rootdev);
    }
    
    else if (strcmp(func_name, "setup_memory_tags") == 0)
    {
      mv_swab32(params->hdr.tag);
      mv_swab32(params->hdr.size);
      mv_swab32(params->u.mem.start);
      mv_swab32(params->u.mem.size);
    }

    else if (strcmp(func_name, "setup_commandline_tag") == 0)
    {
      mv_swab32(params->hdr.tag);
      mv_swab32(params->hdr.size);
    }
    
    else if (strcmp(func_name, "setup_marvell_tag") == 0)
    {
      mv_swab32(params->hdr.tag);
      mv_swab32(params->hdr.size);
      mv_swab32(params->u.mv_uboot.uboot_version);
      mv_swab32(params->u.mv_uboot.tclk);
      mv_swab32(params->u.mv_uboot.sysclk);
      mv_swab32(params->u.mv_uboot.fw_image_base);
      mv_swab32(params->u.mv_uboot.fw_image_size);

#if defined(MV_INCLUDE_USB)
      mv_swab32(params->u.mv_uboot.isUsbHost);
#endif

#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH)
			for (i = 0; i < 4;i++)
			{
				printf("Before swap: %d   ",params->u.mv_uboot.mtu[i]);
				mv_swab16(params->u.mv_uboot.mtu[i]);
				printf("after swap: %d  \n",params->u.mv_uboot.mtu[i]);
			}
#endif
    }
  }

  return next;
}

void do_bootm_linux (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		     ulong addr, ulong *len_ptr, int verify)
{
	DECLARE_GLOBAL_DATA_PTR;

	ulong len = 0, checksum;
	ulong initrd_start, initrd_end;
	ulong data;
  LINUX_KERNEL_PTR theKernel;
	image_header_t *hdr = &header;
	bd_t *bd = gd->bd;
  int kernel_is_big_endian = 0;

#ifdef CONFIG_CMDLINE_TAG
	char *commandline = getenv ("bootargs");
#endif

	theKernel = (void (*)(int, int, uint))ntohl(hdr->ih_ep);
	debug(" theKernel %x\n", theKernel);

	/*
	 * Check if there is an initrd image
	 */
	if (argc >= 3) {
		SHOW_BOOT_PROGRESS (9);

		addr = simple_strtoul (argv[2], NULL, 16);

		printf ("## Loading Ramdisk Image at %08lx ...\n", addr);

		/* Copy header so we can blank CRC field for re-calculation */
#ifdef CONFIG_HAS_DATAFLASH
		if (addr_dataflash (addr)) {
			read_dataflash (addr, sizeof (image_header_t),
					(char *) &header);
		} else
#endif
			memcpy (&header, (char *) addr,
				sizeof (image_header_t));

		if (ntohl (hdr->ih_magic) != IH_MAGIC) {
			printf ("Bad Magic Number\n");
			SHOW_BOOT_PROGRESS (-10);
			do_reset (cmdtp, flag, argc, argv);
		}

		data = (ulong) & header;
		len = sizeof (image_header_t);

		checksum = ntohl (hdr->ih_hcrc);
		hdr->ih_hcrc = 0;

		if (crc32 (0, (const char *) data, len) != checksum) {
			printf ("Bad Header Checksum\n");
			SHOW_BOOT_PROGRESS (-11);
			do_reset (cmdtp, flag, argc, argv);
		}

		SHOW_BOOT_PROGRESS (10);

		print_image_hdr (hdr);

		data = addr + sizeof (image_header_t);
		len = ntohl (hdr->ih_size);

#ifdef CONFIG_HAS_DATAFLASH
		if (addr_dataflash (addr)) {
			read_dataflash (data, len, (char *) CFG_LOAD_ADDR);
			data = CFG_LOAD_ADDR;
		}
#endif

		if (verify) {
			ulong csum = 0;

			//printf ("   Verifying Checksum ... ");
			csum = crc32 (0, (const char *) data, len);
			if (csum != ntohl (hdr->ih_dcrc)) {
				printf ("Bad Data CRC\n");
				SHOW_BOOT_PROGRESS (-12);
				do_reset (cmdtp, flag, argc, argv);
			}
			//printf ("OK\n");
		}

		SHOW_BOOT_PROGRESS (11);

		if ((hdr->ih_os != IH_OS_LINUX) ||
		    (hdr->ih_arch != IH_CPU_ARM) ||
		    (hdr->ih_type != IH_TYPE_RAMDISK)) {
			printf ("No Linux ARM Ramdisk Image\n");
			SHOW_BOOT_PROGRESS (-13);
			do_reset (cmdtp, flag, argc, argv);
		}

#if defined(CONFIG_B2) || defined(CONFIG_EVB4510) || defined(CONFIG_ARMADILLO)
		/*
		 *we need to copy the ramdisk to SRAM to let Linux boot
		 */
		memmove ((void *) ntohl(hdr->ih_load), (uchar *)data, len);
		data = ntohl(hdr->ih_load);
#endif /* CONFIG_B2 || CONFIG_EVB4510 */

		/*
		 * Now check if we have a multifile image
		 */
	} else if ((hdr->ih_type == IH_TYPE_MULTI) && (len_ptr[1])) {
		ulong tail = ntohl (len_ptr[0]) % 4;
		int i;

		SHOW_BOOT_PROGRESS (13);

		/* skip kernel length and terminator */
		data = (ulong) (&len_ptr[2]);
		/* skip any additional image length fields */
		for (i = 1; len_ptr[i]; ++i)
			data += 4;
		/* add kernel length, and align */
		data += ntohl (len_ptr[0]);
		if (tail) {
			data += 4 - tail;
		}

		len = ntohl (len_ptr[1]);

	} else {
		/*
		 * no initrd image
		 */
		SHOW_BOOT_PROGRESS (14);

		len = data = 0;
	}

#ifdef	DEBUG
	if (!data) {
		printf ("No initrd\n");
	}
#endif

	if (data) {
		initrd_start = data;
		initrd_end = initrd_start + len;
	} else {
		initrd_start = 0;
		initrd_end = 0;
	}

	SHOW_BOOT_PROGRESS (15);

  //printf("\nChecking for Linux kernel endianess at 0x%08x = 0x%08x\n", 
  //       addr+0x40, (*(unsigned long *)(addr+0x40)));
#ifdef __BE
  kernel_is_big_endian = (*(unsigned long *)(addr+0x40) == 0xe1a00000);
  if (!kernel_is_big_endian)
#else
  kernel_is_big_endian = (*(unsigned long *)(addr+0x40) != 0xe1a00000);
  if (kernel_is_big_endian)
#endif
    s_changeEndian = 1;

//  printf("Linux Kernel is %s\n",(kernel_is_big_endian)?"BE":"LE"); 

	debug ("## Transferring control to Linux (at address %08lx) ...\n",
	       (ulong) theKernel);

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_LCD) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_MARVELL_TAG)
	setup_start_tag (bd);
#ifdef CONFIG_SERIAL_TAG
	setup_serial_tag (&params);
#endif
#ifdef CONFIG_REVISION_TAG
	setup_revision_tag (&params);
#endif
#ifdef CONFIG_SETUP_MEMORY_TAGS
	setup_memory_tags (bd);
#endif
#ifdef CONFIG_CMDLINE_TAG
	setup_commandline_tag (bd, commandline);
#endif
#ifdef CONFIG_INITRD_TAG
	if (initrd_start && initrd_end)
		setup_initrd_tag (bd, initrd_start, initrd_end);
#endif
#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
	setup_videolfb_tag ((gd_t *) gd);
#endif
#if defined (CONFIG_MARVELL_TAG)
        /* Linux open port doesn't support the Marvell TAG */
	char *env = getenv("mainlineLinux");
	if(!env || ((strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0)))
	    setup_marvell_tag ();
#endif
	setup_end_tag (bd);
#endif

	/* we assume that the kernel is in place */
//  printf ("\nStarting kernel in %s mode ...\n", (kernel_is_big_endian)?"BE":"LE");

#ifdef CONFIG_USB_DEVICE
	{
		extern void udc_disconnect (void);
		udc_disconnect ();
	}
#endif

	cleanup_before_linux ();

#ifdef __BE
  if (!kernel_is_big_endian)
    printf("Switching cpu to LE mode\n");
#else
  if (kernel_is_big_endian)
    printf("Switching cpu to BE mode\n");
#endif
  boot_linux_kernel(theKernel, bd->bi_arch_number, bd->bi_boot_params, 
                    s_changeEndian);
}


#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_LCD) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_MARVELL_TAG)
static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *) bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

  params = mv_tag_next (params);
}


#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd)
{
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = bd->bi_dram[i].start;
		params->u.mem.size = bd->bi_dram[i].size;

    params = mv_tag_next (params);
	}
}
#endif /* CONFIG_SETUP_MEMORY_TAGS */


static void setup_commandline_tag (bd_t *bd, char *commandline)
{
  char *pstart;
  char *p;
  char *q;
  int len = 0;
  char commandline1[2048];

	if (!commandline)
		return;


  q = commandline;

  if (s_changeEndian)
  {
    char *env = getenv("fix_nfsroot_to_BE");
    if (env && (strcmp(env, "yes") == 0))
    {
      printf("Fixing nfsroot name due to fix_nfsroot_to_BE=yes\n");
      pstart = p = commandline;
      q = commandline1;
      strcpy(q, p);
        
      p = strstr(p, "nfsroot=");
      if (p)
      {
        p = strchr(p, ' ');
        if (p)
          len = p - pstart;
        if (len)
        {
          q+=len;
          strcpy(q, "-be ");
          q+=3;
          strcat(q, p+1);
          q = commandline1;
        }
      }
    }
  }
       
  /* eat leading white space */
  for (p = q; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

  params = mv_tag_next (params);
}


#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

  params = mv_tag_next (params);
}
#endif /* CONFIG_INITRD_TAG */


#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
extern ulong calc_fbsize (void);
static void setup_videolfb_tag (gd_t *gd)
{
	/* An ATAG_VIDEOLFB node tells the kernel where and how large
	 * the framebuffer for video was allocated (among other things).
	 * Note that a _physical_ address is passed !
	 *
	 * We only use it to pass the address and size, the other entries
	 * in the tag_videolfb are not of interest.
	 */
	params->hdr.tag = ATAG_VIDEOLFB;
	params->hdr.size = tag_size (tag_videolfb);

	params->u.videolfb.lfb_base = (u32) gd->fb_base;
	/* Fb size is calculated according to parameters for our panel
	 */
	params->u.videolfb.lfb_size = calc_fbsize();

  params = mv_tag_next (params);
}
#endif /* CONFIG_VFD || CONFIG_LCD */

#if defined(CONFIG_MARVELL_TAG)
static void setup_marvell_tag (void)
{
	char *env;
	char temp[20];
	int i;
	unsigned int boardId;

	params->hdr.tag = ATAG_MARVELL;
	params->hdr.size = tag_size (tag_mv_uboot);

	params->u.mv_uboot.uboot_version = VER_NUM;

	extern unsigned int mvBoardIdGet(void);	

	boardId = mvBoardIdGet();
	params->u.mv_uboot.uboot_version |= boardId;

	params->u.mv_uboot.tclk = CFG_TCLK;
	params->u.mv_uboot.sysclk = CFG_BUS_CLK;
	
#if defined(MV78XX0)
	/* Dual CPU Firmware load address */
        env = getenv("fw_image_base");
        if(env)
		params->u.mv_uboot.fw_image_base = simple_strtoul(env, NULL, 16);
	else
		params->u.mv_uboot.fw_image_base = 0;

	/* Dual CPU Firmware size */
        env = getenv("fw_image_size");
        if(env)
		params->u.mv_uboot.fw_image_size = simple_strtoul(env, NULL, 16);
	else
		params->u.mv_uboot.fw_image_size = 0;
#endif

#if defined(MV_INCLUDE_USB)
	extern unsigned int mvCtrlUsbMaxGet(void);

    for (i = 0 ; i < mvCtrlUsbMaxGet(); i++)
	{
	sprintf( temp, "usb%dMode", i);
	env = getenv(temp);
		if((!env) || (strcmp(env,"Host") == 0 ) || (strcmp(env,"host") == 0) )
		params->u.mv_uboot.isUsbHost |= (1 << i);
		else
		params->u.mv_uboot.isUsbHost &= ~(1 << i);

	}
#endif /*#if defined(MV_INCLUDE_USB)*/
#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH)
	extern unsigned int mvCtrlEthMaxPortGet(void);
	extern int mvMacStrToHex(const char* macStr, unsigned char* macHex);

	for (i = 0 ;i < 4;i++)
	{
		memset(params->u.mv_uboot.macAddr[i], 0, sizeof(params->u.mv_uboot.macAddr[i]));
		params->u.mv_uboot.mtu[i] = 0; 
	}

	for (i = 0 ;i < mvCtrlEthMaxPortGet();i++)
	{
	    sprintf( temp,(i ? "eth%daddr" : "ethaddr"), i);
	    env = getenv(temp);
	    if (env)
		mvMacStrToHex(env, params->u.mv_uboot.macAddr[i]);

	    sprintf( temp,(i ? "eth%dmtu" : "ethmtu"), i);
	    env = getenv(temp);
	    if (env)
		params->u.mv_uboot.mtu[i] = simple_strtoul(env, NULL, 10); 
     	}
#endif /* (MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH) */

  params = mv_tag_next (params);
}	
#endif

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag (struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;
	void get_board_serial(struct tag_serialnr *serialnr);

	get_board_serial(&serialnr);
	params->hdr.tag = ATAG_SERIAL;
	params->hdr.size = tag_size (tag_serialnr);
	params->u.serialnr.low = serialnr.low;
	params->u.serialnr.high= serialnr.high;
  params = mv_tag_next (params);
	*tmp = params;
}
#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;
	u32 get_board_rev(void);

	rev = get_board_rev();
	params->hdr.tag = ATAG_REVISION;
	params->hdr.size = tag_size (tag_revision);
	params->u.revision.rev = rev;
  params = mv_tag_next (params);
}
#endif  /* CONFIG_REVISION_TAG */


static void setup_end_tag (bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
  if (s_changeEndian)
  {
    mv_swab32(params->hdr.tag);
    mv_swab32(params->hdr.size);
  }
}

#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */
