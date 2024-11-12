// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKETS 13

struct bucket {
  struct spinlock lock;
  struct buf *head;
};

struct {
  struct buf buf[NBUF];
  struct bucket buckets[NBUCKETS];
} bcache;

void
binit(void)
{
  struct buf *b;

  for(int i = 0; i < NBUCKETS; i++) {
    char name[16];
    snprintf(name, sizeof(name), "bcache.bucket%d", i);
    initlock(&bcache.buckets[i].lock, name);
    bcache.buckets[i].head = 0;
  }

  // Initialize buffer heads
  for(b = bcache.buf; b < bcache.buf + NBUF; b++) {
    b->next = 0;
    b->valid = 0;
    b->refcnt = 0;
    initsleeplock(&b->lock, "buffer");
  }
}
int
hash(uint dev, uint blockno)
{
  return (dev + blockno) % NBUCKETS;
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int idx = hash(dev, blockno);

  // Step 1: Look for the block in the bucket
  acquire(&bcache.buckets[idx].lock);
  for(b = bcache.buckets[idx].head; b; b = b->next) {
    if(b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.buckets[idx].lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  release(&bcache.buckets[idx].lock);
  static struct spinlock alloc_lock;
  static int alloc_lock_initialized = 0;
  if(!alloc_lock_initialized) {
    initlock(&alloc_lock, "bcache.alloc");
    alloc_lock_initialized = 1;
  }
  acquire(&alloc_lock);
  for(b = bcache.buf; b < bcache.buf + NBUF; b++) {
    acquire(&bcache.buckets[hash(b->dev, b->blockno)].lock);
    if(b->refcnt == 0) {
      struct buf **pprev = &bcache.buckets[hash(b->dev, b->blockno)].head;
      while(*pprev && *pprev != b)
        pprev = &(*pprev)->next;
      if(*pprev == b)
        *pprev = b->next;
      release(&bcache.buckets[hash(b->dev, b->blockno)].lock);

      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;

      idx = hash(dev, blockno);
      acquire(&bcache.buckets[idx].lock);
      b->next = bcache.buckets[idx].head;
      bcache.buckets[idx].head = b;
      release(&bcache.buckets[idx].lock);

      release(&alloc_lock);
      acquiresleep(&b->lock);
      return b;
    }
    release(&bcache.buckets[hash(b->dev, b->blockno)].lock);
  }
  release(&alloc_lock);

  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  int idx = hash(b->dev, b->blockno);
  acquire(&bcache.buckets[idx].lock);
  b->refcnt--;
  if(b->refcnt == 0) {
  }
  release(&bcache.buckets[idx].lock);
}

void
bpin(struct buf *b) {
  int idx = hash(b->dev, b->blockno);
  acquire(&bcache.buckets[idx].lock);
  b->refcnt++;
  release(&bcache.buckets[idx].lock);
}

void
bunpin(struct buf *b) {
  int idx = hash(b->dev, b->blockno);
  acquire(&bcache.buckets[idx].lock);
  b->refcnt--;
  release(&bcache.buckets[idx].lock);
}


