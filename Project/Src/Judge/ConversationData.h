#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace ConversationData {

    inline const std::unordered_map<std::string, std::vector<std::string>> demos = {
        {
            "demoA", {
                "demoA1.png",
                "demoA2.png",
                "demoA3.png"
            }
        },
        {
            "demoB", {
                "demoB1.png",
                "demoB2.png"
            }
        },
        {
            "demoC", {
                "demoC1.png",
                "demoC2.png",
                "demoC3.png"
            }
        }
    };

}