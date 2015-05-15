#! ruby -Ks


=begin sample data

路線一意下
ken	JR北海道	函館本線	0		函館駅	はこだて	 
len JR北海道	函館本線	3.4		五稜郭駅	ごりょうかく	■江差線
len JR北海道	函館本線	8.3		桔梗駅	ききょう	 
 :

jr6.rb		駅名で、駅でおわっていない駅(全行スキャン)。

jr.txt
を抽出

=end

if ARGV[0] 
  fn ARGV[0] 
else
  fn = 'jr.txt'
end
ln = 0
r2 = Regexp::new('.+駅\Z')
IO.foreach(fn) do |lin|
  ln += 1
  eki = lin.split(/\t/)[3]		# 駅
  unless r2.match(eki)
	  puts sprintf("%s(%d): %s", fn, ln, eki)
  end
end

