#! ruby -Ks


=begin sample data

路線一意下
北海道	JR北海道	函館線	函館駅	はこだて	0	0	 	

JR北海道	函館本線	0		函館駅	はこだて	 
JR北海道	函館本線	3.4		五稜郭駅	ごりょうかく	■江差線
JR北海道	函館本線	8.3		桔梗駅	ききょう	 
 :


最初の営業キロが0kmでないもの
営業キロが後退しているもの
を抽出する

=end

if ARGV[0] 
  fn ARGV[0] 
else
  fn = 'jr.txt'
end

re = Regexp::new('^\d+\.{0,1}\d*\s*\Z')
rosen = ''
ln = 0
km = 0.0
ckm = 0.0
IO.foreach(fn) do |lin|
  ln += 1
  item = lin.split(/\t/)
  if rosen != item[2]
    if (item[5].to_i != 0) || (item[6].to_i != 0)   # 最初の営業キロが0ではない
		  puts "#{fn}(#{ln}): " + lin
    end
      # puts rosen #   + "\t" + km.to_s
    rosen = item[2]
    km = item[5].to_f
    ckm = item[6].to_f
  else
    cur_km = item[5].to_f
    cur_ckm = item[6].to_f
    if cur_km <= km || ((ckm != 0) && (cur_ckm <= ckm))  # 営業キロが進んでないまたは後退
		  puts "#{fn}(#{ln})! " + lin
    end
    km = cur_km
    ckm = cur_ckm
	end
end

