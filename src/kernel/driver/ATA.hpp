/**
 * IDE.hpp
 *
 * @since 13.04.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef KERNEL_ATA_HPP_
#define KERNEL_ATA_HPP_

#include "PCI.hpp"
#include <lib/sync/Mutex.hpp>
#include <lib/File.hpp>

namespace kernel {

   namespace driver {

      /**
       * Represents the ATA controller aka IDE drives.
       *
       * @section Register
       *
       * <table>
       * <tr>
       *    <th> Description </th><th> 1. Channel </th><th> 2. Channel </th><th> 3. Channel </th><th> 4. Channel </th><th> Read/Write</th>
       * </tr>
       * <tr>
       *    <td> Datenregister </td><td> 0x1F0 </td><td> 0x170 </td><td> 0x1E8 </td><td> 0x168 </td><td> R/W</td>
       * </tr>
       * <tr>
       *    <td> Fehlerregister </td><td> 0x1F1 </td><td> 0x171 </td><td> 0x1E9 </td><td> 0x169 </td><td> R</td>
       * </tr>
       * <tr>
       *    <td> Sektoranzahl </td><td> 0x1F2 </td><td> 0x172 </td><td> 0x1EA </td><td> 0x16A </td><td> R/W</td>
       * </tr>
       * <tr>
       *    <td> LBA niedrig </td><td> 0x1F3 </td><td> 0x173 </td><td> 0x1EB </td><td> 0x16B </td><td> R/W</td>
       * </tr>
       * <tr>
       *    <td> LBA mittel </td><td> 0x1F4 </td><td> 0x174 </td><td> 0x1EC </td><td> 0x16C </td><td> R/W</td>
       * </tr>
       * <tr>
       *    <td> LBA hoch </td><td> 0x1F5 </td><td> 0x175 </td><td> 0x1ED </td><td> 0x16D </td><td> R/W</td>
       * </tr>
       * <tr>
       *    <td> Geräteauswahl </td><td> 0x1F6 </td><td> 0x176 </td><td> 0x1EE </td><td> 0x16E </td><td> R/W</td>
       * </tr>
       * <tr>
       *    <td> Status/Kommandoregister </td><td> 0x1F7 </td><td> 0x177 </td><td> 0x1EF </td><td> 0x16F </td><td> R(Status) / W(Kommando)</td>
       * </tr>
       * <tr>
       *    <td> Status2/Steuerregister </td><td> 0x3F6 </td><td> 0x376 </td><td> 0x3EE </td><td> 0x36E </td><td> R(Status2) / W(Steuer)</td>
       * </tr>
       * </table>
       *
       * @subsection ds  Device-Selection
       * <table>
       * <tr>
       * <th width="60pt"> 7 </th>
       * <th width="60pt"> 6 </th>
       * <th width="60pt"> 5 </th>
       * <th width="60pt"> 4 </th>
       * <th width="60pt"> 3 </th>
       * <th width="60pt"> 2 </th>
       * <th width="60pt"> 1 </th>
       * <th width="60pt"> 0 </th>
       * </tr>
       * <tr>
       * <td> 1</td>
       * <td>1</td>
       * <td>1</td>
       * <td>DR</td>
       * <td colspan="4" align="center"> LBA Bit 27-24</td>
       * </tr>
       * </table>
       * <ul>
       * <li> DR: Drive Master(0) / Slave(1)</li>
       * </ul>
       *
       * @subsection scr State-/Command-Register
       * <table>
       * <tr>
       * <th width="60pt"> 7 </th>
       * <th width="60pt"> 6 </th>
       * <th width="60pt"> 5 </th>
       * <th width="60pt"> 4 </th>
       * <th width="60pt"> 3 </th>
       * <th width="60pt"> 2 </th>
       * <th width="60pt"> 1 </th>
       * <th width="60pt"> 0 </th>
       * </tr>
       * <tr>
       * <td> BSY </td>
       * <td>DRDY</td>
       * <td>DF</td>
       * <td>#</td>
       * <td>DRQ </td>
       * <td> r </td>
       * <td> r </td>
       * <td> ERR</td>
       * </tr>
       * </table>
       * <ul><li> BSY: Busy</li>
       * <li> DRDY: Drive Ready. Gesetzt, wenn das Laufwerk Kommandos annehmen kann.</li>
       * <li> DF: Device Fault. Betrifft Fehler, die nicht im Fehlerregister angezeigt werden.</li>
       * <li> #: Befehlsabhängig.</li>
       * <li> DRQ: Ein Zugriff auf das Datenregister wird erwartet</li>
       * <li> ERR: Error-Bit. Nach Fehler gesetzt, Beschreibung im Fehlerregister.</li></ul>
       *
       * @subsection s2cr Status2-/Control-Register
       * <table>
       * <tr>
       * <th width="60pt"> 7 </th>
       * <th width="60pt"> 6 </th>
       * <th width="60pt"> 5 </th>
       * <th width="60pt"> 4 </th>
       * <th width="60pt"> 3 </th>
       * <th width="60pt"> 2 </th>
       * <th width="60pt"> 1 </th>
       * <th width="60pt"> 0 </th>
       * </tr>
       * <tr>
       * <td> HOB </td>
       * <td>r</td>
       * <td>r</td>
       * <td>r</td>
       * <td>r </td>
       * <td> SRST </td>
       * <td> nIEN </td>
       * <td> 0 </td>
       * </tr>
       * </table>
       * <ul><li> HOB-Bit - needs to be deleted? at initialisation</li>
       * <li> SRST: Software Reset</li>
       * <li> nIEN: Wenn gesetzt, sendet der Controller keine IRQs mehr</li>
       * <li> 0: Immer 0</li></ul>
       *
       * @subsection er Error-Register
       *
       * <table>
       * <tr>
       * <th width="60pt"> 7 </th>
       * <th width="60pt"> 6 </th>
       * <th width="60pt"> 5 </th>
       * <th width="60pt"> 4 </th>
       * <th width="60pt"> 3 </th>
       * <th width="60pt"> 2 </th>
       * <th width="60pt"> 1 </th>
       * <th width="60pt"> 0 </th>
       * </tr>
       * <tr>
       * <td> ICRC </td><td>UNC</td><td>MC</td><td>IDNF</td><td>MCR </td><td>ABRT </td><td> NM </td><td> r</td>
       * </tr>
       * </table>
       * <ul>
       *    <li> ICRC: CRC-Vergleichsfehler bei Ultra-DMA-Übertragungen</li>
       *    <li> UNC: Fehler im Datenfeld des Sektors</li>
       *    <li> MC: Das Speichermedium (CD) wurde gewechselt</li>
       *    <li> IDNF: Die Sektor-ID wurde nicht gefunden</li>
       *    <li> MCR: Bei einem CD-Laufwerk wurde der Auswurf-Knopf gedrückt</li>
       *    <li> ABRT: Command aborted (falsches Kommando oder Gerätefehler)</li>
       *    <li> NM: No Media</li>
       *    <li> r: reserviert</li>
       * </ul>
       *
       * @note http://wiki.osdev.org/IDE
       * @note http://www.ata-atapi.com/pata.html
       * @note http://www.lowlevel.eu/wiki/ATA
       *
       * @attention Keep in mind a Sector in LBA is 512 Byte in size!
       *
       * @since 13.04.2010
       * @date 18.10.2011
       * @author Arne Simon => email::[arne_simon@gmx.de]
       */
      class ATA: public lib::File {
         protected:

            static const uint8 ATA_SR_BSY = 0x80;
            static const uint8 ATA_SR_DRDY = 0x40;
            static const uint8 ATA_SR_DF = 0x20;
            static const uint8 ATA_SR_DSC = 0x10;
            static const uint8 ATA_SR_DRQ = 0x08;
            static const uint8 ATA_SR_CORR = 0x04;
            static const uint8 ATA_SR_IDX = 0x02;
            static const uint8 ATA_SR_ERR = 0x01;

            static const uint8 ATA_ER_BBK = 0x80;
            static const uint8 ATA_ER_UNC = 0x40;
            static const uint8 ATA_ER_MC = 0x20;
            static const uint8 ATA_ER_IDNF = 0x10;
            static const uint8 ATA_ER_MCR = 0x08;
            static const uint8 ATA_ER_ABRT = 0x04;
            static const uint8 ATA_ER_TK0NF = 0x02;
            static const uint8 ATA_ER_AMNF = 0x01;

            static const uint8 ATA_CMD_READ_PIO = 0x20;
            static const uint8 ATA_CMD_READ_PIO_EXT = 0x24;
            static const uint8 ATA_CMD_READ_DMA = 0xC8;
            static const uint8 ATA_CMD_READ_DMA_EXT = 0x25;
            static const uint8 ATA_CMD_WRITE_PIO = 0x30;
            static const uint8 ATA_CMD_WRITE_PIO_EXT = 0x34;
            static const uint8 ATA_CMD_WRITE_DMA = 0xCA;
            static const uint8 ATA_CMD_WRITE_DMA_EXT = 0x35;
            static const uint8 ATA_CMD_CACHE_FLUSH = 0xE7;
            static const uint8 ATA_CMD_CACHE_FLUSH_EXT = 0xEA;
            static const uint8 ATA_CMD_PACKET = 0xA0;
            static const uint8 ATA_CMD_IDENTIFY_PACKET = 0xA1;
            static const uint8 ATA_CMD_IDENTIFY = 0xEC;

            static const uint8 ATAPI_CMD_READ = 0xA8;
            static const uint8 ATAPI_CMD_EJECT = 0x1B;

            static const uint8 ATA_IDENT_DEVICETYPE = 0;
            static const uint8 ATA_IDENT_CYLINDERS = 2;
            static const uint8 ATA_IDENT_HEADS = 6;
            static const uint8 ATA_IDENT_SECTORS = 12;
            static const uint8 ATA_IDENT_SERIAL = 20;
            static const uint8 ATA_IDENT_MODEL = 54;
            static const uint8 ATA_IDENT_CAPABILITIES = 98;
            static const uint8 ATA_IDENT_FIELDVALID = 106;
            static const uint8 ATA_IDENT_MAX_LBA = 120;
            static const uint8 ATA_IDENT_COMMANDSETS = 164;
            static const uint8 ATA_IDENT_MAX_LBA_EXT = 200;

            // Types
            static const uint8 PATA = 0x00;
            static const uint8 PATAPI = 0x01;
            static const uint8 SATA = 0x02;
            static const uint8 SATAPI = 0x03;

            static const uint8 ATA_MASTER = 0x00;
            static const uint8 ATA_SLAVE = 0x01;

            static const uint8 ATA_REG_DATA = 0x00;
            static const uint8 ATA_REG_ERROR = 0x01;
            static const uint8 ATA_REG_FEATURES = 0x01;
            static const uint8 ATA_REG_SECCOUNT0 = 0x02;
            static const uint8 ATA_REG_LBA0 = 0x03;
            static const uint8 ATA_REG_LBA1 = 0x04;
            static const uint8 ATA_REG_LBA2 = 0x05;
            static const uint8 ATA_REG_HDDEVSEL = 0x06;
            static const uint8 ATA_REG_COMMAND = 0x07;
            static const uint8 ATA_REG_STATUS = 0x07;
            static const uint8 ATA_REG_SECCOUNT1 = 0x08;
            static const uint8 ATA_REG_LBA3 = 0x09;
            static const uint8 ATA_REG_LBA4 = 0x0A;
            static const uint8 ATA_REG_LBA5 = 0x0B;
            static const uint8 ATA_REG_CONTROL = 0x0C;
            static const uint8 ATA_REG_ALTSTATUS = 0x0C;
            static const uint8 ATA_REG_DEVADDRESS = 0x0D;

            // Channels
            static const uint8 ATA_PRIMARY = 0x00;
            static const uint8 ATA_SECONDARY = 0x01;

            // Directions
            static const uint8 ATA_READ = 0x00;
            static const uint8 ATA_WRITE = 0x01;

            struct ChannelRegisters {
                  uint16 base; ///< I/O Base.
                  uint16 ctrl; ///< Control Base
                  uint16 bmide; ///< Bus Master IDE
                  uint8 nIEN; ///< nIEN (No Interrupt);
            } channels[ 2 ];

            uint8 *buf;
            uint8 irq_invoked;
            uint8 atapi_packet[ 12 ];

            PCI::Device* _device;

            uint8 polling( uint8 channel, uint32 advanced_check );

            uint8 read( uint8 channel, uint8 reg );

            void write( uint8 channel, uint8 reg, uint8 data );

            void read_buffer( uchar channel, uchar reg, uint32 buffer, uint32 quads );

            void initialize( uint32 BAR0, uint32 BAR1, uint32 BAR2, uint32 BAR3, uint32 BAR4 );

         public:
            /**
             * The Master Boot Record.
             */
            struct MBR {
                  uint8 bootstrap[ 436 ];
                  uint8 disk_id[ 10 ];
                  struct {
                        uint8 indicator; ///< 0 = not bootable, 0x80 = bootable
                        uint8 start_head;
                        uint16 start_sector_cylinder;
                        uint8 id;
                        uint8 end_head;
                        uint16 end_sector_cylinder;
                        uint32 sector; ///< The LBA starting sector.
                        uint32 size; ///< Sectors in partition.
                  } partition[ 4 ];
                  uint8 validation[ 2 ]; ///< should be { 0x55, 0xAA }

                  bool isValid();
            }__attribute__((packed));

            /**
             * An ide drive.
             */
            class Drive {
                  friend class ATA;
               public:
                  ATA* ata; ///< The ATA controller
                  uint8 reserved; ///< 0 = empty, 1 = This drive really exists
                  uint8 channel; ///< 0  = primary channel, 1 = secondary channel
                  uint8 drive; ///< 0 = master drive, 1 = slave drive
                  uint16 type; ///< 0 = ATA, 1 = ATAPI
                  uint16 signature; ///< Drive signature.
                  uint16 capabilities; ///< Features.
                  uint32 commandSets; ///< Command sets supported.
                  uint32 size; ///< Size in sectors.
                  uchar model[ 41 ]; ///< Name of the model.
                  uint8 package[ 2 ];

                  uint8 print_error( uint8 err );

                  uchar access( uint8 direction, uint32 lba, uint8 numsects, void* edi );

               public:

                  /**
                   * Reads the master boot record from the drive.
                   */
                  void read( MBR* mbr );

                  void write( MBR* mbr );

                  void readSector( uint8 numsects, uint32 lba, void* edi );

                  void writeSector( uint8 numsects, uint32 lba, void* edi );
            };

            Drive drives[ 4 ];
            Files files;

            ATA( PCI::Device* Dev );

            virtual ~ATA();
      };

   }

}

#endif /* KERNEL_ATA_HPP_ */
