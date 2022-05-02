/**
 * @file noncopyable.h
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-04-29
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __TINYLOG_INCLUDE_NONCOPYABLE_H_
#define __TINYLOG_INCLUDE_NONCOPYABLE_H_

class noncopyable {
public:
  noncopyable(const noncopyable &) = delete;
  noncopyable operator=(const noncopyable &) = delete;

protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

#endif /* __TINYLOG_INCLUDE_NONCOPYABLE_H_ */
