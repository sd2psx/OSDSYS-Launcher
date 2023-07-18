#include <debug.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <unistd.h>
#include <input.h>
#include <time.h>
#include <string.h>
#include <fileXio.h>
#include <fileXio_rpc.h>
#include <sbv_patches.h>
#include <libmc.h>
#include <stdbool.h>

#ifndef MC_TYPE_MC
#define MC_TYPE_MC 0
#endif


void ResetIOP()
{
    SifInitRpc(0);
    while(!SifIopReset("", 0)){};
    while(!SifIopSync()){};
    SifInitRpc(0);
}

void InitPS2()
{
    init_scr();
    ResetIOP();
    SifInitIopHeap();
    SifLoadFileInit();
    fioInit();
    sbv_patch_disable_prefix_check();
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:MCMAN", 0, NULL);
    SifLoadModule("rom0:MCSERV", 0, NULL);
    mcInit(MC_TYPE_MC);
}

void LoadElf(const char *elf, char* path)
{
    char* args[1];
    t_ExecData exec;    
    SifLoadElf(elf, &exec);

    if(exec.epc > 0)
    {    
        ResetIOP();

        if(path != 0)
        {
            args[0] = path;
            ExecPS2((void*)exec.epc, (void*)exec.gp, 1, args);
        }
        else
        {
            ExecPS2((void*)exec.epc, (void*)exec.gp, 0, NULL);
        }
    }
}

int file_exists(char filepath[], char ps2loc)
{    
    if (filepath[6] == '?')
    {
        filepath[6] = ps2loc;
    }
    
    int fdn;
    
    fdn = open(filepath, O_RDONLY);
    if (fdn < 0) return 0;
    close(fdn);
    
    return 1;
}

static void loadKELF(char default_OSDSYS_path[], char ps2loc)
{
    char arg0[20], arg1[20], arg2[20], arg3[40];
    char *args[4] = {arg0, arg1, arg2, arg3};
    char kelf_loader[40];
    int argc;
    char path[1025];
    
    if (default_OSDSYS_path[5] == '?')
    {
        default_OSDSYS_path[5] = ps2loc;
    }

    
    strcpy(path, default_OSDSYS_path);
    
    strcpy(arg0, "-m rom0:SIO2MAN");
    strcpy(arg1, "-m rom0:MCMAN");
    strcpy(arg2, "-m rom0:MCSERV");
    sprintf(arg3, "-x %s", path);
    argc = 4;
    strcpy(kelf_loader, "moduleload");
    
    LoadExecPS2(kelf_loader, argc, args);
}

static char getPs2Loc()
{
    u8 romver[16];
    char ps2loc;
    int fdn;
    if((fdn = open("rom0:ROMVER", O_RDONLY)) > 0) 
    {
        read(fdn, romver, sizeof(romver));
        close(fdn);
    }

    switch (romver[4])
    {
        case 'H': //asian PS2 outside of japan(I) and mainland china(C) use american(A) system update
            ps2loc = 'A'; // (eg: SCPH-xxx05/SCPH-xxx06/SCPH-xxx07)
            break;
        default:
            ps2loc = romver[4]; //the rest should use their ROMVER region char 
            break;
    }
    return ps2loc;
}

static bool cardAvailable(void)
{
    int ret, type, free, format;
    mcGetInfo(0 ,0, &type, &free, &format);
    mcSync(0, NULL, &ret);
    
    return (ret >= -2);
}

int main(int argc, char *argv[], char **envp)
{
    char ps2loc = 'E';
    bool card_change_det = false;
    bool bootup_done = false;

    InitPS2();
    scr_clear();
    scr_printf("\n\n\nSD2PSX Launcher \n");    //NOGUI
    
    int cntr = 0;

    ps2loc = getPs2Loc();

    while(!cardAvailable() || file_exists("mc0:/SD2PSX_BOOT", ps2loc)) {sleep(1);};

    scr_printf("Card re-plug detected. - Booting PS2\n");
#ifndef PSX
    if (file_exists("mc0:/B?EXEC-SYSTEM/osdmain.elf", ps2loc))  loadKELF("mc:/B?EXEC-SYSTEM/osdmain.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd110.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd110.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd120.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd120.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd130.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd130.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd140.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd140.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd150.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd150.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd160.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd160.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd170.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd170.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd180.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd180.elf", ps2loc);
    if (ps2loc != 'I')
        if (file_exists("mc0:/B?EXEC-SYSTEM/osd190.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd190.elf", ps2loc); //should be avoided... this update is shared by both 1st gen PSX and DTL-H50009
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd200.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd200.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd210.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd210.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd230.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd230.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd240.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd240.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd250.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd250.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd260.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd260.elf", ps2loc);
    if (file_exists("mc0:/B?EXEC-SYSTEM/osd270.elf", ps2loc) )  loadKELF("mc:/B?EXEC-SYSTEM/osd270.elf", ps2loc);
#else
    if (file_exists("mc0:/BIEXEC-SYSTEM/xosdmain.elf", ps2loc)) loadKELF("mc:/BIEXEC-SYSTEM/xosdmain.elf", ps2loc); //main PSX update
    if (file_exists("mc0:/BIEXEC-SYSTEM/osd190.elf", ps2loc) )  loadKELF("mc:/BIEXEC-SYSTEM/osd190.elf", ps2loc); // 1st gen PSX
    if (file_exists("mc0:/BIEXEC-SYSTEM/osd220.elf", ps2loc) )  loadKELF("mc:/BIEXEC-SYSTEM/osd220.elf", ps2loc); // 2nd gen PSX
#endif
    if (file_exists("mc0:/BOOT/BOOT2.ELF", ps2loc)) LoadElf("mc0:/BOOT/BOOT2.ELF", "mc0:/BOOT/");
    if (file_exists("mc0:/FORTUNA/BOOT2.ELF", ps2loc)) LoadElf("mc0:/FORTUNA/BOOT2.ELF", "mc0:/FORTUNA/");
    if (file_exists("mc0:/APPS/BOOT.ELF", ps2loc)) LoadElf("mc0:/APPS/BOOT.ELF", "mc0:/APPS/");
    if (file_exists("mc0:/APPS/ULE.ELF", ps2loc)) LoadElf("mc0:/APPS/ULE.ELF", "mc0:/APPS/");
    if (file_exists("mc0:/BOOT/ULE.ELF", ps2loc)) LoadElf("mc0:/BOOT/ULE.ELF", "mc0:/BOOT/");
    if (file_exists("mc0:/APPS/WLE.ELF", ps2loc)) LoadElf("mc0:/APPS/WLE.ELF", "mc0:/APPS/");
    if (file_exists("mc0:/BOOT/WLE.ELF", ps2loc)) LoadElf("mc0:/BOOT/WLE.ELF", "mc0:/BOOT/");
    if (file_exists("mc1:/BOOT/BOOT.ELF", ps2loc)) LoadElf("mc1:/BOOT/BOOT.ELF", "mc1:/BOOT/");
    if (file_exists("mc1:/FORTUNA/BOOT2.ELF", ps2loc)) LoadElf("mc1:/FORTUNA/BOOT2.ELF", "mc1:/FORTUNA/");
    if (file_exists("mc1:/APPS/BOOT.ELF", ps2loc)) LoadElf("mc1:/APPS/BOOT.ELF", "mc1:/APPS/");
    if (file_exists("mc1:/APPS/ULE.ELF", ps2loc)) LoadElf("mc1:/APPS/ULE.ELF", "mc1:/APPS/");
    if (file_exists("mc1:/BOOT/ULE.ELF", ps2loc)) LoadElf("mc1:/BOOT/ULE.ELF", "mc1:/BOOT/");
    if (file_exists("mc1:/APPS/WLE.ELF", ps2loc)) LoadElf("mc1:/APPS/WLE.ELF", "mc1:/APPS/");
    if (file_exists("mc1:/BOOT/WLE.ELF", ps2loc)) LoadElf("mc1:/BOOT/WLE.ELF", "mc1:/BOOT/");
    if (file_exists("mc1:/FORTUNA/BOOT.ELF", ps2loc)) LoadElf("mc1:/FORTUNA/BOOT.ELF", "mc1:/FORTUNA/");

    return 0;
}
