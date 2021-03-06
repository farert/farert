#! ruby -Ks


=begin sample data

路線一意下
JR北海道	函館本線	0		函館駅	はこだて	 
JR北海道	函館本線	3.4		五稜郭駅	ごりょうかく	■江差線
JR北海道	函館本線	8.3		桔梗駅	ききょう	 
 :

北海道	JR北海道	函館線	函館駅	はこだて	0	0	 	

駅名で、駅という文字が混じっている駅
重複駅名

を抽出

=end

if ARGV[0] 
  fn ARGV[0] 
else
  fn = 'jr.txt'
end
ln = 0
h_eki = Hash.new(0)
r1 = Regexp::new('駅.*駅')
r2 = Regexp::new('.+駅\Z')
e1 = Array::new
e2 = Array::new
branch = Hash.new([])
IO.foreach(fn) do |lin|
  ln += 1
  linitems = lin.split(/\t/)
  eki = linitems[3]		# 駅
  h_eki[eki] += 1
  branches = Array.new
  branches << linitems[2]
	if linitems[7]
  	branches.concat linitems[7].split(/\//)
  end
  branches.map! {|m| m.strip }
  branch[eki] << branches.join(",")
  if r1.match(eki)
	  e1 << sprintf("%s(%d): %s", fn, ln, eki)
  end
  unless r2.match(eki)
	  e2 << sprintf("%s(%d): %s", fn, ln, eki)
  end
end

puts("＊＊＊駅という文字が複数＊＊＊")
e1.each do |e|
   puts(e)
end
#puts("＊＊＊駅という文字で終わっていない＊＊＊")
#e2.each do |e|
#   puts(e)
#end
puts("＊＊＊重複駅名＊＊＊")
h_eki.keys.each do |e|
  if 1 < h_eki[e]
    s = ""
    d = []
    1.upto(branch[e].length - 1) do |i|
      if branch[e][0] != branch[e][i]
puts e + "===" + branch[e][i] + "/"
        d << branch[e][i]
      else
      puts "ppppppppppppppppppppppppppp"
      end
    end
    if 0 < d.length
      s = branch[e][0]
      d.each do |de|
        if de
         s += "=>"
         s += de
        else
        puts "そんなのあるかい"
        end
      end
    else
      s = ""
    end
    puts(e + "(#{h_eki[e]})\t#{s}")
exit
  end
end


