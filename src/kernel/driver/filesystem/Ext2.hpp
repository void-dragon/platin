/**
 * Ext2.hpp
 *
 * @since 30.04.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef EXT2_HPP_
#define EXT2_HPP_

#include <cpp.hpp>
#include <lib/String.hpp>
#include <lib/File.hpp>
#include <lib/collection/Map.hpp>
#include <kernel/driver/ATA.hpp>

namespace kernel {

  namespace driver {

    /**
     * An implementation of the good old Linux Ext2 file system.
     *
     * This is not a full implementation of the Ext2 file system, but
     * enough to read, write files and directories, and to create new
     * files and directories.
     *
     * Not Supported:
     *  - Formatting
     *  - Checking
     *  - Managing of bad blocks
     *
     * @note linux-src://include/linux/ext2_fs.h
     * @note linux-src://include/linux/ext2_fs_sb.h
     * @note http://www.nongnu.org/ext2-doc/ext2.html
     * @note http://www.science.unitn.it/~fiorella/guidelinux/tlk/node95.html
     * @note ebook://Linux - Understanding The Linux Kernel
     *
     * @code
     *       | Begin of partition!
     * +-----+-------------+--------------------+--------------+--------------+
     * | ... | Super Block | Groupe Description | Block Bitmap | Inode Bitmap |
     * +-----+-------------+--------------------+--------------+--------------+
     * @endcode
     *
     * @since 30.04.2010
     * @date 30.09.2010
     * @author Arne Simon => email::[arne_simon@gmx.de]
     */
    class Ext2 {
      protected:
        static const uint8 EXT2_N_BLOCKS = 12;
        static const uint8 DEFAULT_RESERVE_BLOCKS = 8;
        static const uint16 MAX_RESERVE_BLOCKS = 1027;
        static const uint8 RESERVE_WINDOW_NOT_ALLOCATED = 0;
        static const uint32 DATA_BLOCK_SIZE = 512; ///< The size of a file data block.

        /*
         * Special inode numbers
         */
        static const uint8 BAD_INO = 1; ///< Bad blocks inode
        static const uint8 ROOT_INO = 2; ///< Root inode
        static const uint8 BOOT_LOADER_INO = 5; ///< Boot loader inode
        static const uint8 UNDEL_DIR_INO = 6; ///< Undelete directory inode

        static const uint8 OS_LINUX = 0;
        static const uint8 OS_HURD = 1;
        static const uint8 OS_MASIX = 2;
        static const uint8 OS_FREEBSD = 3;
        static const uint8 OS_LITES = 4;

        static const uint16 ERRORS_CONTINUE = 1; ///<   continue as if nothing happened
        static const uint16 ERRORS_RO = 2; ///<   remount read-only
        static const uint16 ERRORS_PANIC = 3; ///<   cause a kernel panic

        static const uint16 SUPER_MAGIC = 0xEF53;

        // file format
        static const uint16 MODE_FILE_SOCK = 0xC000; ///<  socket
        static const uint16 MODE_FILE_LNK = 0xA000; ///<  symbolic link
        static const uint16 MODE_FILE_REG = 0x8000; ///<  regular file
        static const uint16 MODE_FILE_BLK = 0x6000; ///<  block device
        static const uint16 MODE_FILE_DIR = 0x4000; ///<  directory
        static const uint16 MODE_FILE_CHR = 0x2000; ///<  character device
        static const uint16 MODE_FILE_FIFO = 0x1000; ///<  fifo
        // process execution user/group override
        static const uint16 EXT2_S_ISUID = 0x0800;        ///<  Set process User ID
        static const uint16 EXT2_S_ISGID = 0x0400;        ///<  Set process Group ID
        static const uint16 EXT2_S_ISVTX = 0x0200;        ///<  sticky bit
        // access rights
        static const uint16 ACCESS_READ_USR = 0x0100;        ///<  user read
        static const uint16 ACCESS_WRITE_USR = 0x0080;        ///<  user write
        static const uint16 ACCESS_EXE_USR = 0x0040;        ///<  user execute
        static const uint16 ACCESS_READ_GRP = 0x0020;        ///<  group read
        static const uint16 ACCESS_WRITE_GRP = 0x0010;        ///<  group write
        static const uint16 ACCESS_EXE_GRP = 0x0008;        ///<  group execute
        static const uint16 ACCESS_READ_OTH = 0x0004;        ///<  others read
        static const uint16 ACCESS_WRITE_OTH = 0x0002;        ///<  others write
        static const uint16 ACCESS_EXE_OTH = 0x0001;        ///< others execute

        static const uint32 FLAG_SECRM = 0x00000001;        ///<  secure deletion
        static const uint32 FLAG_UNRM = 0x00000002;        ///<  record for undelete
        static const uint32 FLAG_COMPR = 0x00000004;        ///<  compressed file
        static const uint32 FLAG_SYNC = 0x00000008;        ///<  synchronous updates
        static const uint32 FLAG_IMMUTABLE = 0x00000010;        ///< immutable file
        static const uint32 FLAG_APPEND = 0x00000020;        ///<  append only
        static const uint32 FLAG_NODUMP = 0x00000040;        ///<  do not dump/delete file
        static const uint32 FLAG_NOATIME = 0x00000080;        ///<  do not update .i_atime
        static const uint32 FLAG_DIRTY = 0x00000100;        ///<  Dirty (modified)
        static const uint32 FLAG_COMPRBLK = 0x00000200;        ///<  compressed blocks
        static const uint32 FLAG_NOCOMPR = 0x00000400;        ///<  access raw compressed data
        static const uint32 FLAG_ECOMPR = 0x00000800;        ///<  compression error
        static const uint32 FLAG_BTREE = 0x00010000;        ///<  b-tree format directory
        static const uint32 FLAG_INDEX = 0x00010000;        ///<  hash indexed directory
        static const uint32 FLAG_IMAGIC = 0x00020000;        ///< AFS directory
        static const uint32 FLAG_JOURNAL_DATA = 0x00040000;        ///<  journal file data
        static const uint32 FLAG_RESERVED = 0x80000000;        ///< reserved for ext2 library

        static const uint8 FT_UNKNOWN = 0;        ///<   Unknown File Type
        static const uint8 FT_REG_FILE = 1;        ///<   Regular File
        static const uint8 FT_DIR = 2;        ///<   Directory File
        static const uint8 FT_CHRDEV = 3;        ///<  Character Device
        static const uint8 FT_BLKDEV = 4;        ///<   Block Device
        static const uint8 FT_FIFO = 5;        ///<  Buffer File
        static const uint8 FT_SOCK = 6;        ///<  Socket File
        static const uint8 FT_SYMLINK = 7;        ///<  Symbolic Link

        /**
         * A Linux Ext2 INode.
         * @note INode - Group relation => group = (inode – 1) / INODES_PER_GROUP
         * @attention INode( 2 ) is the root directory!
         * @attention Data and Ext2 blocks are not the same.
         *            A data block has always the size of 512 Bytes, the same size like a Sector on
         *            an ATA/PI drive. The Ext2 block size is set in the Super-Block.
         *            How ever, in the INode the blocks count is given in number of data blocks!!
         *            And the block array points to Ext2 blocks!
         *            So the blocks count should be like this => data block count = pointer count * ext2 block size / 512;
         */
        struct INode {
            uint16 mode; ///< File mode
            uint16 uid; ///< Low 16 bits of Owner UID
            uint32 size; ///< Size in bytes.
            uint32 atime; ///< Access time in POSIX.
            uint32 ctime; ///< Creation time in POSIX.
            uint32 mtime; ///< Modification time in POSIX.
            uint32 dtime; ///< Deletion Time
            uint16 gid; ///< Low 16 bits of Group Id
            uint16 links_count; ///< Links count
            uint32 blocks; ///< The count of data blocks. @attention Data blocks have always a size of 512 byte.
            uint32 flags; ///< File flags
            union {
                struct {
                    uint32 reserved1;
                } linux1;
                struct {
                    uint32 translator;
                } hurd1;
                struct {
                    uint32 reserved1;
                } masix1;
            } osd1; ///< OS dependent 1
            uint32 block[ EXT2_N_BLOCKS ]; ///< Pointers to standard ext2 blocks.
            uint32 first_indirect_block; ///< Pointer to the first indirect block
            uint32 double_indirect_block;
            uint32 triple_indirect_block;
            uint32 generation; ///< File version (for NFS)
            uint32 file_acl; ///< File ACL
            uint32 dir_acl; ///< Directory ACL
            uint32 faddr; ///< Fragment address
            union {
                struct {
                    uint8 frag; ///< Fragment number
                    uint8 fsize; ///< Fragment size
                    uint16 pad1;
                    uint16 uid_high; ///< these 2 fields
                    uint16 gid_high; ///< were reserved2[0]
                    uint32 reserved2;
                } linux2;
                struct {
                    uint8 frag; ///< Fragment number
                    uint8 fsize; ///< Fragment size
                    uint16 mode_high;
                    uint16 uid_high;
                    uint16 gid_high;
                    uint32 author;
                } hurd2;
                struct {
                    uint8 frag; ///< Fragment number
                    uint8 fsize; ///< Fragment size
                    uint16 pad1;
                    uint32 reserved2[ 2 ];
                } masix2;
            } osd2; ///< OS dependent 2
        }__attribute__((packed));

        /**
         * Structure of the super block.
         *
         * @note The postion of the super block is at byte 1024 of the partion, which
         *        is the same as an sector offset of 2 from the beginning of the partition.
         */
        struct SuperBlock {
            uint32 inodecount;
            uint32 blockcount;
            uint32 r_blockcount; ///< reserved blocks count
            uint32 free_blockcount;
            uint32 free_inodecount;
            uint32 first_data_block; ///< First Data Block
            uint32 log_block_size; ///< Block size
            uint32 log_frag_size; ///< Fragment size
            uint32 blockper_group; ///< # Blocks per group
            uint32 fragper_group; ///< # Fragments per group
            uint32 inodeper_group; ///< # Inodes per group
            uint32 mtime; ///< Mount time
            uint32 wtime; ///< Write time/
            uint16 mnt_count; ///< Mount count/
            uint16 max_mnt_count; ///< Maximal mount count
            uint16 magic; ///< Magic signature
            uint16 state; ///< file system state
            uint16 errors; ///< Behavior when detecting errors
            uint16 minor_rev_level; ///< minor revision level /
            uint32 lastcheck; ///< time of last check
            uint32 checkinterval; ///< max. time between checks
            uint32 creator_os; ///< OS
            uint32 rev_level; ///< Revision level
            uint16 def_resuid; ///< Default UID for reserved blocks
            uint16 def_resgid; ///< Default GID for reserved blocks
            /**
             * These fields are for EXT2_DYNAMIC_REV super blocks only.
             */
            uint32 first_ino; ///< First non-reserved inode
            uint16 inode_size;
            uint16 block_group_nr; ///< block group # of this super block
            uint32 feature_compat; ///< compatible feature set
            uint32 feature_incompat; ///< incompatible feature set
            uint32 feature_ro_compat; ///< readonly-compatible feature set
            uint8 uuid[ 16 ]; ///< 128-bit UUID for volume
            char volume_name[ 16 ]; ///< volume name
            char last_mounted[ 64 ]; ///< directory where last mounted
            uint32 algorithm_usage_bitmap; ///< For compression
            /**
             * Performance hints.  Directory preallocation should only
             * happen if the EXT2_COMPAT_PREALLOC flag is on.
             */
            uint8 prealloc_blocks; ///< Nr of blocks to try to preallocate
            uint8 prealloc_dir_blocks; ///< Nr to preallocate for dirs
            uint16 padding1;
            /**
             * Journaling support valid if EXT3_FEATURE_COMPAT_HAJOURNAL set.
             */
            uint8 journal_uuid[ 16 ]; ///< uuid of journal superblock
            uint32 journal_inum; ///< inode number of journal file
            uint32 journal_dev; ///< device number of journal file
            uint32 last_orphan; ///< start of list of inodes to delete
            uint32 hash_seed[ 4 ]; ///< HTREE hash seed
            uint8 def_hash_version; ///< Default hash version to use
            uint8 reserved_char_pad;
            uint16 reserved_word_pad;
            uint32 default_mount_opts;
            uint32 first_meta_bg; ///< First metablock block group
            uint32 reserved[ 190 ]; ///< Padding to the end of the block
        }__attribute__((packed));

        struct GroupDesc {
            uint32 block_bitmap; ///< Blocks bitmap block
            uint32 inode_bitmap; ///< Inodes bitmap block
            uint32 inode_table; ///< Inodes table block
            uint16 free_blocks_count; ///< Free blocks count
            uint16 free_inodes_count; ///< Free inodes count
            uint16 used_dirs_count; ///< Directories count
            uint16 pad;
            uint32 reserved[ 3 ];
        }__attribute__((packed));

        /**
         * The new version of the directory entry.
         *
         * Since EXT2 structures are stored in intel byte order, and the name_len field
         * could never be bigger than 255 chars, it's safe to reclaim the extra byte for the
         * file_type field.
         *
         * @attention In the last DirEntry of a directory list, holds the field rec_len
         *   not the length of the directory record, but the length of the remaining
         *   bytes from the DirEntry start( pointer to the structure ), to the
         *   end of the 512 byte hard-drive sector.
         *   ( This was hard to get, because it is not mentioned in the documentation :)
         */
        struct DirEntry {
            uint32 inode; ///< inode number
            uint16 rec_len; ///< directory entry length
            uint8 name_len; ///< name length
            uint8 file_type;
            char name[ 255 ]; ///< file name
        }__attribute__((packed));

        SuperBlock sblock;
        GroupDesc grp_desc_table[ 32 ]; ///< The group description table
        uint8* tmp_block_bitmap;
        uint8* tmp_inode_bitmap; ///< A temporary read inode bitmap.
        INode* tmp_inode_table; ///< A temporary part of an inode table.
        ATA::Drive *drive;
        uint8 partition; ///< which partition of the device we use.
        uint32 partition_offset;
        uint32 block_size; ///< The size of an ext2 block in byte.
        uint32 block_sector_size; ///< How many sectors a block uses.

        uint32 block2lba( uint32 blk );

        uint32 alloc_inode( uint32 grp );

        uint32 alloc_block( uint32 grp );

        void free_block( uint32 blk );

        void free_inode( uint32 inode );

        void write_superblock();

        void read_superblock();

      public:
        /**
         * A file or directory.
         */
        class File: public lib::File {
          public:
            static const uint32 FILE_TYPE = 60;
            Ext2* _fs; ///< Pointer to the Ext2 filesystem we working on.
            File* parent; ///< The parent directory.
            uint32 inode_id; ///< The inode of this file/directory.
            INode *inode; ///< Pointer to the inode structure of this inode.

            /**
             * Gets the index in the inode table for a given INode.
             *
             * @return The index in the inode table.
             */
            uint32 index();

            /**
             * Returns the group of the given INode.
             */
            uint32 group();

            /**
             * Loads the INode by the absolute INode id.
             *
             * @note We are only reading the sector of the hard-disk, where the INode is located.
             */
            void load();

            void save();

            void read( void** data, uint32* size );

            uint32 write( void* data, uint32 size );

            uint64 size();

            bool hasFiles();

            File::Files* files();

            /**
             * Create a new file.
             * @param Name The name of the new file.
             */
            void mkfile( lib::String* Name );

            /**
             * Deletes a file by it's name.
             * @param Name The of the file wich will be deleted.
             */
            void rmfile( lib::String* Name );

            File( Ext2* FS, uint32 Id );

            File( Ext2* FS, uint32 Id, File* Parent );

            virtual ~File();
        };

        File *root; ///< The root directory.

        Ext2( ATA::Drive* IDE, uint32 Partition );

        char* volumename();

        virtual ~Ext2();
    };

  }
}

#endif /* EXT2_HPP_ */
