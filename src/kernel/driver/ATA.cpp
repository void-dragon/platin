/**
 * IDE.cpp
 *
 * @since 13.04.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "ATA.hpp"
#include <kernel/System.hpp>
#include <lib/std.hpp>
#include <lib/Exception.hpp>

namespace kernel {

  namespace driver {

    bool ATA::MBR::isValid() {
      return validation[ 0 ] == 0x55 && validation[ 1 ] == 0xAA;
    }

    uchar ATA::Drive::access( uint8 direction, uint32 lba, uint8 numsects, void* edi ) {
      uint8 lba_mode /* 0: CHS, 1:LBA28, 2: LBA48 */, dma /* 0: No DMA, 1: DMA */, cmd;
      uint8 lba_io[ 6 ];
      uint32 slavebit = drive; // Read the Drive [Master/Slave]
      uint32 bus = ata->channels[ channel ].base; // Bus Base, like 0x1F0 which is also data port.
      uint32 words = 256; // Almost every ATA drive has a sector-size of 512-byte.
      uint16 cyl, i;
      uint8 head, sect, err;
      ata->irq_invoked = 0x02;

      ata->write( channel, ATA_REG_CONTROL, ata->channels[ channel ].nIEN = 0x02 );

      // (I) Select one from LBA28, LBA48 or CHS;
      if ( lba >= 0x10000000 ) { // Sure Drive should support LBA in this case, or you are
        // giving a wrong LBA.
        // LBA48:
        lba_mode = 2;
        lba_io[ 0 ] = ( lba & 0x000000FF ) >> 0;
        lba_io[ 1 ] = ( lba & 0x0000FF00 ) >> 8;
        lba_io[ 2 ] = ( lba & 0x00FF0000 ) >> 16;
        lba_io[ 3 ] = ( lba & 0xFF000000 ) >> 24;
        lba_io[ 4 ] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
        lba_io[ 5 ] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
        head = 0; // Lower 4-bits of HDDEVSEL are not used here.
      }
      else if ( capabilities & 0x200 ) { // Drive supports LBA?
        // LBA28:
        lba_mode = 1;
        lba_io[ 0 ] = ( lba & 0x00000FF ) >> 0;
        lba_io[ 1 ] = ( lba & 0x000FF00 ) >> 8;
        lba_io[ 2 ] = ( lba & 0x0FF0000 ) >> 16;
        lba_io[ 3 ] = 0; // These Registers are not used here.
        lba_io[ 4 ] = 0; // These Registers are not used here.
        lba_io[ 5 ] = 0; // These Registers are not used here.
        head = ( lba & 0xF000000 ) >> 24;
      }
      else {
        // CHS:
        lba_mode = 0;
        sect = ( lba % 63 ) + 1;
        cyl = ( lba + 1 - sect ) / ( 16 * 63 );
        lba_io[ 0 ] = sect;
        lba_io[ 1 ] = ( cyl >> 0 ) & 0xFF;
        lba_io[ 2 ] = ( cyl >> 8 ) & 0xFF;
        lba_io[ 3 ] = 0;
        lba_io[ 4 ] = 0;
        lba_io[ 5 ] = 0;
        head = ( lba + 1 - sect ) % ( 16 * 63 ) / ( 63 ); // Head number is written to HDDEVSEL lower 4-bits.
      }

      dma = 0; // we don't support DMA yet!!

      // wait if the drive is busy
      while ( ata->read( channel, ATA_REG_STATUS ) & ATA_SR_BSY )
        ;

      // select drive from the controller
      if ( lba_mode == 0 )
        ata->write( channel, ATA_REG_HDDEVSEL, 0xA0 | ( slavebit << 4 ) | head ); // Drive & CHS.
      else
        ata->write( channel, ATA_REG_HDDEVSEL, 0xE0 | ( slavebit << 4 ) | head ); // Drive & LBA

      // write parameters
      if ( lba_mode == 2 ) {
        ata->write( channel, ATA_REG_SECCOUNT1, 0 );
        ata->write( channel, ATA_REG_LBA3, lba_io[ 3 ] );
        ata->write( channel, ATA_REG_LBA4, lba_io[ 4 ] );
        ata->write( channel, ATA_REG_LBA5, lba_io[ 5 ] );
      }
      ata->write( channel, ATA_REG_SECCOUNT0, numsects );
      ata->write( channel, ATA_REG_LBA0, lba_io[ 0 ] );
      ata->write( channel, ATA_REG_LBA1, lba_io[ 1 ] );
      ata->write( channel, ATA_REG_LBA2, lba_io[ 2 ] );

      if ( lba_mode == 0 && dma == 0 && direction == 0 )
        cmd = ATA_CMD_READ_PIO;
      if ( lba_mode == 1 && dma == 0 && direction == 0 )
        cmd = ATA_CMD_READ_PIO;
      if ( lba_mode == 2 && dma == 0 && direction == 0 )
        cmd = ATA_CMD_READ_PIO_EXT;
      if ( lba_mode == 0 && dma == 1 && direction == 0 )
        cmd = ATA_CMD_READ_DMA;
      if ( lba_mode == 1 && dma == 1 && direction == 0 )
        cmd = ATA_CMD_READ_DMA;
      if ( lba_mode == 2 && dma == 1 && direction == 0 )
        cmd = ATA_CMD_READ_DMA_EXT;
      if ( lba_mode == 0 && dma == 0 && direction == 1 )
        cmd = ATA_CMD_WRITE_PIO;
      if ( lba_mode == 1 && dma == 0 && direction == 1 )
        cmd = ATA_CMD_WRITE_PIO;
      if ( lba_mode == 2 && dma == 0 && direction == 1 )
        cmd = ATA_CMD_WRITE_PIO_EXT;
      if ( lba_mode == 0 && dma == 1 && direction == 1 )
        cmd = ATA_CMD_WRITE_DMA;
      if ( lba_mode == 1 && dma == 1 && direction == 1 )
        cmd = ATA_CMD_WRITE_DMA;
      if ( lba_mode == 2 && dma == 1 && direction == 1 )
        cmd = ATA_CMD_WRITE_DMA_EXT;

      ata->write( channel, ATA_REG_COMMAND, cmd );

      if ( dma )
        if ( direction == 0 ) {
          // DMA Read.
        }
        else {
          // DMA Write.
        }
      else if ( direction == 0 )
        // PIO Read.
        for ( i = 0; i < numsects; i++ ) {
          // polling, set error and exit if there is
          if ( ( err = ata->polling( channel, 1 ) ) )
            return err;

          lib::ins( bus, ( uint32 ) edi, 2 * words );

          edi = ( void* ) ( ( uint32 ) edi + ( words * 2 ) );
        }
      else {
        // PIO Write.
        for ( i = 0; i < numsects; i++ ) {
          // polling, set error and exit if there is
          if ( ( err = ata->polling( channel, 0 ) ) )
            return err;

          lib::outs( bus, ( uint32 ) edi, 2 * words );

          edi = ( void* ) ( ( uint32 ) edi + ( words * 2 ) );
        }

        ata->write( channel, ATA_REG_COMMAND, (uint8 []) {ATA_CMD_CACHE_FLUSH,
          ATA_CMD_CACHE_FLUSH,
          ATA_CMD_CACHE_FLUSH_EXT}[ lba_mode ] );

        if ( ( err = ata->polling( channel, 0 ) ) )
          return err;
      }

      return 0;
    }

    void ATA::Drive::readSector( uint8 numsects, uint32 lba, void* edi ) {

      // check if the drive presents
      if ( drive > 3 || reserved == 0 ) {
        package[ 0 ] = 0x1; // drive not found!
        lib::Exception::throwing( "ATA - drive not found!" );
      }
      // check if inputs are valid
      else if ( ( ( lba + numsects ) > size ) && ( type == PATA ) ) {
        package[ 0 ] = 0x2; // seeking to invalid position.
        lib::Exception::throwing( "seeking invalid position!" );
      } // read in PIO mode through polling & IRQ
      else {
        uint8 err = 0;
        if ( type == PATA ) {
          err = access( ATA_READ, lba, numsects, edi );
        }
        else if ( type == PATAPI ) {
          lib::Exception::throwing( "read ATAPI!" );
          for ( int i = 0; i < numsects; i++ ) {
            // err = ide_atapi_read( drive, lba + i, 1, es, edi + ( i * 2048 ) );
          }
        }

        package[ 0 ] = print_error( err );
      }

    }

    void ATA::Drive::writeSector( uint8 numsects, uint32 lba, void* edi ) {
      // check if the drive is present
      if ( drive > 3 || reserved == 0 ) {
        package[ 0 ] = 0x1; // Drive Not Found!
      }
      // check if inputs are valid
      else if ( ( ( lba + numsects ) > size ) && ( type == PATA ) ) {
        package[ 0 ] = 0x2; // Seeking to invalid position.
      }
      // 3: Read in PIO Mode through Polling & IRQs:
      else {
        uint8 err = 0;
        if ( type == PATA )
          err = access( ATA_WRITE, lba, numsects, edi );
        else if ( type == PATAPI )
          err = 4; // Write-Protected.
        package[ 0 ] = print_error( err );
      }
    }

    void ATA::Drive::read( MBR* mbr ) {
      readSector( 1, 0, mbr );
    }

    void ATA::Drive::write( MBR* mbr ) {
      writeSector( 1, 0, mbr );
    }

    uint8 ATA::Drive::print_error( uint8 err ) {
      if ( err == 0 )
        return err;

      system->video.color( Video::LightRed );

      system->video << "ATA: ";
      system->video << (const char *[]) {"Primary-", "Secondary-"}[ channel ]; // Use the channel as an index into the array
      system->video << (const char *[]) {"Master ", "Slave "}[ drive ]; // Same as above, using the drive
      system->video << model;
      system->video << " ";

      if ( err == 1 ) {
        system->video << "- Device Fault\n";
        err = 19;
      }
      else if ( err == 2 ) {
        uint8 st = ata->read( channel, ATA_REG_ERROR );
        if ( st & ATA_ER_AMNF ) {
          system->video << "- No Address Mark Found\n";
          err = 7;
        }
        if ( st & ATA_ER_TK0NF ) {
          system->video << "- No Media or Media Error\n";
          err = 3;
        }
        if ( st & ATA_ER_ABRT ) {
          system->video << "- Command Aborted\n";
          err = 20;
        }
        if ( st & ATA_ER_MCR ) {
          system->video << "- No Media or Media Error\n";
          err = 3;
        }
        if ( st & ATA_ER_IDNF ) {
          system->video << "- ID mark not Found\n";
          err = 21;
        }
        if ( st & ATA_ER_MC ) {
          system->video << "- No Media or Media Error\n";
          err = 3;
        }
        if ( st & ATA_ER_UNC ) {
          system->video << "- Uncorrectable Data Error\n";
          err = 22;
        }
        if ( st & ATA_ER_BBK ) {
          system->video << "- Bad Sectors\n";
          err = 13;
        }
      }
      else if ( err == 3 ) {
        system->video << "- Reads Nothing\n";
        err = 23;
      }
      else if ( err == 4 ) {
        system->video << "- Write Protected\n";
        err = 8;
      }

      system->video.color( Video::LightGrey );

      return err;
    }

    ATA::ATA( PCI::Device* Dev ) {
      buf = new uint8[ 2048 ];

      for ( uint8 i = 0; i < 4; ++i ) {
        drives[ i ].ata = this;
      }

      atapi_packet[ 0 ] = 0xA8;
      atapi_packet[ 1 ] = 0;
      atapi_packet[ 2 ] = 0;
      atapi_packet[ 3 ] = 0;
      atapi_packet[ 4 ] = 0;
      atapi_packet[ 5 ] = 0;
      atapi_packet[ 6 ] = 0;
      atapi_packet[ 7 ] = 0;
      atapi_packet[ 8 ] = 0;
      atapi_packet[ 9 ] = 0;
      atapi_packet[ 10 ] = 0;
      atapi_packet[ 11 ] = 0;

      _device = Dev;

//      initialize( 0x1F0, 0x3F4, 0x170, 0x374, _device->config.standart.bar4_addr );
      initialize( _device->config.standart.bar0_addr, _device->config.standart.bar1_addr,
          _device->config.standart.bar2_addr, _device->config.standart.bar3_addr, _device->config.standart.bar4_addr );

    }

    uint8 ATA::polling( uint8 channel, uint32 advanced_check ) {

      // delay of 400 nanosecond
      for ( int i = 0; i < 4; i++ )
        read( channel, ATA_REG_ALTSTATUS ); // Reading the Alternate Status port wastes 100ns.

      while ( read( channel, ATA_REG_STATUS ) & ATA_SR_BSY )
        ; // Wait for BSY to be zero.

      if ( advanced_check ) {
        uint8 state = read( channel, ATA_REG_STATUS ); // Read Status Register.

        if ( state & ATA_SR_ERR )
          return 2; // Error.

        if ( state & ATA_SR_DF )
          return 1; // Device Fault.

        // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
        if ( ( state & ATA_SR_DRQ ) == 0 )
          return 3; // DRQ should be set

      }

      return 0; // No Error.

    }

    uint8 ATA::read( uint8 channel, uint8 reg ) {
      uint8 result = 0;

      if ( reg > 0x07 && reg < 0x0C )
        write( channel, ATA_REG_CONTROL, 0x80 | channels[ channel ].nIEN );

      if ( reg < 0x08 )
        result = lib::inb( channels[ channel ].base + reg - 0x00 );
      else if ( reg < 0x0C )
        result = lib::inb( channels[ channel ].base + reg - 0x06 );
      else if ( reg < 0x0E )
        result = lib::inb( channels[ channel ].ctrl + reg - 0x0A );
      else if ( reg < 0x16 )
        result = lib::inb( channels[ channel ].bmide + reg - 0x0E );

      if ( reg > 0x07 && reg < 0x0C )
        write( channel, ATA_REG_CONTROL, channels[ channel ].nIEN );

      return result;
    }

    void ATA::write( uint8 channel, uint8 reg, uint8 data ) {
      if ( reg > 0x07 && reg < 0x0C )
        write( channel, ATA_REG_CONTROL, 0x80 | channels[ channel ].nIEN );

      if ( reg < 0x08 )
        lib::outb( channels[ channel ].base + reg - 0x00, data );
      else if ( reg < 0x0C )
        lib::outb( channels[ channel ].base + reg - 0x06, data );
      else if ( reg < 0x0E )
        lib::outb( channels[ channel ].ctrl + reg - 0x0A, data );
      else if ( reg < 0x16 )
        lib::outb( channels[ channel ].bmide + reg - 0x0E, data );

      if ( reg > 0x07 && reg < 0x0C )
        write( channel, ATA_REG_CONTROL, channels[ channel ].nIEN );
    }

    void ATA::read_buffer( uchar channel, uchar reg, uint32 buffer, uint32 quads ) {
      if ( reg > 0x07 && reg < 0x0C )
        write( channel, ATA_REG_CONTROL, 0x80 | channels[ channel ].nIEN );

      if ( reg < 0x08 )
        lib::ins( channels[ channel ].base + reg - 0x00, buffer, quads );
      else if ( reg < 0x0C )
        lib::ins( channels[ channel ].base + reg - 0x06, buffer, quads );
      else if ( reg < 0x0E )
        lib::ins( channels[ channel ].ctrl + reg - 0x0A, buffer, quads );
      else if ( reg < 0x16 )
        lib::ins( channels[ channel ].bmide + reg - 0x0E, buffer, quads );

      if ( reg > 0x07 && reg < 0x0C )
        write( channel, ATA_REG_CONTROL, channels[ channel ].nIEN );
    }

    void ATA::initialize( uint32 BAR0, uint32 BAR1, uint32 BAR2, uint32 BAR3, uint32 BAR4 ) {

      int j, i, k, count = 0;

      // Detect I/O ports which interface IDE controller.
      // If no bars are zero use the legacy addresses.
      channels[ ATA_PRIMARY ].base = ( BAR0 & 0xFFFFFFFC ) + 0x1F0 * ( !BAR0 );
      channels[ ATA_PRIMARY ].ctrl = ( BAR1 & 0xFFFFFFFC ) + 0x3F4 * ( !BAR1 );
      channels[ ATA_SECONDARY ].base = ( BAR2 & 0xFFFFFFFC ) + 0x170 * ( !BAR2 );
      channels[ ATA_SECONDARY ].ctrl = ( BAR3 & 0xFFFFFFFC ) + 0x374 * ( !BAR3 );
      channels[ ATA_PRIMARY ].bmide = ( BAR4 & 0xFFFFFFFC ) + 0; // Bus Master IDE
      channels[ ATA_SECONDARY ].bmide = ( BAR4 & 0xFFFFFFFC ) + 8; // Bus Master IDE
      channels[ ATA_PRIMARY ].nIEN = 0x02;
      channels[ ATA_SECONDARY ].nIEN = 0x02;

      //software reset
//         write( ATA_PRIMARY, ATA_REG_CONTROL, 6u );
//         write( ATA_SECONDARY, ATA_REG_CONTROL, 6u );
//         write( ATA_PRIMARY, ATA_REG_HDDEVSEL, 0xA0 | ( 1 << 4 ) );
//         write( ATA_PRIMARY, ATA_REG_HDDEVSEL, 0xA0 | ( 1 << 4 ) );
//         write( ATA_PRIMARY, ATA_REG_CONTROL, 6u );
//         write( ATA_SECONDARY, ATA_REG_CONTROL, 6u );

      // Disable IRQs
      write( ATA_PRIMARY, ATA_REG_CONTROL, 2u );
      write( ATA_SECONDARY, ATA_REG_CONTROL, 2u );

      // Detect ATA-ATAPI Devices:
      for ( i = 0; i < 2; i++ )
        for ( j = 0; j < 2; j++ ) {

          unsigned char err = 0, type = PATA, status;
          drives[ count ].reserved = 0; // Assuming that no drive here.

          // Select Drive:
          write( i, ATA_REG_HDDEVSEL, 0xA0 | ( j << 4 ) ); // Select Drive.
          //sleep( 1 ); // Wait 1ms for drive select to work.

          // Send ATA Identify Command:
          write( i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY );
          //sleep( 1 ); // This function should be implemented in your OS. which waits for 1 ms.

          // Polling:
          if ( read( i, ATA_REG_STATUS ) == 0 ) {
            continue; // If Status = 0, No Device.
          }

          while ( 1 ) {
            status = read( i, ATA_REG_STATUS );

            if ( ( status & ATA_SR_ERR ) ) {
              err = 1;
              break;
            }

            if ( !( status & ATA_SR_BSY ) && ( status & ATA_SR_DRQ ) )
              break; // Everything is right.
          }

          // Probe for ATAPI Devices:

          if ( err != 0 ) {
            uchar cl = read( i, ATA_REG_LBA1 );
            uchar ch = read( i, ATA_REG_LBA2 );

            if ( cl == 0x14 && ch == 0xEB )
              type = PATAPI;
            else if ( cl == 0x69 && ch == 0x96 )
              type = PATAPI;
            else
              continue; // Unknown Type (may not be a device).

            write( i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET );
            //sleep( 1 );
          }

          // Read Identification Space of the Device:
          read_buffer( i, ATA_REG_DATA, ( uint32 ) buf, 512 );

          // Read Device Parameters:
          drives[ count ].reserved = 1;
          drives[ count ].type = type;
          drives[ count ].channel = i;
          drives[ count ].drive = j;
          drives[ count ].signature = ( *( uint16* ) ( buf + ATA_IDENT_DEVICETYPE ) );
          drives[ count ].capabilities = ( *( uint16* ) ( buf + ATA_IDENT_CAPABILITIES ) );
          drives[ count ].commandSets = ( *( uint16* ) ( buf + ATA_IDENT_COMMANDSETS ) );

          // get size
          if ( drives[ count ].commandSets & ( 1 << 26 ) ) {
            // Device uses 48-Bit Addressing:
            drives[ count ].size = *( ( uint32* ) ( buf + ATA_IDENT_MAX_LBA_EXT ) );
          }
          else {
            // Device uses CHS or 28-bit Addressing:
            drives[ count ].size = *( ( uint32* ) ( buf + ATA_IDENT_MAX_LBA ) );
          }

          //  String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
          for ( k = 0; k < 40; k += 2 ) {
            drives[ count ].model[ k ] = buf[ ATA_IDENT_MODEL + k + 1 ];
            drives[ count ].model[ k + 1 ] = buf[ ATA_IDENT_MODEL + k ];
          }
          drives[ count ].model[ 40 ] = 0; // terminate string

          count++;
        }

      // Print Summary:
      system->video.color( Video::LightBlue );
      for ( i = 0; i < 4; i++ ) {
        if ( drives[ i ].reserved == 1 ) {
          system->video << (const char *[]) {"ATA", "ATAPI"}[ drives[ i ].type ];

          int size = drives[ i ].size >> 1;
          int idx = 0;

          while ( size >= 1024 ) {
            size >>= 10; // Divided by 1024
            ++idx;
          }
          system->video << " Drive " << size;
          system->video << (const char *[]) {"KByte", "MByte", "GByte", "TByte"}[ idx ];
          system->video << " - " << drives[ i ].model << "\n";
        }
      }
      system->video.color( Video::LightGrey );

    }

    ATA::~ATA() {
      delete buf;
    }

  }

}
