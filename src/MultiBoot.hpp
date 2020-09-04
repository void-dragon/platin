/**
 * MultiBoot.hpp
 *
 * @since 03.12.2009
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef MULTIBOOT_HPP_
#define MULTIBOOT_HPP_

#include "cpp.hpp"

extern "C" struct ModAddr {
    uint32 start; ///< Adresse an der das Modul liegt.
    uint32 end;///< Adresse an der das Modul endet.
    uint32 string;///< Pointer zu einem 0-term. ASCII-String, der den Namen des Moduls enthält.
    uint32 reserved;///< Reserviert (0)
} __attribute__((packed));

extern "C" struct MmapAddr {
    uint32 Size;
    uint64 BaseAddr;
    uint64 Length;
    uint32 Type;
}__attribute__((packed));

extern "C" struct DriveAddr {
    uint32 size; ///< Grösse dieser Struktur.
    uint32 number; ///< BIOS Driver Number.
    uint32 mode; ///< 0 = CHS | 1 = LBA
    uint32 cylinders;
    uint32 heads;
    uint32 sectors;
    void* port; ///< Die I/O Ports die von dem Gert benutzt werden.
} __attribute__((packed));

extern "C" struct APMTable {
    uint16 version;
    uint16 cseg;
    uint32 offset;
    uint16 cseg_16;
    uint16 dseg;
    uint16 flags;
    uint16 cseg_len;
    uint16 cseg_16_len;
    uint16 dseg_len;
} __attribute__((packed));

/**
 * @note http://lowlevel.brainsware.org/wiki/index.php/Multiboot
 */
extern "C" struct MultiBoot {
    uint32 flags; ///< Gibt an welche Felder anwesend sind.

    uint32 mem_lower;///< Gibt die Größe des unteren Speichers in kB an.

    uint32 mem_upper;///<  Gibt die Größe des höheren Speichers in kB an.

    /**
     *  Dieses Doubleword ist in 4 Bytes unterteilt. Das 1. Byte steht für die BIOS-Laufwerknummer.
     *  Byte 2-4 gibt die Partition und Subpartition an. Wenn ein Wert 0xFF ist,
     *  wird keine Partitionierung (auf diesem Level) benutzt.
     */
    uint32 bootdevice;

    uint32 cmdline; ///<  Pouinter zu einem 0-terminated ASCII-String, der die Kommandos enthält, die an den Kernel übergeben wurden.

    uint32 mods_count;///<  Gibt an wie viele Module von GRUB geladen wurden

    ModAddr* mods_addr; ///< Pointer auf die erste Modulstruktur.

    uint32 syms[ 4 ];

    uint32 mmap_length;

    MmapAddr* mmap_addr;

    uint32 drives_length;

    DriveAddr* drives_addr;

    uint32 config_table;

    uint32 boot_loader_name;

    APMTable* apm_table;

    uint32 vbe_control_info;

    uint32 vbe_mode_info;

    uint16 vbe_mode;
    uint16 vbe_uinterface_seg;
    uint16 vbe_uinterface_off;
    uint16 vbe_uinterface_len;
} __attribute__((packed));

#endif /* MULTIBOOT_HPP_ */
