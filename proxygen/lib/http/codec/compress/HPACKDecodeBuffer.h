/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include "proxygen/lib/http/codec/compress/HPACKConstants.h"

#include <folly/io/Cursor.h>
#include <folly/io/IOBuf.h>

namespace proxygen {

class HPACKDecodeBuffer {
 public:
  explicit HPACKDecodeBuffer(HPACK::MessageType msgType,
                             folly::io::Cursor& cursorVal,
                             uint32_t totalBytes)
      : msgType_(msgType),
        cursor_(cursorVal),
        totalBytes_(totalBytes),
        remainingBytes_(totalBytes) {}

  ~HPACKDecodeBuffer() {}

  void reset(folly::io::Cursor& cursorVal) {
    reset(cursorVal, cursorVal.totalLength());
  }

  void reset(folly::io::Cursor& cursorVal,
             uint32_t totalBytes) {
    cursor_ = cursorVal;
    totalBytes_ = totalBytes;
    remainingBytes_ = totalBytes;
  }

  uint32_t consumedBytes() const {
    return totalBytes_ - remainingBytes_;
  }

  const folly::io::Cursor& cursor() const {
    return cursor_;
  }

  /**
   * @returns true if there are no more bytes to decode. Calling this method
   * might move the cursor from the current IOBuf to the next one
   */
  bool empty();

  /**
   * extracts one byte from the buffer and advances the cursor
   */
  uint8_t next();

  /**
   * just peeks at the next available byte without moving the cursor
   */
  uint8_t peek();

  /**
   * decode an integer from the current position, given a nbit prefix
   * that basically needs to be ignored
   */
  bool decodeInteger(uint8_t nbit, uint32_t& integer);

  /**
   * decode a literal starting from the current position
   */
  bool decodeLiteral(std::string& literal);

private:
  HPACK::MessageType msgType_;
  folly::io::Cursor& cursor_;
  uint32_t totalBytes_;
  uint32_t remainingBytes_;
};

}
