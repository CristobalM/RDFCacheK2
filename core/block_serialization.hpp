#ifndef _BLOCK_SERIALIZATION_HPP_
#define _BLOCK_SERIALIZATION_HPP_

#include <cstdint>
#include <istream>
#include <list>
#include <ostream>

struct block;

struct k2tree_data {
  struct block *root;
  uint32_t treedepth;
  uint32_t max_node_count;
};

k2tree_data read_tree_from_istream(std::istream &is);
struct block *read_block_from_istream(std::istream &is);
unsigned long write_block_to_ostream(struct block *b, std::ostream &os);

unsigned long write_tree_to_ostream(k2tree_data data, std::ostream &os);
uint32_t traverse_tree_write_to_ostream(struct block *b, std::ostream &os,
                                        unsigned long &bytes_written);
void adjust_blocks(std::list<struct block *> &blocks);

bool same_blocks(const struct block *lhs, const struct block *rhs);
bool same_block_topologies(const struct block *lhs, const struct block *rhs);
bool same_bvs(const struct block *lhs, const struct block *rhs);
bool same_block_frontiers(const struct block *lhs, const struct block *rhs);

#endif
