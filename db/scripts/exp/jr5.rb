#! ruby -Ks


=begin sample data

�H����Ӊ�
JR�k�C��	���ٖ{��	0		���ىw	�͂�����	 
JR�k�C��	���ٖ{��	3.4		�ܗŊs�w	����傤����	���]����
JR�k�C��	���ٖ{��	8.3		�j�[�w	�����傤	 
 :

�k�C��	JR�k�C��	���ِ�	���ىw	�͂�����	0	0	 	

�w���ŁA�w�Ƃ����������������Ă���w
�d���w��

�𒊏o

=end

if ARGV[0] 
  fn = ARGV[0] 
else
  fn = 'jr.txt'
end
ln = 0
h_eki = Hash.new(0)
r1 = Regexp::new('�w.*�w')
r2 = Regexp::new('.+�w\Z')
e1 = Array::new
e2 = Array::new
branch = Hash.new("")
IO.foreach(fn) do |lin|
  ln += 1
  linitems = lin.split(/\t/)
  eki = linitems[3]		# �w
  h_eki[eki] += 1
  train_lines = Array.new
  train_lines << linitems[2]    # �H�� 
	if linitems[7].strip != ""
  	train_lines.concat linitems[7].split(/\//)
  end
  train_lines.map! {|m| m.strip }
	train_lines.compact!     				   # nil ��z�񂩂���o��
	train_lines.reject! {|m| m == ""}  # �󕶎����z�񂩂珜��
  train_lines.sort!									 # �H���A����H���Q�̔z��\�[�g
  if branch[eki] != ""
    branch[eki] += ","
  end
  branch[eki] += train_lines.join("|")   # �e�w���ɘH���A����H���Q��|��؂�̕�����ŕێ�

  if r1.match(eki)   # �w���܂މw
	  e1 << sprintf("%s(%d): %s", fn, ln, eki)
  end
  unless r2.match(eki) # �w�ŏI����Ă��Ȃ��w
	  e2 << sprintf("%s(%d): %s", fn, ln, eki)
  end
end    
puts("�������w�Ƃ�������������������")
e1.each do |e|
   puts(e)
end
#puts("�������w�Ƃ��������ŏI����Ă��Ȃ�������")
#e2.each do |e|
#   puts(e)
#end
puts("�������d���w�� or ����w(�����H��������)������")
# �]����|���ِ�,�]����|���ِ�
# ���ِ�

# h_keys  �e�w�o��񐔂�ێ�
# branch  �e�w�����H����ێ�
#
h_eki.keys.each do |e|				# �e�w��
  bra = branch[e].split(",")   # �w��2��ȏ゠��ꍇ, bra�Ɍ����̏����H���̔z�񂪓���
  bra.map! {|r| r.split("|") }
  if 1 < h_eki[e] # ����w or 2�x�ȏ�o�ꂵ���w e
    if bra.length != h_eki[e]
      puts "!!!!!!!!!bug!!!!! #{e}:#{bra.length}"
    end
    puts(e + "(#{h_eki[e]})")   # �w��(�o�ꐔ)
		other_station_lines = nil
		n_eki = 0
    bra.each do |station_lines|		# �����w��
      n_eki += 1
			unless other_station_lines
				other_station_lines = station_lines
				if station_lines.length <= 0
					puts "!!!!!bug!!!!!"
				elsif station_lines.length  == 1
					puts "  #{e}�w#{n_eki}/#{bra.length}: �w�����Ȃ̂ɕ���w�Ȃ�(�����w)"
				elsif station_lines.length != h_eki[e]
					puts "  #{e}�w#{n_eki}/#{bra.length}: ����H�����̑��� #{station_lines.length}, #{h_eki[e]}: #{station_lines.join('/')}"
				end
			end
			if other_station_lines != station_lines and station_lines.length != 1
				puts "  #{e}�w#{n_eki}/#{bra.length}: ����H���s��: #{other_station_lines.join('/')}, #{station_lines.join('/')}"
			elsif station_lines.length == 1
				puts "               #{station_lines.join('/')}"
			end
    end
  else # �w1��(����Ȃ�)
	  if 1 < bra[0].length
      puts "�w��Ȃ̂ɕ���w�H #{e}:#{bra.length}->#{bra[0]}"
	  end
  end
end


