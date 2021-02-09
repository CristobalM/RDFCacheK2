#ifndef _LRU_CONTROLLER_HPP_
#define _LRU_CONTROLLER_HPP_

class LRUController{
  public:
  virtual void retrieve_element(unsigned long element_id) = 0;
  virtual void discard_element(unsigned long element_id) = 0;
  virtual unsigned long get_max_size_bytes() = 0;
};

#endif /* _LRU_CONTROLLER_HPP_ */
