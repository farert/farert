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
  fn ARGV[0] 
else
  fn = 'jr.txt'
end
ln = 0
h_eki = Hash.new(0)
r1 = Regexp::new('�w.*�w')
r2 = Regexp::new('.+�w\Z')
e1 = Array::new
e2 = Array::new
branch = Hash.new([])
IO.foreach(fn) do |lin|
  ln += 1
  linitems = lin.split(/\t/)
  eki = linitems[3]		# �w
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

puts("�������w�Ƃ�������������������")
e1.each do |e|
   puts(e)
end
#puts("�������w�Ƃ��������ŏI����Ă��Ȃ�������")
#e2.each do |e|
#   puts(e)
#end
puts("�������d���w��������")
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
        puts "����Ȃ̂��邩��"
        end
      end
    else
      s = ""
    end
    puts(e + "(#{h_eki[e]})\t#{s}")
exit
  end
end


