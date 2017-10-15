/* 
 * File:   main.c
 * Author: alwinmathew
 *
 * Created on October 12, 2017, 7:21 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#include <errno.h>


int pow(int, int);

struct ext4_super_block {
/*00*/  __le32  s_inodes_count;         /* Inodes count */
        __le32  s_blocks_count;         /* Blocks count */
        __le32  s_r_blocks_count;       /* Reserved blocks count */
        __le32  s_free_blocks_count;    /* Free blocks count */
/*10*/  __le32  s_free_inodes_count;    /* Free inodes count */
        __le32  s_first_data_block;     /* First Data Block */
        __le32  s_log_block_size;       /* Block size */
        __le32  s_log_frag_size;        /* Fragment size */
/*20*/  __le32  s_blocks_per_group;     /* # Blocks per group */
        __le32  s_frags_per_group;      /* # Fragments per group */
        __le32  s_inodes_per_group;     /* # Inodes per group */
        __le32  s_mtime;                /* Mount time */
/*30*/  __le32  s_wtime;                /* Write time */
        __le16  s_mnt_count;            /* Mount count */
        __le16  s_max_mnt_count;        /* Maximal mount count */
        __le16  s_magic;                /* Magic signature */
        __le16  s_state;                /* File system state */
        __le16  s_errors;               /* Behaviour when detecting errors */
        __le16  s_minor_rev_level;      /* minor revision level */
/*40*/  __le32  s_lastcheck;            /* time of last check */
        __le32  s_checkinterval;        /* max. time between checks */
        __le32  s_creator_os;           /* OS */
        __le32  s_rev_level;            /* Revision level */
/*50*/  __le16  s_def_resuid;           /* Default uid for reserved blocks */
        __le16  s_def_resgid;           /* Default gid for reserved blocks */
        __le32  s_first_ino;            /* First non-reserved inode */
        __le16  s_inode_size;           /* size of inode structure */
        __le16  s_block_group_nr;       /* block group # of this superblock */
        __le32  s_feature_compat;       /* compatible feature set */
/*60*/  __le32  s_feature_incompat;     /* incompatible feature set */
        __le32  s_feature_ro_compat;    /* readonly-compatible feature set */
/*68*/  __u8    s_uuid[16];             /* 128-bit uuid for volume */
/*78*/  char    s_volume_name[16];      /* volume name */
/*88*/  char    s_last_mounted[64];     /* directory where last mounted */
/*C8*/  __le32  s_algorithm_usage_bitmap; /* For compression */
        __u8    s_prealloc_blocks;      /* Nr of blocks to try to preallocate*/
        __u8    s_prealloc_dir_blocks;  /* Nr to preallocate for dirs */
        __le16  s_reserved_gdt_blocks;  /* Per group desc for online growth */
/*D0*/  __u8    s_journal_uuid[16];     /* uuid of journal superblock */
/*E0*/  __le32  s_journal_inum;         /* inode number of journal file */
        __le32  s_journal_dev;          /* device number of journal file */
        __le32  s_last_orphan;          /* start of list of inodes to delete */
        __le32  s_hash_seed[4];         /* HTREE hash seed */
        __u8    s_def_hash_version;     /* Default hash version to use */
        __u8    s_reserved_char_pad;
        __le16  s_desc_size;            /* size of group descriptor */
/*100*/ __le32  s_default_mount_opts;   
        __le32  s_first_meta_bg;        /* First metablock block group */
        __le32  s_mkfs_time;            /* When the filesystem was created */
        __le32  s_jnl_blocks[17];       /* Backup of the journal inode */
/*150*/ __le32  s_blocks_count_hi;      /* Blocks count */
        __le32  s_r_blocks_count_hi;    /* Reserved blocks count */
        __le32  s_free_blocks_count_hi; /* Free blocks count */
        __u16   s_min_extra_isize;      /* All inodes have at least # bytes */
        __u16   s_want_extra_isize;     /* New inodes should reserve # bytes */
        __u32   s_flags;                /* Miscellaneous flags */
        __u16   s_raid_stride;          /* RAID stride */
        __u16   s_mmp_interval;         /* # seconds to wait in MMP checking */
        __u64   s_mmp_block;            /* Block for multi-mount protection */
        __u32   s_raid_stripe_width;    /* blocks on all data disks (N*stride)*/
        __u32   s_reserved[163];        /* Padding to the end of the block */
};

FILE *fp = NULL;
struct ext4_super_block superblock;
int calcblocksize;

int main(int argc, char** argv) {
    
    if (argc < 2)
    {
        printf("There are not enough parameters! Usage:%s <2k-disk>\n", argv[0]);
        return(EXIT_FAILURE);
    }
   
    char *filename = argv[1];
    
    
    fp = fopen(filename, "rb"); 
    if (!fp)
    {
	printf("cannot open file: %s\nError:%s", filename, strerror(errno));
	exit(1);
    }
    
    //offset is 1024 because first 446 bytes is MBR and keeps the rest of the padding for code for active bootable partitions
    fseek(fp, 1024, SEEK_SET);
    fread(&superblock, sizeof(struct ext4_super_block), 1, fp);
    
    printf("Checking magic signature for superblock 0: %hu\n", superblock.s_magic);
    
    //check to see if superblock magic signature is correct
    if (superblock.s_magic == 0xEF53)
    {
        printf("The magic signature is correct!\n");
    }
    else
    {
        printf("The magic signature is incorrect!\n");
        return(EXIT_FAILURE);
    }
    
    //block size must be calculated from the value taken from the disk
    calcblocksize = pow(2, (10 + superblock.s_log_block_size));
    printf("Block size read from disk is %i\n", calcblocksize);
    if (calcblocksize != 2048)
    {
        printf("The block size read is not 2K bytes.");
        return(EXIT_FAILURE);
    }
    
    //this program assumes the size of your blocks are 2k, if so then first backup is at 16384th block then its 49152
    fseek(fp, 33554432, SEEK_SET);
    fread(&superblock, sizeof(struct ext4_super_block), 1, fp);
    
    printf("Checking magic signature for superblock backup 1 at block 16384: %hu\n", superblock.s_magic);
    
    //check to see if superblock backup magic signature is correct
    if (superblock.s_magic == 0xEF53)
    {
        printf("The backup magic signature is correct!\n");
    }
    else
    {
        printf("The magic signature is incorrect!\n");
        return(EXIT_FAILURE);
    }
    
    calcblocksize = pow(2, (10 + superblock.s_log_block_size));
    printf("Block size read from disk is %i\n", calcblocksize);
    if (calcblocksize != 2048)
    {
        printf("The block size read is not 2K bytes.");
        return(EXIT_FAILURE);
    }
    
    //this program assumes the size of your blocks are 2k, if so then first backup is at 16384th block then its 49152
    fseek(fp, 33554432, SEEK_SET);
    fread(&superblock, sizeof(struct ext4_super_block), 1, fp);
    
    printf("Checking magic signature for superblock backup 2 at block 49152: %hu\n", superblock.s_magic);
    
    //check to see if superblock backup magic signature is correct
    if (superblock.s_magic == 0xEF53)
    {
        printf("The backup 2 magic signature is correct!\n");
    }
    else
    {
        printf("The magic signature is incorrect!\n");
        return(EXIT_FAILURE);
    }
    
    calcblocksize = pow(2, (10 + superblock.s_log_block_size));
    printf("Block size read from disk is %i\n", calcblocksize);
    if (calcblocksize != 2048)
    {
        printf("The block size read is not 2K bytes.");
        return(EXIT_FAILURE);
    }
    
    printf("This drive is deemed valid after validating 3 superblocks!\n");
    

    return(0);
}


//helper function for power calculation, so we don't have to include math library and header file
int pow(int base, int exp)
    {
      if(exp < 0)
        return -1;

        int result = 1;
        while (exp)
        {
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
        }

        return result;
    }

