/**
 * MyFS.hpp
 *
 * @since 30.09.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef MYFS_HPP_
#define MYFS_HPP_

#include <cpp.hpp>
#include <kernel/driver/ATA.hpp>
#include <lib/File.hpp>
#include <lib/collection/Map.hpp>
#include <lib/String.hpp>
#include <lib/sync/Mutex.hpp>
#include <lib/hash/Tiger.hpp>
#include <lib/Time.hpp>

namespace kernel {

   namespace driver {

      /**
       * MyFS is a small, simple and straight forward file system.
       *
       * MyFS is my personal idea of a file system.
       *
       * @note One block has always the size of 512 byte.
       *
       * @since 30.10.2010
       * @date 23.10.2011
       * @author Arne Simon => email[arne_simon@gmx.de]
       */
      class MyFS {
         public:

            static const uint8 NODE_TYPE_FILE = 1; ///< Indicates that the node is a file.
            static const uint8 NODE_TYPE_DIR = 2; ///< Indicates that the node is a directory.
            static const uint8 NODE_DATA_START = 32;
            static const uint16 NODE_DATA_SIZE = 512 - NODE_DATA_START;

            /**
             * A file node.
             *
             * @note With first, second, third and fourth level block addresses, there
             * can be a total of 270.549.611 blocks addressed, which is
             * 270.549.611 * 512 byte + NODE_DATA_SIZE = ~138 GByte.
             * This is the maximum file size.
             */
            struct Node {
                  uint8 type;
                  uint64 size; ///< data size in byte.
                  uint32 first; ///< LBA for a block with 128 block addresses.
                  uint32 second; ///< 128 * 128 blocks.
                  uint32 third; ///< 128 * 128 * 128 blocks.
                  uint32 fourth; ///< 128 * 128 * 128 * 128 blocks.
                  lib::hash::Tiger locked_by; ///< This file locked by something with this hash.
                  lib::Time::PosixTimeStamp last_access;
                  uint8 data[ NODE_DATA_SIZE ]; ///< data which can be directly stored in the inode.
            }__attribute__((packed));

            /**
             * A directory entry.
             */
            struct DirectoryEntry {
                  uint32 size; ///< Size of the entire entry, size+node+name.
                  uint32 node; ///< LBA of the node.
                  char* name; ///< Name of the node.
            };

            /**
             * This information table lies in the sector at the beginning of the
             * partition.
             *
             * The bitmap is in the next sector.
             * @code
             * +-----+-----------------------------------+
             * | ... |           Partition               |
             * +-----+-------+--------+-----------+------+
             * | ... | Table | Bitmap | Root-Node | Data |
             * +-----+-------+--------+-----------+------+
             * @endcode
             *
             * bitmap blocks needed = disc size in sectors / ( 8 * 512 ) + 1;
             * bitmap blocks needed = ( disc size in sectors >> 12 ) + 1;
             */
            struct InfoTable {
                  uint64 free_block_count; ///< Free sector blocks.
                  uint32 root_node; ///< LBA of the root_node.
                  uint32 bitmap_size; ///< data size in sectors!
                  uint32 label_len; ///< The length of the volume label.
                  uint8 label[ 492 ]; ///< The label of the volume.
            }__attribute__((packed));

            void read_table();

            void write_table();

            /**
             * This function converts the LBA to a allocation bitmap index
             * and sets the bit to the appropriated boolean given.
             *
             * @param lba The address of the block.
             * @param used true= the block is used | false= the block is free
             */
            void mark_lba( uint32 lba, bool used );

            /**
             * Returns the LBA of a free block.
             * @return The LBA.
             */
            uint32 get_free_lba();

            /**
             * Frees a block.
             * @param lba The LBA of the block to free.
             */
            void free( uint32 lba );

            /**
             * A file or directory in the file system.
             *
             * This class represents a file or directory of the MyFS
             * file system in the Scriptol language.
             */
            class File: public lib::File {
               protected:
                  void read_blocks( void** data, uint32* size );

                  void write_blocks( void* data, uint32 size );

                  void readFileList();

               public:
                  uint32 lba; ///< The LBA of this file.
                  Node node; ///< The node information about this file.
                  MyFS* myfs; ///< Pointer to the MyFS system this file belongs to.
                  File* parent; ///< A parent directory or 0.
                  lib::collection::Map< lib::String*, File* > _files;

                  /**
                   * Creates a new file from LBA.
                   * @param M The pointer to the MyFS system.
                   * @param LBA The LBA of the file node.
                   * @param P Pointer to a parent directory.
                   */
                  File( MyFS* M, uint32 LBA, File* P = 0 );

                  // outsream

                  uint32 write( void* data, uint32 length );

                  // instream

                  void read( void** data, uint32* length );

                  uint32 read( void* data, uint32 length );

                  // lib::File

                  uint64 size();

                  void* range( uint32 start, uint32 size );

                  void range( uint32 start, uint32 size, void* data );
            };

            lib::sync::Mutex mutex;
            ATA::Drive* drive;
            uint8 partition; ///< which partition of the device to use.
            uint32 partition_offset; ///< The LBA of the first block of the partition to use.
            InfoTable table; ///< The information table.
            uint32* bitmap; ///< The allocation bitmap of free and used blocks.
            File* root;

            MyFS( ATA::Drive* Drv, uint32 Partition );

            /**
             * Formats the drive.
             */
            void format();

            virtual ~MyFS();
      };

   }

}

#endif /* MYFS_HPP_ */
