#ifndef __PSPINIT_H__
#define __PSPINIT_H__

#include <pspkerneltypes.h>

enum PSPBootFrom
{
    PSP_BOOT_FLASH = 0, /* ? */
    PSP_BOOT_DISC = 0x20,
    PSP_BOOT_MS = 0x40,
};

enum PSPInitApitype
{
    PSP_INIT_APITYPE_DISC = 0x120,
    PSP_INIT_APITYPE_DISC_UPDATER = 0x121,
    PSP_INIT_APITYPE_MS1 = 0x140,
    PSP_INIT_APITYPE_MS2 = 0x141,
    PSP_INIT_APITYPE_MS3 = 0x142,
    PSP_INIT_APITYPE_MS4 = 0x143,
    PSP_INIT_APITYPE_MS5 = 0x144,
    PSP_INIT_APITYPE_VSH1 = 0x210, /* ExitGame */
    PSP_INIT_APITYPE_VSH2 = 0x220, /* ExitVSH */
};

enum PSPKeyConfig
{
    PSP_INIT_KEYCONFIG_VSH = 0x100,
    PSP_INIT_KEYCONFIG_UPDATER = 0x110,
    PSP_INIT_KEYCONFIG_GAME = 0x200,
    PSP_INIT_KEYCONFIG_POPS = 0x300,
    PSP_INIT_KEYCONFIG_APP = 0x400,
};

typedef struct
{
    u32 flag;
    u32 mode;
} BtcnfBootModes;

/*BtcnfBootModes g_btcnf_boot_modes[] =
{
        { SCE_REBOOT_FLAG_GAME, 	SCE_REBOOT_MODE_GAME 		},
        { SCE_REBOOT_FLAG_VSH, 		SCE_REBOOT_MODE_VSH 		},
        { SCE_REBOOT_FLAG_UPDATER, 	SCE_REBOOT_MODE_UPDATER 	},
        { SCE_REBOOT_FLAG_POPS, 	SCE_REBOOT_MODE_POPS		},
        { SCE_REBOOT_FLAG_UMDEMU, 	SCE_REBOOT_MODE_UMDEMU 		},
        { SCE_REBOOT_FLAG_MLNAPP, 	SCE_REBOOT_MODE_MLNAPP		},
        { SCE_REBOOT_FLAG_UNK6, 	SCE_REBOOT_MODE_UNK6 		},
};*/

#define SCE_REBOOT_FLAG_GAME (1 << 1)
#define SCE_REBOOT_FLAG_VSH (1 << 0)
#define SCE_REBOOT_FLAG_UPDATER (1 << 2)
#define SCE_REBOOT_FLAG_POPS (1 << 3)
#define SCE_REBOOT_FLAG_APP (1 << 5)
#define SCE_REBOOT_FLAG_UMDEMU (1 << 6)
#define SCE_REBOOT_FLAG_MLNAPP (1 << 7)

#define SCE_REBOOT_MODE_GAME (1)
#define SCE_REBOOT_MODE_VSH (2)
#define SCE_REBOOT_MODE_UPDATER (3)
#define SCE_REBOOT_MODE_POPS (4)
#define SCE_REBOOT_MODE_UMDEMU (7)
#define SCE_REBOOT_MODE_MLNAPP (8)
#define SCE_REBOOT_MODE_APP (6)

#define SCE_INIT_PARAM_DISC (0)
#define SCE_INIT_PARAM_UNK1 (1)
#define SCE_INIT_PARAM_EXEC (2)
#define SCE_INIT_PARAM_UNK3 (3)
#define SCE_INIT_PARAM_VSH (4)
#define SCE_INIT_PARAM_IPL (8)

#define SCE_INIT_MODULE_USER (1 << 0)
#define SCE_INIT_MODULE_MAIN_EXEC (1 << 1)
#define SCE_INIT_MODULE_LOAD_FLASH0 (1 << 2)
#define SCE_INIT_MODULE_SIGNCHECK (1 << 8)

typedef struct
{
    char* arg;      // path of the executable, disc image etc
    u32 arg_size;   // size of the path + NULL
    u32 param_type; // one of the SCE_INIT_PARAM_* defines
    SceUID memid;   // the memory uid of the allocated memory
    u32 unk_16;
    u32 unk_20;
    u32 unk_24;
} SceInitModeParam; // size = 28

typedef struct
{
    char* module_path;    // 0
    void* module_buffer;  // 4
    u32 module_size;      // 8
    u32 unk_12;           // 12 (seems unused)
    u32 flags;            // 16 (SCE_INIT_MODULE_* flags)
    u32 api_type;         // 20
    u32 param_size;       // 24 (size of param)
    SceUID param_memid;   // 28 (memid from the param?)
} SceInitBootModuleEntry; // 32

typedef struct
{
    u32 sysmem_addr;                      // 0 (or kernel start? idk, it's 0x88000000)
    u32 real_mem_size;                    // 4
    u32 cur_modent_n;                     // 8
    u32 modentry_n;                       // 12
    SceInitBootModuleEntry* module_entry; // 16
    u32 unk_20;
    u8 is_warm_reset; // 24
    u8 unk_25;
    u16 unk_26;
    u32 init_param_n;              // 28
    SceInitModeParam* init_params; // 32
    u32 unk_36;
    u32 unk_40;
    u32 psp_model;     // 44
    u32 build_version; // 48
    u32 unk_52;
    u32 unk_56;
    u32 reboot_mode; // 60 (one of the SCE_REBOOT_MODE_* defines)
    u32 dipsw_low;   // 64
    u32 dipsw_high;  // 68
    u32 unk_72;      // 72
    u32 lpt_summary; // 76
    u32 cp_time;     // 80
    u32 unk_84;
    u32 unk_88;
    u32 unk_92;
    u32 unk_96;
    u32 unk_100;
    u32 unk_104;
    u32 unk_108;
    u32 unk_112;
    u32 unk_116;
    u32 unk_124;
} SceKernelBootParam; // 128, 0x80

typedef struct
{
    u32 size;               // 0
    u32 arg_size;           // 4
    void* argp;             // 8
    char* reboot_mode;      // 12 ("game", "updater", "umdemu" etc)
    u32 vshmain_arg_size;   // 16
    void* vshmain_argp;     // 20
    char* config_file;      // 24
    u32 unk_28;             // 28
    u32 unk_32;             // 32
    u32 unk_36;             // 36
    u32 unk_40;             // 40
    u32 unk_44;             // 44
} SceLoadExecInternalParam; // 48

typedef struct BtcnfHeader
{
    u32 signature;    // 0
    u32 devkit;       // 4
    u32 unknown[2];   // 8
    u32 modestart;    // 0x10
    int nmodes;       // 0x14
    u32 unknown2[2];  // 0x18
    u32 modulestart;  // 0x20
    int nmodules;     // 0x24
    u32 unknown3[2];  // 0x28
    u32 modnamestart; // 0x30
    u32 modnameend;   // 0x34
    u32 unknown4[2];  // 0x38
} __attribute__((packed)) BtcnfHeader;

typedef struct ModeEntry
{
    u16 maxsearch;
    u16 searchstart; //
    int mode1;
    int mode2;
    int reserved[5];
} __attribute__((packed)) ModeEntry;

typedef struct ModuleEntry
{
    u32 stroffset;                     // 0
    int reserved;                      // 4
    u16 flags;                         // 8
    u8 loadmode;                       // 10
    u8 signcheck;                      // 11
    int reserved2;                     // 12
    u8 hash[0x10];                     // 16
} __attribute__((packed)) ModuleEntry; // 32

enum BootLoadFlags
{
    BOOTLOAD_VSH = 1,
    BOOTLOAD_GAME = 2,
    BOOTLOAD_UPDATER = 4,
    BOOTLOAD_POPS = 8,
    BOOTLOAD_APP = 32,
    BOOTLOAD_UMDEMU = 64, /* for original NP9660 */
    BOOTLOAD_MLNAPP = 128,
};

/**
 * Gets the api type
 *
 * @returns the api type in which the system has booted
 */
int sceKernelInitApitype();

/**
 * Gets the filename of the executable to be launched after all modules of the api.
 *
 * @returns filename of executable or NULL if no executable found.
 */
char* sceKernelInitFileName();

/**
 *
 * Gets the device in which the application was launched.
 *
 * @returns the device code, one of PSPBootFrom values.
 */
int sceKernelBootFrom();

/**
 * Get the key configuration in which the system has booted.
 *
 * @returns the key configuration code, one of PSPKeyConfig values
 */
int InitForKernel_7233B5BC();

#define sceKernelInitKeyConfig InitForKernel_7233B5BC
#define sceKernelApplicationType InitForKernel_7233B5BC

#endif
