/**
 * Ext2.cpp
 *
 * @since 30.04.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "Ext2.hpp"
#include <kernel/System.hpp>
#include <lib/Exception.hpp>
#include <lib/String.hpp>
#include <lib/collection/Array.hpp>
#include <lib/std.hpp>

namespace kernel {

  namespace driver {

    Ext2::File::File( Ext2* FS, uint32 Id )
        : _fs( FS ), parent( 0 ), inode_id( Id ) {

      inode = new INode;

      load();
    }

    Ext2::File::File( Ext2* FS, uint32 Id, File* Parent )
        : _fs( FS ), parent( Parent ), inode_id( Id ) {

      inode = new INode;

      load();
    }

    uint32 Ext2::File::index() {
      return ( inode_id - 1 ) % _fs->sblock.inodeper_group;
    }

    uint32 Ext2::File::group() {
      return ( inode_id - 1 ) / _fs->sblock.inodeper_group;
    }

    void Ext2::File::load() {
      uint32 addr = _fs->block2lba( _fs->grp_desc_table[ group() ].inode_table ); // address of inode table
      uint32 idx = index(); // index of the inode in the table

      addr += ( idx * sizeof(INode) ) / 512; // jump to the sector where our inode is

      idx %= ( 512 / sizeof(INode) ); // calculate our index for the part of the inode table, we are reading

      // read the sector, where or inode is located in the table
      _fs->drive->readSector( 1, addr, _fs->tmp_inode_table );

      *inode = _fs->tmp_inode_table[ idx ]; // copy inode data
    }

    void Ext2::File::save() {
      uint32 addr = _fs->block2lba( _fs->grp_desc_table[ group() ].inode_table ); // address of inode table
      uint32 idx = index(); // index of the inode in the table

      addr += ( idx * sizeof(INode) ) / 512; // jump to the sector where our inode is

      idx %= ( 512 / sizeof(INode) ); // calculate our index for the part of the inode table, we are reading

      _fs->drive->readSector( 1, addr, _fs->tmp_inode_table ); // read on sector

      _fs->tmp_inode_table[ idx ] = *inode; // copy inode data

      _fs->drive->writeSector( 1, addr, _fs->tmp_inode_table ); // write on sector
    }

    void Ext2::File::read( void** data, uint32* size ) {
      *size = inode->size;
      *data = new uint8[ inode->blocks * 512 ]; // allocate blocks * 512 Byte

      uint8 * it = ( uint8* ) *data; // pointer to the current buffer location
      uint32 blks = 0; // blocks read
      uint8 blks_per_read = _fs->block_size / 512; // reading step

      // read direct blocks
      for ( uint8 i = 0; blks < inode->blocks && i < 12; ++i ) {
        _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( inode->block[ i ] ), it );
        it += _fs->block_size; // increment data pointer
        blks += blks_per_read; // increment blocks read
      }

      // read indirect blocks first level
      if ( blks < inode->blocks ) {
        uint32 max = _fs->block_size / sizeof(uint32);
        uint32* firstlevel = new uint32[ max ];

        _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( inode->first_indirect_block ), firstlevel );

        for ( uint32 i = 0; blks < inode->blocks && i < max; ++i ) {
          _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( firstlevel[ i ] ), it );
          it += _fs->block_size; // increment data pointer
          blks += blks_per_read; // increment blocks read
        }

        // read indirect blocks second level
        if ( blks < inode->blocks ) {
          uint32 max = _fs->block_size / sizeof(uint32);
          uint32* secondlevel = new uint32[ max ];

          _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( inode->double_indirect_block ), firstlevel );

          for ( uint32 i = 0; blks < inode->blocks && i < max; ++i ) {
            _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( firstlevel[ i ] ), secondlevel );

            for ( uint32 j = 0; blks < inode->blocks && j < max; ++j ) {
              _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( secondlevel[ j ] ), it );
              it += _fs->block_size; // increment data pointer
              blks += blks_per_read; // increment blocks read
            }
          }

          // read indirect blocks third level
          if ( blks < inode->blocks ) {
            uint32* thirdlevel = new uint32[ max ];

            _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( inode->triple_indirect_block ),
                firstlevel );

            for ( uint32 i = 0; blks < inode->blocks && i < max; ++i ) {
              _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( firstlevel[ i ] ), secondlevel );

              for ( uint32 j = 0; blks < inode->blocks && j < max; ++j ) {
                _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( secondlevel[ j ] ), thirdlevel );

                for ( uint32 k = 0; blks < inode->blocks && k < max; ++k ) {
                  _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( thirdlevel[ k ] ), it );
                  it += _fs->block_size; // increment data pointer
                  blks += blks_per_read; // increment blocks read
                }
              }
            }

            delete thirdlevel;
          }

          delete secondlevel;
        }

        delete firstlevel;
      }

    }

    uint32 Ext2::File::write( void* data, uint32 size ) {
      uint8* it = ( uint8* ) data;
      uint32 blks = 0, i = 0;
      uint8 blks_per_read = _fs->block_size >> 9; // block_size / 512

      uint32 needed_blks = ( size + 256 ) / 512;

      for ( i = 0; blks < needed_blks && i < 12; ++i ) {
        if ( inode->block[ i ] == 0 ) {
          inode->block[ i ] = _fs->alloc_block( group() );
        }

        _fs->drive->writeSector( _fs->block_sector_size, _fs->block2lba( inode->block[ i ] ), it );

        it += _fs->block_size;
        blks += blks_per_read;
      }

      // first level indirect blocks
      if ( blks < needed_blks ) {
        uint32 max = _fs->block_size / 4;
        uint32* firstlevel = new uint32[ max ];

        // allocate new block for first level indirect addresses if needed
        if ( inode->first_indirect_block == 0 ) {
          inode->first_indirect_block = _fs->alloc_block( group() );
        }
        else {
          _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( inode->first_indirect_block ), firstlevel );
        }

        // writing to the first level indirect blocks
        for ( i = 0; blks < needed_blks && i < max; ++i ) {
          if ( firstlevel[ i ] == 0 ) {
            firstlevel[ i ] = _fs->alloc_block( group() );
          }

          _fs->drive->writeSector( _fs->block_sector_size, _fs->block2lba( firstlevel[ i ] ), it );

          it += _fs->block_size;
          blks += blks_per_read;
        }

        // second level indirect blocks
        if ( blks < needed_blks ) {
          uint32* secondlevel = new uint32[ max ];

          // allocate new block for double level indirect addresses if needed
          if ( inode->double_indirect_block == 0 ) {
            inode->double_indirect_block = _fs->alloc_block( group() );
          }
          else {
            _fs->drive->readSector( _fs->block_sector_size, _fs->block2lba( inode->double_indirect_block ),
                firstlevel );
          }

          // triple level indirect blocks
          if ( blks < needed_blks ) {
            uint32* thirdlevel = new uint32[ max ];

            delete thirdlevel;
          }

          delete secondlevel;
        }

        for ( ; blks < inode->blocks && i < max; ++i ) {
          _fs->free_block( firstlevel[ i ] );
          firstlevel[ i ] = 0;
        }

        _fs->drive->writeSector( _fs->block_sector_size, _fs->block2lba( inode->first_indirect_block ), firstlevel );

        delete firstlevel;
      }

      // free block not needed
//         for ( ; blks < inode->blocks && i < 12; ++i ) {
//            _fs->free_block( inode->block[ i ] );
//            inode->block[ i ] = 0;
//         }

      return size;
    }

    uint64 Ext2::File::size() {
      return inode->size;
    }

    bool Ext2::File::hasFiles() {
      return inode->mode | MODE_FILE_DIR;
    }

    lib::File::Files* Ext2::File::files() {
      Files* files = 0;

      if ( inode->mode & MODE_FILE_DIR ) {
        DirEntry* e;
        uint8* data;
        uint32 data_size;
        lib::String *a;
        files = new Files();

        read( ( void** ) &data, &data_size );

        e = ( DirEntry* ) data;
        do {
          a = new lib::String( e->name, e->name_len );
          files->put( a, new File( _fs, e->inode, this ) ); // add new file to list
          e = ( DirEntry* ) ( ( uint32 ) e + e->rec_len );
        } while ( ( uint32 ) e < ( ( uint32 ) data + inode->size ) );
        // in the last directory entry is the size of the remaining bytes of the
        // 512 byte sector of the hard-drive, because of this we check against the directory size.

        delete data;
      }

      return files;
    }

    void Ext2::File::mkfile( lib::String* Name ) {
      if ( inode->mode & MODE_FILE_DIR ) {
        uint32 freenode = _fs->alloc_inode( group() );

        system->video << "inode: " << freenode << "\n";

        File* nfile = new File( _fs, freenode );

        nfile->inode->mode = MODE_FILE_REG;

        nfile->inode->mode = ACCESS_READ_USR | ACCESS_READ_GRP | ACCESS_READ_OTH;
        nfile->inode->size = 0;
        nfile->inode->blocks = 0;
        nfile->inode->uid = inode->uid;
        nfile->inode->osd2.linux2.uid_high = inode->osd2.linux2.uid_high;

        nfile->save();

        void* data;
        uint32 size;

        read( &data, &size );

        DirEntry* e = ( DirEntry* ) data;
        do {
          system->video << e->name << "\n";
          e = ( DirEntry* ) ( ( uint32 ) e + e->rec_len );
        } while ( ( ( uint32 ) e + e->rec_len ) < ( ( uint32 ) data + inode->size ) );
        system->video << e->name << "\n";
        uint32 offset = 8 + e->name_len + ( 4 - ( e->name_len % 4 ) );

        DirEntry* n = ( DirEntry* ) ( ( uint32 ) e + offset );

        n->inode = freenode;
        n->file_type = FT_REG_FILE;
        lib::memcpy( n->name, Name->ptr(), Name->size() );
        n->name_len = Name->size();
        n->rec_len = e->rec_len - offset;
        e->rec_len = offset;

        write( data, size );

        delete ( uint8* ) data;

      }
    }

    void Ext2::File::rmfile( lib::String* Name ) {

    }

    Ext2::File::~File() {
      delete inode;
    }

    uint32 Ext2::block2lba( uint32 blk ) {
      // add partition offset, add numbers of blocks too jump
      return partition_offset + blk * block_sector_size;
    }

    uint32 Ext2::alloc_inode( uint32 grp ) {
      drive->readSector( 2, block2lba( grp_desc_table[ grp ].inode_bitmap ), tmp_inode_bitmap );

      uint8* bitmap = tmp_inode_bitmap;
      uint32 freenode = 0;

      for ( uint32 j = 0; j < 1024; ++j ) {
        for ( uint8 i = 0; i < 8; ++i ) {
          if ( ~( *bitmap ) & ( 1 << i ) ) {
            j = 1024;
            ( *bitmap ) |= ( 1 << i ); // setting the bit in inode bitmap
            break;
          }
          freenode++;
        }
        bitmap++;
      }

      // writing the updated inode bitmap back to the hard-drive
      drive->writeSector( 2, block2lba( grp_desc_table[ grp ].inode_bitmap ), tmp_inode_bitmap );

      grp_desc_table[ grp ].free_inodes_count--;
      sblock.free_inodecount--;

      // update group description table
      drive->writeSector( 1, block2lba( 2 ), grp_desc_table );

      // update super block on disk
      write_superblock();

      return freenode + grp * sblock.inodeper_group + 1;
    }

    uint32 Ext2::alloc_block( uint32 grp ) {
      drive->readSector( 2, block2lba( grp_desc_table[ grp ].block_bitmap ), tmp_block_bitmap );

      uint8* bitmap = tmp_block_bitmap;
      uint32 freeblock = 0;

      for ( uint32 j = 0; j < 1024; ++j ) {
        for ( uint8 i = 0; i < 32; ++i ) {
          if ( ~( *bitmap ) & ( 1 << i ) ) {
            j = 1024;
            *bitmap |= ( 1 << i );
            break;
          }
          freeblock++;
        }
        bitmap++;
      }

      drive->writeSector( 2, block2lba( grp_desc_table[ grp ].block_bitmap ), tmp_block_bitmap );

      grp_desc_table[ grp ].free_blocks_count--;
      sblock.free_blockcount--;

      // update group description table
      drive->writeSector( 1, block2lba( 2 ), grp_desc_table );

      // update super block on disk
      write_superblock();

      return freeblock + grp * sblock.blockper_group + 1;
    }

    void Ext2::free_block( uint32 blk ) {
      uint32 grp = ( blk - 1 ) / sblock.blockper_group;

      drive->readSector( 2, block2lba( grp_desc_table[ grp ].block_bitmap ), tmp_block_bitmap );

      uint32* bitmap = ( uint32* ) tmp_block_bitmap;

      bitmap += ( blk - 1 ) / 32;

      *bitmap &= ~( 1 << ( ( blk - 1 ) % 32 ) );

      drive->writeSector( 2, block2lba( grp_desc_table[ grp ].block_bitmap ), tmp_block_bitmap );

      grp_desc_table[ grp ].free_blocks_count++;
      sblock.free_blockcount++;

      // update group description table
      drive->writeSector( 1, block2lba( 2 ), grp_desc_table );

      // update super block on disk
      write_superblock();

    }

    void Ext2::free_inode( uint32 inode ) {
      uint32 grp = ( inode - 1 ) / sblock.inodeper_group;

      drive->readSector( 2, block2lba( grp_desc_table[ grp ].inode_bitmap ), tmp_inode_bitmap );

      uint32* bitmap = ( uint32* ) tmp_inode_bitmap;

      bitmap += ( inode - 1 ) / 32;

      *bitmap &= ~( 1 << ( ( inode - 1 ) % 32 ) );

      drive->writeSector( 2, block2lba( grp_desc_table[ grp ].inode_bitmap ), tmp_inode_bitmap );

      grp_desc_table[ grp ].free_inodes_count++;
      sblock.free_inodecount++;

      // update group description table
      drive->writeSector( 1, block2lba( 2 ), grp_desc_table );

      // update super block on disk
      write_superblock();

    }

    void Ext2::write_superblock() {
      drive->writeSector( 2, partition_offset + 2, &sblock );
    }

    void Ext2::read_superblock() {
      drive->readSector( 2, partition_offset + 2, &sblock );
    }

    Ext2::Ext2( ATA::Drive* IDE, uint32 Partition )
        : drive( IDE ), partition( Partition ) {

      ATA::MBR mbr;

      drive->read( &mbr );

      if ( !mbr.isValid() ) {
        lib::Exception::throwing( "Invalid MBR! Can not read partition table." );
      }

      partition_offset = mbr.partition[ partition ].sector;

      read_superblock();

      if ( sblock.magic != SUPER_MAGIC ) {
        lib::Exception::throwing( "Ext2 super block magic does not fit!!!" );
      }

      block_size = ( 1024 << sblock.log_block_size );
      block_sector_size = block_size >> 9;

      // we read only on sector of the inode table, so the tmp_inode_table has only
      // the size of INode's in a ATA-Sector, which is 512 Byte.
      tmp_inode_table = new INode[ 512 / sizeof(INode) ];

      tmp_inode_bitmap = new uint8[ block_size ];
      tmp_block_bitmap = new uint8[ block_size ];

      // reading group description
      drive->readSector( 1, block2lba( 2 ), grp_desc_table );

      root = new File( this, ROOT_INO );
    }

    char* Ext2::volumename() {
      return sblock.volume_name;
    }

    Ext2::~Ext2() {
      delete root;
      delete tmp_inode_table;
      delete tmp_block_bitmap;
      delete tmp_inode_bitmap;
    }

  }

}
