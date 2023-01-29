# SD2PSX First Stage bootloader

This is a heavily modified version of HWCs OSDSysLauncher to use it as a first stage bootloader on SD2PSX.

It will check, if the current card has a file named "SD2PSX_BOOT" at the root. If so, first stage bootloader card is still active.

If the file is gone, bootloader assumes the load up of the actual boot card is finished and tries to boot the FMCB updates from this card.

*Note:* You need to make sure you have an FMCB based card for your console as boot up image in place. In the future this procedd will be simplified by SD2PSX.

## Usage

This makefile produces an ELF file named *sd2psx_bl-packed.ELF*.
To create the actual first stage bootloader the following steps need to be performed:

1. Create kelf from ELF: ```kelftool encrypt fmcb sd2psx_bl-packed.ELF SYSTEM.XLF```
2. Install KELF as FMCB binary (use either FMCB Installer or El Isras KELFBinder)
3. Copy ```sd2psx_bl-packed.ELF``` to ```mc0:/BOOT/BOOT.ELF```

The last step also enables the usage of this Bootloader with DEV1 Enabled modded consoles.

After installation, the resulting MC Image can be used as bootloader image by hanging it into the build process of sd2psx.
TODO: More Info on this
