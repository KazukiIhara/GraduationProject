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

	inline const std::unordered_map<std::string, std::vector<std::string_view>> rating = {
		{
			"rateS", {
				"これぞ至高の一品！",
				"「チャーハンマスター」\nの称号を与えよう！"
			}
		},
		{
			"rateA", {
				"うむ、なかなかの腕前だ！",
				"さらに研鑽を積み、\n「チャーハンマスター」を目指せ！"
			}
		},
		{
			"rateB", {
				"悪くはないが、\nまだまだ精進が必要だな。",
				"一応合格とするが、\nさらに高見を目指せ！"
			}
		},
		{
			"rateC", {
				"これはひどい…",
				"不合格だ！\nもう一度出直してこい！"
			}
		}
	};

}