/**
 * MyFS.cpp
 *
 * @since 30.09.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "MyFS.hpp"
#include <lib/Exception.hpp>
#include <lib/std.hpp>
#include <kernel/Video.hpp>

namespace kernel {

  namespace driver {

    MyFS::MyFS( ATA::Drive* Drv, uint32 Partition ) :
      drive( Drv ), partition( Partition ), bitmap( 0 ) {
      ATA::MBR mbr;

      drive->read( &mbr ); // reading master boot record of the drive

      // look if master boot record is valid!
      if ( !mbr.isValid() ) {
        lib::Exception::throwing( "unvalid mbr" );
      }

      bitmap = 0;

      // optain the partition offset of the partition we use.
      partition_offset = mbr.partition[ partition ].sector;

      format();
      //read_table();

      root = new File( this, table.root_node );
    }

    void MyFS::read_table() {
      drive->readSector( 1, partition_offset, &table ); // read table data

      // allocate bitmap array
      if ( !bitmap ) {
        bitmap = new uint32[ table.bitmap_size * 512 ];
      }

      // read the damn fucking bitmap data from harddrive. YEEHAAAAA!!!!
      drive->readSector( table.bitmap_size, partition_offset + 1, bitmap );
    }

    void MyFS::write_table() {
      // write the table
      drive->writeSector( 1, partition_offset, &table );

      // write the bitmap data
      drive->writeSector( table.bitmap_size, partition_offset + 1, bitmap );
    }

    void MyFS::mark_lba( uint32 lba, bool used ) {
      uint32 idx = lba - partition_offset;

      if ( used )
        bitmap[ idx / 32 ] |= 1 << ( idx % 32 );
      else
        bitmap[ idx / 32 ] &= ~( 1 << ( idx % 32 ) );
    }

    uint32 MyFS::get_free_lba() {
      uint32 lba = 0;
      bool _search = true;

      mutex.enter();

      // look for a free block
      for ( uint32 i = 0; i < table.bitmap_size * 512 && _search; ++i ) {
        // if there is any free block
        if ( bitmap[ i ] != 0xffffffff ) {
          // look for the free block
          for ( uint32 j = 0; j < 32 && _search; ++j ) {
            if ( ~bitmap[ i ] & ( 1 << j ) ) {
              lba = partition_offset + i * 32 + j; // calculate the LBA
              bitmap[ i ] |= 1 << j; // mark the block as used
              _search = false; // exit the search
            }
          }
        }
      }

      table.free_block_count--; // we lost a free block :-(

      write_table();

      mutex.leave();

      return lba;
    }

    void MyFS::free( uint32 lba ) {
      lba -= partition_offset;
      uint32 idx = lba / 32;
      uint32 offset = lba % 32;

      bitmap[ idx ] &= ~( 1 << offset );
    }

    void MyFS::format() {
      table.bitmap_size = ( drive->size >> 12 ) + 1; // calculate the size of the bitmap

      table.free_block_count = drive->size; // Number of free blocks. In the beginning all blocks are free.

      if ( !bitmap )
        bitmap = new uint32[ table.bitmap_size * 512 ];

      mark_lba( partition_offset, true ); // first sector is used for the info table

      // mark sectors used for the bitmap
      for ( uint32 i = 1; i <= table.bitmap_size; ++i ) {
        mark_lba( partition_offset + i, true );
      }

      table.free_block_count -= 1 + table.bitmap_size; // update the count of free blocks

      table.root_node = get_free_lba(); // calc the position of the root node

      write_table(); // write the info table and bitmap

      // create and write the root node

      Node root;

      root.type = NODE_TYPE_DIR;
      root.size = 0;
      root.first = 0;
      root.second = 0;
      root.third = 0;
      root.fourth = 0;

      drive->writeSector( 1, table.root_node, &root );
    }

    void MyFS::File::read_blocks( void** data, uint32* size ) {
      *size = 0;

      *data = new uint8[ node.size ];

      for ( uint32 i = 0; i < NODE_DATA_SIZE && *size < node.size; ++i ) {
        ( ( uint8* ) *data )[ i ] = ( ( uint8* ) ( &node ) )[ i
            + NODE_DATA_START ];
        *size += 1;
      }

      // read first level of data blocks
      if ( *size < node.size ) {
        uint8 tmp[ 512 ];
        uint32 addr[ 128 ];

        for ( uint32 a = 0; addr[ a ] && a < 128; ++a ) {
          myfs->drive->readSector( 1, addr[ a ], tmp );

          for ( uint32 i = 0; *size < node.size && i < 512; ++i ) {
            ( ( uint8* ) *data )[ *size ] = ( ( uint8* ) ( &tmp ) )[ i ];
            *size += 1;
          }
        }
      }

    }

    void MyFS::File::write_blocks( void* data, uint32 size ) {
      // do clean-up first!
      // free unneeded blocks !

      node.size = 0;

      for ( uint32 i = 0; i < NODE_DATA_SIZE && node.size < size; ++i ) {
        ( ( uint8* ) ( &node ) )[ i + NODE_DATA_START ]
            = ( ( uint8* ) data )[ i ];
        node.size += 1;
      }

      myfs->drive->writeSector( 1, lba, &node );

      // writing frist level data blocks
      if ( node.size < size ) {
        uint8 tmp[ 512 ]; // a temporary block
        uint32 addr[ 128 ]; // first level block addresses
        uint32 a;

        if ( node.first ) {
          myfs->drive->readSector( 1, node.first, addr );
        }
        else {
          node.first = myfs->get_free_lba();

          lib::memset( addr, 0, 512 );
        }

        for ( a = 0; node.size < size && a < 128; ++a ) {

          lib::memset( tmp, 0, 512 );

          if ( not addr[ a ] ) {
            addr[ a ] = myfs->get_free_lba();
          }

          for ( uint32 i = 0; node.size < size && i < 512; ++i ) {
            ( ( uint8* ) ( &tmp ) )[ i ] = ( ( uint8* ) data )[ node.size ];
            node.size++;
          }

          myfs->drive->writeSector( 1, addr[ a ], tmp );
        }

        for ( a = 0; a < 128; ++a ) {

        }

      }
    }

    void MyFS::File::readFileList() {

      if ( node.type & NODE_TYPE_DIR ) {
        DirectoryEntry* e;
        void* data;
        uint32 data_size;
        lib::String *a;

        read_blocks( &data, &data_size );

        e = ( DirectoryEntry* ) data;
        do {
          a = new lib::String( e->name, e->size - sizeof(DirectoryEntry) );

          _files.put( a, new File( myfs, e->node, this ) ); // add new file to list
          e = ( DirectoryEntry* ) ( ( uint32 ) e + e->size );
        } while ( ( uint32 ) e < ( ( uint32 ) data + node.size ) );

      }

    }

    MyFS::File::File( MyFS* M, uint32 LBA, File* P ) :
      lba( LBA ), myfs( M ), parent( P ) {

      myfs->drive->readSector( 1, lba, &node );
    }

    uint32 MyFS::File::write( void* data, uint32 length ) {

      write_blocks( data, length );

      return length;
    }

    void MyFS::File::read( void** data, uint32* length ) {
      *data = new uint8[ node.size ];

      *length = 0;

      for ( uint32 i = 0; i < NODE_DATA_SIZE && *length < node.size; ++i ) {
        ( ( uint8* ) *data )[ i ] = ( ( uint8* ) ( &node ) )[ i
            + NODE_DATA_START ];
        ( *length )++;
      }
    }

    uint32 MyFS::File::read( void* data, uint32 length ) {
      return 0;
    }

    uint64 MyFS::File::size() {
      return node.size;
    }

    void* MyFS::File::range( uint32 start, uint32 size ) {
      return 0;
    }

    void MyFS::File::range( uint32 start, uint32 size, void* data ) {

    }

    MyFS::~MyFS() {
      write_table();
      delete bitmap;
    }

  }

}
