#! ruby -Ku
create table 社員 (
  名前 varchar(10),
  年齢 integer,
  部署 varchar(200)
);
SQL
db.execute(sql)
db.execute(sql)
db.execute(sql, '小泉', 35, '営業部')
db.execute(sql, '亀井', 40, '営業部')

  #rowは結果の配列
  puts row.join("\t")
end