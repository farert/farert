#! ruby -Ks


=begin sample data

路線一意下
北海道	JR北海道	函館線	函館駅	はこだて	0	0	 	
 :


路線一覧(重複は*マークと重複数)
線で終わってない線区
支線一覧
(xxx〜xxx)になっていない支線

を抽出

=end

if ARGV[0] 
  fn ARGV[0] 
else
  fn = 'jr.txt'
end

rosens = []
rosen = ''

IO.foreach(fn) do |lin|
  item = lin.split(/\t/)
  if rosen != item[2]
    rosens << item[2]
    rosen = item[2]
	end
end


h = Hash.new(0)
rosens.each do |r|
  h[r] += 1
  if 1 < h[r]
    puts('*' + r + "(#{h[r]})")
  else
    puts(r)
  end
end
puts("全#{h.length}路線")

puts("******* 支線一覧 ********")
sisen = []
r = Regexp::new('.+\(.+〜.+\)\Z')
h.keys.each do |k|
  if r.match(k)
    puts("#{k}")
    sisen << k
  end
end


puts("******* 線でない ********")
h.keys.each do |k|
  if k[-1] != '線'
    fsisen = 0
    sisen.each do |ss|
      if ss == k 
        fsisen = 1
      end
    end
    if fsisen != 1
      puts("#{k}")
    end
  end
end


