#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace ConversationData {

    inline const std::unordered_map<std::string, std::vector<std::string_view>> demos = {
        {
            "demoA", {
                "チャーハンとは、\n米と炎の真剣勝負である！",
                "フライパンを恐れるな、\n己の腕を信じろ！",
                "さあ、今日も米粒一つ残さず\n炒め上げるのだ！！！"
            }
        },
        {
            "demoB", {
                "おい、そこの若者よ",
                "わしにチャーハンを\nもってこい！！！"
            }
        },
        {
            "demoC", {
                "よいか！\n火加減こそが料理の命！！",
                "強火だけでは真の味は\n生まれぬ！！",
                "炎を制する者こそ、\nチャーハンを制す！！"
            }
        }
    };

}