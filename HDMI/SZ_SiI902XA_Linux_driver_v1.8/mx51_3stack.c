/*
 * Copyright (C) 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/fsl_devices.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <asm/mach/flash.h>

#include <linux/regulator/consumer.h>
#include <linux/pmic_external.h>
#include <linux/pmic_status.h>
#include <linux/ipu.h>
#include <linux/ata.h>
#include <linux/mxcfb.h>
#include <linux/pwm_backlight.h>
#include <mach/common.h>
#include <mach/hardware.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/mach/keypad.h>

#include <mach/gpio.h>
#include <mach/mmc.h>
#include <mach/mxc_dvfs.h>
#include <mach/mxc_edid.h>

#include "devices.h"
#include "iomux.h"
#include "mx51_pins.h"
#include "crm_regs.h"
#include "usb.h"
#include <linux/android_pmem.h>
#include <linux/usb/android.h>

/*!
 * @file mach-mx51/mx51_3stack.c
 *
 * @brief This file contains the board specific initialization routines.
 *
 * @ingroup MSL_MX51
 */
 extern int __init mx51_3stack_init_mc13892(void);
extern void __init mx51_3stack_io_init(void);
extern struct cpu_wp *(*get_cpu_wp)(int *wp);
extern void (*set_num_cpu_wp)(int num);
static int num_cpu_wp = 3;
//static bool debug_board_present;

/* working point(wp): 0 - 800MHz; 1 - 166.25MHz; */
static struct cpu_wp cpu_wp_auto[] = {
	{
	 .pll_rate = 1000000000,
	 .cpu_rate = 1000000000,
	 .pdf = 0,
	 .mfi = 10,
	 .mfd = 11,
	 .mfn = 5,
	 .cpu_podf = 0,
	 .cpu_voltage = 1175000,},
	{
	 .pll_rate = 800000000,
	 .cpu_rate = 800000000,
	 .pdf = 0,
	 .mfi = 8,
	 .mfd = 2,
	 .mfn = 1,
	 .cpu_podf = 0,
	 .cpu_voltage = 1100000,},
	{
	 .pll_rate = 800000000,
	 .cpu_rate = 166250000,
	 .pdf = 4,
	 .mfi = 8,
	 .mfd = 2,
	 .mfn = 1,
	 .cpu_podf = 4,
	 .cpu_voltage = 850000,},
};

static struct fb_videomode video_modes[] = {
	{
	 /* NTSC TV output */
	 "TV-NTSC", 60, 720, 480, 74074,
	 122, 15,
	 18, 26,
	 1, 1,
	 FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT | FB_SYNC_EXT,
	 FB_VMODE_INTERLACED,
	 0,},
	{
	 /* PAL TV output */
	 "TV-PAL", 50, 720, 576, 74074,
	 132, 11,
	 22, 26,
	 1, 1,
	 FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT | FB_SYNC_EXT,
	 FB_VMODE_INTERLACED | FB_VMODE_ODD_FLD_FIRST,
	 0,},
	{
	 /* 720p60 TV output */
	 "720P60", 60, 1280, 720, 13468,
	 260, 109,
	 25, 4,
	 1, 1,
	 FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT |
			FB_SYNC_EXT,
	 FB_VMODE_NONINTERLACED,
	 0,},
	{
	 /* MITSUBISHI LVDS panel */
	 "XGA", 60, 1024, 768, 15385,
	 220, 40,
	 21, 7,
	 60, 10,
	 0,
	 FB_VMODE_NONINTERLACED,
	 0,},
	{
	 /* 800x480 @ 55 Hz , pixel clk @ 25MHz */
	 "CLAA-WVGA", 55, 800, 480, 40000, 40, 40, 5, 5, 20, 10,
	 FB_SYNC_CLK_LAT_FALL,
	 FB_VMODE_NONINTERLACED,
	 0,},
};

struct cpu_wp *mx51_3stack_get_cpu_wp(int *wp)
{
	*wp = num_cpu_wp;
	return cpu_wp_auto;
}

void mx51_3stack_set_num_cpu_wp(int num)
{
	num_cpu_wp = num;
	return;
}

static void mxc_nop_release(struct device *dev)
{
	/* Nothing */
}

static struct mxc_w1_config mxc_w1_data = {
	.search_rom_accelerator = 1,
};

static u16 keymapping[4*3] = {
		KEY_UP, KEY_DOWN, KEY_SELECT,
		KEY_OK, KEY_RIGHT, KEY_LEFT,
		KEY_ENTER, KEY_MENU, KEY_END,
		KEY_KBDILLUMDOWN, KEY_HOME, KEY_BACK,
};

static struct keypad_data keypad_plat_data = {
	.rowmax = 4,
	.colmax = 3,
	.irq = MXC_INT_KPP,
	.learning = 0,
	.delay = 2,
	.matrix = keymapping,
};

static struct platform_pwm_backlight_data mxc_pwm_backlight_data = {
	.pwm_id = 0,
	.max_brightness = 255,
	.dft_brightness = 128,
	.pwm_period_ns = 78770,
};

extern void mx5_ipu_reset(void);
static struct mxc_ipu_config mxc_ipu_data = {
	.rev = 2,
	.reset = mx5_ipu_reset,	
};

extern void mx5_vpu_reset(void);
static struct mxc_vpu_platform_data mxc_vpu_data = {
	.reset = mx5_vpu_reset,
};

extern void mx51_babbage_gpio_spi_chipselect_active(int cspi_mode, int status,
						    int chipselect);
extern void mx51_babbage_gpio_spi_chipselect_inactive(int cspi_mode, int status,
						      int chipselect);
static struct mxc_spi_master mxcspi1_data = {
	.maxchipselect = 4,
	.spi_version = 23,
	.chipselect_active = mx51_babbage_gpio_spi_chipselect_active,
	.chipselect_inactive = mx51_babbage_gpio_spi_chipselect_inactive,
};

static struct mxc_i2c_platform_data mxci2c_data = {
	.i2c_clk = 100000,
};

static struct mxc_i2c_platform_data mxci2c_hs_data = {
	.i2c_clk = 400000,
};

static struct mxc_srtc_platform_data srtc_data = {
	.srtc_sec_mode_addr = 0x83F98840,
};

static struct tve_platform_data tve_data = {
	.dac_reg = "VVIDEO",
	.dig_reg = "VDIG",
};

static struct mxc_dvfs_platform_data dvfs_core_data = {
	.reg_id = "SW1",
	.clk1_id = "cpu_clk",
	.clk2_id = "gpc_dvfs_clk",
	.gpc_cntr_reg_addr = MXC_GPC_CNTR,
	.gpc_vcr_reg_addr = MXC_GPC_VCR,
	.ccm_cdcr_reg_addr = MXC_CCM_CDCR,
	.ccm_cacrr_reg_addr = MXC_CCM_CACRR,
	.ccm_cdhipr_reg_addr = MXC_CCM_CDHIPR,
	.prediv_mask = 0x1F800,
	.prediv_offset = 11,
	.prediv_val = 3,
	.div3ck_mask = 0xE0000000,
	.div3ck_offset = 29,
	.div3ck_val = 2,
	.emac_val = 0x08,
	.upthr_val = 25,
	.dnthr_val = 9,
	.pncthr_val = 33,
	.upcnt_val = 10,
	.dncnt_val = 10,
	.delay_time = 30,
	.num_wp = 3,
};

static struct mxc_dvfsper_data dvfs_per_data = {
	.reg_id = "SW2",
	.clk_id = "gpc_dvfs_clk",
	.gpc_cntr_reg_addr = MXC_GPC_CNTR,
	.gpc_vcr_reg_addr = MXC_GPC_VCR,
	.gpc_adu = 0x0,
	.vai_mask = MXC_DVFSPMCR0_FSVAI_MASK,
	.vai_offset = MXC_DVFSPMCR0_FSVAI_OFFSET,
	.dvfs_enable_bit = MXC_DVFSPMCR0_DVFEN,
	.irq_mask = MXC_DVFSPMCR0_FSVAIM,
	.div3_offset = 0,
	.div3_mask = 0x7,
	.div3_div = 2,
	.lp_high = 1200000,
	.lp_low = 1200000,
};

static struct mxc_spdif_platform_data mxc_spdif_data = {
	.spdif_tx = 1,
	.spdif_rx = 0,
	.spdif_clk_44100 = 0,	/* spdif_ext_clk source for 44.1KHz */
	.spdif_clk_48000 = 7,	/* audio osc source */
	.spdif_clkid = 0,
	.spdif_clk = NULL,	/* spdif bus clk */
};


/* NAND Flash Partitions */
#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition nand_flash_partitions[] = {
	{
	 .name = "bootloader",
	 .offset = 0,
	 .size = 3 * 1024 * 1024},
	{
	 .name = "nand.kernel",
	 .offset = MTDPART_OFS_APPEND,
	 .size = 5 * 1024 * 1024},
	{
	 .name = "nand.rootfs",
	 .offset = MTDPART_OFS_APPEND,
	 .size = 256 * 1024 * 1024},
	{
	 .name = "nand.userfs1",
	 .offset = MTDPART_OFS_APPEND,
	 .size = 256 * 1024 * 1024},
	{
	 .name = "nand.userfs2",
	 .offset = MTDPART_OFS_APPEND,
	 .size = MTDPART_SIZ_FULL},
};
#endif

static int nand_init(void)
{
	return 0;
}

static struct flash_platform_data mxc_nand_data = {
	#ifdef CONFIG_MTD_PARTITIONS
		.parts = nand_flash_partitions,
		.nr_parts = ARRAY_SIZE(nand_flash_partitions),
	#endif
	.width = 1,
	.init = nand_init,
};

static struct resource mxcfb_resources[] = {
	[0] = {
	       .flags = IORESOURCE_MEM,
	       },
};

static struct mxc_fb_platform_data fb_data[] = {
	{
	 .interface_pix_fmt = IPU_PIX_FMT_RGB24,
	 .mode_str = "1024x768M-16@60",
	 .mode = video_modes,
	 .num_modes = ARRAY_SIZE(video_modes),
	 },
	{
	 .interface_pix_fmt = IPU_PIX_FMT_RGB24,
	 .mode_str = "CLAA-WVGA",
	 .mode = video_modes,
	 .num_modes = ARRAY_SIZE(video_modes),
	 },
};

static void wvga_reset(void)
{
	//gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_DI1_D1_CS), 1);
}



static struct platform_device lcd_wvga_device = {
	.name = "lcd_claa",
};


extern int primary_di;
static int __init mxc_init_fb(void)
{
	/* WVGA Reset */
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_DI1_D1_CS), 1);
	if (primary_di) {
		printk(KERN_INFO "DVI is primary\n");

		mxc_fb_devices[0].num_resources = ARRAY_SIZE(mxcfb_resources);
		mxc_fb_devices[0].resource = mxcfb_resources;
		mxc_register_device(&mxc_fb_devices[0], &fb_data[0]);
	} else {
		printk(KERN_INFO "WVGA LCD is primary\n");

		mxc_fb_devices[0].num_resources = ARRAY_SIZE(mxcfb_resources);
		mxc_fb_devices[0].resource = mxcfb_resources;
		mxc_register_device(&mxc_fb_devices[0], &fb_data[1]);
	}
	/* DISP3 BG - DI1 */
	mxc_register_device(&mxc_fb_devices[1], NULL);
	
	/* DISP3 FG */
	mxc_register_device(&mxc_fb_devices[2], NULL);

	mxc_register_device(&lcd_wvga_device, NULL);
	
	return 0;
}
device_initcall(mxc_init_fb);




/******* add by caizp  vga_reset*******/
static void vga_reset(void)
{

	gpio_request(IOMUX_TO_GPIO(MX51_PIN_EIM_A19), "eim_a19");
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_EIM_A19), 0);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_EIM_A19), 0);
	msleep(50);
	// do reset 
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_EIM_A19), 1);
	msleep(10);		// tRES >= 50us 
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_EIM_A19), 0);
}

static struct mxc_lcd_platform_data vga_data = {
	.core_reg = "VCAM",
	.io_reg = "VGEN3",
	.analog_reg = "VAUDIO",
	.reset = vga_reset,
};


static struct mxc_camera_platform_data camera_data2 = {
	.mclk = 27000000,
	.csi = 0,
};

#ifdef CONFIG_HDMI_Sii902xA
#define CI2CA true
#ifdef CI2CA 
#define SII902xA_plus 0x02  //Define sii902xA's I2c Address of all pages by the status of CI2CA pin.
#else
#define SII902xA_plus 0x00  //Define sii902xA's I2c Address of all pages by the status of CI2CA pin.
#endif

static void HDMI_reset(void)
{
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI1_D8), 0);
	msleep(200);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI1_D8), 3);
}

struct platform_data {
	void (*reset) (void);
};

static struct platform_data HDMI_data = {
	.reset = HDMI_reset,
};
#endif

#ifdef CONFIG_MHL_Sii9244

#define SII9244_add 0x39  //定义sii9244的I2C地址，根据sii9244 CI2CA引脚硬件设置的不同，902xA的器件地址也可能是0x3B 
static void Sii9244_reset(void)
{	
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI2_D18), 0);
	msleep(200);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI2_D18), 3);
}


static struct mxc_lcd_platform_data Sii9244_data = {
	.reset = Sii9244_reset,
};
#endif

#ifdef CONFIG_MHL_Sii8334
//#define CI2CA true
#ifdef CI2CA 
#define SII8334_plus 0x02  //Define sii8334's I2c Address of all pages by the status of CI2CA.
#else
#define SII8334_plus 0x00  //Define sii8334's I2c Address of all pages by the status of CI2CA.
#endif
static void Sii8334_reset(void)
{	
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI2_D18), 0);
	msleep(200);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI2_D18), 3);
}


static struct mxc_lcd_platform_data Sii8334_data = {
	.reset = Sii8334_reset,
};
#endif


static struct  i2c_board_info __initdata mxc_i2c1_board_info[] = {
	{
		.type = "mt9v112",
		.addr = 0x5D,
		.platform_data = (void *)&camera_data2,
	},
	{
	 	.type = "ov7725",
	 	.addr = 0x21,
	 	.platform_data = (void *)&camera_data2,
	 },
	 {
	 .type = "ch7026",            //add by caizp
	 .addr = 0x76,
	 .platform_data = &vga_data,
	 },
	 {                                            
		.type = "tvp5150",                    //add by caizp
		.addr = 0x5C,
	},
	 {
	 	.type = "wm8731-i2c",
	 	.addr = 0x1a,
	 },
/*	 {
	 .type = "tfp410",
	 .addr = 0x5C,
	 .platform_data = &dvi_data,
	 },
*/	
	{					//add by caizp
	 	.type="tfp410-i2c",		
	 	.addr=0x38,		
	 },
	#ifdef CONFIG_HDMI_Sii902xA
	{
	 .type = "siiEDID",
	 .addr = 0x50, 
	},
	{
	 .type = "siiSegEDID",
	 .addr = 0x30, 
	},
	{
	 .type = "siiHDCP",
	 .addr = 0x3A+SII902xA_plus, 
	},
	{
	 .type = "siiCEC",
	 .addr = 0x60+SII902xA_plus, 
	},
	{
	 .type = "sii902xA", 
	 .addr = 0x39+SII902xA_plus, 
	 .irq = IOMUX_TO_IRQ(MX51_PIN_EIM_OE),  //define the interrupt signal input pin
	 .platform_data = &HDMI_data,
	},
	#endif
	//sii9244
	#ifdef CONFIG_MHL_Sii9244
	{
	 .type = "mhl_Sii9244_page0",
	 .addr = SII9244_add, 
	 .irq = IOMUX_TO_IRQ(MX51_PIN_CSI2_D19),  //define the interrupt signal input pin
	 .platform_data = &Sii9244_data,
	},
	{
	 .type = "mhl_Sii9244_page1",
	 .addr = 0x3D, 
	},
	{
	 .type = "mhl_Sii9244_page2",
	 .addr = 0x49, 
	},
	{
	 .type = "mhl_Sii9244_cbus",
	 .addr = 0x64, 
	},
	#endif
	
	#ifdef CONFIG_MHL_Sii8334
	{
	 .type = "sii8334_PAGE_TPI",
	 .addr = 0x39 + SII8334_plus, 
	 .irq = IOMUX_TO_IRQ(MX51_PIN_CSI2_D19),  //define the interrupt signal input pin
	 .platform_data = &Sii8334_data,
	},
	{
	 .type = "sii8334_PAGE_TX_L0",
	 .addr = 0x39 + SII8334_plus, 
	},
	{
	 .type = "sii8334_PAGE_TX_L1",
	 .addr = 0x3d + SII8334_plus, 
	},
	{
	 .type = "sii8334_PAGE_TX_2",
	 .addr = 0x49 + SII8334_plus, 
	},
	{
	 .type = "sii8334_PAGE_TX_3",
	 .addr = 0x4d + SII8334_plus, 
	},
	{
	 .type = "sii8334_PAGE_CBUS",
	 .addr = 0x64 + SII8334_plus, 
	},
	#endif
};


static struct mxc_camera_platform_data camera_data = {
	.io_regulator = "SW4",
	.analog_regulator = "VIOHI",
	.mclk = 24000000,
	.csi = 0,
};
static struct mxc_lightsensor_platform_data ls_data = {
	.vdd_reg = NULL,
	.rext = 100,
};

static struct i2c_board_info __initdata mxc_i2c_hs_board_info[]  = {
	{
		.type = "ov3640",
		.addr = 0x3C,
		.platform_data = (void *)&camera_data,
	},
	{
		.type = "isl29003",
		.addr = 0x44,
		.platform_data = &ls_data,
	 },
};

/*!
 * Get WP pin value to detect write protection
 */
static int sdhc_write_protect(struct device *dev)
{
	unsigned short rc = 0;

	if (to_platform_device(dev)->id == 0)
		rc = gpio_get_value(IOMUX_TO_GPIO(MX51_PIN_GPIO1_1));
	else
		//rc = gpio_get_value(IOMUX_TO_GPIO(MX51_PIN_GPIO1_7));
	return rc;
}

static unsigned int sdhc_get_card_det_status(struct device *dev)
{
	int ret;

	if (to_platform_device(dev)->id == 0) {
		ret = gpio_get_value(IOMUX_TO_GPIO(MX51_PIN_GPIO1_0));
		return ret;
	} else {		/* config the det pin for SDHC2 */
		return 0;
	}
}

static struct mxc_mmc_platform_data mmc1_data = {
	.ocr_mask = MMC_VDD_32_33,
	.caps = MMC_CAP_4_BIT_DATA,
	.min_clk = 150000,
	.max_clk = 52000000,
	.card_inserted_state = 0,
	.status = sdhc_get_card_det_status,
	.wp_status = sdhc_write_protect,
	.clock_mmc = "esdhc_clk",
	.power_mmc = NULL,
};
static struct mxc_mmc_platform_data mmc2_data = {
	.ocr_mask = MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_29_30 |
	    MMC_VDD_31_32,
	.caps = MMC_CAP_4_BIT_DATA,
	.min_clk = 150000,
	.max_clk = 50000000,
	.card_inserted_state = 0,
	.status = sdhc_get_card_det_status,
	.wp_status = sdhc_write_protect,
	.clock_mmc = "esdhc_clk",
};

extern void gpio_ata_active(void);
extern void gpio_ata_inactive(void);

static int ata_init(struct platform_device *pdev)
{
	/* Configure the pins */
	gpio_ata_active();
	return 0;
}

static void ata_exit(void)
{
	/* Free the pins */
	gpio_ata_inactive();
}

static struct fsl_ata_platform_data ata_data = {
	.udma_mask = ATA_UDMA3,
	.mwdma_mask = ATA_MWDMA2,
	.pio_mask = ATA_PIO4,
	.fifo_alarm = MXC_IDE_DMA_WATERMARK / 2,
	.max_sg = MXC_IDE_DMA_BD_NR,
	.init = ata_init,
	.exit = ata_exit,
	.core_reg = NULL,
	.io_reg = NULL,
};

static int __init mxc_init_srpgconfig(void)
{
	struct clk *gpcclk = clk_get(NULL, "gpc_dvfs_clk");
	clk_enable(gpcclk);

	/* Setup the number of clock cycles to wait for SRPG
	 * power up and power down requests.
	 */
	__raw_writel(0x010F0201, MXC_SRPG_ARM_PUPSCR);
	__raw_writel(0x010F0201, MXC_SRPG_NEON_PUPSCR);
	__raw_writel(0x00000008, MXC_SRPG_EMPGC0_PUPSCR);
	__raw_writel(0x00000008, MXC_SRPG_EMPGC1_PUPSCR);

	__raw_writel(0x01010101, MXC_SRPG_ARM_PDNSCR);
	__raw_writel(0x01010101, MXC_SRPG_NEON_PDNSCR);
	__raw_writel(0x00000018, MXC_SRPG_EMPGC0_PDNSCR);
	__raw_writel(0x00000018, MXC_SRPG_EMPGC1_PDNSCR);

	clk_disable(gpcclk);
	clk_put(gpcclk);

	return 0;
}

static struct mxc_audio_platform_data wm8731_data;

static struct platform_device mxc_wm8731_device = {
	.name = "imx-3stack-wm8731",
	.id = 0,
	.dev = {
		.release = mxc_nop_release,
		.platform_data = &wm8731_data,
		},
};

static void __init mxc_init_wm8731(void)
{
	wm8731_data.ssi_clk[0] = clk_get(NULL, "ssi_clk.0");
	clk_put(wm8731_data.ssi_clk[0]);

	wm8731_data.ssi_clk[1] = clk_get(NULL, "ssi_clk.1");
	clk_put(wm8731_data.ssi_clk[1]);

	wm8731_data.ssi_num = 1;
	wm8731_data.src_port = 2;
	wm8731_data.ext_port = 6;

	(void)platform_device_register(&mxc_wm8731_device);
}

static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem_adsp",
	.start = 0,
	.size = SZ_32M,
	.no_allocator = 0,
	.cached = PMEM_NONCACHE_NORMAL,
};

static struct android_pmem_platform_data android_pmem_gpu_pdata = {
	.name = "pmem_gpu",
	.start = 0,
	.size = SZ_32M,
	.no_allocator = 0,
	.cached = PMEM_CACHE_ENABLE,
};

static struct android_usb_platform_data android_usb_pdata = {
	.vendor_id      = 0x0bb4,
	.product_id     = 0x0c01,
	.adb_product_id = 0x0c02,
	.version        = 0x0100,
	.product_name   = "Android Phone",
	.manufacturer_name = "Freescale",
	.nluns = 3,
};

/*!
 * Board specific fixup function. It is called by \b setup_arch() in
 * setup.c file very early on during kernel starts. It allows the user to
 * statically fill in the proper values for the passed-in parameters. None of
 * the parameters is used currently.
 *
 * @param  desc         pointer to \b struct \b machine_desc
 * @param  tags         pointer to \b struct \b tag
 * @param  cmdline      pointer to the command line
 * @param  mi           pointer to \b struct \b meminfo
 */
static void __init fixup_mxc_board(struct machine_desc *desc, struct tag *tags,
				   char **cmdline, struct meminfo *mi)
{
	struct tag *t;
	int size;

	mxc_set_cpu_type(MXC_CPU_MX51);

	get_cpu_wp = mx51_3stack_get_cpu_wp;
	set_num_cpu_wp = mx51_3stack_set_num_cpu_wp;

	for_each_tag(t, tags) {
		if (t->hdr.tag != ATAG_MEM)
			continue;
		size = t->u.mem.size;

		android_pmem_pdata.start =
				PHYS_OFFSET + size - android_pmem_pdata.size;
		android_pmem_gpu_pdata.start =
				android_pmem_pdata.start - android_pmem_gpu_pdata.size;
		gpu_device.resource[5].start =
				android_pmem_gpu_pdata.start - SZ_16M;
		gpu_device.resource[5].end =
				gpu_device.resource[5].start + SZ_16M - 1;
		size -= android_pmem_pdata.size;
		size -= android_pmem_gpu_pdata.size;
		size -= SZ_16M;
		t->u.mem.size = size;
	}
}

#define PWGT1SPIEN (1<<15)
#define PWGT2SPIEN (1<<16)
#define USEROFFSPI (1<<3)

static void mxc_power_off(void)
{
	/* We can do power down one of two ways:
	   Set the power gating
	   Set USEROFFSPI */

	/* Set the power gate bits to power down */
	pmic_write_reg(REG_POWER_MISC, (PWGT1SPIEN|PWGT2SPIEN),
		(PWGT1SPIEN|PWGT2SPIEN));
}

/*!
 * Power Key interrupt handler.
 */
static irqreturn_t power_key_int(int irq, void *dev_id)
{
	pr_info(KERN_INFO "PWR key pressed\n");
	return 0;
}

/*!
 * Power Key initialization.
 */
static int __init mxc_init_power_key(void)
{
	/* Set power key as wakeup resource */
	int irq, ret;
	irq = IOMUX_TO_IRQ(MX51_PIN_EIM_A27);
	set_irq_type(irq, IRQF_TRIGGER_RISING);
	ret = request_irq(irq, power_key_int, 0, "power_key", 0);
	if (ret)
		pr_info("register on-off key interrupt failed\n");
	else
		enable_irq_wake(irq);
	return ret;
}
late_initcall(mxc_init_power_key);

/*!
 * Board specific initialization.
 */
static void __init mxc_board_init(void)
{
	mxc_ipu_data.di_clk[0] = clk_get(NULL, "ipu_di0_clk");
	mxc_ipu_data.di_clk[1] = clk_get(NULL, "ipu_di1_clk");

	mxc_spdif_data.spdif_core_clk = clk_get(NULL, "spdif_xtal_clk");
	clk_put(mxc_spdif_data.spdif_core_clk);

	/* SD card detect irqs */
	mxcsdhc2_device.resource[2].start = IOMUX_TO_IRQ(MX51_PIN_GPIO1_8);
	mxcsdhc2_device.resource[2].end = IOMUX_TO_IRQ(MX51_PIN_GPIO1_8);
	mxcsdhc1_device.resource[2].start = IOMUX_TO_IRQ(MX51_PIN_GPIO1_0);
	mxcsdhc1_device.resource[2].end = IOMUX_TO_IRQ(MX51_PIN_GPIO1_0);
	
	mxc_cpu_common_init();
	mxc_register_gpios();
	mx51_3stack_io_init();

	mxc_register_device(&mxc_dma_device, NULL);
	mxc_register_device(&mxc_wdt_device, NULL);
	mxc_register_device(&mxcspi1_device, &mxcspi1_data);
	mxc_register_device(&mxci2c_devices[0], &mxci2c_data);
	mxc_register_device(&mxci2c_devices[1], &mxci2c_data);
	mxc_register_device(&mxci2c_hs_device, &mxci2c_hs_data);
	mxc_register_device(&mxc_rtc_device, &srtc_data);
	mxc_register_device(&mxc_w1_master_device, &mxc_w1_data);
	mxc_register_device(&mxc_ipu_device, &mxc_ipu_data);
	mxc_register_device(&mxc_tve_device, &tve_data);
	mxc_register_device(&mxcvpu_device, &mxc_vpu_data);
	mxc_register_device(&gpu_device, NULL);
	mxc_register_device(&mxcscc_device, NULL);
	mxc_register_device(&mx51_lpmode_device, NULL);
	mxc_register_device(&busfreq_device, NULL);
	mxc_register_device(&sdram_autogating_device, NULL);
	mxc_register_device(&mxc_dvfs_core_device, &dvfs_core_data);
	mxc_register_device(&mxc_dvfs_per_device, &dvfs_per_data);
	mxc_register_device(&mxc_iim_device, NULL);
	mxc_register_device(&mxc_pwm1_device, NULL);
	mxc_register_device(&mxc_pwm_backlight_device, &mxc_pwm_backlight_data);
	mxc_register_device(&mxc_keypad_device, &keypad_plat_data);
	mxcsdhc1_device.resource[2].start = IOMUX_TO_IRQ(MX51_PIN_GPIO1_0),
	mxcsdhc1_device.resource[2].end = IOMUX_TO_IRQ(MX51_PIN_GPIO1_0),
	mxc_register_device(&mxcsdhc1_device, &mmc1_data);
	mxc_register_device(&mxcsdhc2_device, &mmc2_data);
	mxc_register_device(&pata_fsl_device, &ata_data);
	mxc_register_device(&mxc_ssi1_device, NULL);
	mxc_register_device(&mxc_ssi2_device, NULL);
	mxc_register_device(&mxc_alsa_spdif_device, &mxc_spdif_data);
	mxc_register_device(&mxc_v4l2_device, NULL);
	mxc_register_device(&mxc_v4l2out_device, NULL);
	mxc_register_device(&mxc_android_pmem_device, &android_pmem_pdata);
	mxc_register_device(&mxc_android_pmem_gpu_device, &android_pmem_gpu_pdata);
	mxc_register_device(&android_usb_device, &android_usb_pdata);
	
	mxc_register_device(&mxc_nandv2_mtd_device, &mxc_nand_data);

	mxc_init_srpgconfig();
	mx51_3stack_init_mc13892();

	i2c_register_board_info(1, mxc_i2c1_board_info,
				ARRAY_SIZE(mxc_i2c1_board_info));
	i2c_register_board_info(3, mxc_i2c_hs_board_info,
				ARRAY_SIZE(mxc_i2c_hs_board_info));
	
	mxc_init_wm8731();

	pm_power_off = mxc_power_off;
	
	mx5_usb_dr_init();
	mx5_usbh1_init();	
	mx5_usbh2_init();

	/* Setup Touchscreen interrupt */
	gpio_request(IOMUX_TO_GPIO(MX51_PIN_GPIO1_5), "gpio1_5");
	gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_GPIO1_5));

	/* Setup USBOTG USBPHY_ID interrupt */
	gpio_request(IOMUX_TO_GPIO(MX51_PIN_CSI2_D13), "csi2_d13");
	gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_CSI2_D13));
	//HDMI_reset();
}

static void __init mx51_3stack_timer_init(void)
{
	struct clk *uart_clk;

	/* Change the CPU voltages for TO2*/
	if (cpu_is_mx51_rev(CHIP_REV_2_0) <= 1) {
		cpu_wp_auto[0].cpu_voltage = 1175000;
		cpu_wp_auto[1].cpu_voltage = 1100000;
		cpu_wp_auto[2].cpu_voltage = 1000000;
	}

	mx51_clocks_init(32768, 24000000, 22579200, 24576000);

	uart_clk = clk_get(NULL, "uart_clk.0");
	early_console_setup(UART1_BASE_ADDR, uart_clk);
}

static struct sys_timer mxc_timer = {
	.init	= mx51_3stack_timer_init,
};

/*
 * The following uses standard kernel macros define in arch.h in order to
 * initialize __mach_desc_MX51_3STACK data structure.
 */
/* *INDENT-OFF* */
MACHINE_START(MX51_3DS, "Freescale MX51 3-Stack Board")
	/* Maintainer: Freescale Semiconductor, Inc. */
	.phys_io = AIPS1_BASE_ADDR,
	.io_pg_offst = ((AIPS1_BASE_ADDR_VIRT) >> 18) & 0xfffc,
	.fixup = fixup_mxc_board,
	.map_io = mx5_map_io,
	.init_irq = mx5_init_irq,
	.init_machine = mxc_board_init,
	.timer = &mxc_timer,
MACHINE_END
