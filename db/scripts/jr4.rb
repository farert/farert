#! ruby -Ks


=begin sample data

�H����Ӊ�
�k�C��	JR�k�C��	���ِ�	���ىw	�͂�����	0	0	 	
 :


�H���ꗗ(�d����*�}�[�N�Əd����)
���ŏI����ĂȂ�����
�x���ꗗ
(xxx�`xxx)�ɂȂ��Ă��Ȃ��x��

�𒊏o

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
puts("�S#{h.length}�H��")

puts("******* �x���ꗗ ********")
sisen = []
r = Regexp::new('.+\(.+�`.+\)\Z')
h.keys.each do |k|
  if r.match(k)
    puts("#{k}")
    sisen << k
  end
end


puts("******* ���łȂ� ********")
h.keys.each do |k|
  if k[-1] != '��'
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


