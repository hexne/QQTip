# QQTip

tasks.json 示例:
```
{
    "舔狗反派": {
        "event-type": "NovelUpdate",
        "url": [
            "http://www.shukuge.com/book/148130/",
            "https://www.biqugezw.us/book/322586/",
            "https://www.96dushu.com/book/448587/"
        ],
        "timer-interval": "1m"
    },
    "背单词提醒": {
        "event-type": "Tip",
        "tip-time": "17:00:00",
        "interval": "1d",
        "is_repeat": true
    }
}
```

config.json 示例:
```
`{
    "admin" : "",
    "host" : "127.0.0.1",
    "http" : {
        "port" : 3000,
        "token" : ""
    },
    "ws" : {
        "port" : 3001,
        "token" : ""
    }
}`
```