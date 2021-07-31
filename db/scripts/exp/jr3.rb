#! ruby -Ks


=begin sample data

�H����Ӊ�
�k�C��	JR�k�C��	���ِ�	���ىw	�͂�����	0	0	 	

JR�k�C��	���ٖ{��	0		���ىw	�͂�����	 
JR�k�C��	���ٖ{��	3.4		�ܗŊs�w	����傤����	���]����
JR�k�C��	���ٖ{��	8.3		�j�[�w	�����傤	 
 :


�ŏ��̉c�ƃL����0km�łȂ�����
�c�ƃL������ނ��Ă������
�𒊏o����

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
    if (item[5].to_i != 0) || (item[6].to_i != 0)   # �ŏ��̉c�ƃL����0�ł͂Ȃ�
		  puts "#{fn}(#{ln}): " + lin
    end
      # puts rosen #   + "\t" + km.to_s
    rosen = item[2]
    km = item[5].to_f
    ckm = item[6].to_f
  else
    cur_km = item[5].to_f
    cur_ckm = item[6].to_f
    if cur_km <= km || ((ckm != 0) && (cur_ckm <= ckm))  # �c�ƃL�����i��łȂ��܂��͌��
		  puts "#{fn}(#{ln})! " + lin
    end
    km = cur_km
    ckm = cur_ckm
	end
end

