import random

# print("CREATE DATABASE test IF NOT EXISTS test;")
print("use test;")
print("drop TABLE csq_1;")
print("drop TABLE csq_2;")
print("drop TABLE csq_3;")
print("drop TABLE csq_4;")

print("CREATE TABLE csq_1(id int, col1 int, feat1 float);")
print("CREATE TABLE csq_2(id int, col2 int, feat2 float);")
print("CREATE TABLE csq_3(id int, col3 int, feat3 float);")
print("CREATE TABLE csq_4(id int, col4 int, feat4 float);")

def insert_one(table_id):
    id = random.randint(1, 20)
    col = random.randint(1, 20)
    feat = round(random.uniform(5, 15), 2)
    print(f"INSERT INTO csq_{table_id} VALUES ({id}, {col}, {feat});")

def insert_many(table_num):
    for i in range(1, table_num + 1):
        n = random.randint(5,10)
        for j in range(n):
            insert_one(i)

insert_many(4)

print("select * from csq_1 where feat1 > (select min(csq_2.feat2) from csq_2 where csq_2.id in (select csq_3.id from csq_3 where csq_3.col3 <> (select csq_4.col4 from csq_4 where csq_4.id = csq_1.id)));")