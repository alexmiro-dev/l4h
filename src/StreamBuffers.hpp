
#pragma once

#include <string_view>

namespace l4h::stream {


    // each line in buffer must have the file descriptor BEGIN and END to help associating the line number and its
    // position in the actual file

class StreamBuffer {
public:

    std::vector<defs::StreamLineData> insert(std::string_view data) {

        return {};
    }

    void peek() {

    }

};

} // namespace omlog::stream
