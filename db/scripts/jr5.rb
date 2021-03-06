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
  fn = ARGV[0] 
else
  fn = 'jr.txt'
end
ln = 0
h_eki = Hash.new(0)
r1 = Regexp::new('駅.*駅')
r2 = Regexp::new('.+駅\Z')
e1 = Array::new
e2 = Array::new
branch = Hash.new("")
IO.foreach(fn) do |lin|
  ln += 1
  linitems = lin.split(/\t/)
  eki = linitems[3]		# 駅
  h_eki[eki] += 1
  train_lines = Array.new
  train_lines << linitems[2]    # 路線 
	if linitems[7].strip != ""
  	train_lines.concat linitems[7].split(/\//)
  end
  train_lines.map! {|m| m.strip }
	train_lines.compact!     				   # nil を配列から取り出す
	train_lines.reject! {|m| m == ""}  # 空文字も配列から除去
  train_lines.sort!									 # 路線、分岐路線群の配列ソート
  if branch[eki] != ""
    branch[eki] += ","
  end
  branch[eki] += train_lines.join("|")   # 各駅毎に路線、分岐路線群を|区切りの文字列で保持

  if r1.match(eki)   # 駅を含む駅
	  e1 << sprintf("%s(%d): %s", fn, ln, eki)
  end
  unless r2.match(eki) # 駅で終わっていない駅
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
puts("＊＊＊重複駅名 or 分岐駅(所属路線が複数)＊＊＊")
# 江差線|函館線,江差線|函館線
# 函館線

# h_keys  各駅登場回数を保持
# branch  各駅所属路線を保持
#
h_eki.keys.each do |e|				# 各駅毎
  bra = branch[e].split(",")   # 駅が2回以上ある場合, braに個数分の所属路線の配列が入る
  bra.map! {|r| r.split("|") }
  if 1 < h_eki[e] # 分岐駅 or 2度以上登場した駅 e
    if bra.length != h_eki[e]
      puts "!!!!!!!!!bug!!!!! #{e}:#{bra.length}"
    end
    puts(e + "(#{h_eki[e]})")   # 駅名(登場数)
		other_station_lines = nil
		n_eki = 0
    bra.each do |station_lines|		# 同名駅毎
      n_eki += 1
			unless other_station_lines
				other_station_lines = station_lines
				if station_lines.length <= 0
					puts "!!!!!bug!!!!!"
				elsif station_lines.length  == 1
					puts "  #{e}駅#{n_eki}/#{bra.length}: 駅複数なのに分岐駅なし(同名駅)"
				elsif station_lines.length != h_eki[e]
					puts "  #{e}駅#{n_eki}/#{bra.length}: 分岐路線数の相違 #{station_lines.length}, #{h_eki[e]}: #{station_lines.join('/')}"
				end
			end
			if other_station_lines != station_lines and station_lines.length != 1
				puts "  #{e}駅#{n_eki}/#{bra.length}: 分岐路線不足: #{other_station_lines.join('/')}, #{station_lines.join('/')}"
			elsif station_lines.length == 1
				puts "               #{station_lines.join('/')}"
			end
    end
  else # 駅1個(分岐なし)
	  if 1 < bra[0].length
      puts "駅一個なのに分岐駅？ #{e}:#{bra.length}->#{bra[0]}"
	  end
  end
end


