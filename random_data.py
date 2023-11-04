import random

# print("CREATE DATABASE test IF NOT EXISTS test;")
print("use test;")
print("drop table t_group_by;")
print("create table t_group_by (id int not null, score float not null, name char(1) null);")

def insert_one():
    id = random.randint(1, 10)
    score = round(random.uniform(5, 15), 2)
    is_null = random.randint(1, 10) % 4 == 0
    random_char = chr(random.randint(65, 90))

    if is_null:
        print(f"insert into t_group_by values({id}, {score}, null);")
    else:
        print(f"insert into t_group_by values({id}, {score}, '{random_char}');")
    

def insert_many(n):
    for i in range(n):
        insert_one()

def query_many(n):
    for i in range(n):
        id = random.randint(1, 10)
        random_char = chr(random.randint(65, 90))
        print(f"select name, count(id), max(score) from t_group_by where name > '{random_char}' and id>={id} group by name;")

insert_many(10)
query_many(1)
# print("select * from csq_1 where feat1 > (select min(csq_2.feat2) from csq_2 where csq_2.id in (select csq_3.id from csq_3 where csq_3.col3 <> (select csq_4.col4 from csq_4 where csq_4.id = csq_1.id)));")