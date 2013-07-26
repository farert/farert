select id from t_lines l join t_station t on t.rowid=l.station_id join t_jct j on j.station_id=l.station_id where 
line_id=?1 and jctflg!=0 and 
((sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)) and 
 (sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3))) order by sales_km;


select id, t.name from t_lines l join t_station t on t.rowid=l.station_id join t_jct j on j.station_id=l.station_id where 
line_id=70 and jctflg!=0 and 
case when 
(select sales_km from t_lines where line_id=70 and station_id=1471) <
(select sales_km from t_lines where line_id=70 and station_id=1536) then 
((sales_km>=(select sales_km from t_lines where line_id=70 and station_id=1471)) and 
 (sales_km<=(select sales_km from t_lines where line_id=70 and station_id=1536))) order by sales_km
else 
((sales_km<=(select sales_km from t_lines where line_id=70 and station_id=1471)) and 
 (sales_km>=(select sales_km from t_lines where line_id=70 and station_id=1536))) order by sales_km desc end


���C���� 70
���˒� 1471


�K�c 1536



select id, t.name from t_lines l join t_station t on t.rowid=l.station_id join t_jct j on j.station_id=l.station_id where 
line_id=70 and jctflg!=0 and 
case when 
(select sales_km from t_lines where line_id=70 and station_id=1471) <
(select sales_km from t_lines where line_id=70 and station_id=1536) then 
(((sales_km>=(select sales_km from t_lines where line_id=70 and station_id=1471)) and 
 (sales_km<=(select sales_km from t_lines where line_id=70 and station_id=1536))) ) else 
(((sales_km<=(select sales_km from t_lines where line_id=70 and station_id=1471)) and 
 (sales_km>=(select sales_km from t_lines where line_id=70 and station_id=1536))) ) end;
 
 order by sales_km desc
 order by sales_km
 
 
select id, t.name from t_lines l join t_station t on t.rowid=l.station_id join t_jct j on j.station_id=l.station_id where 
line_id=70 and jctflg!=0 and 
case when 
(select sales_km from t_lines where line_id=70 and station_id=1536) <
(select sales_km from t_lines where line_id=70 and station_id=1471) then 
(((sales_km>=(select sales_km from t_lines where line_id=70 and station_id=1536)) and 
 (sales_km<=(select sales_km from t_lines where line_id=70 and station_id=1471))) ) else 
(((sales_km<=(select sales_km from t_lines where line_id=70 and station_id=1536)) and 
 (sales_km>=(select sales_km from t_lines where line_id=70 and station_id=1471))) ) end
order by 
case when 
(select sales_km from t_lines where line_id=70 and station_id=1536) <
(select sales_km from t_lines where line_id=70 and station_id=1471) then 
sales_km  
end asc,
case when 
(select sales_km from t_lines where line_id=70 and station_id=1536) >
(select sales_km from t_lines where line_id=70 and station_id=1471) then 
sales_km  
end desc;

--
select id, t.name from t_lines l join t_station t on t.rowid=l.station_id join t_jct j on j.station_id=l.station_id where 
line_id=?1 and jctflg!=0 and 
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) <
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
(((sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and 
 (sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) else 
(((sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and 
 (sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) end
order by 
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) <
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
sales_km  
end asc,
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) >
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
sales_km  
end desc;

-----------------
2�_�ԃL��
select l1.sales_km-l2.sales_km, l1.calc_km-l2.sales_km 
from t_lines l1 
join t_lines l2 
where line_id=?1 
and l1.sales_km>l2.sales_km 
and ((l1.station_id=?2 and l2.station_id=?3) 
or (l1.station_id=?3 and l2.station_id=?2))



select l1.sales_km - l2.sales_km, l1.calc_km - l2.sales_km 
from t_lines l1 
join t_lines l2 
where line_id=139 
and l1.sales_km>l2.sales_km 
and ((l1.station_id=3143 and l2.station_id=3102) 
or (l1.station_id=3102 and l2.station_id=3143))

-----------------
�g�i�@1952�@3260
�O�΁@2023�@3570
�a�C�@1899�@3260

�R�z���@139
�g�����@107

select * from t_lines l join t_station t on t.rowid=l.station_id join t_line n on n.rowid=l.line_id where n.name='�g����' order by sales_km;

?1=1145(���l-�x�m�c�ƃL��)
?2=1145(���l-�x�m�v�Z�L��)
?3=�g����
?4=2000
select ?1+sales_km as is114s, ?2+calc_km as is114c 
from t_lines  
where n.name=?3
and sales_km+?1>?4  
order by sales_km limit(1);


�b��Z�g 2340
���� 2341
�O�� 3100
�g�i 3101
�a�C 3102

�L�� 3143
�x�m 1495

�|���� �䌴�s-�L�� 371m, 408m

***�L��(3143)���牺�������200km���B�n�_
select l1.sales_km-l2.sales_km as sales_km, l1.station_id
from  t_lines l1 join t_lines l2 
where l1.line_id=139 and l2.line_id=139 
and   l1.sales_km>l2.sales_km 
and   l2.station_id=3143 
and   (l1.sales_km-l2.sales_km)>2000 order by l1.sales_km limit(1);

****�L��(3143)�����������200km���B�n�_
select l2.sales_km-l1.sales_km as sales_km, l1.station_id
from  t_lines l1 join t_lines l2 
where l1.line_id=139 and l2.line_id=139 
and   l2.sales_km>l1.sales_km 
and   l2.station_id=3143 
and   (l2.sales_km-l1.sales_km)>2000 order by l1.sales_km desc limit(1);

-----------------------------
����w�ׂ̗̕���w�ꗗ

�܂�����w�̑S�H��
select line_id from t_lines where station_id=?

535: �����q 474
23�F��������
73�F���l��
30�F������
633�@�q��
1636�@���{
556�@�b�{
532�@���� 375


�H���̕���w�̗��ׂ̕���w�i����w���[�w��������1�A�ȊO�Ȃ�2�j
select (
select station_id from t_lines where line_id=23 and sales_km=(select max(sales_km) from t_lines l join t_station t on t.rowid=l.station_id where line_id=23 and jctflg<>0 and sales_km<
(select sales_km from t_station t, t_lines l where t.rowid=l.station_id and jctflg<>0 and line_id=23 and station_id=535)
)
) as up,
(
select station_id from t_lines where line_id=23 and sales_km=(select min(sales_km) from t_lines l join t_station t on t.rowid=l.station_id where line_id=23 and jctflg<>0 and sales_km>
(select sales_km from t_station t, t_lines l where t.rowid=l.station_id and jctflg<>0 and line_id=23 and station_id=535)
)
) as down

-> 532, 556���Ԃ����(2�J�����ŕԂ�)



select 
(
select y.station_id  from t_lines x, t_lines y, t_station t where x.line_id=y.line_id and x.line_id=23 and x.station_id=535 and t.rowid=y.station_id and t.jctflg<>0 and x.sales_km>y.sales_km order by y.sales_km desc limit(1)
) as up, (
select y.station_id  from t_lines x, t_lines y, t_station t where x.line_id=y.line_id and x.line_id=23 and x.station_id=535 and t.rowid=y.station_id and t.jctflg<>0 and x.sales_km<y.sales_km order by y.sales_km limit(1)
) as down
-> 532, 556���Ԃ����


other(�s�ŕԂ�)
--
select 	station_id 
from 	t_lines 
where 	line_id=23 
and 	sales_km in ((select max(y.sales_km)  
		      from t_lines x, t_lines y, t_station t 
		      where x.line_id=y.line_id 
		      and x.line_id=23 
		      and x.station_id=535 
		      and t.rowid=y.station_id 
		      and t.jctflg<>0 
		      and x.sales_km>y.sales_km) ,
		     (select min(y.sales_km)  
		      from t_lines x, t_lines y, t_station t 
		      where x.line_id=y.line_id 
		      and x.line_id=23 
		      and x.station_id=535 
		      and t.rowid=y.station_id 
		      and t.jctflg<>0 
		      and x.sales_km<y.sales_km));
532
556
--
select 	station_id 
from 	t_lines 
where 	line_id=23 
and 	sales_km=(select max(sales_km) 
		  from t_lines l 
		  join t_station t 
		  on t.rowid=l.station_id 
		  where line_id=23 
		  and jctflg<>0 
		  and sales_km<(select 	sales_km 
		  		from 	t_station t, t_lines l 
		  		where 	t.rowid=l.station_id 
		  		and 	jctflg<>0 
		  		and 	line_id=23 
		  		and 	station_id=535))
union
select 	station_id 
from 	t_lines 
where 	line_id=23 
and 	sales_km=(select min(sales_km) 
		  from t_lines l 
		  join t_station t 
		  on t.rowid=l.station_id 
		  where line_id=23 
		  and jctflg<>0 
		  and sales_km>(select sales_km 
		  		from t_station t, t_lines l 
		  		where t.rowid=l.station_id 
		  		and jctflg<>0 
		  		and line_id=23 
		  		and station_id=535))
--
�ӊO��ӊO�ɁA��҂̕�����������

---------------------------
select 	station_id, (select sales_km from t_lines where line_id=23 and station_id=535)-sales_km as cost
from 	t_lines 
where 	line_id=23 
and 	sales_km=(select max(sales_km) 
		  from t_lines l 
		  join t_station t 
		  on t.rowid=l.station_id 
		  where line_id=23 
		  and jctflg<>0 
		  and sales_km<(select 	sales_km 
		  		from 	t_station t, t_lines l 
		  		where 	t.rowid=l.station_id 
		  		and 	jctflg<>0 
		  		and 	line_id=23 
		  		and 	station_id=535))
union
select 	station_id, sales_km - (select sales_km from t_lines where line_id=23 and station_id=535) as cost
from 	t_lines 
where 	line_id=23 
and 	sales_km=(select min(sales_km) 
		  from t_lines l 
		  join t_station t 
		  on t.rowid=l.station_id 
		  where line_id=23 
		  and jctflg<>0 
		  and sales_km>(select sales_km 
		  		from t_station t, t_lines l 
		  		where t.rowid=l.station_id 
		  		and jctflg<>0 
		  		and line_id=23 
		  		and station_id=535))
---------------------------
select 	station_id, (select case when calc_km>0 then calc_km else sales_km end from t_lines where line_id=?1 and station_id=?2)-case when calc_km>0 then calc_km else sales_km end as cost
from 	t_lines 
where 	line_id=?1 
and 	sales_km=(select max(sales_km) 
		  from t_lines l 
		  join t_station t 
		  on t.rowid=l.station_id 
		  where line_id=?1 
		  and jctflg<>0 
		  and sales_km<(select 	sales_km 
		  		from 	t_station t, t_lines l 
		  		where 	t.rowid=l.station_id 
		  		and 	jctflg<>0 
		  		and 	line_id=?1 
		  		and 	station_id=?2))
union
select 	station_id, case when calc_km>0 then calc_km else sales_km end - (select case when calc_km>0 then calc_km else sales_km end from t_lines where line_id=?1 and station_id=?2) as cost
from 	t_lines 
where 	line_id=?1 
and 	sales_km=(select min(sales_km) 
		  from t_lines l 
		  join t_station t 
		  on t.rowid=l.station_id 
		  where line_id=?1 
		  and jctflg<>0 
		  and sales_km>(select sales_km 
		  		from t_station t, t_lines l 
		  		where t.rowid=l.station_id 
		  		and jctflg<>0 
		  		and line_id=?1 
		  		and station_id=?2))

---------------------------
�w����אڂ��镪��w�𓾂�

���C 1918
���C�� 90
������ 564
���v�� 1932

select 	station_id 
from 	t_lines 
where 	line_id=(select line_id from t_lines where station_id=1918)
and 	sales_km in ((select max(y.sales_km)  
		      from t_lines x, t_lines y, t_station t 
		      where x.line_id=y.line_id 
		      -- and x.line_id=(select line_id from t_lines where station_id=1918)
		      and x.station_id=1918
		      and t.rowid=y.station_id 
		      and t.jctflg<>0 
		      and x.sales_km>y.sales_km) ,
		     (select min(y.sales_km)  
		      from t_lines x, t_lines y, t_station t 
		      where x.line_id=y.line_id 
		      -- and x.line_id=(select line_id from t_lines where station_id=1918)
		      and x.station_id=1918
		      and t.rowid=y.station_id 
		      and t.jctflg<>0 
		      and x.sales_km<y.sales_km));








select 	station_id , abs((select case when calc_km>0 then calc_km else sales_km end from t_lines where line_id=(select line_id from t_lines where station_id=1918) and station_id=1918)-case when calc_km>0 then calc_km else sales_km end) as cost
from 	t_lines 
where 	line_id=(select line_id from t_lines where station_id=1918)
and 	sales_km in ((select max(y.sales_km)  
		      from t_lines x, t_lines y, t_station t 
		      where x.line_id=y.line_id 
		      -- and x.line_id=(select line_id from t_lines where station_id=1918)
		      and x.station_id=1918
		      and t.rowid=y.station_id 
		      and t.jctflg<>0 
		      and x.sales_km>y.sales_km) ,
		     (select min(y.sales_km)  
		      from t_lines x, t_lines y, t_station t 
		      where x.line_id=y.line_id 
		      -- and x.line_id=(select line_id from t_lines where station_id=1918)
		      and x.station_id=1918
		      and t.rowid=y.station_id 
		      and t.jctflg<>0 
		      and x.sales_km<y.sales_km));



------------------------
select name, rowid from t_company where name like 'JR%'
union
select name, rowid*65536 from t_prefect
order by rowid;





----------------
0=(lflg & ((1 << 24)|(1 << 31)) and sales_km>0
	70���ł��������ł��V����(����}�[�N�p�ݗ����w)�ł͂Ȃ�.

(sflg&(1<<23))=0 JR�݂̂ŉ�А������O
(sflg&(1<<12))!=0 ����w
----------------


-- ���&�s���{���ꗗ���
select name, rowid from t_company where name like 'JR%' union select name, rowid*65536 from t_prefect order by rowid


-- �s���{��or��Ђɑ�����H���ꗗ(JR�̂�)
select n.name, line_id, lflg from t_lines l left join t_station t on t.rowid=l.station_id left join 
t_line n on n.rowid=l.line_id where %s=%d and (l.lflg&(1<<23))=0 group by l.line_id order by n.name
�� JR���������ʂ���������

select n.name, line_id, lflg from t_line n left join t_lines l on n.rowid=l.line_id left join  t_station t on t.rowid=l.station_id  
 where %s=%d and (l.lflg&(1<<23))=0 group by l.line_id order by n.name

"select n.name, line_id, lflg from t_line n left join t_lines l on n.rowid=l.line_id left join"
" t_station t on t.rowid=l.station_id where %s=%d and (l.lflg&((1<<23)|(1<<31)|(1<<24)))=0 and sales_km>=0 group by l.line_id order by n.name";



-- ���or�s���{��+�H���̉w�̃C�e���[�^��Ԃ�
select t.name, station_id from t_lines l left join t_station t on t.rowid=l.station_id 
where line_id=? and ?=1 order by sales_km

select t.name, station_id from t_station t left join t_lines l on t.rowid=l.station_id 
where line_id=? and ?=1 order by sales_km

"select t.name, station_id from t_station t left join t_lines l on t.rowid=l.station_id"
" where line_id=? and %s=? and (l.lflg&((1<<23)|(1<<31)|(1<<24)))=0 and sales_km>=0 order by sales_km";

----------------
�H���̕���w(�Ǝw��w���܂܂��΂��̉w��)�ꗗ
select t.name, station_id, sflg&(1<<12) 
from t_lines l left join t_station t on t.rowid=l.station_id 
where line_id=?1 and ((lflg & (1<<12))<>0  or station_id=?2)  
and (lflg&((1<<24)|(1<<31)))=0 and sales_km>=0
order by l.sales_km;

�H���̉w�ꗗ
select t.name, station_id, sflg&(1<<12) 
from t_lines l left join t_station t on t.rowid=l.station_id 
where line_id=?  
and (lflg&((1<<24)|(1<<31)))=0 and sales_km>=0 
order by l.sales_km;


�w�̏����H��
select n.name, line_id from t_line n left join t_lines l on n.rowid=l.line_id
where station_id=? and (lflg&((1<<31)|(1<<24)))=0 and sales_km>=0
-- join�̏��Ԃ�����ւ���NG�I

--------------
select id, t.name from t_lines l join t_station t on t.rowid=l.station_id join t_jct j on j.station_id=l.station_id where 
line_id=?1 and jctflg!=0 and 
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) <
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
(((sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and 
 (sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) else 
(((sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and 
 (sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) end
order by 
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) <
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
sales_km  
end asc,
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) >
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
sales_km  
end desc;
------------------


-- �H���̉w�Ԃɉw�͂��邩�H
select count(*) from (
 select station_id
 from t_lines
 where line_id=?1 
 and station_id
 in (select station_id 
      from t_lines 
      where line_id=?1 
      and ((sales_km>=(select sales_km 
                   from t_lines 
                   where line_id=?1 
                   and station_id=?2)
      and  (sales_km<=(select sales_km 
                   from t_lines 
                   where line_id=?1 
                   and station_id=?3))) 
      or  (sales_km<=(select sales_km 
                  from t_lines 
                  where line_id=?1 
                  and station_id=?2)
      and (sales_km>=(select sales_km 
                  from t_lines 
                  where line_id=?1 
                  and station_id=?3)))))
)where station_id=?4;

-------
-- ��肩���肩�H

select case when 
   ((select sales_km from t_lines where line_id=?1 and station_id=?2) - 
    (select sales_km from t_lines where line_id=?1 and station_id=?3)) < 0 
then 0 else 1 end;


------------------------------
-- 2012-6-22
-----------
�������R�[�h(����w)
select name from t_station where rowid in (select station_id from t_lines group by station_id having count(station_id)>1);
�� 329

--a
select name from t_station where rowid in (select station_id from t_lines where (lflg&((1<<24)|(1<<31)))=0 group by station_id having count(station_id)>1);
�� 312

--b
select name from t_station where rowid in (select station_id from t_lines where (lflg&((1<<24)|(1<<31)))=0 and (lflg&(1<<12))!=0);
�� 318

-- difference of a & b
select * from (
select name from t_station where rowid in (select station_id from t_lines where (lflg&((1<<24)|(1<<31)))=0 group by station_id having count(station_id)>1)
 except 
select name from t_station where rowid in (select station_id from t_lines where (lflg&((1<<24)|(1<<31)))=0 and (lflg&(1<<12))!=0)
)
union all
select * from (
select name from t_station where rowid in (select station_id from t_lines where (lflg&((1<<24)|(1<<31)))=0 and (lflg&(1<<12))!=0)
 except 
select name from t_station where rowid in (select station_id from t_lines where (lflg&((1<<24)|(1<<31)))=0 group by station_id having count(station_id)>1)
);

-> 8�{������c�A�v���A�ɖ����A�����ہA�Õl�A�{�ÁA�L�c�A���j

�{������c�́A�k������s�����Ə�z�V�����ɓo�ꂷ�邽�߁B
�v�����O���S���k���A�X�����폜�C��(�ʉߘA���^�A�ł͂Ȃ�����)
�ɖ����A�����ہF���Y�S��:����B���A
�Õl�F������ʋ�
�{�ÁF�O���S��:�k���A�X��
�L�c�F���Y�S��:����B��
���F�O���S��:�샊�A�X��
�ȏ�C���ς�



-------------------
-- t_lines �e�[�u���\���{���p
select n.name, t.name, l1.* from t_lines l1 join t_station t on t.rowid=l1.station_id join t_line n on n.rowid=l1.line_id where (lflg&((1<<24)|(1<<31)))=0;
-> 5021

-- t_lines�ŉw��������o�ꂷ�郌�R�[�h(����w)
select n.name, t.name, l1.* from t_lines l1 join t_station t on t.rowid=l1.station_id join t_line n on n.rowid=l1.line_id where (lflg&((1<<24)|(1<<31)))=0 and
exists (select * from t_lines l2 where (lflg&((1<<24)|(1<<31)))=0 and l1.line_id!=l2.line_id and l1.station_id=l2.station_id) order by l1.station_id;
-> 743
����w�A��d�ːЉw�Ȃ�

select t.name from t_lines l1 join t_station t on t.rowid=l1.station_id join t_line n on n.rowid=l1.line_id where (lflg&((1<<24)|(1<<31)))=0 and
exists (select * from t_lines l2 where (lflg&((1<<24)|(1<<31)))=0 and l1.line_id!=l2.line_id and l1.station_id=l2.station_id) group by t.name
-> ���Ԃ������肷�� 10���o�Ă��߂��ė��Ȃ��B
��
create table t(name);
insert into t(name)
select t.name from t_lines l1 join t_station t on t.rowid=l1.station_id join t_line n on n.rowid=l1.line_id where (lflg&((1<<24)|(1<<31)))=0 and
exists (select * from t_lines l2 where (lflg&((1<<24)|(1<<31)))=0 and l1.line_id!=l2.line_id and l1.station_id=l2.station_id);
select name, count(*) from t group by name;
-> 311

t_lines ����w�̐�
select n.name, t.name, l1.* from t_lines l1 join t_station t on t.rowid=l1.station_id 
					�@�@join t_line n on n.rowid=l1.line_id 
where (lflg&((1<<24)|(1<<31)))=0 and (lflg&(1<<12))!=0;
-> 748

--(c) ����w�̐�(1<12)=ON�̂���
select t.name, count(*) from t_lines l1 join t_station t on t.rowid=l1.station_id join t_line n on n.rowid=l1.line_id where (lflg&((1<<24)|(1<<31)))=0 and (lflg&(1<<12))!=0 group by t.name
-> 317
--(d) ����w�̐�(t_station����)
select rowid from t_station where (sflg&(1<<12))!=0 ;
-> 318
-- (c)��(d)��(d)��1�������̂͒Ǖ����W�񂳂�Ȃ�����. t_jct��(d)���g�p

-- t_lines lflg��sflg�̈قȂ��
select n.name, t.name, l1.*,sflg from t_lines l1 join t_station t on t.rowid=l1.station_id join t_line n on n.rowid=l1.line_id where (lflg&((1<<24)|(1<<31)))=0 and
lflg!=(select sflg from t_lines l2 where (lflg&((1<<24)|(1<<31)))=0 and l1.line_id!=l2.line_id and l1.station_id=l2.station_id) order by l1.station_id;
�� 160�s
��179�s


-------------------
��Ԍo�H�}�[�N
-�\�\�\�\�\�\�\�\�\�\�\

select id, t.name from t_lines l join t_station t on t.rowid=l.station_id join t_jct j on j.station_id=l.station_id where 
line_id=?1 and (lflg&(1<<12))!=0 and 
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) <
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
(((sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and 
 (sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) else 
(((sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and 
 (sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) end
order by 
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) <
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
sales_km  
end desc,
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) >
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
sales_km  
end asc;

select id, t.name from t_lines l join t_station t on t.rowid=l.station_id join t_jct j on j.station_id=l.station_id where 
line_id=29 and (lflg&(1<<12))!=0 and 
case when 
(select sales_km from t_lines where line_id=29 and station_id=517) <
(select sales_km from t_lines where line_id=29 and station_id=540) then 
(((sales_km>=(select sales_km from t_lines where line_id=29 and station_id=517)) and 
 (sales_km<=(select sales_km from t_lines where line_id=29 and station_id=540))) ) else 
(((sales_km<=(select sales_km from t_lines where line_id=29 and station_id=517)) and 
 (sales_km>=(select sales_km from t_lines where line_id=29 and station_id=540))) ) end
order by 
case when 
(select sales_km from t_lines where line_id=29 and station_id=517) <
(select sales_km from t_lines where line_id=29 and station_id=540) then 
sales_km  
end desc,
case when 
(select sales_km from t_lines where line_id=29 and station_id=517) >
(select sales_km from t_lines where line_id=29 and station_id=540) then 
sales_km  
end asc;

-- �݌v����asc, desc���t
-- �\���p�w���͕s�v�Ȃ̂ňȉ��Ƃ���(routemark.py�ɂ���)
select id from t_lines l join t_jct j on j.station_id=l.station_id where 
line_id=?1 and (lflg&(1<<12))!=0 and 
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) <
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
(((sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and 
 (sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) else 
(((sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and 
 (sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) end
order by 
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) <
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
sales_km  
end desc,
case when 
(select sales_km from t_lines where line_id=?1 and station_id=?3) >
(select sales_km from t_lines where line_id=?1 and station_id=?2) then 
sales_km  
end asc;

----
rule69

select 15 & (lflg>>17),* from t_lines l left join t_station t on t.rowid=l.station_id where (lflg&(15<<17))!=0 order by 15 & (lflg>>17);



141:�R�z��

 select station_id, (lflg>>17)&15, (lflg>>29)&1,  (lflg>>28)&1
 from t_lines
 where line_id=141
 and station_id
 in (select station_id 
      from t_lines 
      where line_id=         141
      and ((sales_km>=(select sales_km 
                   from t_lines 
                   where line_id=   141
                   and station_id=        3065)
      and  (sales_km<=(select sales_km 
                   from t_lines 
                   where line_id=   141 
                   and station_id=       3179))) 
      or  (sales_km<=(select sales_km 
                  from t_lines 
                  where line_id=   141 
                  and station_id=        3065)
      and (sales_km>=(select sales_km 
                  from t_lines 
                  where line_id=  141
                  and station_id=      3179))))) and (lflg&(1<<29))!=0;

-->
3144	8	1	0
3158	8	1	0



-------------------------
�͈͓��ɉw�����邩�H

select count(*) from (
 select station_id
 from t_lines
 where line_id=?1
 and station_id
 in (select station_id 
	from t_lines 
	where line_id=?1 
	and ((sales_km>=(select sales_km 
			from t_lines 
			where line_id=?1 
			and station_id=?2 
			and (lflg&(1<<31))=0)
		and (sales_km<=(select sales_km 
			from t_lines 
			where line_id=?1 
			and station_id=?3 and (lflg&(1<<31))=0))) 
	or (sales_km<=(select sales_km 
			from t_lines 
			where line_id=?1 
			and station_id=?2 and (lflg&(1<<31))=0)
	and (sales_km>=(select sales_km 
			from t_lines 
			where line_id=?1 
			and station_id=?3 and (lflg&(1<<31))=0)))))
) where station_id=?4;

select count(*) from (
 select station_id
 from t_lines
 where line_id=?1
 and station_id
 in (select station_id 
	from t_lines 
	where line_id=?1 
	and ((sales_km>=(select sales_km 
			from t_lines 
			where line_id=?1 
			and station_id=?2 
			and (lflg&(1<<31))=0)
		and (sales_km<=(select sales_km 
			from t_lines 
			where line_id=?1 
			and station_id=?3 and (lflg&(1<<31))=0))) 
	or (sales_km<=(select sales_km 
			from t_lines 
			where line_id=?1 
			and station_id=?2 and (lflg&(1<<31))=0)
	and (sales_km>=(select sales_km 
			from t_lines 
			where line_id=?1 
			and station_id=?3 and (lflg&(1<<31))=0)))))
) where station_id=?4;


----------�̗p
 select count(*) 
	from t_lines 
	where line_id=?1 
	and station_id=?4
	and sales_km>=
			(select min(sales_km)
			from t_lines 
			where line_id=?1
			and (station_id=?2 or 
				 station_id=?3)
			and (lflg&(1<<31))=0)
	and sales_km<=
			(select max(sales_km)
			from t_lines 
			where line_id=?1
			and (station_id=?2 or 
				 station_id=?3)
			and (lflg&(1<<31))=0)

--------------------------------------------------------.
�͈͓��̕���w���X�g
select id from t_lines l join t_jct j on j.station_id=l.station_id where
 line_id=?1 and (lflg&(1<<12))!=0 and (lflg&(1<<31))=0 and
 case when
 (select sales_km from t_lines where line_id=?1 and station_id=?3) <
 (select sales_km from t_lines where line_id=?1 and station_id=?2) then 
 (((sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and
 (sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) else
 (((sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and
 (sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) end
 order by
 case when
 (select sales_km from t_lines where line_id=?1 and station_id=?3) <
 (select sales_km from t_lines where line_id=?1 and station_id=?2) then
 sales_km
 end desc,
 case when
 (select sales_km from t_lines where line_id=?1 and station_id=?3) >
 (select sales_km from t_lines where line_id=?1 and station_id=?2) then
 sales_km
 end asc;

----------------------------------------------
- distance
select l1.sales_km - l2.sales_km, l1.calc_km - l2.calc_km
 from	t_lines l1
 left join t_lines l2
 where	l1.line_id=?1
 and	l2.line_id=?1
 and	l1.sales_km>l2.sales_km
 and	((l1.station_id=?2 and l2.station_id=?3) 
 	or (l1.station_id=?3 and l2.station_id=?2));





0	a
10	b
13	c
15	d
19	e
20	f
43	g
55	h

c g


select	(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-
		(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)),
		(select max(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-
		(select min(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)),
		abs((select sales_km from t_lines where line_id=?1 and (lflg&(1<<21))!=0)-
		(select sales_km from t_lines where line_id=?1 and station_id=?2)),
		(select max(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-
		(select calc_km from t_lines where line_id=?1 and (lflg&(1<<21))!=0),
		(select company_id from t_station where rowid=?2),
		(select company_id from t_station where rowid=?3)
"""

# sales_km, calc_km, sales_km(station1�̉�Ћ��), calc_km(station1�̉�Ћ��), station1��company_id, station2��company_id



