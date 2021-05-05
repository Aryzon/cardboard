/*
 * Copyright 2021 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "sixdof/position_data.h"

#include <string>
#include <sstream>
#include "util/logging.h"

namespace cardboard {

PositionData::PositionData(size_t buffer_size) : buffer_size_(buffer_size) {}

void PositionData::AddSample(const Vector3& sample, const int64_t timestamp_ns) {
    buffer_.push_back(sample);
    if (buffer_.size() > buffer_size_) {
        buffer_.pop_front();
    }
    
    timestamp_buffer_.push_back(timestamp_ns);
    if (timestamp_buffer_.size() > buffer_size_) {
        timestamp_buffer_.pop_front();
    }
}

bool PositionData::IsValid() const { return buffer_.size() == buffer_size_; }

long long PositionData::GetLatestTimestamp() const {
    if (timestamp_buffer_.size() > 0) {
        return timestamp_buffer_[timestamp_buffer_.size() -1];
    }
    return 0;
}

Vector3 PositionData::GetLatestData() const {
    if (buffer_.size() > 0) {
        return buffer_[buffer_.size() -1];
    }
    return Vector3::Zero();
}


Vector3 PositionData::GetExtrapolatedForTimeStamp(const int64_t timestamp_ns) {
  
    if (!IsValid() || buffer_size_ < 2) {
        return {0.0,0.0,0.0};
    }
    
    if (timestamp_ns > timestamp_buffer_[buffer_size_-1]) {
        const Vector3 v = (buffer_[buffer_size_-1] - buffer_[buffer_size_-2]) / (timestamp_buffer_[buffer_size_-1] - timestamp_buffer_[buffer_size_-2]);
        return buffer_[buffer_size_-1] + v * (timestamp_ns - timestamp_buffer_[buffer_size_ - 1]);
    }
    
    return buffer_[buffer_size_-1];
}

void PositionData::Reset() {
    buffer_.clear();
    timestamp_buffer_.clear();
}

}  // namespace cardboard