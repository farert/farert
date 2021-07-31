#! ruby -Ks


=begin sample data

�H����Ӊ�
ken	JR�k�C��	���ٖ{��	0		���ىw	�͂�����	 
len JR�k�C��	���ٖ{��	3.4		�ܗŊs�w	����傤����	���]����
len JR�k�C��	���ٖ{��	8.3		�j�[�w	�����傤	 
 :

jr7.rb		�����ŁA���ł�����Ă��Ȃ��w(�S�s�X�L����)�B

jr.txt
�𒊏o

=end

if ARGV[0] 
  fn ARGV[0] 
else
  fn = 'jr.txt'
end
ln = 0
r2 = Regexp::new('.+��\Z')
IO.foreach(fn) do |lin|
  ln += 1
  eki = lin.split(/\t/)[2]		# �w
  unless r2.match(eki)
	  puts sprintf("%s(%d): %s", fn, ln, eki)
  end
end

