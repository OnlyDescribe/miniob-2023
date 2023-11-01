#!/bin/bash

# 测试脚本，包括sql生成语句，和启动脚本命令
python3 random_data.py >random.sql

# docker run -it --name test_sql -e MYSQL_ALLOW_EMPTY_PASSWORD=yes -v /Users/yangwendeng/Documents/miniob-2023:/code mysql:latest
docker exec -i test_sql mysql </Users/yangwendeng/Documents/miniob-2023/random.sql >/Users/yangwendeng/Documents/miniob-2023/out1.sql
nohup tail -n +2 random.sql | ./build_debug/bin/observer -f ./etc/observer.ini -P cli >out2.sql
diff out1.sql out2.sql
pgrep -f observer | xargs kill -9
