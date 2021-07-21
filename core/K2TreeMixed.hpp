#ifndef RDFCACHEK2_K2TREE_MIXED_HPP
#define RDFCACHEK2_K2TREE_MIXED_HPP

#include <istream>
#include <memory>
#include <utility>
#include <vector>

#include "ResultTable.hpp"
#include "k2tree_stats.hpp"

#include <TimeControl.hpp>
#include <serialization_util.hpp>

extern "C" {
#include <definitions.h>
#include <k2node.h>
}

struct k2node;
struct k2qstate;

struct K2TreeConfig {
  uint32_t treedepth;
  uint32_t max_node_count;
  uint32_t cut_depth;

  void write_to_ostream(std::ostream &os) {
    write_u32(os, treedepth);
    write_u32(os, max_node_count);
    write_u32(os, cut_depth);
  }

  void read_from_istream(std::istream &is) {
    treedepth = read_u32(is);
    max_node_count = read_u32(is);
    cut_depth = read_u32(is);
  }
};

class K2TreeMixed {
  struct k2node *root;
  std::unique_ptr<struct k2qstate> st;
  uint64_t points_count;

public:
  explicit K2TreeMixed(uint32_t treedepth);
  K2TreeMixed(uint32_t treedepth, uint32_t max_node_count);
  K2TreeMixed(uint32_t treedepth, uint32_t max_node_count, uint32_t cut_depth);
  K2TreeMixed(struct k2node *root, uint32_t treedepth, uint32_t max_node_count,
              uint32_t cut_depth, uint64_t points_count);

  K2TreeMixed(K2TreeConfig config);

  K2TreeMixed(const K2TreeMixed &other) = delete;
  K2TreeMixed &operator=(const K2TreeMixed &other) = delete;

  K2TreeMixed(K2TreeMixed &&other);
  K2TreeMixed &operator=(K2TreeMixed &&other);

  ~K2TreeMixed();

  struct k2node *get_root_k2node();
  struct k2qstate *get_k2qstate();

  void insert(unsigned long col, unsigned long row);
  bool has(unsigned long col, unsigned long row) const;

  static std::vector<unsigned long>
  sip_join_k2trees(const std::vector<const K2TreeMixed *> &trees,
                   std::vector<struct sip_ipoint> &join_coordinates);

  unsigned long get_tree_depth() const;

  std::vector<std::pair<unsigned long, unsigned long>> get_all_points();
  std::vector<unsigned long> get_row(unsigned long row) const;
  std::vector<unsigned long> get_column(unsigned long col) const;

  void scan_points(point_reporter_fun_t fun_reporter, void *report_state) const;
  std::vector<std::pair<unsigned long, unsigned long>>
  scan_points_into_vector() const;
  void traverse_row(unsigned long row, point_reporter_fun_t fun_reporter,
                    void *report_state) const;
  void traverse_column(unsigned long column, point_reporter_fun_t fun_reporter,
                       void *report_state) const;

  struct k2tree_measurement measure_in_memory_size() const;

  ResultTable column_as_table(unsigned long column) const;
  ResultTable row_as_table(unsigned long row) const;

  K2TreeStats k2tree_stats() const;

  bool same_as(const K2TreeMixed &other) const;

  unsigned long write_to_ostream(std::ostream &os) const;
  static K2TreeMixed read_from_istream(std::istream &is);

  size_t size() const;

  bool has_valid_structure() const;

  enum BandType { COLUMN_BAND_TYPE = 0, ROW_BAND_TYPE = 1 };

  struct K2TreeScanner {
    using ul_pair_t = std::pair<unsigned long, unsigned long>;

    virtual bool has_next() = 0;
    virtual std::pair<unsigned long, unsigned long> next() = 0;
    virtual void reset_scan() = 0;

    virtual bool is_band() = 0;
    virtual BandType get_band_type() = 0;
    virtual unsigned long get_band_value() = 0;

    virtual K2TreeMixed &get_tree() = 0;

    virtual ~K2TreeScanner() = default;
  };

  struct FullScanner : public K2TreeScanner {

    FullScanner(K2TreeMixed &k2tree, TimeControl &time_control);

    bool has_next() override;
    std::pair<unsigned long, unsigned long> next() override;
    ~FullScanner() override;
    void reset_scan() override;
    bool is_band() override;
    BandType get_band_type() override;
    K2TreeMixed &get_tree() override;
    unsigned long get_band_value() override;

  private:
    k2node_lazy_handler_naive_scan_t lazy_handler;
    K2TreeMixed &k2tree;
    TimeControl &time_control;
  };

  struct BandScanner : public K2TreeScanner {
    BandScanner(K2TreeMixed &k2tree, unsigned long band,
                K2TreeMixed::BandType band_type, TimeControl &time_control);
    bool has_next() override;
    std::pair<unsigned long, unsigned long> next() override;
    ~BandScanner() override;
    void reset_scan() override;
    bool is_band() override;
    BandType get_band_type() override;
    K2TreeMixed &get_tree() override;
    unsigned long get_band_value() override;

  private:
    unsigned long band;
    K2TreeMixed::BandType band_type;
    k2node_lazy_handler_report_band_t lazy_handler;
    K2TreeMixed &k2tree;
    TimeControl &time_control;
  };

  struct EmptyScanner : public K2TreeScanner {
    explicit EmptyScanner(K2TreeMixed &k2tree);
    bool has_next() override;
    std::pair<unsigned long, unsigned long> next() override;
    void reset_scan() override;
    bool is_band() override;
    BandType get_band_type() override;
    unsigned long get_band_value() override;
    K2TreeMixed &get_tree() override;

  private:
    K2TreeMixed &k2tree;
  };

  std::unique_ptr<K2TreeMixed::K2TreeScanner>
  create_full_scanner(TimeControl &time_control);
  std::unique_ptr<K2TreeMixed::K2TreeScanner>
  create_band_scanner(unsigned long band, K2TreeMixed::BandType band_type,
                      TimeControl &time_control);

  struct FullScanIterator {
    using ul_pair_t = std::pair<unsigned long, unsigned long>;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = ul_pair_t;
    using pointer = ul_pair_t *;
    using reference = ul_pair_t &;

    explicit FullScanIterator(K2TreeMixed &k2tree);
    explicit FullScanIterator(pointer ptr);

    FullScanIterator(const FullScanIterator &original);

    ~FullScanIterator();

    reference operator*();
    pointer operator->();
    FullScanIterator &operator++();
    FullScanIterator operator++(int);
    friend bool operator==(const FullScanIterator &lhs,
                           const FullScanIterator &rhs);
    friend bool operator!=(const FullScanIterator &lhs,
                           const FullScanIterator &rhs);

  private:
    k2node_lazy_handler_naive_scan_t lazy_handler;
    ul_pair_t current;
    bool finished;
    bool init_called;

    k2node_lazy_handler_naive_scan_t
    deep_copy_handler(const k2node_lazy_handler_naive_scan_t &handler);
    lazy_handler_naive_scan_t
    deep_copy_sub_handler(const lazy_handler_naive_scan_t &handler);
    k2node_lazy_naive_state_stack
    deep_copy_states_stack(const k2node_lazy_naive_state_stack &original_stack);
    lazy_naive_state_stack
    deep_copy_sub_states_stack(const lazy_naive_state_stack &original_stack);
  };

  FullScanIterator begin_full_scan();
  FullScanIterator end_full_scan();

  struct BandScanIterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = unsigned long;
    using pointer = unsigned long *;
    using reference = unsigned long &;

    explicit BandScanIterator(K2TreeMixed &k2tree, unsigned long band,
                              BandType band_type);
    explicit BandScanIterator(pointer ptr);
    BandScanIterator(const BandScanIterator &original);
    ~BandScanIterator();

    reference operator*();
    pointer operator->();
    BandScanIterator &operator++();
    BandScanIterator operator++(int);
    friend bool operator==(const BandScanIterator &lhs,
                           const BandScanIterator &rhs);
    friend bool operator!=(const BandScanIterator &lhs,
                           const BandScanIterator &rhs);

  private:
    k2node_lazy_handler_report_band_t lazy_handler;
    bool finished;
    bool init_called;
    unsigned long current;
    k2node_lazy_handler_report_band_t deep_copy_band_lazy_handler(
        const k2node_lazy_handler_report_band_t &original_handler);
    lazy_handler_report_band_t deep_copy_sub_band_lazy_handler(
        const lazy_handler_report_band_t &sub_handler);
    k2node_lazy_report_band_state_t_stack deep_copy_band_lazy_stack(
        const k2node_lazy_report_band_state_t_stack &original_stack);
    lazy_report_band_state_t_stack deep_copy_band_sub_stack(
        const lazy_report_band_state_t_stack &original_stack);
  };

  BandScanIterator begin_band_scan(unsigned long band_value,
                                   BandType band_type);
  K2TreeMixed::BandScanIterator end_band_scan();

  std::unique_ptr<K2TreeScanner> create_empty_scanner();

private:
  void clean_up();
};

#endif
